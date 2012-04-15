// == MODULE parprobpipeline.cc ==

// -- Own headers --

#include "parprobpipeline.hh"
#include "multovl_config.hh"

// -- Boost headers --

#include "boost/thread.hpp"
#include "boost/scoped_ptr.hpp"

// -- Standard headers --

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
    boost::thread_group workers;
    unsigned int rndseed = opt_ptr()->random_seed();
    for (unsigned int i = 0; i < opt_ptr()->threads(); ++i)
    {
        boost::thread *shuffleworker = 
            new boost::thread(
                &ParProbPipeline::shuffle, this,
                rndseed, progress
            );
        workers.add_thread(shuffleworker);
        ++rndseed;
    }
    workers.join_all();
    
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
    boost::lock_guard<boost::mutex> lock(_shufflecounter_mutex);  // 1 thread only
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
