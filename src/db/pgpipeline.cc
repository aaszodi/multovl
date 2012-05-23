/* <LICENSE>
License for the MULTOVL multiple genomic overlap tools

Copyright (c) 2007-2012, Dr Andras Aszodi, 
Campus Science Support Facilities GmbH (CSF),
Dr-Bohr-Gasse 3, A-1030 Vienna, Austria, Europe.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of the Campus Science Support Facilities GmbH
      nor the names of its contributors may be used to endorse
      or promote products derived from this software without specific prior
      written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS
AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
</LICENSE> */
// == MODULE pgpipeline.cc ==

// -- Own headers --

#include "pgpipeline.hh"
#include "multioverlap.hh"
#include "region.hh"
#include "multovl_config.hh"

// -- Boost headers --

#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/property_tree/exceptions.hpp"

// -- Postgres --

#include "pqxx/pqxx"

// Starting with libpqxx 4.0, the terrible ("varchar", pqxx::prepare::treat_string)
// syntax for prepared statement parameters is gone
#if PQXX_VERSION_MAJOR <= 3
#define OLD_PQXX_PARAMSPEC
#else
#undef OLD_PQXX_PARAMSPEC
#endif

// -- Standard headers --

#include <unistd.h>     // for getlogin

// == Implementation ==

