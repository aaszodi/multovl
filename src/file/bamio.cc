// == MODULE bamio.cc ==

// -- Own headers --

#include "bamio.hh"
#include "region.hh"

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
        _errors.add_error("Cannot open input file: " + infname);
    }
    
    // get the reference sequence data (i.e. the chromosomes)
    _refs = _bamreader.GetReferenceData();
}

std::string BamReader::read_into(std::string& chrom, Region& reg)
{
    if (!errors().ok())
        return errors().last_error();   // already in bad shape...
    if (!_bamreader.IsOpen())
        return "Input file not open";
        
    bool ok = _bamreader.GetNextAlignmentCore(_albuf);
    if (!ok)
        return "EOF";   // hopefully...

    // chromosome is the reference sequence
    chrom = _refs.at(_albuf.RefID).RefName;
    
    // coordinates
    int32_t first = _albuf.Position;
    int32_t last = _albuf.GetEndPosition() + 1;
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
