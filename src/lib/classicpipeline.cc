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
// == MODULE classicpipeline.cc ==

// -- Own headers --

#include "multovl/classicpipeline.hh"
#include "multovl/io/fileio.hh"
#include "multovl/multioverlap.hh"
#include "multovl/baseregion.hh"
#include "multovl/io/linewriter.hh"
#include "multovl/config.hh"

// -- Boost headers --

#include <boost/archive/binary_oarchive.hpp>    // serialization
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

// -- Standard headers --

#include <fstream>
#include <ctime>

// == Implementation ==

// -- Serialization --

namespace boost {
namespace serialization {

// Non-intrusive serialization for the Input struct
template<class Archive>
void serialize(Archive& ar, multovl::ClassicPipeline::Input& inp, const unsigned int version)
{
    ar & inp.name & inp.trackid & inp.regcnt;
}

}   // namespace serialization
}   // namespace boost

// -- ClassicPipeline methods --

namespace multovl {

ClassicPipeline::ClassicPipeline(int argc, char* argv[])
{
    set_optpimpl(new ClassicOpts());
    opt_ptr()->process_commandline(argc, argv); // exits on error or help request
}

// -- Pipeline virtual method implementations

unsigned int ClassicPipeline::read_input()
{
    unsigned int trackcnt = 0;
    if (opt_ptr()->load_from() != "")
    {
        // read input data from a previously saved archive
        try {
            std::ifstream ifs(opt_ptr()->load_from().c_str());
            boost::archive::binary_iarchive ia(ifs);
            ia >> inputs() >> cmovl();
            trackcnt = inputs().size();
        } catch (const boost::archive::archive_exception& aex)
        {
            add_error("Cannot load archive", std::string(aex.what()));
            return 0;
        }
    } else {
        // read tracks from cmdline arg files
        trackcnt = read_tracks();
    }
    return trackcnt;
}

// Read tracks from separate files specified as pos args on the command line (private)
unsigned int ClassicPipeline::read_tracks()
{
    typedef std::vector<std::string> str_vec;
    const str_vec& inputfiles = opt_ptr()->input_files();
    unsigned int trackid = 0;   // current ID, will be equal to the number of OK tracks on return
    
    for (const auto& inf : inputfiles) {
        Input currinp(inf);
        io::FileReader reader(currinp.name);    // automatic format detection
        if (!reader.errors().ok())
        {
            // make a note of all errors seen by the reader
            add_all_errors(reader.errors());
            inputs().push_back(currinp);
            continue;
        }
        
        std::string chrom;
        BaseRegion reg;
        unsigned int regcnt = 0, problemcnt = 0;
        while (true)
        {
            bool ok = reader.read_into(chrom, reg);
            if (reader.finished())
                break;
            if (!ok)
            {
                ++problemcnt;
                continue;
            }

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
        if (problemcnt > 0)
        {
            reader.errors().print(std::cerr);    // print errors & warnings
            add_warning("Summary",
                boost::lexical_cast<std::string>(problemcnt) +
                "x problem reading from file " + currinp.name
            );
        }
        if (regcnt > 0)
        {
            // good input
            currinp.trackid = ++trackid;
            currinp.regcnt = regcnt;
            inputs().push_back(currinp);
            continue;
        }
        inputs().push_back(currinp);  // bad input, with trackid,regcnt still 0
        if (!reader.errors().ok())
        {
            add_warning("Could not read valid regions from file", currinp.name);
        }
    }
    return trackid; // number of tracks from which at least 1 region could be read
}

// Serial implementation of Multovl
unsigned int ClassicPipeline::detect_overlaps()
{
    // the overlaps are detected chromosome by chromosome
    unsigned int totalcounts = 0;
    for (auto& cm : cmovl()) {
        MultiOverlap& movl = cm.second;      // "current overlap"
        
        // generate and store overlaps
        if (opt_ptr()->uniregion())
        {
            totalcounts += movl.find_unionoverlaps(opt_ptr()->ovlen(), 
                opt_ptr()->minmult(), opt_ptr()->maxmult(), opt_ptr()->extension());
        }
        else
        {
            totalcounts += movl.find_overlaps(opt_ptr()->ovlen(), 
                opt_ptr()->minmult(), opt_ptr()->maxmult(), 
                opt_ptr()->extension(), !opt_ptr()->nointrack());
        }
    }
    return totalcounts;
}

bool ClassicPipeline::write_output()
{
    // save current status in archive if asked to do so
    if (opt_ptr()->save_to() != "")
    {
        // write status data to archive file
        try {
            std::ofstream ofs(opt_ptr()->save_to().c_str());
            boost::archive::binary_oarchive oa(ofs);
            
            // Note that only the input data and the chrom=>MultiOverlap map is saved
            // neither the results nor the parameter settings are.
            // The idea is to load these again and re-run with possibly different settings.
            oa << inputs() << cmovl();
        } catch (const boost::archive::archive_exception& aex)
        {
            add_error("Cannot save archive", std::string(aex.what()));
            return false;
        }
    }
    
    // output the result in the selected format to stdout
    if (opt_ptr()->outformat() == "BED")
    {
        // write BED output
        return write_bed_output();
    } else {
        // default GFF2
        return write_gff_output();
    }
}

// -- Result output methods --

bool ClassicPipeline::write_gff_output()
{
    // GFF2 metainfo
    std::cout << "##gff-version 2" << std::endl;
    
    // get the current date, print in ISO-8601 format
    // inspired by CPPreference.com
    std::time_t time = std::time({});
    const unsigned int TSTRLEN = 11;
    char timestr[TSTRLEN];
    std::strftime(timestr, TSTRLEN, "%Y-%m-%d", std::gmtime(&time));
    std::cout << "##date " << timestr << std::endl;

    // version information, GFF style
    std::cout << "##source-version " << config::versioninfo() << ", "
        << config::build_type() << " build, compiler: "
        << config::build_compiler() 
        << ", system: "<< config::build_system() << std::endl;
    
    // MultOvl standard comments
    write_comments();
    
    // process each chromosome in turn
    for (const auto& cm : cmovl()) {
        io::GffLinewriter lw(
            opt_ptr()->source(), 
            2, cm.first
        );    // init with chromosome
        const MultiOverlap::multiregvec_t& mregs = cm.second.overlaps();
        
        // simply write the regions
        for (const auto& mreg : mregs) {
            std::cout << lw.write(mreg) << std::endl;
        }
    }
    return true;    // cannot really go wrong
}

bool ClassicPipeline::write_bed_output()
{
    // MultOvl standard comments
    write_comments();
    
    // process each chromosome in turn
    for (const auto& cm : cmovl()) {
        io::BedLinewriter lw(cm.first); // init with chromosome
        const MultiOverlap::multiregvec_t& mregs = cm.second.overlaps();
        
        // simply write the regions
        for (const auto& mreg : mregs) {
            std::cout << lw.write(mreg) << std::endl;
        }
    }
    return true;    // cannot really go wrong
}

// Writes the standard MultOvl comments to stdout.
// Since the comments have the same syntax for BED and GFF,
// this could be factored out.
// Lists the parameters, the input files, multiplicity statistics. Private
void ClassicPipeline::write_comments()
{
    // count "histograms": overlap combinations
    MultiOverlap::Counter counter;
    for (auto& cm : cmovl()) {
        cm.second.overlap_stats(counter);      // "current overlap"
    }
    
    // the command-line parameters
    std::cout << "# Parameters = " << opt_ptr()->param_str() << std::endl;
    
    // add the input file names as comments
    if (opt_ptr()->load_from() != "")
    {
        std::cout << "# Input data loaded from archive = " 
            << opt_ptr()->load_from() << std::endl;
    }
    std::cout << "# Input files = " << inputs().size() << std::endl;
    for (const auto& inp : inputs()) {
        std::cout << "# " << inp.name;
        if (inp.trackid > 0)
        {
            std::cout << " = track " 
                << inp.trackid << ", region count = " 
                << inp.regcnt;
        } else {
            std::cout << " : skipped";
        }
        std::cout << std::endl;
    }
    
    // list multiplicity information
    std::cout << "# Overlap count = " << counter.total() << std::endl;
    std::cout << "# Multiplicity counts = " << counter.to_string() << std::endl;
}

}   // namespace multovl
