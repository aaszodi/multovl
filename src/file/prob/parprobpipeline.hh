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
#ifndef MULTOVL_PROB_PARPIPELINE_HEADER
#define MULTOVL_PROB_PARPIPELINE_HEADER

// == Header parprobpipeline.hh ==

// -- Own headers --

#include "probpipeline.hh"
#include "parprobopts.hh"

// -- Boost headers --

#include "boost/progress.hpp"

// -- Standard headers --

#include <thread>
#include <mutex>

namespace multovl {
namespace prob {

// -- Classes --

/// The ParProbPipeline implements the parallel version of the MULTOVL probability pipeline.
/// The inputs are files (text or binary),
/// the overlap calculations are serial/single core,
/// the repetitions after reshuffling (some) tracks to estimate the probability
/// of overlaps by chance are done in parallel.
class ParProbPipeline: public ProbPipeline
{
public:
    
    /// Inits the pipeline with the command-line arguments.
    /// These will be parsed inside and the program exits with an error message
    /// if parsing goes wrong.
    ParProbPipeline(int argc, char* argv[]);
    
    ~ParProbPipeline();
    
protected:
    
    /// Detects overlaps.
    /// First the overlaps without shuffling are calculated. Then the shufflable tracks
    /// are permuted and the number of overlaps counted again and again which will provide
    /// an estimate of the null distribution (ie. the extent of overlaps by chance).
    /// The reshufflings are done in parallel.
    /// \return the total number of overlaps found in the unpermuted case.
    virtual
    unsigned int detect_overlaps();
    
    /// \return access to the option-handling object
    virtual
    ParProbOpts* opt_ptr() { return dynamic_cast<ParProbOpts*>(opt_pimpl()); }
    
    /// \return thread-safe non-const access to the statistics collector object
    virtual
    Stat& stat()
    {
        std::lock_guard<std::mutex> lock(_stat_mutex);  // 1 thread only
        return ProbPipeline::stat();
    }
    
private:

    void shuffle(
        unsigned int rndseed,
        boost::progress_display* progressptr
    );
    
    /// Before a worker thread starts a shuffle, the shuffle counter will be checked.
    /// If it is >0, then this means there are still jobs to do. The counter will be 
    /// decremented and the worker performs one reshuffling. If it is 0, then the
    /// worker knows that it can stop, there are no more shuffling jobs.
    /// \param progressptr if not NULL then the global progress object is updated
    /// \return the number of remaining shuffles, 0 indicates the thread may stop
    unsigned int check_update_shufflecounter(boost::progress_display* progressptr=NULL);

    unsigned int _shufflecounter;
    std::mutex _shufflecounter_mutex, _stat_mutex;
    
};

}   // namespace prob
}   // namespace multovl

#endif  // MULTOVL_PROB_PARPIPELINE_HEADER
