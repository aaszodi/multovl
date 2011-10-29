// == MODULE fileformat.cc ==

// -- Own header --

#include "fileformat.hh"

// -- Boost headers --

#include "boost/filesystem.hpp"
#include "boost/algorithm/string.hpp"

// == Implementation ==

namespace multovl {
namespace io {

Fileformat::Kind Fileformat::from_filename(const std::string& filenm)
{
    boost::filesystem::path filepath(filenm);
    
    // before Boost 1.36, extension() was a standalone function
    std::string ext =
    #if BOOST_VERSION_MAJOR == 1 && BOOST_VERSION_MINOR <= 35
        boost::filesystem::extension(filepath);
    #elif BOOST_FILESYSTEM_VERSION == 3
        // default from V1.45 on...
        filepath.extension().string();
    #else
        // Version 2, will go away in 1.48
        filepath.extension();
    #endif

    if (ext[0] == '.')
        return from_string(ext.substr(1));
    else
        return UNKNOWN;
}

std::string Fileformat::to_string(Fileformat::Kind format)
{
    if (format == GFF)
        return "GFF";
    else if (format == BED)
        return "BED";
    else if (format == BAM)
        return "BAM";
    else
        return "UNKNOWN";
}

// this is private
Fileformat::Kind Fileformat::from_string(const std::string& fstr)
{
    std::string formstr = boost::algorithm::to_lower_copy(fstr);
    if (formstr == "gff" || formstr == "gtf")
        return GFF;
    else if (formstr == "bed")
        return BED;
    else if (formstr == "bam")
        return BAM;
    else
        return UNKNOWN;
}

}   // namespace io
}   // namespace multovl
