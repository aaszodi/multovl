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

// == Implementation ==

namespace multovl {

ClassicPipeline::ClassicPipeline(int argc, char* argv[]):
    Pipeline()
{
    _optp = new ClassicOpts();
    _optp->process_commandline(argc, argv); // exits on error or help request
}

ClassicPipeline::~ClassicPipeline()
{
    delete _optp;
}

unsigned int ClassicPipeline::read_input()
{
    typedef std::vector<std::string> str_vec;
    const str_vec& inputfiles = dynamic_cast<ClassicOpts*>(_optp)->input_files();
    unsigned int filecnt = inputfiles.size();
    _inputs.reserve(filecnt);
    unsigned int trackid = 0;   // current ID, will be equal to the number of OK tracks on return
    
    for (str_vec::const_iterator ifit = inputfiles.begin();
        ifit != inputfiles.end(); ++ifit)
    {
        Input currinp(*ifit);
        io::FileReader reader(currinp.name);    // automatic format detection
        if (!reader.errors().ok())
        {
            // make a note
            _errors += reader.errors();
            _inputs.push_back(currinp);
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
                continue;

            chrom_multovl_map::iterator cmit = _cmovl.find(chrom);
            if (cmit != _cmovl.end())
            {
                // this chromosome has been seen already
                // add current region to the corresponding MultiOverlap object
                cmit->second.add(reg, trackid+1);
            }
            else
            {
                // new chromosome with new MultiOverlap object
                MultiOverlap mo(reg, trackid+1);
                _cmovl[chrom] = mo;
            }
            ++regcnt;
        }
        if (problemcnt > 0)
        {
            _errors.add_warning(
                boost::lexical_cast<std::string>(problemcnt) +
                "x problem reading from file " + currinp.name
            );
        }
        if (regcnt > 0)
        {
            // good input
            currinp.trackid = ++trackid;
            currinp.regcnt = regcnt;
            _inputs.push_back(currinp);
            continue;
        }
        _inputs.push_back(currinp);  // bad input, with trackid,regcnt still 0
        if (!reader.errors().ok())
        {
            _errors.add_warning("Could not read valid regions from file " + currinp.name);
        }
    }
    return trackid; // number of tracks from which at least 1 region could be read
}

// NOTE: detect_overlaps() is the default, not overridden

bool ClassicPipeline::write_output()
{
    // output format
    // we may very well downcast _optp...
    if (dynamic_cast< ClassicOpts* >(_optp)->outformat() == "BED")
    {
        // write BED output
        return write_bed_output();
    } else {
        // default GFF2
        return write_gff_output();
    }
}

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
    for (chrom_multovl_map::const_iterator cmit = _cmovl.begin();
        cmit != _cmovl.end(); ++cmit)
    {
        io::GffLinewriter lw(
            dynamic_cast<ClassicOpts*>(_optp)->source(), 
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
    for (chrom_multovl_map::const_iterator cmit = _cmovl.begin();
        cmit != _cmovl.end(); ++cmit)
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
    for (chrom_multovl_map::iterator cmit = _cmovl.begin();
        cmit != _cmovl.end(); ++cmit)
    {
        cmit->second.overlap_stats(counter);      // "current overlap"
    }
    
    // the command-line parameters
    std::cout << "# Parameters = " << _optp->param_str() << std::endl;
    
    // add the input file names as comments
    std::cout << "# Input files = " << _inputs.size() << std::endl;
    for (input_vec::const_iterator it = _inputs.begin();
        it != _inputs.end(); ++it)
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
