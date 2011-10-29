#ifndef MULTOVL_FILEIO_HEADER
#define MULTOVL_FILEIO_HEADER

// == HEADER fileio.hh ==

/** \file 
 * \brief Classes to read input files and to write output.
 * \author Andras Aszodi
 * \date 2011-06-15.
 */

// -- Standard headers --

#include <fstream>
#include <string>
#include <vector>
#include <map>

// -- Boost headers --

#include "boost/noncopyable.hpp"

// -- Own headers --

#include "fileformat.hh"
#include "errors.hh"

namespace multovl {

class Region;

namespace io {

class TrackReader;

// -- Input --

/// FileReader objects implement reading regions from a track file.
/// Clients should instantiate a FileReader with a file name,
/// then invoke its read_into() method repeatedly,
/// and use the resulting Region immediately for building up a MultiOverlap object.
class FileReader: private boost::noncopyable
{
    public:
    
    /// Init a FileReader object to read from a file.
    /// \param infname the input file name. 
    /// \param format the file format if known.
    /// By default or if UNKNOWN is specified, then the format will be deduced automatically,
    ///     see the Fileformat class for details.
    explicit FileReader(
        const std::string& infname,
        Fileformat::Kind format = Fileformat::UNKNOWN);
    
    /// Attempts to read from the wrapped input file into a region.
    /// Precondition: state() must be NO_ERROR when this method is invoked.
    /// \param chrom string to store the chromosome name for /reg/
    /// \param reg the region this method tries to read into.
    /// \return /true/ if all went well, /false/ on errors.
    ///     For details, invoke errors() and finished().
    bool read_into(std::string& chrom, Region& reg);

    /// \return true if all input has been squeezed out of the input file.
    bool finished() const { return _finished; }
    
    /// \return const access to the internal error collecting object
    const Errors& errors() const { return _errors; }
    
    ~FileReader();
    
    private:
    
    TrackReader* _reader;   // pimpl
    Errors _errors;
    bool _finished;
    FileReader();   // no default ctor
    
};  // END OF CLASS FileReader

// -- Output --

// NOTE: Implement a FileWriter if you want to let the user select an output format.
// Until then, just use the GFF2-based TextWriter.

}   // namespace io
}   // namespace multovl

#endif  // MULTOVL_FILEIO_HEADER
