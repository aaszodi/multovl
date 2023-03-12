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
/// The class is non-copyable.
class FileReader
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
    
    // Non-copyable class
    FileReader(const FileReader&) = delete;
    FileReader& operator=(const FileReader&) = delete;
    
    /// Attempts to read from the wrapped input file into a region.
    /// We keep reading the file so that the user gets all problems in one go.
    /// \param chrom string to store the chromosome name for /reg/
    /// \param reg the region this method tries to read into.
    /// \return /true/ if all went well, /false/ on errors.
    ///     For details, invoke errors() and finished().
    bool read_into(std::string& chrom, Region& reg);

    /// \return true if all input has been squeezed out of the input file.
    bool finished() const { return _finished; }
    
    /// \return const access to the reader implementation's error collection object
    const Errors& errors() const;
    
    ~FileReader();
    
    private:
    
    void add_error(const std::string& msg);
    
    TrackReader* _reader;   // pimpl
    bool _finished;
    FileReader();   // no default ctor
    
};  // END OF CLASS FileReader

// -- Output --

// NOTE: Implement a FileWriter if you want to let the user select an output format.
// Until then, just use the GFF2-based TextWriter.

}   // namespace io
}   // namespace multovl

#endif  // MULTOVL_FILEIO_HEADER
