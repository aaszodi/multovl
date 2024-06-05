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
// == MODULE bamio.cc ==

// -- Own headers --

#include "multovl/io/bamio.hh"

// == Implementation ==

namespace multovl {
namespace io {

// -- BamReader methods --

BamReader::BamReader(const std::string& infname):
    TrackReader(),
    _bamreader(),
    _refs()
{
    
    // open the BAM reader
    if (!_bamreader.Open(infname))  // could not open
    {
        add_error(_bamreader.GetErrorString());
        add_error("Cannot open input BAM file: " + infname);
    }
    
    // get the reference sequence data (i.e. the chromosomes)
    _refs = _bamreader.GetReferenceData();
    if (_refs.size() == 0)  // no data
    {
        add_error("Cannot read data from input BAM file: " + infname);
    }
}

std::string BamReader::read_into(std::string& chrom, BaseRegion& reg)
{
    if (!_bamreader.IsOpen())
        return "Input file not open";
        
    bool ok = _bamreader.GetNextAlignmentCore(_albuf);
    if (!ok)
        return "EOF";   // hopefully...

    // chromosome is the reference sequence
    chrom = _refs.at(_albuf.RefID).RefName;
    
    // coordinates
    int32_t first = _albuf.Position;
    int32_t last = _albuf.GetEndPosition(); // BamTools V2: 0-based, half-open
    char strand = _albuf.IsReverseStrand()? '-': '+';
    
    reg.set_coords(first, last);
    reg.strand(strand);
	reg.name("bam");    // we don't read it from the BAM, although we could
    return "";  // OK
}

BamReader::~BamReader()
{
    _bamreader.Close();
}

}   // namespace io
}   // namespace multovl
