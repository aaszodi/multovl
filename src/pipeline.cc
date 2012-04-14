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
        if (opt_baseptr()->uniregion())
        {
            totalcounts += movl.find_unionoverlaps(opt_baseptr()->ovlen(), 
                opt_baseptr()->minmult(), opt_baseptr()->maxmult());
        }
        else
        {
            totalcounts += movl.find_overlaps(opt_baseptr()->ovlen(), 
                opt_baseptr()->minmult(), opt_baseptr()->maxmult(), !opt_baseptr()->nointrack());
        }
    }
    return totalcounts;
}

}   // namespace multovl
