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
// == MODULE parprobpipeline.cc ==

// -- Own headers --

#include "parprobpipeline.hh"

// -- Boost headers --

// -- Standard headers --

#include <thread>
#include <vector>

// == Implementation ==

namespace multovl {
namespace prob {

// -- ParProbPipeline methods --

ParProbPipeline::ParProbPipeline(int argc, char* argv[]):
    ProbPipeline(), // no option processing yet!
    _shufflecounter(0)
{
    set_optpimpl(new ParProbOpts());
    opt_ptr()->process_commandline(argc, argv); // exits on error or help request
    _shufflecounter = opt_ptr()->reshufflings();
}

ParProbPipeline::~ParProbPipeline()
{
    ParProbOpts *op = opt_ptr();
    if (op != NULL)
    {
        delete op;
        set_optpimpl(NULL);
    }
}

// -- Virtual method implementations

unsigned int ParProbPipeline::detect_overlaps()
{
    // first calculate the actual overlaps without shuffling
    unsigned int acts = calc_actual_overlaps();
    
    // optional ASCII progress bar to stderr
    boost::progress_display *progress = NULL;
    if (opt_ptr()->progress())
    {
        // prints display at creation time
        progress = new boost::progress_display(opt_ptr()->reshufflings(), std::cerr);
    }
    
    // now estimate the null distribution in parallel by reshuffling the shufflable tracks, 
    // and re-doing the overlaps with the same settings
    std::vector<std::thread> workers;
    unsigned int rndseed = opt_ptr()->random_seed();
    for (unsigned int i = 0; i < opt_ptr()->threads(); ++i)
    {
        workers.emplace_back(&ParProbPipeline::shuffle, this, rndseed, progress);
        ++rndseed;
    }
    for (auto& worker : workers) { worker.join(); }
    
    // evaluate the stats
    stat().evaluate();
    
    return acts;    // the NUMBER of actual overlaps
}

// Worker thread
void ParProbPipeline::shuffle(
    unsigned int rndseed,
    boost::progress_display* progressptr
)
{
    UniformGen rng(rndseed);
    chrom_shufovl_map csovl(this->csovl());   // local copy of csovl() map, hopefully with deeply copied ShuffleOvl
    while(check_update_shufflecounter(progressptr))
    {
        OvlenCounter rndcounter;
        for (chrom_shufovl_map::iterator csit = csovl.begin();
             csit != csovl.end(); ++csit)
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
            stat().add(mtcit->first, mtcit->second, false);
        }
    }
}

unsigned int ParProbPipeline::check_update_shufflecounter(boost::progress_display* progressptr)
{
    std::lock_guard<std::mutex> lock(_shufflecounter_mutex);  // 1 thread only
    unsigned int oldcounter = _shufflecounter;
    if (_shufflecounter > 0)
    {
        if (progressptr != NULL)
            ++(*progressptr);
        --_shufflecounter;
    }
    return oldcounter;
}

}   // namespace prob
}   // namespace multovl
