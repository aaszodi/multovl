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
    std::string read_into(std::string& chrom, BaseRegion& reg);

    virtual
    ~TextReader();
    
    /// Adds an error message to _errors, nicely decorated with linecount.
    virtual
    void add_error(const std::string& msg);
    
private:
    
    bool is_valid() const { return _valid; }

    Linereader* _lrp;   // can be BedLinereader or GffLinereader
    std::ifstream _inf; // input file stream
    std::string _line;  // input buffer
    unsigned int _linecount;
    bool _valid;
    
};  // END OF CLASS TextReader

// -- Output --

// Add code here if you want to do more complex output than the classic
// "write-GFF2-to-stdout" approach.

}   // namespace io
}   // namespace multovl

#endif  // MULTOVL_TEXTIO_HEADER
