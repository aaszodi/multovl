#ifndef MULTOVL_TEST_TEMPFILE_HEADER
#define MULTOVL_TEST_TEMPFILE_HEADER

// == Header tempfile.hh ==

// Little RAII utility to create/remove temporary files.
// \date 2011-12-02

// -- Boost headers --

#include "boost/filesystem.hpp"

// -- Standard headers --

#include <iostream>

// == Classes ==

class Tempfile
{
    public:
    Tempfile();
    ~Tempfile();
    
    const char* name() const { return _path.string().c_str(); }
    
    // print the contents of the temp file to /outs/    
    std::ostream& print(std::ostream& outs = std::cout) const;
    
    private:
    boost::filesystem::path _path;
};

#endif  // MULTOVL_TEST_TEMPFILE_HEADER
