// == MODULE probpipeline.cc ==

// -- Own headers --

#include "probpipeline.hh"
#include "probopts.hh"
#include "fileio.hh"
#include "multovl_config.hh"

// -- Boost headers --

// -- Standard headers --

#include <utility>
#include <fstream>

// == Implementation ==

// -- ProbPipeline methods --

namespace multovl {
namespace prob {

ProbPipeline::ProbPipeline(int argc, char* argv[]):
    BasePipeline(),
    _csovl(),
    _nulldistr() // NOTE: this will change!
{
    _optp = new ProbOpts();
    _optp->process_commandline(argc, argv); // exits on error or help request
}

ProbPipeline::~ProbPipeline()
{
    delete _optp;
}

// -- Virtual method implementations

unsigned int ProbPipeline::read_input()
{
    unsigned int trackcnt = 0, regcnt = 0;
    
    // first read the free regions
    // if successful, then _csovl is set up, no more chromosomes will be accepted
    regcnt = read_free_regions(_optp->free_file());
    if (regcnt == 0)
        return 0;
    
    // then read the optional fixed regions
    if (_optp->fixed_filecnt() > 0)
    {
        read_tracks(_optp->fixed_files(), trackcnt, false);
    }
    
    // read shufflable tracks from cmdline arg files
    regcnt = read_tracks(_optp->shuffle_files(), trackcnt, true);
    if (regcnt == 0)
        return 0;
    
    // all went well
    return trackcnt;
}

// Reads the free regions file, separates the regions by chromosome,
// sets up the chrom => ShuffleOvl map.
// Private
// \param freefile the filename of the free regions
// \return the number of free regions successfully read (0 on error)
unsigned int ProbPipeline::read_free_regions(const std::string& freefile)
{
    io::FileReader reader(freefile);    // automatic format detection
    if (!reader.errors().ok())
    {
        // make a note
        _errors += reader.errors();
        return 0;
    }
    
    std::string chrom;
    Region reg; // temp input
    typedef std::vector<Region> rv_t;
    typedef std::map<std::string, rv_t> crv_t;
    crv_t crv; // collect free regions per chromosome here
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
        
        crv_t::iterator crvit = crv.find(chrom);
        if (crvit != crv.end())
        {
            // this chromosome has been seen already
            crvit->second.push_back(reg);
        }
        else
        {
            // new chromosome
            crv.insert(std::make_pair<std::string, rv_t>(chrom, rv_t(1, reg)));
        }
        ++regcnt;
    }
    if (problemcnt > 0)
    {
        std::cerr << "NOTE: Problems while parsing free region file " << freefile << std::endl;
        reader.errors().print(std::cerr);    // print errors & warnings
        _errors.add_warning(
                            boost::lexical_cast<std::string>(problemcnt) +
                            "x problem reading from free region file " + freefile
                            );
    }
    if (regcnt == 0)
    {
        _errors.add_warning("Could not read valid regions from free region file " + freefile);
        return 0;
    }
    
