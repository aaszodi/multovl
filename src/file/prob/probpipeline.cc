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

namespace multovl {
namespace prob {

// -- OvlenCounter methods --

void ProbPipeline::OvlenCounter::update(const MultiOverlap::multiregvec_t& overlaps)
{
    for (MultiOverlap::multiregvec_t::const_iterator mrcit = overlaps.begin();
        mrcit != overlaps.end(); ++mrcit)
    {
        unsigned int m = mrcit->multiplicity(),
            len = mrcit->length();
        _mtolen[m] += len;
    }
}

// -- ProbPipeline methods --

ProbPipeline::ProbPipeline(int argc, char* argv[]):
    BasePipeline(),
    _csovl(),
    _stat()
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
        add_all_errors(reader.errors());
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
        reader.errors().print(std::cerr);    // print errors & warnings
        add_warning("Summary",
					boost::lexical_cast<std::string>(problemcnt) +
					"x problem reading from free region file " + freefile
					);
    }
    if (regcnt == 0)
    {
        add_warning("Could not read valid regions from free region file", freefile);
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
            add_all_errors(reader.errors());
            inputs().push_back(currinp);
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
                add_warning(ERRPREFIX, "Chromosome '" + chrom + "' not in free regions");
                ++problemcnt;
                continue;
            }
            if (! csit->second.fit_into_frees(reg))
            {
                add_warning(ERRPREFIX, "Region " + chrom + ":[" +
							boost::lexical_cast<std::string>(reg.first()) + "-" +
							boost::lexical_cast<std::string>(reg.last()) +
							"] not in free regions");
                ++problemcnt;
                continue;
            }
            
            // OK, add the region
            csit->second.add(reg, trackid+1);
            if (shuffle)
            {
                csit->second.add_randomplacer(reg.length(), trackid+1);
            }
            ++regcnt;
        }
        if (problemcnt > 0)
        {
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
            totalregcnt += regcnt;
            continue;
        }
        inputs().push_back(currinp);  // totally bad input, with regcnt still 0
        if (!reader.errors().ok())
        {
            add_warning("Could not read valid regions from file", currinp.name);
        }
    }
    return totalregcnt;
}

unsigned int ProbPipeline::detect_overlaps()
{
    // first calculate the actual overlaps without shuffling
    OvlenCounter actcounter;
    unsigned int acts = 0;
    for (chrom_shufovl_map::iterator csit = _csovl.begin();
         csit != _csovl.end(); ++csit)
    {
        ShuffleOvl& sovl = csit->second;      // "current overlap"
        
        // generate and store overlaps
        if (opt_ptr()->uniregion())
        {
            acts += sovl.find_unionoverlaps(opt_ptr()->ovlen(), 
                opt_ptr()->minmult(), 
                opt_ptr()->maxmult());
        }
        else
        {
            acts += sovl.find_overlaps(opt_ptr()->ovlen(), 
                opt_ptr()->minmult(), 
                opt_ptr()->maxmult(), 
                !opt_ptr()->nointrack());
        }
        actcounter.update(sovl.overlaps()); // update actual counts
    }
    
    // add actual counts to statistics
    for (OvlenCounter::mtolen_t::const_iterator mtcit = actcounter.mtolen().begin();
        mtcit != actcounter.mtolen().end(); ++mtcit)
    {
        _stat.add(mtcit->first, mtcit->second, true);
    }
    
    // now estimate the null distribution by reshuffling the shufflable tracks, 
    // and re-doing the overlaps with the same settings
    UniformGen rng(_optp->random_seed());
    for (unsigned int r = 0; r < _optp->reshufflings(); ++r)
    {
    	// TODO this loop may be parallelised
        OvlenCounter rndcounter;
        for (chrom_shufovl_map::iterator csit = _csovl.begin();
             csit != _csovl.end(); ++csit)
        {
            ShuffleOvl& sovl = csit->second;
            sovl.shuffle(rng);
            
            // generate and store overlaps
            if (opt_ptr()->uniregion())
            {
                sovl.find_unionoverlaps(opt_ptr()->ovlen(), 
                    opt_ptr()->minmult(), 
                    opt_ptr()->maxmult());
            }
            else
            {
                sovl.find_overlaps(opt_ptr()->ovlen(), 
                    opt_ptr()->minmult(), 
                    opt_ptr()->maxmult(), 
                    !opt_ptr()->nointrack());
            }
            rndcounter.update(sovl.overlaps()); // update actual counts
        }
        
        // update the empirical distributions
        for (OvlenCounter::mtolen_t::const_iterator mtcit = rndcounter.mtolen().begin();
            mtcit != rndcounter.mtolen().end(); ++mtcit)
        {
            _stat.add(mtcit->first, mtcit->second, false);
        }
    }
    
    // evaluate the stats
    _stat.evaluate();
    
    return acts;    // the NUMBER of actual overlaps
}

bool ProbPipeline::write_output()
{
    // Preamble: the command-line parameters
    std::cout << "# Parameters = " << _optp->param_str() << std::endl;
    
    // input file names (the fixed are listed in the parameters above)
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
        if (_optp->file_is_fixed(it->name))
        	std::cout << " [fixed]";
        else
        	std::cout << " [shuffled]";
        std::cout << std::endl;
    }

    // output the result in the selected format to stdout as CSV
    // for the whole range of multiplicities seen
    // we count how many multiplicities were properly analysed
    unsigned int mcount = 0;
    std::cout << "# == Overlap length statistics ==" << std::endl
        << "Multiplicity,Actual,Mean,SD,Pvalue,Zscore" << std::endl;
    for (unsigned int m = _stat.min_mult(); m <= _stat.max_mult(); ++m)
    {
        try {
            const Stat::Distr& distr = _stat.distr(m);
            if (distr.is_valid())
            {
                const EmpirDistr& nd = distr.nulldistr();
                std::cout << m << ',' << distr.actual() << ','
                    << nd.mean() << ',' << nd.std_dev() << ','
                    << distr.p_value() << ',' << distr.z_score() << std::endl;
                ++mcount;
            }
        } catch (const Stat::NotfoundException& ex) {
            // skip this multiplicity
        }
    }
    return (mcount > 0);
}

}   // namespace prob
}   // namespace multovl
