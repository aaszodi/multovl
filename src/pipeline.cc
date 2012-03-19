// == MODULE pipeline.cc ==

// -- Own headers --

#include "pipeline.hh"
#include "multioverlap.hh"
#include "multovlopts.hh"

// -- Standard headers --

// == Implementation ==

namespace multovl {

// Serial implementation of Multovl
unsigned int Pipeline::detect_overlaps()
{
    // the overlaps are detected chromosome by chromosome
    unsigned int totalcounts = 0;
    for (chrom_multovl_map::iterator cmit = cmovl().begin();
        cmit != cmovl().end(); ++cmit)
    {
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
