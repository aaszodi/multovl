// == MODULE fileio.cc ==

// -- Own headers --

#include "fileio.hh"
#include "multovl_config.hh"
#include "textio.hh"

#if USE_BAMTOOLS
#include "bamio.hh"
#endif

#include "multioverlap.hh"

// == Implementation ==

namespace multovl {
namespace io {

FileReader::FileReader(
    const std::string& infname,
    Fileformat::Kind format
):
    _reader(NULL),
    _errors(),
    _finished(false)
{
    // figure out the file format
    Fileformat::Kind fmt = format;
    if (format == Fileformat::UNKNOWN)
         fmt = Fileformat::from_filename(infname);
    if (fmt == Fileformat::UNKNOWN)
    {
        _errors.add_error("Cannot deduce format of file " + infname);
        _finished = true;
        return;
    }
    
    // select the appropriate track reader (factory pattern)
#if USE_BAMTOOLS
    if (fmt == Fileformat::BAM)
        _reader = new BamReader(infname);
    else
#endif
        _reader = new TextReader(infname, fmt);

    // maybe something went wrong...
    if (!_reader->errors().ok())
    {
        _errors += _reader->errors();
        _finished = true;
    }
}

FileReader::~FileReader()
{
    if (_reader != NULL)
        delete _reader;
}

bool FileReader::read_into(std::string& chrom, Region& reg)
{
    if (finished() || !errors().ok())
        return false;   // already in bad shape...
    
    std::string msg = _reader->read_into(chrom, reg);
    if (msg != "")
    {
        if (msg == "EOF")
        {
            _finished = true;
            return true;
        }
        _errors.add_error(msg);
        return false;
    }
    return true;
}

}   // namespace io
}   // namespace multovl
