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
// == MODULE fileio.cc ==

// -- Own headers --

#include "fileio.hh"
#include "textio.hh"

#if USE_BAMTOOLS
#include "bamio.hh"
#endif

#include "multioverlap.hh"

// == Implementation ==

namespace multovl {
namespace io {

FileReader::FileReader(
    const std::string& infname,
    Fileformat::Kind format
):
    _reader(NULL),
    _finished(false)
{
    // figure out the file format
    Fileformat::Kind fmt = format;
    if (format == Fileformat::UNKNOWN)
         fmt = Fileformat::from_filename(infname);

    // select the appropriate track reader (factory pattern)
#if USE_BAMTOOLS
    if (fmt == Fileformat::BAM)
        _reader = new BamReader(infname);
    else
#endif
        _reader = new TextReader(infname, fmt);

    // maybe something went wrong...
    if (!errors().ok())
        _finished = true;
}

FileReader::~FileReader()
{
    if (_reader != NULL)
    {
        delete _reader;
        _reader = NULL;
    }
}

bool FileReader::read_into(std::string& chrom, Region& reg)
{
    if (finished()) return true;
    
    std::string msg = _reader->read_into(chrom, reg);
    if (msg != "")
    {
        if (msg == "EOF")
        {
            _finished = true;
            return true;
        }
        return false;
    }
    return true;
}

const Errors& FileReader::errors() const { return _reader->errors(); }
void FileReader::add_error(const std::string& msg) { _reader->add_error(msg); }

}   // namespace io
}   // namespace multovl
