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

#include "linewriter.hh"
#include "region.hh"
#include "multiregion.hh"

// -- Implementation --

namespace multovl {
namespace io {

// -- Linewriter methods --

Linewriter::Linewriter(const std::string& chrom):
	_chr(chrom)
{
	reset();
}

// -- BedLinewriter methods --

std::string BedLinewriter::write(const Region& reg)
{
	reset();
	_ostr << _chr << '\t' << reg.first() << '\t' << reg.last()
	    << '\t' << reg.name() << "\t0\t" << reg.strand();
	return (_ostr.good()? _ostr.str(): "");
}

std::string BedLinewriter::write(const MultiRegion& reg)
{
	reset();
	_ostr << _chr << '\t' << reg.first() << '\t' << reg.last()
	    << '\t' << reg.anc_str() << '\t'
	    << reg.multiplicity() << '\t' << reg.strand();
	return (_ostr.good()? _ostr.str(): "");
}

// -- GffLinewriter methods --

GffLinewriter::GffLinewriter(const std::string& source, unsigned int version, 
    	const std::string& chrom):
	Linewriter(chrom), _source(source)
{
	if (version <= 2) 
	{
	    _version = 2; _sep = ' ';
    } else {
	    _version = 3; _sep = '=';
    }
}

std::string GffLinewriter::write(const Region& reg)
{
	reset();
	
	_ostr << _chr << '\t' << source() << '\t' << reg.name()
		<< '\t' << reg.first() << '\t' << reg.last()
		<< "\t.\t" << reg.strand();
	return (_ostr.good()? _ostr.str(): "");
}

std::string GffLinewriter::write(const MultiRegion& reg)
{
	reset();
	
	_ostr << _chr << '\t' << source() << '\t' << reg.name()
		<< '\t' << reg.first() << '\t' << reg.last()
		<< '\t' << reg.multiplicity()
		<< '\t' << reg.strand()
		<< "\t."    // no frame info
		<< "\tANCESTORS" << _sep << reg.anc_str();
	return (_ostr.good()? _ostr.str(): "");
}

}   // namespace io
}   // namespace multovl
