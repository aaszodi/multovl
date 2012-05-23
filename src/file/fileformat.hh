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
