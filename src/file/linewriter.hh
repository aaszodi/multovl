#ifndef LINEWRITER_HEADER
#define LINEWRITER_HEADER

// == HEADER linewriter.hh ==

/** \file 
 * \brief Text-based output.
 * Classes for writing MultiRegions to text line strings.
 * \author Andras Aszodi
 * \date 2011-01-23.
 */

// -- Standard headers --

#include <string>
#include <sstream>

namespace multovl {

class Region;
class MultiRegion;

namespace io {

/**
 * \brief Base class of stringifier objects.
 */
class Linewriter
{
    public:
    
    /// Sets up a Linewriter object.
    /// \param chrom the chromosome name containing the
    /// regions to be stringified
    explicit Linewriter(const std::string& chrom);
    
    virtual ~Linewriter() {}
    
    /// Writes a Region to a string (no newline) and returns it.
    /// \param reg the Region to be stringified
    /// \return the stringified region or the empty string if something went wrong
    virtual std::string write(const Region& reg) = 0;
    
    /// Writes a MultiRegion to a string (no newline) and returns it.
    /// \param reg the MultiRegion to be stringified
    /// \return the stringified region or the empty string if something went wrong
    virtual std::string write(const MultiRegion& reg) = 0;
    
    protected:
    
    void reset() { _ostr.clear(); _ostr.str(""); }
    
    std::string _chr;
    std::ostringstream _ostr;
    
};

/**
 \brief Linewriter for BED-formatted lines.
 The BedLinewriter writes columns 1 to 6 of the BED format specification,
 (http://genome.ucsc.edu/FAQ/FAQformat.html#format1).
 */
class BedLinewriter: public Linewriter
{
    public:
    
    /// Sets up a BedLinewriter object.
    /// \param chrom the chromosome containing the
    /// regions to be stringified
    /// \param prefix the chromosome prefix, "chr" by default
    explicit BedLinewriter(const std::string& chrom): 
    	Linewriter(chrom) {}
    
    /// Writes a Region to a string (no newline) in BED format and returns it.
    /// The name of the region is written into the 4th column ("name"),
    /// the 5th column ("score") will be 0.
    /// \param region the region to be stringified
    /// \return the stringified region or the empty string if something went wrong
    virtual std::string write(const Region& reg);
    
    /// Writes a MultiRegion to a string (no newline) in BED format and returns it.
    /// The ancestor string is written into the 4th column ("name"),
    /// the multiplicity into the 5th column ("score").
    /// \param region the region to be stringified
    /// \return the stringified region or the empty string if something went wrong
    virtual std::string write(const MultiRegion& reg);
};

/**
 \brief Linewriter for GFF-formatted lines.
 The GffLinewriter class understands Versions 2 and 3 of the GFF format 
 specification (http://www.sanger.ac.uk/resources/software/gff/spec.html).
 */
class GffLinewriter: public Linewriter
{
    public:
    
    /// Init the GFF line writer
    /// \param source this gets written in Column 2
    /// \param version GFF version (2 or 3, clamped silently)
    /// \param chrom the chromosome containing the
    /// regions to be stringified
    /// \param prefix the chromosome prefix, "chr" by default
    GffLinewriter(const std::string& source, unsigned int version, 
    	const std::string& chrom);
    
    /// Writes a Region to a string (no newline) in GFF format and returns it.
    /// The score column will be '.', columns 8 and 9 (frame, attributes)
    /// are not printed at all.
    /// \param region the region to be stringified
    /// \return the stringified region or the empty string if something went wrong
    virtual std::string write(const Region& region);
    
    /// Writes a MultiRegion to a string (no newline) in GFF format and returns it.
    /// The score column will contain the multiplicity, frame will be '.',
    /// the ancestor strings are presented as the ANCESTORS attribute in column 9.
    /// \param region the region to be stringified
    /// \return the stringified region or the empty string if something went wrong
    virtual std::string write(const MultiRegion& region);
    
    // Accessors
    
    const std::string& source() const { return _source; }
    unsigned int version() const { return _version; }
    
    private:
    
    std::string _source;
    unsigned int _version;  // 2 or 3
    char _sep;  // ' ' for version 2, '=' for version 3
};

}   // namespace io
}   // namespace multovl

#endif      // MULTOVL_LINEWRITER_HEADER
