#ifndef MULTOVL_TEXTIO_HEADER
#define MULTOVL_TEXTIO_HEADER

// == HEADER textio.hh ==

/** \file 
 * \brief Classes to read ASCII text input streams (or files) and (later) to write text output.
 * \author Andras Aszodi
 * \date 2011-06-06.
 */

// -- Standard headers --

#include <string>
#include <vector>

// -- Library headers --

#include "trackio.hh"
#include "fileformat.hh"

namespace multovl {
namespace io {

class Linereader;

/// TextReader objects encapsulate an input stream from which they can read
/// into a Reader object, one by one. TextReader-s are init-ed with a file name,
/// the ctor opens the file for writing and the dtor closes it, RAII-style.
/// Clients should instantiate a TextReader, then invoke its read_into() method,
/// and use the resulting Region immediately for building up a MultiOverlap object.
class TextReader: public TrackReader
{
    public:
    
    /// Init a TextReader object to read from a file.
    /// \param infname the input file name. 
    /// \param format the format of the input file (must be known)
    TextReader(
        const std::string& infname,
        Fileformat::Kind format);
    
    /// Attempts to read from the wrapped input stream into a region.
    /// Reads the stream internally line-by-line, parses each line
    /// until a valid data line is found with which /reg/ is then updated.
    /// Comment and metainfo lines are ignored.
    /// \param chrom string to store the chromosome name for /reg/
    /// \param reg the region this method tries to read into.
    /// \return message containing information about how the parsing went.
    ///     "" if all is OK, "EOF" if the internal stream was exhausted,
    ///     or some error message.
    virtual
    std::string read_into(std::string& chrom, Region& reg);

    ~TextReader();
    
    protected:
    
    virtual
    void add_error(const std::string& msg);
    
    private:
    
    Linereader* _lrp;   // can be BedLinereader or GffLinereader
    std::ifstream _inf; // input file stream
    std::string _line;  // input buffer
    unsigned int _linecount;
    
};  // END OF CLASS TextReader

// -- Output --

// Add code here if you want to do more complex output than the classic
// "write-GFF2-to-stdout" approach.

}   // namespace io
}   // namespace multovl

#endif  // MULTOVL_TEXTIO_HEADER