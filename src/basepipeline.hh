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
#ifndef MULTOVL_BASEPIPELINE_HEADER
#define MULTOVL_BASEPIPELINE_HEADER

// == Header basepipeline.hh ==

/// \file Base class for multiple overlap "pipelines". 
/// \date 2012-02-27
/// \author Andras Aszodi

// -- Own headers --

#include "errors.hh"
#include "multovlopts.hh"

// -- Boost headers --

// -- Standard headers --

#include <string>
#include <memory>

namespace multovl {

// -- Classes --

/// Abstract class that provides a framework for detecting multiple overlaps.
/// Running a Multovl program involves the following steps: read input, do the work, write output.
/// The Pipeline subclasses implement variations on this theme, such as reading from files vs databases,
/// running the Multovl algorithm on one CPU or in parallel, estimating the probabilities of random overlaps etc.
/// The BasePipeline class lays the foundation by providing a means of storing input track metadata,
/// error handling and a run() method with optional timing information.
/// BasePipeline objects are non-copyable.
class BasePipeline
{
public:
    
    /// Default init of a BasePipeline object.
    BasePipeline():
        _optpimpl(),    // derived classes allocate this
        _inputs(),
        _errors()
    {}
    
    // non-copyable
    BasePipeline(const BasePipeline&) = delete;
    BasePipeline& operator=(const BasePipeline&) = delete;
    
    /// Go through the whole analysis.
    /// \return true on success, false if something went wrong.
    bool run();
    
    /// \return const access to the internal error/warning object.
    const Errors& errors() const { return _errors; }
    
    virtual
    ~BasePipeline() = default;
    
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
    
    typedef std::unique_ptr<MultovlOptbase> optptr_t;
    typedef std::vector<Input> input_seq_t;

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
    
    /// Sets the option handling object (invoke in ctor) 
    /// if it was not initialised.
    /// \param optp pointer to an appropriate option handling object instance.
    /// \return /true/ on success, /false/ if opt_ptr() != NULL.
    bool set_optpimpl(MultovlOptbase* optp);
    
    /// \return non-const access to the option-handling pointer itself
    optptr_t& opt_pimpl() { return _optpimpl; }
    
    /// \return correctly typed non-const pointer to the option-handling object.
    virtual
    MultovlOptbase* opt_ptr() = 0;
    
    /// \return const access to the input records
    const input_seq_t& inputs() const { return _inputs; }

    /// \return non-const access to the input records
    input_seq_t& inputs() { return _inputs; }

    /// Adds an error message
    void add_error(const std::string& prefix,
    		const std::string& what) { _errors.add_error(prefix + ": " + what); }

    /// Adds the full contents of another Errors object
    /// \param other another Errors object to be merged
    void add_all_errors(const Errors& other) { _errors += other; }

    /// Adds a warning message
    void add_warning(const std::string& prefix,
    		const std::string& what) { _errors.add_warning(prefix + ": " + what); }

    /// Clears the errors
    void clear_errors() { _errors.clear(); }

private:

    // Each derived class will have its option-handling object
    // which is derived from MultovlOptbase (parallel inheritance hierarchy). 
    // The ptr to it remains NULL in this base class.
    optptr_t _optpimpl;
    input_seq_t _inputs;      ///< vector of input track descriptors
    Errors _errors; ///< collect error messages here
};

}   // namespace multovl

#endif  // MULTOVL_BASEPIPELINE_HEADER
