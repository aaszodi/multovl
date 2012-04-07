// == Module tempfile.cc ==

// -- Own header --

#include "tempfile.hh"

// -- Standard headers --

#include <string>
#include <fstream>

// == Implementation --

Tempfile::Tempfile()
{
    // used to have boost::filesystem::temp_directory_path() as prefix
    // but this is supported only in later versions of Filesystem V3 :-(
    // it definitely is not available in Boost 1.44
    _path = boost::filesystem::unique_path();
}

Tempfile::~Tempfile()
{
    boost::filesystem::remove(_path);
}

std::ostream& Tempfile::print(std::ostream& outs) const
{
    outs << "** TEMP FILE CONTENTS:" << std::endl;
    std::ifstream ifs(name());
    std::string buf;
    while (std::getline(ifs, buf))
        outs << buf << std::endl;
    return outs;
}

