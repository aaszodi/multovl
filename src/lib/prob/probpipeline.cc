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
// == MODULE probpipeline.cc ==

// -- Own headers --

#include "multovl/prob/probpipeline.hh"
#include "multovl/io/fileio.hh"

// -- Boost headers --

#include "boost/timer/progress_display.hpp"

// -- Standard headers --

#include <utility>
#include <fstream>

// == Implementation ==

namespace multovl {
namespace prob {

// -- OvlenCounter methods --

void ProbPipeline::OvlenCounter::update(const MultiOverlap::multiregvec_t& overlaps)
{
    for (const auto& ovl : overlaps) {
        unsigned int m = ovl.multiplicity(),
            len = ovl.length();
        _mtolen[m] += len;
    }
}

// -- ProbPipeline methods --

// needed for the derived class, hence protected
ProbPipeline::ProbPipeline():
    _csovl(),
    _stat()
{}

ProbPipeline::ProbPipeline(int argc, char* argv[]):
    ProbPipeline()
{
    set_optpimpl(new ProbOpts());
    opt_ptr()->process_commandline(argc, argv); // exits on error or help request
}

// -- Virtual method implementations

unsigned int ProbPipeline::read_input()
{
    unsigned int trackcnt = 0, regcnt = 0;
    
    // first read the free regions
    // if successful, then csovl() is set up, no more chromosomes will be accepted
    regcnt = read_free_regions(opt_ptr()->free_file());
    if (regcnt == 0)
        return 0;
    
    // then read the optional fixed regions
    if (opt_ptr()->fixed_filecnt() > 0)
    {
        read_tracks(opt_ptr()->fixed_files(), trackcnt, false);
    }
    
    // read shufflable tracks from cmdline arg files
    regcnt = read_tracks(opt_ptr()->shuffle_files(), trackcnt, true);
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
    BaseRegion reg; // temp input
    typedef std::vector<BaseRegion> rv_t;
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
            crv.insert(std::make_pair(chrom, rv_t(1, reg)));
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
    
    // set up csovl() map
    for (const auto& cr : crv)
    {
        const std::string& chrom = cr.first;
        const rv_t& regs = cr.second;
        csovl().insert(std::make_pair(chrom, ShuffleOvl(regs)));
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
    unsigned int totalregcnt = 0;
    
    for (const auto& inputfilenm : inputfiles)
    {
        Input currinp(inputfilenm);
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
            chrom_shufovl_map::iterator csit = csovl().find(chrom);
            if (csit == csovl().end())
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
            
            // OK, add the region to its chromosome's ShuffleOvl instance
            // note the shuffleability
            csit->second.add(reg, trackid + 1, shuffle);
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
    unsigned int acts = calc_actual_overlaps();
    
    // now estimate the null distribution by reshuffling the shufflable tracks, 
    // and re-doing the overlaps with the same settings
    unsigned int maxreshufflings = opt_ptr()->reshufflings();
    boost::timer::progress_display *progress = nullptr;
    if (opt_ptr()->progress())
    {
        // prints display at creation time
        progress = new boost::timer::progress_display(maxreshufflings, std::cerr);
    }
    UniformGen rng(opt_ptr()->random_seed());
    for (unsigned int r = 0; r < maxreshufflings; ++r)
    {
    	// TODO this loop may be parallelised
        OvlenCounter rndcounter;
        for (auto& csit : csovl())
        {
            ShuffleOvl& sovl = csit.second;
            
            // generate and store overlaps
            if (opt_ptr()->uniregion())
            {
                sovl.shuffle_unionoverlaps(rng,
                    opt_ptr()->ovlen(), 
                    opt_ptr()->minmult(), 
                    opt_ptr()->maxmult(),
                    opt_ptr()->extension());
            }
            else
            {
                sovl.shuffle_overlaps(rng,
                    opt_ptr()->ovlen(), 
                    opt_ptr()->minmult(), 
                    opt_ptr()->maxmult(),
                    opt_ptr()->extension(),
                    !opt_ptr()->nointrack());
            }
            rndcounter.update(sovl.overlaps()); // update actual counts
        }
        
        // update the empirical distributions
        for (const auto& mtc : rndcounter.mtolen())
        {
            stat().add(mtc.first, mtc.second, false);
        }
        
        if (opt_ptr()->progress())
        {
            // update the ASCII progress display
            ++(*progress);
        }
    }   // end of reshufflings
    
    if (opt_ptr()->progress())
    {
        delete progress;
        progress = nullptr;
    }
    
    // evaluate the stats
    stat().evaluate();
    
    return acts;    // the NUMBER of actual overlaps
}

unsigned int ProbPipeline::calc_actual_overlaps()
{
    OvlenCounter actcounter;
    unsigned int acts = 0;
    for (auto& csit : csovl())
    {
        ShuffleOvl& sovl = csit.second;      // "current overlap"
        
        // generate and store overlaps
        if (opt_ptr()->uniregion())
        {
            acts += sovl.find_unionoverlaps(opt_ptr()->ovlen(), 
                opt_ptr()->minmult(), 
                opt_ptr()->maxmult(),
                opt_ptr()->extension());
        }
        else
        {
            acts += sovl.find_overlaps(opt_ptr()->ovlen(), 
                opt_ptr()->minmult(), 
                opt_ptr()->maxmult(),
                opt_ptr()->extension(),
                !opt_ptr()->nointrack());
        }
        actcounter.update(sovl.overlaps()); // update actual counts
    }
    
    // add actual counts to statistics
    for (const auto& mtc : actcounter.mtolen())
    {
        stat().add(mtc.first, mtc.second, true);
    }
    return acts;
}

bool ProbPipeline::write_output()
{
    // Preamble: the command-line parameters
    std::cout << "# Parameters = " << opt_ptr()->param_str() << '\n';
    
    // input file names (the fixed are listed in the parameters above)
    std::cout << "# Input files = " << inputs().size() << '\n';
    for (const auto& inp : inputs())
    {
        std::cout << "# " << inp.name;
        if (inp.trackid > 0)
        {
            std::cout << " = track " 
                << inp.trackid << ", region count = " 
                << inp.regcnt;
        } else {
            std::cout << " : skipped";
        }
        if (opt_ptr()->file_is_fixed(inp.name))
        	std::cout << " [fixed]";
        else
        	std::cout << " [shuffled]";
        std::cout << '\n';
    }

    // output the result in the selected format to stdout as CSV
    // for the whole range of multiplicities seen
    // we count how many multiplicities were properly analysed
    unsigned int mcount = 0;
    std::cout << "# == Overlap length statistics ==" << '\n'
        << "Multiplicity,Actual,Mean,SD,Pvalue,Zscore" << '\n';
    for (unsigned int m = stat().min_mult(); m <= stat().max_mult(); ++m)
    {
        try {
            const Stat::Distr& distr = stat().distr(m);
            if (distr.is_valid())
            {
                const EmpirDistr& nd = distr.nulldistr();
                std::cout << m << ',' << distr.actual() << ','
                    << nd.mean() << ',' << nd.std_dev() << ','
                    << distr.p_value() << ',' << distr.z_score() << '\n';
                ++mcount;
            }
        } catch (const Stat::NotfoundException&) {
            // skip this multiplicity
        }
    }
    return (mcount > 0);
}

}   // namespace prob
}   // namespace multovl
