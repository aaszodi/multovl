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
// == MODULE fileformat.cc ==

// -- Own header --

#include "fileformat.hh"

// -- Boost headers --

#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

// == Implementation ==

namespace multovl {
namespace io {

Fileformat::Kind Fileformat::from_filename(const std::string& filenm)
{
    boost::filesystem::path filepath(filenm);
    
    // before Boost 1.36, extension() was a standalone function
    std::string ext =
    #if BOOST_VERSION_MAJOR == 1 && BOOST_VERSION_MINOR <= 35
        boost::filesystem::extension(filepath);
    #elif BOOST_FILESYSTEM_VERSION == 3
        // default from V1.45 on...
        filepath.extension().string();
    #else
        // Version 2, will go away in 1.48
        filepath.extension();
    #endif

    if (ext[0] == '.')
        return from_string(ext.substr(1));
    else
        return UNKNOWN;
}

std::string Fileformat::to_string(Fileformat::Kind format)
{
    if (format == GFF)
        return "GFF";
    else if (format == BED)
        return "BED";
    else if (format == BAM)
        return "BAM";
    else
        return "UNKNOWN";
}

std::string Fileformat::known_extensions()
{
    std::string extensions = ".bed, .gff, .gtf, .bam";
    return extensions;
}

// this is private
Fileformat::Kind Fileformat::from_string(const std::string& fstr)
{
    std::string formstr = boost::algorithm::to_lower_copy(fstr);
    if (formstr == "gff" || formstr == "gtf")
        return GFF;
    else if (formstr == "bed")
        return BED;
    else if (formstr == "bam")
        return BAM;
    else
        return UNKNOWN;
}

}   // namespace io
}   // namespace multovl
