// == MODULE pipeline.cc ==

// -- Own headers --

#include "pipeline.hh"
#include "multioverlap.hh"
#include "multovlopts.hh"
#include "timer.hh"

// -- Standard headers --

#include <iostream>

// == Implementation ==

namespace multovl {

bool Pipeline::run()
{
    Timer timer;    // start with current time as 0-th time point
    
    // read the regions into the chrom->MultiOverlap map
    unsigned int trackcnt = read_input();
    if (trackcnt == 0 || !errors().ok())
        return false;
    if (opt_ptr()->timing())
        timer.add_timepoint();  // [1]-st time point is the end of input
    
    // detect overlaps
    unsigned int totalcounts = detect_overlaps();
    if (!errors().ok())
        return false;
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
    if (!errors().ok())
        return false;
        
    return true;    // all is fine
}

// Serial implementation of Multovl
unsigned int Pipeline::detect_overlaps()
{
    // the overlaps are detected chromosome by chromosome
    unsigned int totalcounts = 0;
    for (chrom_multovl_map::iterator cmit = _cmovl.begin();
        cmit != _cmovl.end(); ++cmit)
    {
        const std::string& chrom = cmit->first;    // "current chromosome"
        MultiOverlap& movl = cmit->second;      // "current overlap"
        
        // generate and store overlaps
        if (opt_ptr()->uniregion())
        {
            totalcounts += movl.find_unionoverlaps(opt_ptr()->ovlen(), 
                opt_ptr()->minmult(), opt_ptr()->maxmult());
        }
        else
        {
            totalcounts += movl.find_overlaps(opt_ptr()->ovlen(), 
                opt_ptr()->minmult(), opt_ptr()->maxmult(), !opt_ptr()->nointrack());
        }
    }
    return totalcounts;
}

}   // namespace multovl
