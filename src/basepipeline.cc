// == MODULE basepipeline.cc ==

// -- Own headers --

#include "basepipeline.hh"
#include "timer.hh"
#include "multovlopts.hh"

// -- Standard headers --

#include <iostream>

// == Implementation ==

namespace multovl {

bool BasePipeline::set_optpimpl(MultovlOptbase* optp)
{
    if (_optpimpl == NULL)
    {
        _optpimpl = optp;
        return true;
    }
    else
        return false;
}

bool BasePipeline::run()
{
    Timer timer;    // start with current time as 0-th time point
    
    // read the regions
    unsigned int trackcnt = read_input();
    if (trackcnt == 0 || !errors().perfect())
    {
        std::cerr << "! Problems encountered during input" << std::endl;
        errors().print(std::cerr);
        if (!errors().ok())
            return false;  // stop only on serious errors
        else
            clear_errors();
    }
    if (opt_ptr()->timing())
        timer.add_timepoint();  // [1]-st time point is the end of input
    
    // detect overlaps
    detect_overlaps();
    if (!errors().perfect())
    {
        std::cerr << "! Problems encountered during overlap detection" << std::endl;
        errors().print(std::cerr);
        if (!errors().ok())
            return false;  // stop on errors only
        else
            clear_errors();
    }
    if (opt_ptr()->timing())
    {
        timer.add_timepoint();  // [2]-nd timepoint is the end of the multioverlap calc

        // no overlap output is produced, just the
        // timing results are printed to stdout
        std::cout << "Parameters: " << opt_ptr()->param_str() 
            << ", Timing: input = " << timer.interval(1) 
            << ", multovl = " << timer.interval(2,1)
            << ", total = " << timer.interval(2) << std::endl;
        return true;    // OK
    }
    
    // generate output: this can hardly go wrong, but who knows...
    write_output();
    if (!errors().perfect())
    {
        std::cerr << "! Problems encountered during output" << std::endl;
        if (!errors().ok())
            return false;   // stop on errors only
        else
            clear_errors();
    }
        
    return true;    // all is fine
}

}   // namespace multovl
