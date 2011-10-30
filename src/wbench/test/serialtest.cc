#define BOOST_TEST_MODULE serialtest
#include "boost/test/unit_test.hpp"

// -- Own headers --

#include "wbench/wbregion.hh"
using namespace multovl;

// -- Standard headers --

#include <vector>
#include <algorithm>
#include <string>
#include <fstream>

// -- other Boost headers --

#include "boost/lexical_cast.hpp"
#include "boost/filesystem.hpp"

// play with the archive type (1 -> text, 0 -> binary)
#if 0
#define IARCHIVE text_iarchive
#define OARCHIVE text_oarchive
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#else
#define IARCHIVE binary_iarchive
#define OARCHIVE binary_oarchive
#include "boost/archive/binary_iarchive.hpp"
#include "boost/archive/binary_oarchive.hpp"
#endif

//// #include "boost/assign/list_inserter.hpp"   // for insert() ()()()...()

// -- Fixture

struct SerialFixture
{
    SerialFixture():
        r15(1, 5, '+', "r15")
    {
        // some temp file
        tmpfilepath = boost::filesystem::temp_directory_path() /
            boost::filesystem::unique_path();
    }
    
    ~SerialFixture()
    {
        boost::filesystem::remove(tmpfilepath);
    }
    
    boost::filesystem::path tmpfilepath;
    WbRegion r15;
};

static
std::string reg_tostr(const Region& reg)
{
    std::string s = boost::lexical_cast<std::string>(reg.first()) + ":"
        + boost::lexical_cast<std::string>(reg.last()) + ":"
        + reg.strand() + reg.name();
    return s;
}

BOOST_FIXTURE_TEST_SUITE(serialsuite, SerialFixture)

BOOST_AUTO_TEST_CASE(wbregion_test)
{
    {
        std::ofstream ofs(tmpfilepath.string().c_str());
        boost::archive::OARCHIVE oa(ofs);
        oa << r15;
    }
    
    {
        std::ifstream ifs(tmpfilepath.string().c_str());
        boost::archive::IARCHIVE ia(ifs);
        WbRegion inr;
        ia >> inr;
        BOOST_CHECK_EQUAL(reg_tostr(inr), reg_tostr(r15));
    }
}

BOOST_AUTO_TEST_SUITE_END()
