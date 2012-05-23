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
#ifndef MULTOVL_ERRORS_HEADER
#define MULTOVL_ERRORS_HEADER

// == Header errors.hh ==

// -- Standard headers --

#include <string>
#include <vector>
#include <iostream>

namespace multovl {

/// Simple facility to keep track of errors and warnings.
class Errors
{
public:
    
    /// Init the Errors object.
    /// \param errprefix string to prefix the error messages with, default "ERROR"
    /// \param warnprefix string to prefix the warning messages with, default "WARNING"
    explicit Errors(
        const std::string& errprefix = "ERROR",
        const std::string& warnprefix = "WARNING"
    );
    
    /// Adds an error message to the calling object.
    /// \param msg the error message. Does nothing if /msg/ is empty.
    void add_error(const std::string& msg);
    
    /// Adds a warning to the calling object.
    /// \param msg the warning message. Does nothing if /msg/ is empty.
    void add_warning(const std::string& msg);
    
    /// Adds the content of another Errors object to the calling object.
    /// \param another Error object.
    /// \return the calling object.
    Errors& operator+=(const Errors& another);
    
    /// \return the number of errors seen so far.
    unsigned int error_count() const { return _errors.size(); }
    
    /// \return the number of warnings seen so far.
    unsigned int warning_count() const { return _warnings.size(); }
    
    /// \return true if there have been no errors. There may have been warnings, though.
    bool ok() const { return (error_count() == 0); }
    
    /// \return true if there have been neither errors nor warnings. Isn't that nice? :-)
    bool perfect() const { return (error_count() + warning_count() == 0); }
    
    /// \return the last error encountered or the empty string if everything was fine.
    std::string last_error() const { return (ok()? "" : _errors.back()); }
    
    /// Prints the messages to an output stream, one per line.
    /// \param outf the output stream
    /// \param warnings if true, then print the warnings as well (this is the default)
    std::ostream& print(std::ostream& outf, bool warnings=true) const;
    
    /// Tell the calling object to forget all errors and warnings seen so far.
    void clear();
    
private:
    
    std::vector<std::string> _errors, _warnings;
    std::string _errprefix, _warnprefix;
};

}   // namespace multovl

#endif  // MULTOVL_ERRORS_HEADER
