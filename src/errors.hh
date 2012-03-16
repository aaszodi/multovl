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
