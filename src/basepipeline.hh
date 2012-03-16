#ifndef MULTOVL_BASEPIPELINE_HEADER
#define MULTOVL_BASEPIPELINE_HEADER

// == Header basepipeline.hh ==

/// \file Base class for multiple overlap "pipelines". 
/// \date 2012-02-27
/// \author Andras Aszodi

// -- Own headers --

#include "errors.hh"

// -- Boost headers --

#include "boost/noncopyable.hpp"

// -- Standard headers --

#include <string>

namespace multovl {

/// Fwd declaration for command-line option processing
class MultovlOptbase;

// -- Classes --

/// Abstract class that provides a framework for detecting multiple overlaps.
/// Running a Multovl program involves the following steps: read input, do the work, write output.
/// The Pipeline subclasses implement variations on this theme, such as reading from files vs databases,
/// running the Multovl algorithm on one CPU or in parallel, estimating the probabilities of random overlaps etc.
/// The BasePipeline class lays the foundation by providing a means of storing input track metadata,
/// error handling and a run() method with optional timing information.
class BasePipeline: private boost::noncopyable
{
public:
    
    /// Go through the whole analysis.
    /// \return true on success, false if something went wrong.
    bool run();
    
    /// \return const access to the internal error/warning object.
    const Errors& errors() const { return _errors; }
    
    virtual
    ~BasePipeline() {}
    
    /// The Input structure stores the name of the input (a filename 
    /// or track name in a database), an ID assigned by Multovl,
    /// and the number of regions that could be read from it.
    struct Input
    {
        std::string name;
        unsigned int trackid, regcnt;
        
        explicit Input(const std::string& nm = ""): 
            name(nm), trackid(0), regcnt(0)
        {}
    };
    
protected:
    
    typedef std::vector<Input> input_vec;

    /// Reads the input tracks. Pure virtual.
    /// \return the number of tracks successfully read, 0 on error.
    virtual
    unsigned int read_input() = 0;
    
    /// Detects the overlaps. Pure virtual.
    /// \return the total number of overlaps found.
    virtual
    unsigned int detect_overlaps() = 0;
    
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
    Errors _errors; ///< collect error messages here
};

}   // namespace multovl

#endif  // MULTOVL_BASEPIPELINE_HEADER
