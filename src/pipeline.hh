#ifndef MULTOVL_PIPELINE_HEADER
#define MULTOVL_PIPELINE_HEADER

// == Header pipeline.hh ==

// -- Own headers --

#include "errors.hh"
#include "multioverlap.hh"

// -- Boost headers --

#include "boost/noncopyable.hpp"

// -- Standard headers --

#include <map>
#include <string>

namespace multovl {

class MultovlOptbase;

// -- Classes --

/// Abstract class that provides a framework for detecting multiple overlaps.
class Pipeline: private boost::noncopyable
{
    public:
    
    /// Go through the whole analysis.
    /// \return true on success, false if something went wrong.
    bool run();
    
    /// \return const access to the internal error/warning object.
    const Errors& errors() const { return _errors; }
    
    virtual
    ~Pipeline() {}
    
    protected:
    
    /// The Input structure stores the name of the input (a filename 
    /// or track name in a database), an ID assigned by Multovl,
    /// and the number of regions that could be read from it.
    struct Input
    {
        std::string name;
        unsigned int trackid, regcnt;
        
        explicit Input(const std::string& nm): 
            name(nm), trackid(0), regcnt(0)
        {}
    };
    
    typedef std::vector<Input> input_vec;
    typedef std::map<std::string, MultiOverlap> chrom_multovl_map;

    /// Reads the input tracks. Pure virtual.
    /// Implementations must make sure that the internal chromosome => MultiOverlap map
    /// is properly set up after all input data has been processed.
    /// \return the number of tracks successfully read, 0 on error.
    virtual
    unsigned int read_input() = 0;
    
    /// Detects the overlaps.
    /// The default implementation provided uses 1 CPU.
    /// Parallel versions may provide specialized implementations of this method.
    /// \return the total number of overlaps found, including solitary regions.
    virtual
    unsigned int detect_overlaps();
    
    /// Writes the results somewhere. Pure virtual.
    /// Implementations must make sure each overlap region and the corresponding
    /// ancestry information are properly written.
    /// This method is not run when timing is requested.
    /// \return true if all went well, false on errors.
    virtual
    bool write_output() = 0;
    
    /// Base-class sliced access to an option-handling object.
    /// Derived classes will use their own option-handling,
    /// it should be derived from MultovlOptbase.
    virtual
    MultovlOptbase* opt_ptr() = 0;
    
    // data which should be accessible
    // to derived classes without too much bureaucracy
    input_vec _inputs;      ///< vector of input track descriptors
    chrom_multovl_map _cmovl;   ///< chromosome ==> MultiOverlap map
    Errors _errors; ///< collect error messages here
};

}   // namespace multovl

#endif  // MULTOVL_PIPELINE_HEADER
