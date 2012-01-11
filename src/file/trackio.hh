#ifndef MULTOVL_TRACKIO_HEADER
#define MULTOVL_TRACKIO_HEADER

// == HEADER trackio.hh ==

/** \file 
 * \brief Base classes to read/write input/output tracks (collections of regions).
 * \author Andras Aszodi
 * \date 2011-06-15.
 */

// -- Standard headers --

#include <fstream>
#include <string>

// -- Boost headers --

#include "boost/noncopyable.hpp"

// -- Own headers --

#include "errors.hh"

namespace multovl {

class Region;

namespace io {

// -- Input --

/// Abstract base class for reading regions from a file.
/// Subclasses contain implementations for reading text or binary files.
class TrackReader: private boost::noncopyable
{
    public:
    
    /// default init
    TrackReader(): _errors() {}
    
    /// Attempts to read from the wrapped input file into a region.
    /// \param chrom string to store the chromosome name for /reg/
    /// \param reg the region this method tries to read into.
    /// \return message containing information about how the parsing went.
    ///     "" if all is OK, "EOF" if the internal stream was exhausted,
    ///     or some error message.
    virtual
    std::string read_into(std::string& chrom, Region& reg) = 0;

    /// \return const access to the internal error collecting object.
    const Errors& errors() const { return _errors; }
    
    /// Add an error
    virtual
    void add_error(const std::string& msg) = 0;
    
    virtual
    ~TrackReader() {}
    
    protected:
    
    Errors _errors;

};  // END OF CLASS TrackReader

// -- Output --

// NOTE: necessary only if various output formats are used.

}   // namespace io
}   // namespace multovl

#endif // MULTOVL_TRACKIO_HEADER
