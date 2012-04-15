#ifndef MULTOVL_PROB_PARPIPELINE_HEADER
#define MULTOVL_PROB_PARPIPELINE_HEADER

// == Header parprobpipeline.hh ==

// -- Own headers --

#include "probpipeline.hh"
#include "parprobopts.hh"

// -- Boost headers --

#include "boost/thread.hpp"
#include "boost/progress.hpp"


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
        boost::lock_guard<boost::mutex> lock(_stat_mutex);  // 1 thread only
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
    boost::mutex _shufflecounter_mutex, _stat_mutex;
    
};

}   // namespace prob
}   // namespace multovl

#endif  // MULTOVL_PROB_PARPIPELINE_HEADER
