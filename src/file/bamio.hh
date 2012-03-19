#ifndef MULTOVL_BAMIO_HEADER
#define MULTOVL_BAMIO_HEADER

// == HEADER bamio.hh ==

/** \file 
 * \brief Classes to read BAM-formatted binary files
 * and maybe later to write them, too.
 * \author Andras Aszodi
 * \date 2011-06-20.
 */

// -- Standard headers --

#include <string>
#include <vector>

// -- Library headers --

#include "trackio.hh"
#include "fileformat.hh"

// -- BAM tools --

#include "api/BamReader.h"
#include "api/BamAux.h"

namespace multovl {
namespace io {

/// BamReader objects encapsulate an input file from which they can read
/// into a Region object, one by one. BamReader-s are init-ed with a file name,
/// the ctor opens the file for writing and the dtor closes it, RAII-style.
/// Clients should instantiate a BamReader, then invoke its read_into() method,
/// and use the resulting Region immediately for building up a MultiOverlap object.
class BamReader: public TrackReader
{
public:
    
    /// Init a BamReader object to read from a BAM-formatted binary file.
    /// \param infname the input file name. 
    explicit BamReader(const std::string& infname);
    
    /// Attempts to read from the wrapped input stream into a region.
    /// Reads the stream internally record-by-record.
    /// \param chrom string to store the chromosome name for /reg/
    /// \param reg the region this method tries to read into.
    /// \return message containing information about how the parsing went.
    ///     "" if all is OK, "EOF" if the internal stream was exhausted,
    ///     or some error message.
    virtual
    std::string read_into(std::string& chrom, Region& reg);

    ~BamReader();
    
private:
    
    BamTools::BamReader _bamreader;
    BamTools::RefVector _refs;
    BamTools::BamAlignment _albuf;  // BAM aligned region input buffer
    
};  // END OF CLASS BamReader

// -- Output --

// Add code here if you want to do BAM-formatted output. Not implemented yet.

}   // namespace io
}   // namespace multovl

#endif  // MULTOVL_BAMIO_HEADER
