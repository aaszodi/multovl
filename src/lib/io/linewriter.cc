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
// == MODULE linewriter.cc ==

// -- Library headers --

#include "multovl/io/linewriter.hh"
#include "multovl/region.hh"
#include "multovl/multiregion.hh"

// -- Implementation --

namespace multovl {
namespace io {

// -- BedLinewriter methods --

std::string BedLinewriter::write(const BaseRegion& reg) const
{
    return write_line(
        reg.first(), reg.last(),
        reg.name(), 0, reg.strand()
    );
}

std::string BedLinewriter::write(const MultiRegion& reg) const
{
    return write_line(
        reg.first(), reg.last(),
        reg.anc_str(), reg.multiplicity(),
        reg.strand()
    );
}

// Constructs and returns a BED-formatted line. Private
std::string BedLinewriter::write_line(
    unsigned int first, unsigned int last,
    const std::string& name, unsigned int multiplicity,
    char strand
) const {
    // avoid temporaries... could have been implemented with 
    // chained `.append()` calls as well
    std::string line = chrom();
    line += '\t';
    line += std::to_string(first);
    line += '\t';
    line += std::to_string(last);
    line += '\t';
    line += name;
    line += '\t';
    line += std::to_string(multiplicity);
    line += '\t';
    line += strand;
    return line;
}

// -- GffLinewriter methods --

GffLinewriter::GffLinewriter(const std::string& source, unsigned int version, 
        const std::string& chrom):
    Linewriter(chrom), _source(source)
{
    if (version <= 2) {
        _version = 2; _sep = ' ';
    } else {
        _version = 3; _sep = '=';
    }
}

std::string GffLinewriter::write(const BaseRegion& reg) const
{
    return write_line(
        source(), reg.name(),
        reg.first(), reg.last(),
        0, reg.strand(), ""
    );
}

std::string GffLinewriter::write(const MultiRegion& reg) const
{
    return write_line(
        source(), reg.name(),
        reg.first(), reg.last(),
        reg.multiplicity(), reg.strand(),
        reg.anc_str()
    );
}

// Constructs and returns a GFF-formatted line. Private
std::string GffLinewriter::write_line(
    const std::string& source, const std::string& name,
    unsigned int first, unsigned int last,
    unsigned int multiplicity, char strand,
    const std::string& ancestors
) const {
    // avoid temporaries... could have been implemented with 
    // chained `.append()` calls as well
    std::string line = chrom();
    line += '\t';
    line += source;
    line += '\t';
    line += name;
    line += '\t';
    line += std::to_string(first);
    line += '\t';
    line += std::to_string(last);
    line += '\t';
    line += (multiplicity? std::to_string(multiplicity): ".");
    line += '\t';
    line += strand;
    if (ancestors == "") {
        // BaseRegion and children
        line += "\t.\t.";   // no frame, no group info
    } else {
        // MultiRegion
        line += "\t.\tANCESTORS";
        line += _sep;
        line += ancestors;
    }
    return line;
}

}   // namespace io
}   // namespace multovl