    // set up _csovl map
    for (crv_t::const_iterator crvcit = crv.begin(); crvcit != crv.end(); ++crvcit)
    {
        const std::string& chrom = crvcit->first;
        const rv_t& regs = crvcit->second;
        _csovl.insert(std::make_pair<std::string, ShuffleOvl>(chrom,ShuffleOvl(regs)));
    }
    return regcnt;
}

// Read tracks from separate files. Private
// \param inputfiles a string vector holding the input file names
// \param shuffle true (default) if the input tracks are to be reshuffled. false for fixed tracks
// \return the number of regions successfully read
unsigned int ProbPipeline::read_tracks(
    const std::vector<std::string>& inputfiles,
    unsigned int& trackid,
    bool shuffle
)
{
    unsigned int i, filecnt = inputfiles.size(), totalregcnt = 0;
    
    for (i = 0; i < filecnt; ++i)
    {
        Input currinp(inputfiles[i]);
        io::FileReader reader(currinp.name);    // automatic format detection
        if (!reader.errors().ok())
        {
            // make a note
            _errors += reader.errors();
            _inputs.push_back(currinp);
            continue;
        }
        
        const std::string ERRPREFIX = "While parsing file " + currinp.name;
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
            
            // /chrom/, /reg/ now contain a chromosome and a successfully parsed region
            // check if /reg/ fits into the free regions previously defined for /chrom/
            chrom_shufovl_map::iterator csit = _csovl.find(chrom);
            if (csit == _csovl.end())
            {
                _errors.add_warning(ERRPREFIX + ": Chromosome '" + chrom + "' not in free regions");
                ++problemcnt;
                continue;
            }
            if (! csit->second.fit_into_frees(reg))
            {
                _errors.add_warning(ERRPREFIX + ": Region " + chrom + ":[" + 
                                    boost::lexical_cast<std::string>(reg.first()) + "-" +
                                    boost::lexical_cast<std::string>(reg.last()) +
                                    "] not in free regions");
                ++problemcnt;
                continue;
            }
            
            // OK, add the region
            csit->second.add(reg, trackid+1);
            if (shuffle)
                csit->second.add_randomplacer(reg.length(), trackid+1);
            ++regcnt;
        }
        if (problemcnt > 0)
        {
            std::cerr << "NOTE: Problems while parsing file " << currinp.name << std::endl;
            reader.errors().print(std::cerr);    // print errors & warnings
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
            totalregcnt += regcnt;
            continue;
        }
        _inputs.push_back(currinp);  // bad input, with regcnt still 0
        if (!reader.errors().ok())
        {
            _errors.add_warning("Could not read valid regions from file " + currinp.name);
        }
    }
    return totalregcnt;
}

unsigned int ProbPipeline::detect_overlaps()
{
    
    // first calculate the actual overlaps without shuffling
    unsigned int actcounts = 0;
    MultiOverlap::Counter actcounter;
    for (chrom_shufovl_map::iterator csit = _csovl.begin();
         csit != _csovl.end(); ++csit)
    {
        MultiOverlap& movl = csit->second;      // "current overlap"
        
        // generate and store overlaps
        if (opt_ptr()->uniregion())
        {
            actcounts += movl.find_unionoverlaps(opt_ptr()->ovlen(), 
                                                 opt_ptr()->minmult(), 
                                                 opt_ptr()->maxmult());
        }
        else
        {
            actcounts += movl.find_overlaps(opt_ptr()->ovlen(), 
                                            opt_ptr()->minmult(), 
                                            opt_ptr()->maxmult(), 
                                            !opt_ptr()->nointrack());
        }
        movl.overlap_stats(actcounter); // update actual counts
    }
    
    // now estimate the null distribution by reshuffling the shufflable tracks, 
    // and re-doing the overlaps with the same settings
    UniformGen rng(_optp->random_seed());
    for (unsigned int r = 0; r < _optp->reshufflings(); ++r)
    {
        unsigned int rndcounts = 0;
        MultiOverlap::Counter rndcounter;
        for (chrom_shufovl_map::iterator csit = _csovl.begin();
             csit != _csovl.end(); ++csit)
        {
            ShuffleOvl& sovl = csit->second;
            sovl.shuffle(rng);
            
            // generate and store overlaps
            if (opt_ptr()->uniregion())
            {
                rndcounts += sovl.find_unionoverlaps(opt_ptr()->ovlen(), 
                                                     opt_ptr()->minmult(), 
                                                     opt_ptr()->maxmult());
            }
            else
            {
                rndcounts += sovl.find_overlaps(opt_ptr()->ovlen(), 
                                                opt_ptr()->minmult(), 
                                                opt_ptr()->maxmult(), 
                                                !opt_ptr()->nointrack());
            }
            sovl.overlap_stats(rndcounter); // update actual counts
        }
        
        // update the empirical distributions
        _nulldistr.add(rndcounts); // NOTE: FOR TESTING ONLY!
    }
    _nulldistr.evaluate();
    
    return actcounts;
}

bool ProbPipeline::write_output()
{
    // output the result in the selected format to stdout
    // TODO
}

// -- Result output methods --

// Writes the standard MultOvl comments to stdout.
// Since the comments have the same syntax for BED and GFF,
// this could be factored out.
// Lists the parameters, the input files, ... Private
void ProbPipeline::write_comments() const
{
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
    // TODO modify as necessary
}

}   // namespace prob
}   // namespace multovl
