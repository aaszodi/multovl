// == MODULE textio.cc ==

// -- Own headers --

#include "textio.hh"
#include "linereader.hh"

// -- Standard library --

#include <sstream>
#include <fstream>
#include <stdexcept>

// -- Boost headers --

#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"

// == Implementation ==

namespace multovl {
namespace io {

// -- TextReader methods --

TextReader::TextReader(
    const std::string& infname,
    Fileformat::Kind format
):
    TrackReader(),
    _lrp(NULL),
    _inf(),
    _linecount(0)
{
    
    // input line reader (parser) factory
    if (format == Fileformat::BED) _lrp = new BedLinereader();
    else if (format == Fileformat::GFF) _lrp = new GffLinereader();
    else
    {
        // this should not happen...
        _errors.add_error("Bad format for TextReader");
    }
    
    _inf.open(infname.c_str());
    if (!_inf)  // could not open
    {
        _errors.add_error("Cannot open input file: " + infname);
    }
}

std::string TextReader::read_into(std::string& chrom, Region& reg)
{
    if (!_inf)
        return "Input file not open";
        
    while (std::getline(_inf, _line))
    {
        ++_linecount;
        Linereader::Status status = _lrp->parse(_line);
        if (status & Linereader::ERROR)
        {
            add_error(_lrp->error_msg());
            return errors().last_error();
        }
        if (status & Linereader::DATA)
        {
            bool parseok = _lrp->read_into(reg);
            if (parseok)
            {
                chrom = _lrp->chrom();
                return "";    // success
            }
            else
            {
                // shouldn't really happen...
                add_error(_lrp->error_msg());
                return errors().last_error();
            }
        }
        // anything else (comments etc.) get ignored, keep reading...
    }
    return "EOF";
}

TextReader::~TextReader()
{
    _inf.close();
    if (_lrp != NULL)
        delete _lrp;
}

// Adds an error message to _errors, nicely decorated with linecount etc. Protected
void TextReader::add_error(const std::string& msg)
{
    std::string err = 
        "At line " + boost::lexical_cast<std::string>(_linecount) + ": " + msg;
    _errors.add_error(err);
}

}   // namespace io
}   // namespace multovl
