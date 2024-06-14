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

// -- Own headers --

#include "multovl/multiregion.hh"

namespace multovl {

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
    explicit Linewriter(const std::string& chrom):
        _chr(chrom) {}
        
    virtual
    ~Linewriter() = default;
    
    /// Writes a BaseRegion to a string (no newline) and returns it.
    /// \param reg the BaseRegion to be stringified
    /// \return the stringified region or the empty string if something went wrong
    virtual
    std::string write(const BaseRegion& reg) const = 0;
    
    /// Writes a MultiRegion to a string (no newline) and returns it.
    /// \param reg the MultiRegion to be stringified
    /// \return the stringified region or the empty string if something went wrong
    virtual
    std::string write(const MultiRegion& reg) const = 0;
    
    protected:
    
    /// \return The chromosome name
    const std::string& chrom() const { return _chr; }
    
    private:
    
    std::string _chr;    
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
    
    /// Writes a BaseRegion to a string (no newline) in BED format and returns it.
    /// The name of the region is written into the 4th column ("name"),
    /// the 5th column ("score") will be 0.
    /// \param region the region to be stringified
    /// \return the stringified region or the empty string if something went wrong
    virtual
    std::string write(const BaseRegion& reg) const override final;
    
    /// Writes a MultiRegion to a string (no newline) in BED format and returns it.
    /// The ancestor string is written into the 4th column ("name"),
    /// the multiplicity into the 5th column ("score").
    /// \param region the region to be stringified
    /// \return the stringified region or the empty string if something went wrong
    virtual
    std::string write(const MultiRegion& reg) const override final;

    private:
    
    std::string write_line(
        unsigned int first, unsigned int last,
        const std::string& name, unsigned int multiplicity,
        char strand
    ) const;
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
    
    /// Writes a BaseRegion to a string (no newline) in GFF format and returns it.
    /// The score column will be '.', columns 8 and 9 (frame, attributes)
    /// are not printed at all.
    /// \param region the region to be stringified
    /// \return the stringified region or the empty string if something went wrong
    virtual
    std::string write(const BaseRegion& region) const override final;
    
    /// Writes a MultiRegion to a string (no newline) in GFF format and returns it.
    /// The score column will contain the multiplicity, frame will be '.',
    /// the ancestor strings are presented as the ANCESTORS attribute in column 9.
    /// \param region the region to be stringified
    /// \return the stringified region or the empty string if something went wrong
    virtual
    std::string write(const MultiRegion& region) const override final;
    
    // Accessors
    
    const std::string& source() const { return _source; }
    unsigned int version() const { return _version; }
    
    private:
    
    std::string write_line(
        const std::string& source, const std::string& name,
        unsigned int first, unsigned int last,
        unsigned int multiplicity, char strand,
        const std::string& ancestors
    ) const;

    std::string _source;
    unsigned int _version;  // 2 or 3
    char _sep;  // ' ' for version 2, '=' for version 3
};

}   // namespace io
}   // namespace multovl

#endif      // MULTOVL_LINEWRITER_HEADER
