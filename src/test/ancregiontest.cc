#define BOOST_TEST_MODULE ancregiontest
#include "boost/test/unit_test.hpp"

// -- Own headers --

#include "ancregion.hh"
using namespace multovl;
#include "tempfile.hh"  // temporary file utility

// -- Boost headers --

#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"

// -- Standard headers --

#include <fstream>

struct AncRegionFixture
{
    AncRegionFixture():
        a1(Region(1, 5, '+', "a15")),   // track id = 0
        a2(4, 6, '-', "a46", 9)
    {}
    
    AncestorRegion a1, a2;
};

BOOST_FIXTURE_TEST_SUITE(ancregionsuite, AncRegionFixture)

BOOST_AUTO_TEST_CASE(trackid_test)
{
    // testing the track ID changes
    unsigned int oldid = a1.track_id(17);
    BOOST_CHECK_EQUAL(oldid, 0);
    BOOST_CHECK_EQUAL(a1.track_id(), 17);
    BOOST_CHECK_EQUAL(a2.track_id(), 9);
}

BOOST_AUTO_TEST_CASE(io_test)
{
    // testing the I/O
    std::string out = a2.to_attrstring();
    BOOST_CHECK_EQUAL(out, "9:a46:-:4-6");
}

BOOST_AUTO_TEST_CASE(ancregionser_test)
{
    Tempfile tempfile;
    {
        std::ofstream ofs(tempfile.name());
        boost::archive::text_oarchive oa(ofs);
        oa << a2;
    }
    
    {
        std::ifstream ifs(tempfile.name());
        boost::archive::text_iarchive ia(ifs);
        AncestorRegion inar;
        ia >> inar;
        std::string out = inar.to_attrstring();
        BOOST_CHECK_EQUAL(out, "9:a46:-:4-6");
    }
}

BOOST_AUTO_TEST_SUITE_END()
