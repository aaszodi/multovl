#ifndef MULTOVL_FILEFORMAT_HEADER
#define MULTOVL_FILEFORMAT_HEADER

// == HEADER fileformat.hh ==

/** \file 
 * \brief Module to handle file format recognition.
 * \author Andras Aszodi
 * \date 2011-06-15.
 */

// -- Standard headers --

#include <string>

namespace multovl {
namespace io {

/// The Fileformat class encapsulates file format handling.
/// It provides the Kind enum for the format types it knows about,
/// and some static methods to deduce these types.
class Fileformat
{
    public:
    
    enum Kind {
        UNKNOWN = 0,
        BED = 1,    // these are ASCII...
        GFF2 = 2,
        GFF3 = 4,   // also GTF
        GFF = 6, // GFF2 | GFF3
        BAM = 8     // this is binary
    };

    /// Deduces the format from the filename extension.
    /// \param filenm the filename. The extension should be ".bed", ".gff", ".bam", ... etc.
    /// \return one of the non-0 Fileformat::Kind constants or Fileformat::Kind::UNKNOWN
    /// if the extension did not match anything this method knows about.
    static
    Kind from_filename(const std::string& filenm);

    /// Converts the format const to a string
    /// \param format one of the valid formats
    /// \return the string representation of /format/
    static
    std::string to_string(Kind format);
    
    private:
    
    static
    Kind from_string(const std::string& fstr);
};  // class Fileformat

}   // namespace io
}   // namespace multovl

#endif  // MULTOVL_FILEFORMAT_HEADER