namespace multovl {

PgPipeline::PgPipeline(int argc, char* argv[])
{
    set_optpimpl(new PgMultovlOpts());    // "factory"
    opt_ptr()->process_commandline(argc, argv); // exits on error or help request
    
    // read the configuration file, prepare DB access
    if (!read_config())
    {
        std::cerr << "ERROR: Cannot read DB configuration, exiting." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

PgPipeline::~PgPipeline()
{
    PgMultovlOpts *op = opt_ptr();
    if (op != NULL)
    {
        delete op;
        set_optpimpl(NULL);
    }
}

// Reads a configuration file written in JSON format.
// Private
bool PgPipeline::read_config()
{
    using namespace boost::property_tree;
    try {
        ptree pt;
        
        // throws if input did not work for some reason
        read_json(opt_ptr()->config_file(), pt);
        
        // fetch the connection information, construct Postgres connection strings
        std::string hostdb = "host=" + pt.get("conn.hostname", "localhost")
            + " dbname=" + pt.get<std::string>("conn.database");
        _rconnstr = hostdb + " user=" + pt.get<std::string>("conn.read.user")
            + " password=" + pt.get<std::string>("conn.read.password");
        _wconnstr = hostdb + " user=" + pt.get<std::string>("conn.write.user")
            + " password=" + pt.get<std::string>("conn.write.password");
        
        // fetch the DB-specific command strings...
        
        // the download SQL should look like
        // SELECT id, chrom, first, last, strand FROM regionview WHERE tracknm = $1
        // it expects one variable, the track name,
        // and brings back the coordinates and some sort of unique ID of the regions
        // belonging to the track
        _download_sql = pt.get<std::string>("sql.download");
        
        // track upload statement: this should create the result track
        // expects the following params:-
        // ...
        // and is supposed to return a diagnostic string which should be 'OK' if all went well
        _trackupload_sql = pt.get<std::string>("sql.upload.track");
        
        // region upload statement
        // expects the following params:-
        // ...
        // ...
        _regionupload_sql = pt.get<std::string>("sql.upload.region");
        
    } catch (const ptree_error& pex) {
        std::cerr << "ERROR: " << pex.what() << std::endl;
        return false;
    }
    return true;
}

unsigned int PgPipeline::read_input()
{
    typedef std::vector<std::string> str_vec;
    unsigned int trackid = 0;   // current ID, will be equal to the number of OK tracks on return
    try {
        const str_vec& tracknames = 
            opt_ptr()->input_tracks();
        inputs().reserve(tracknames.size());
        
        // connect to the DB
        pqxx::connection conn(_rconnstr);
        
        // prepare the track download statement
        // that retrieves the region coordinates
        #ifdef DEBUG
        std::cerr << "* download sql=" << _download_sql << "|" << std::endl;
        #endif
        conn.prepare(
            "download_stmt",
            _download_sql   // comes from the config file
        )
        #ifdef OLD_PQXX_PARAMSPEC
        ("varchar", pqxx::prepare::treat_string);
        #else
        ;
        #endif
        
        // attempt to download each track in turn
        for (str_vec::const_iterator trkit = tracknames.begin();
            trkit != tracknames.end();
            ++trkit)
        {
            Input currinp(*trkit);
            unsigned int regcnt = 0;
            Region reg; // input buffer
            try {
                // download one track in a single read-only transaction
                pqxx::read_transaction ta(conn);
                pqxx::result res = ta.prepared("download_stmt")(*trkit).exec();
                #ifdef DEBUG
                std::cerr << "** " << *trkit << " result size = " << res.size() << std::endl;
                #endif
                
                // read the results, one by one, add regions...
                for (pqxx::result::const_iterator rit = res.begin();
                    rit != res.end(); ++rit)
                {
                    // these should be the columns 
                    // SELECT id, chrom, first, last, strand FROM 
                    std::string chrom(rit[1].c_str());
                    reg.set_coords(rit[2].as<unsigned int>(), rit[3].as<unsigned int>());
                    reg.strand(rit[4].c_str()[0]);
                    
                    // we use the DB unique ID as the region's name
                    // this will be needed for ancestry relationships
                    // when writing back the results
                    reg.name(rit[0].c_str());
                    
                    chrom_multovl_map::iterator cmit = cmovl().find(chrom);
                    if (cmit != cmovl().end())
                    {
                        // this chromosome has been seen already
                        // add current region to the corresponding MultiOverlap object
                        cmit->second.add(reg, trackid+1);
                    }
                    else
                    {
                        // new chromosome with new MultiOverlap object
                        MultiOverlap mo(reg, trackid+1);
                        cmovl()[chrom] = mo;
                    }
                    ++regcnt;
                }
                if (regcnt > 0)
                {
                    // good input
                    currinp.trackid = ++trackid;
                    currinp.regcnt = regcnt;
                    inputs().push_back(currinp);
                    ta.commit();
                }
                else
                {
                    add_warning("Could not get regions from track", *trkit);
                    inputs().push_back(currinp); // if regcnt == 0, currinp will be stored with trackid==0
                    ta.abort();
                }
                
            } catch (const pqxx::pqxx_exception& ex) {
                add_warning("Database problem", ex.base().what());  // silly base().what() needed...
            }
        }
        conn.disconnect();  // may not be needed?
    } catch (const pqxx::pqxx_exception& ex) {
        // if the connection or the statement preparation went wrong
        add_error("Database error", ex.base().what());
    }
    return trackid;
}
    
// NOTE: detect_overlaps() is the default, not overridden

bool PgPipeline::write_output()
{
    // count "histograms": overlap combinations
    MultiOverlap::Counter counter;
    for (chrom_multovl_map::iterator cmit = cmovl().begin();
        cmit != cmovl().end(); ++cmit)
    {
        cmit->second.overlap_stats(counter);      // "current overlap"
    }
    
    // Some general stats etc. will just be written to stdout as usual
    std::cout << "# PgMultovl version " << MULTOVL_VER() << MULTOVL_BUILD() << std::endl
        << "# Built with libpqxx version " << PQXX_VERSION << std::endl;
    std::cout << "# Parameters = " << opt_ptr()->param_str() << std::endl;
    std::cout << "# Input tracks = " << inputs().size() << std::endl;
    for (input_seq_t::const_iterator it = inputs().begin();
        it != inputs().end(); ++it)
    {
        std::cout << "# " << it->name;
        if (it->trackid > 0)
        {
            std::cout << " = track " 
                << it->trackid << ", region count = " 
                << it->regcnt;
        } else {
            std::cout << " : skipped";
        }
        std::cout << std::endl;
    }
    std::cout << "# Overlap count = " << counter.total() << std::endl;
    std::cout << "# Multiplicity counts = " << counter.to_string() << std::endl;

    // now the fun begins... connect to the DB
    try {
        pqxx::connection conn(_wconnstr);
        
        // prepare the output track creation statement (maybe not needed, one exec() could do)
        conn.prepare(
            "trackupload_stmt",
            _trackupload_sql   // comes from the config file
        )
        #ifdef OLD_PQXX_PARAMSPEC
        ("text", pqxx::prepare::treat_string)   // user login
        ("text", pqxx::prepare::treat_string)   // param_str
        ("text", pqxx::prepare::treat_string)   // ancestor tracks as Pg array '{foo,bar}'
        ("text", pqxx::prepare::treat_string);  // output track name
        #else
        ;
        #endif
        
        // prepare the region upload statement
        conn.prepare(
            "regionupload_stmt",
            _regionupload_sql   // comes from the config file
        )
        #ifdef OLD_PQXX_PARAMSPEC
        ("text", pqxx::prepare::treat_string)   // output track name
        ("text", pqxx::prepare::treat_string)   // chromosome name
        ("integer")("integer")   // region first,last coordinates
        ("text", pqxx::prepare::treat_string)  // strand
        ("text", pqxx::prepare::treat_string);   // ancestor IDs, Pg array '{3,4}'
        #else
        ;
        #endif
    
        // start upload transaction
        pqxx::work ta(conn);
        
        // create result track in DB
        std::string outputtrack = opt_ptr()->output_track();
        pqxx::result trackupres = 
            ta.prepared("trackupload_stmt")
            (login_name())
            (opt_ptr()->param_str())
            (inputs_to_anctrack())
            (outputtrack).exec();
        std::string diag;   // will be used later as well
        trackupres[0][0].to(diag);
        if (diag != "OK")
        {
            add_error("Result track upload", diag);
            ta.abort();
            return false;
        }
        
        // now upload the regions
        // process each chromosome in turn
        for (chrom_multovl_map::const_iterator cmit = cmovl().begin();
            cmit != cmovl().end(); ++cmit)
        {
            const std::string& chrom = cmit->first;
            const MultiOverlap::multiregvec_t& mregs = cmit->second.overlaps();
            
            for (MultiOverlap::multiregvec_t::const_iterator mregit = mregs.begin();
                mregit != mregs.end(); ++mregit)
            {
                pqxx::result regionupres =
                    ta.prepared("regionupload_stmt")
                    (outputtrack)
                    (chrom)
                    (mregit->first())(mregit->last())
                    (std::string(1, mregit->strand()))
                    (ancestors_to_pgarray(*mregit)).exec();
                regionupres[0][0].to(diag);
                if (diag != "OK")
                {
                    add_error("Result region upload", diag);
                    ta.abort();
                    return false;
                }
            }
        }
        
        ta.commit();    // all went well
    } catch (const pqxx::pqxx_exception& ex) {
        // if the connection or the statement preparation went wrong
        add_error("Database error", ex.base().what());
        return false;
    }
    return true;    // OK
}

// Converts the _valid_ input tracks to a Postgres array literal
// Private
std::string PgPipeline::inputs_to_anctrack() const
{
    std::string lit("{");
    bool notfirst = false;
    for (input_seq_t::const_iterator it = inputs().begin();
        it != inputs().end(); ++it)
    {
        if (it->trackid == 0)   // could not read from this, no valid ancestor track, skip
            continue;
        if (notfirst)
            lit += ",";
        lit += it->name;
        notfirst = true;
    }
    lit += "}";
    return lit;
}

// Constructs a Postgres array literal holding the names (database IDs)
// of the ancestors of an overlap.
// Private static
std::string PgPipeline::ancestors_to_pgarray(const MultiRegion& mr)
{
    std::string lit("{");
    bool notfirst = false;
    const ancregset_t& ancs = mr.ancestors();
    for (ancregset_t::const_iterator ait = ancs.begin();
        ait != ancs.end(); ++ait)
    {
        if (notfirst) lit += ",";
        lit += ait->name();
        notfirst = true;
    }
    lit += "}";
    return lit;
}

// Returns the login name of the currently logged-in user
// or the string 'pgmultovl_user' if the login name could not be found.
// This is UNIX-specific.
// Static private
std::string PgPipeline::login_name()
{
    char *logname = getlogin(); // may return NULL on error!
    if (logname == NULL)
        return "pgmultovl_user";
    else
        return std::string(logname);
}

}   // namespace multovl
