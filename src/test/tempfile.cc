// == Module tempfile.cc ==

// -- Own header --

#include "tempfile.hh"

// -- Standard headers --

#include <fstream>

// == Implementation --

Tempfile::Tempfile()
{
    _path = boost::filesystem::temp_directory_path() /
        boost::filesystem::unique_path();
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

