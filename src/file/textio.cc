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
// == MODULE textio.cc ==

// -- Own headers --

#include "textio.hh"
#include "linereader.hh"

// -- Standard library --

#include <sstream>
#include <fstream>
#include <stdexcept>

// -- Boost headers --

#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"

// == Implementation ==

namespace multovl {
namespace io {

// -- TextReader methods --

TextReader::TextReader(
    const std::string& infname,
    Fileformat::Kind format
):
    TrackReader(),
    _lrp(NULL),
    _inf(),
    _linecount(0),
    _valid(false)
{
    
    // input line reader (parser) factory
    if (format == Fileformat::BED) _lrp = new BedLinereader();
    else if (format == Fileformat::GFF) _lrp = new GffLinereader();
    else
    {
        // invalid state
    	// use parent class' add_error as there's no meaningful linecount to store
        TrackReader::add_error(
            infname + ": Cannot deduce format from extension: use " + 
            Fileformat::known_extensions()
        );
        return;
    }
    
    _inf.open(infname.c_str());
    if (!_inf)  // could not open
    {
    	// invalid state
        TrackReader::add_error("Cannot open input file: " + infname);
        return;
    }

    // all went well, state is valid, can do input
    _valid = true;
}

std::string TextReader::read_into(std::string& chrom, BaseRegion& reg)
{
    if (!is_valid())
        return "Cannot read";
        
    while (std::getline(_inf, _line))
    {
        ++_linecount;
        Linereader::Status status = _lrp->parse(_line);
        if (status & Linereader::ERROR)
        {
            add_error(_lrp->error_msg());
            return errors().last_error();
        }
        if (status & Linereader::DATA)
        {
            bool parseok = _lrp->read_into(reg);
            if (parseok)
            {
                chrom = _lrp->chrom();
                return "";    // success
            }
            else
            {
                // shouldn't really happen...
                add_error(_lrp->error_msg());
                return errors().last_error();
            }
        }
        // anything else (comments etc.) get ignored, keep reading...
    }
    return "EOF";
}

TextReader::~TextReader()
{
    _inf.close();
    if (_lrp != nullptr)
    {
        delete _lrp;
        _lrp = nullptr;
    }
}

void TextReader::add_error(const std::string& msg)
{
    std::string err = 
        "At line " + boost::lexical_cast<std::string>(_linecount) + ": " + msg;
    TrackReader::add_error(err);
}

}   // namespace io
}   // namespace multovl
