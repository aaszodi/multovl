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
