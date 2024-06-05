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
#ifndef LINEREADER_HEADER
#define LINEREADER_HEADER

// == HEADER linereader.hh ==

/** \file 
 * \brief Text-based input.
 * Classes for reading lines from ASCII genomic data files (BED, GFF).
 * Parsing is simplified, only the coordinates and region names are processed.
 * \author Andras Aszodi
 * \date 2011-05-31.
 */

// -- Standard headers --

#include <string>
#include <regex>
#include <map>
#include <iostream>
#include <stdexcept>

// -- Own headers --

#include "multovl/baseregion.hh"

namespace multovl {
namespace io {

/**
 * \brief Base class of parser objects.
 * Linereader objects process one line at a time,
 * and change their internal state according to what they find in it.
 * Parsing a complete data file involves reading it in line-by-line
 * and submitting each line to a parser object for the appropriate format.
 * Upon successful parsing the parser object stores the coordinate information
 * plus additional data that can be read out and used to construct
 * the appropriate region object. 
 */
class Linereader
{
    public:
    
    /// Symbolic constants to signify the result of a parsing operation
    enum Status
    {
        CLEAN = 0,      ///< state after reset (all bits off)
        ERROR = 1,      ///< could not parse line
        EMPTY = 2,      ///< empty line (whitespace only) was seen
        COMMENT = 4,    ///< comment line was seen
        DATA = 8  ///< data line parsed successfully
    };
    
    /// Sets up a Linereader object.
    /// \param commentchars the character(s) that signify the beginning of a comment
    explicit Linereader(const std::string& commentchars="#");
    
    virtual
    ~Linereader() = default;
    
    /// Parses a line into the calling object from /line/.
    /// \return the status of the parsing operation.
    Status parse(const std::string& line);
    
    /// Updates the contents of a region with what has been parsed.
    /// Does nothing if the calling object is not in the DATA state.
    virtual
    bool read_into(BaseRegion& region) const = 0;
    
    // -- Accessors
    
    /// \return the status after the last parsing operation.
    Status status() const { return _status; }
    
    /// \return a comment line stripped of all leading whitespace and comment characters
    /// or the empty std::string if the parsed line was not a comment
    const std::string& comment() const { return _comment; }
    
    /// \return the number of fields seen in the parsed line.
    /// Makes sense only if the return status is DATA.
    size_t fieldcnt() const { return _fieldcnt; }
    
    /// \return contains parse error messages, empty std::string if the status is not ERROR.
    const std::string& error_msg() const { return _err; }
    
    const std::string& chrom() const { return _chrom; }
    unsigned int first() const { return _first; }
    unsigned int last() const { return _last; }
    char strand() const { return _strand; }
    const std::string& name() const { return _name; }

    protected:
    
    /// Linereader's own exception
    class Exception: public std::logic_error
    {
        public:
        
        /// Create a parser exception.
        /// \param from indicates where the error occurred (ie. which method)
        /// \param problem description of what went wrong
        Exception(const std::string& from, const std::string& problem):
            logic_error("\"" + from + "\": " + problem) {}
    };
    
    /// Prepares the calling object for parsing by resetting all fields.
    /// This method must be invoked by the parse(...) method.
    virtual
    void reset();
    
    /// Sets the given status flag by bitwise OR-ing it with the _status field.
    /// \param flag one or more of the Status enum-s, OR-ed together
    void set_statusflag(Status flag)
    {
        _status = static_cast<Status>(_status | flag);
    }
    
    /// Returns /true/ if /str/ is empty or contains whitespace characters only.
    static bool empty_white(const std::string& str);
    
    /// Parse a /line/ assuming it contains coordinate data.
    /// Implementations must set _status bits (ERROR and DATA) accordingly.
    virtual
    void parse_data(const std::string& line) = 0;
    
    /// Converts /str/ to an unsigned int
    /// generates meaningful error messages via Linereader::Exception
    static
    unsigned int str_to_uint(std::string& str);
    
    // these data members can be set during parsing by the derived classes
    Status _status;	///< the status after parsing
    size_t _fieldcnt;   ///< how many column fields have been seen
    std::string _comment;    ///< store a comment line
    std::string _err;    ///< stores parsing error messages
    
    // same as the contents of a BaseRegion object
    std::string _chrom, _name;
    unsigned int _first, _last;
    char _strand;
    
    private:
    
    std::regex _comment_regex;    
};

/**
 \brief Linereader for BED-formatted lines.
 The BedLinereader class parses columns 1 to 6 of the BED format specification,
 except Column 5 (score) which is skipped,
 and ignores the optional columns 7 to 12
 (http://genome.ucsc.edu/FAQ/FAQformat.html#format1).
 */
class BedLinereader: public Linereader
{
    public:
    
    /// Sets up a BedLinereader object.
    BedLinereader(): Linereader("#") { reset(); }
    
    /// Updates the contents of a region with what has been parsed from a BED-formatted line.
    /// Does nothing if the calling object is not in the DATA state.
    virtual
    bool read_into(BaseRegion& region) const override;
    
    protected:
    
    virtual
    void reset() override;
    
    /// Parse a /line/ assuming it contains BED-formatted data.
    virtual
    void parse_data(const std::string& line) override;
    
};

/**
 \brief Linereader for GFF-formatted lines.
 Only the columns containing coordinate information are parsed,
 the rest are ignored. No distinction between GFF2 and GFF3.
 */
class GffLinereader: public BedLinereader
{
    public:
    
    /// Init to empty
    GffLinereader(): BedLinereader() {}
    
    // NOTE: read_into() is not overridden
    
    protected:
    
    // NOTE: reset() is not overridden
    
    /// Parse a /line/ assuming it contains GFF-formatted data.
    virtual
    void parse_data(const std::string& line) override;
    
};

}   // namespace io
}   // namespace multovl

#endif      // MULTOVL_LINEREADER_HEADER
