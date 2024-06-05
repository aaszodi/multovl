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

// -- Own headers --

#include "multovl/baseregion.hh"
#include "multovl/errors.hh"

namespace multovl {
namespace io {

// -- Input --

/// Abstract base class for reading regions from a file.
/// Subclasses contain implementations for reading text or binary files.
/// TrackReader objects are non-copyable.
class TrackReader
{
public:
    
    /// default init
    TrackReader(): _errors() {}
    
    // non-copyable
    TrackReader(const TrackReader&) = delete;
    TrackReader& operator=(const TrackReader&) = delete;
    
    /// Attempts to read from the wrapped input file into a base region.
    /// \param chrom string to store the chromosome name for /reg/
    /// \param reg the region this method tries to read into.
    /// \return message containing information about how the parsing went.
    ///     "" if all is OK, "EOF" if the internal stream was exhausted,
    ///     or some error message.
    virtual
    std::string read_into(std::string& chrom, BaseRegion& reg) = 0;

    /// \return const access to the internal error collecting object.
    const Errors& errors() const { return _errors; }
    
    // Adds a simple error message to _errors.
    virtual
    void add_error(const std::string& msg)
    {
        _errors.add_error(msg);
    }

    virtual
    ~TrackReader() = default;
    
private:
    
    Errors _errors;

};  // END OF CLASS TrackReader

// -- Output --

// NOTE: necessary only if various output formats are used.

}   // namespace io
}   // namespace multovl

#endif // MULTOVL_TRACKIO_HEADER
