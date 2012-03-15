#ifndef MULTOVL_PIPELINE_HEADER
#define MULTOVL_PIPELINE_HEADER

// == Header pipeline.hh ==

// -- Own headers --

#include "basepipeline.hh"
#include "multioverlap.hh"

// -- Boost headers --

// -- Standard headers --

#include <map>
#include <string>

namespace multovl {

// -- Classes --

/// Abstract class that provides a framework for detecting multiple overlaps using the MultiOverlap class.
class Pipeline: public BasePipeline
{
    public:
    
    protected:
    
    /// There is one MultiOverlap object for each chromosome
    typedef std::map<std::string, MultiOverlap> chrom_multovl_map;

    /// Detects the overlaps.
    /// The default implementation provided here uses 1 CPU core.
    /// \return the total number of overlaps found, including solitary regions.
    virtual
    unsigned int detect_overlaps();
    
    // data which should be accessible
    // to derived classes without too much bureaucracy
    chrom_multovl_map _cmovl;   ///< chromosome ==> MultiOverlap map
};

}   // namespace multovl

#endif  // MULTOVL_PIPELINE_HEADER
