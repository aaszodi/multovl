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
// == MODULE linereader.cc ==

// -- Standard headers --

#include <vector>
#include <sstream>
#include <algorithm>
#include <limits>
#include <type_traits>

using namespace std;

// -- Boost headers --

#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"

// -- Own header --

#include "multovl/io/linereader.hh"

// -- Implementation --

// unnamed namespace to hold utility functions
namespace {

// Tokenizer function by "Marius" (http://stackoverflow.com/users/174650/marius)
// based on his answer to this question
// http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
// modified by me because the original won't compile
// This is about 3x faster than boost::split, and 2.5x faster than boost::tokenizer
// Parameters:-
// tokens: a container that has a ::value_type to which a std::string may be converted to
// and has an operation push_back()
// (in 99.999% of the cases Container will be std::vector<std::string> of course)
// str: the input string to be tokenized
// delimiters: string of delimiter characters
// trimEmpty: if true then empty tokens will be trimmed
template <typename Container>
void tokenizer(
    Container& tokens,
    const std::string& str,
    const std::string& delimiters = " ",
    const bool trimEmpty = false
)
{
    static_assert(
        std::is_convertible<std::string, typename Container::value_type>::value,
        "Elements of `tokens` must be convertible to string"
    );
    
    std::string::size_type pos, lastPos = 0;
    const char* strdata = str.data();
    while(true)
    {
        pos = str.find_first_of(delimiters, lastPos);
        if(pos == std::string::npos)
        {
            // no more delimiters
            pos = str.length();
            if(pos != lastPos || !trimEmpty)
            {
                tokens.push_back(
                    std::string(
                        strdata + lastPos,
                        pos - lastPos
                    )
                );
            }
            break;
        }
        else
        {
            if(pos != lastPos || !trimEmpty)
            {
                tokens.push_back(
                    std::string(
                        strdata + lastPos,
                        pos - lastPos
                    )
                );
            }
        }
    
        lastPos = pos + 1;
    }
}

}   // end of unnamed namespace

namespace multovl {
namespace io {

// -- Linereader methods --

const char Linereader::WHITESPACE[] = " \t\n";

Linereader::Linereader() {
    reset();
}

void Linereader::reset()
{
    _status = CLEAN; _comment = "";
    _fieldcnt = 0; _err = "";
    _chrom = ""; _first = _last = 0; _strand = '.';
}

Linereader::Status Linereader::parse(const string& line)
{
    reset();
    string tline = boost::trim_right_copy_if(line, boost::is_cntrl());
    
    // check if line is empty or contains whitespace only
    if (empty_white(tline))
    {
        _status = EMPTY;
        return _status;
    }

    // parse simple comments
    auto commentoffs = parse_comment(tline);
    if (commentoffs != std::string::npos)
    {
        // strip leading ws and the comment chars
        _comment = tline.substr(commentoffs);
        _status = COMMENT;
        return _status;
    }
    
    // data tline, format-dependent, can go wrong
    // see derived class impls
    parse_data(tline);
    return _status;
}

bool Linereader::empty_white(const string& str)
{
    if (str == "") {
        return true;
    }
    return (std::string::npos == str.find_first_not_of(WHITESPACE));
}

std::string::size_type Linereader::parse_comment(const string& line) {
    auto notws1 = line.find_first_not_of(WHITESPACE);
    if (notws1 == std::string::npos) {
        // unlikely, indicates all-whitespace line
        return notws1;
    }
    if (line[notws1] != COMCH) {
        // need a '#' after initial whitespace
        return std::string::npos;
    }
    if (++notws1 == line.size()) {
        // # was the last character, the comment itself is empty
        return notws1;
    }
    // find the first non-ws after the '#'. notws1 already incremented
    auto notws2 = line.find_first_not_of(WHITESPACE, notws1);
    return (notws2 == std::string::npos? notws1: notws2);
}

unsigned int Linereader::str_to_uint(std::string& str)
{
    boost::trim(str);   // "100" is OK, " 100 " is not...
    if (str[0] == '-')
        throw Exception(str, "must not be negative");
    try
    {
        return boost::lexical_cast<unsigned int>(str);
    }
    catch (const boost::bad_lexical_cast&)
    {
        throw Exception(str, "cannot parse to unsigned int");
    }
}

// -- BedLinereader methods --

bool BedLinereader::read_into(BaseRegion& region) const
{
	if (status() != DATA) return false;
	
    region.set_coords(first(), last());
    region.strand(strand());
	region.name(name());    // note: score parsed but ignored
	return true;
}

void BedLinereader::parse_data(const string& line)
{
    _status = static_cast<Status>(_status | DATA);
    
    vector<string> fields;
    fields.reserve(12); // max. no. of fields of a legal BED record
    tokenizer(fields, line, "\t");   // split on \t
    _fieldcnt = fields.size();   // how many fields are there?
    if (_fieldcnt < 3)
    {
        _err = "Too few fields: " + boost::lexical_cast<string>(_fieldcnt);
        set_statusflag(ERROR);    // 3 fields are mandatory
        return;
    }
    
    try
    {
        // store the mandatory fields
        _chrom = fields[0];
        _first = str_to_uint(fields[1]);
        _last = str_to_uint(fields[2]);

        // go through the implemented non-mandatory ones
        if (_fieldcnt>3) _name = fields[3];

        // NOTE: score field skipped

        if (_fieldcnt>5)
        {
            char s = fields[5][0];  // first char, which will be checked
            _strand = (s=='+' || s=='-')? s: '.';
        }
        
        // OK if we got this far
        return;
    }
    catch (const Exception& lexerr)
    {
        _err = lexerr.what();
        set_statusflag(ERROR);
        return;
    }
}

void BedLinereader::reset()
{
    Linereader::reset();
    _name = ""; 
}

// -- GffLinereader methods --

void GffLinereader::parse_data(const string& line)
{
    set_statusflag(DATA);
        
    vector<string> fields;
    fields.reserve(9);  // max. no. of fields of a legal GFF record
    
    // separate the fields first
    tokenizer(fields, line, "\t");   // split on \t
    
    _fieldcnt = fields.size();
    if (_fieldcnt < 5)
    {
        _err = "Too few fields: " + boost::lexical_cast<string>(_fieldcnt);
        set_statusflag(ERROR);    // 5 fields are mandatory
        return;
    }
    
    try
    {
        // store mandatory fields
        _chrom = fields[0];
        // NOTE: ignore source field
        _name = fields[2];
        _first = str_to_uint(fields[3]);
        _last = str_to_uint(fields[4]);
        
        // NOTE: ignore score field
        // get the strand
        if (_fieldcnt > 6)
        {
            char s = fields[6][0];  // first char, which will be checked
            _strand = (s=='+' || s=='-')? s: '.';
        }
        
        // ignore the rest
        return;
    }
    catch (const Exception& lexerr)
    {
        _err = lexerr.what();
        set_statusflag(ERROR);
        return;
    }
}

}   // namespace textio
}   // namespace multovl
