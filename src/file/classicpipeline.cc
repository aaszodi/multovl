// == MODULE classicpipeline.cc ==

// -- Own headers --

#include "classicpipeline.hh"
#include "fileio.hh"
#include "multioverlap.hh"
#include "region.hh"
#include "multovl_config.hh"
#include "linewriter.hh"

// -- Boost headers --

#include "boost/date_time/gregorian/gregorian.hpp"  // needed by GFF output header
#include <boost/archive/binary_oarchive.hpp>    // serialization
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

// -- Standard headers --

#include <fstream>

// == Implementation ==

// -- Serialization --

namespace boost {
namespace serialization {

// Non-intrusive serialization for the Input struct
template<class Archive>
void serialize(Archive& ar, multovl::Pipeline::Input& inp, const unsigned int version)
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

ClassicPipeline::~ClassicPipeline()
{
    ClassicOpts *op = opt_ptr();
    if (op != NULL)
    {
        delete op;
        set_optpimpl(NULL);
    }
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
    
    for (str_vec::const_iterator ifit = inputfiles.begin();
        ifit != inputfiles.end(); ++ifit)
    {
        Input currinp(*ifit);
        io::FileReader reader(currinp.name);    // automatic format detection
        if (!reader.errors().ok())
        {
            // make a note of all errors seen by the reader
            add_all_errors(reader.errors());
            inputs().push_back(currinp);
            continue;
        }
        
        std::string chrom;
        Region reg;
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

// NOTE: detect_overlaps() is the default, not overridden

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
    boost::gregorian::date d(boost::gregorian::day_clock::local_day());
    std::cout << "##date " << boost::gregorian::to_iso_extended_string(d) << std::endl;

    // version information, GFF style
    std::cout << "##source-version Multovl version " << MULTOVL_VER() << " "
        << MULTOVL_BUILD() << std::endl;
    
    // MultOvl standard comments
    write_comments();
    
    // process each chromosome in turn
    for (chrom_multovl_map::const_iterator cmit = cmovl().begin();
        cmit != cmovl().end(); ++cmit)
    {
        io::GffLinewriter lw(
            opt_ptr()->source(), 
            2, cmit->first
        );    // init with chromosome
        const MultiOverlap::multiregvec_t& mregs = cmit->second.overlaps();
        
        // simply write the regions
        for (MultiOverlap::multiregvec_t::const_iterator mregit = mregs.begin();
            mregit != mregs.end(); ++mregit)
        {
            std::cout << lw.write(*mregit) << std::endl;
        }
    }
    return true;    // cannot really go wrong
}

bool ClassicPipeline::write_bed_output()
{
    // MultOvl standard comments
    write_comments();
    
    // process each chromosome in turn
    for (chrom_multovl_map::const_iterator cmit = cmovl().begin();
        cmit != cmovl().end(); ++cmit)
    {
        io::BedLinewriter lw(cmit->first); // init with chromosome
        const MultiOverlap::multiregvec_t& mregs = cmit->second.overlaps();
        
        // simply write the regions
        for (MultiOverlap::multiregvec_t::const_iterator mregit = mregs.begin();
            mregit != mregs.end(); ++mregit)
        {
            std::cout << lw.write(*mregit) << std::endl;
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
    for (chrom_multovl_map::iterator cmit = cmovl().begin();
        cmit != cmovl().end(); ++cmit)
    {
        cmit->second.overlap_stats(counter);      // "current overlap"
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
    
    // list multiplicity information
    std::cout << "# Overlap count = " << counter.total() << std::endl;
    std::cout << "# Multiplicity counts = " << counter.to_string() << std::endl;
}

}   // namespace multovl
