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

#include "multovl/io/trackio.hh"
#include "multovl/io/fileformat.hh"
#include "multovl/baseregion.hh"

// -- BAM tools --

#include "api/BamReader.h"
#include "api/BamAux.h"

namespace multovl {
namespace io {

/// BamReader objects encapsulate an input file from which they can read
/// into a BaseRegion object, one by one. BamReader-s are init-ed with a file name,
/// the ctor opens the file for writing and the dtor closes it, RAII-style.
/// Clients should instantiate a BamReader, then invoke its read_into() method,
/// and use the resulting BaseRegion immediately for building up a MultiOverlap object.
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
    std::string read_into(std::string& chrom, BaseRegion& reg);

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
