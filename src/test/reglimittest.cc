#define BOOST_TEST_MODULE reglimittest
#include "boost/test/unit_test.hpp"

// -- Own headers --

#include "reglimit.hh"
using namespace multovl;
#include "tempfile.hh"  // temporary file utility

// -- Boost headers --

#include "boost/lexical_cast.hpp"
#include "boost/make_shared.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"

// -- Standard headers --

#include <fstream>

// -- Fixture

struct ReglimitFixture
{
    ReglimitFixture():
        anc(4, 6, '-', "a46", 9),
        ancp(boost::make_shared<AncestorRegion>(anc))
    {}
    
    AncestorRegion anc;
    boost::shared_ptr<AncestorRegion> ancp;
};

BOOST_FIXTURE_TEST_SUITE(reglimitsuite, ReglimitFixture)

BOOST_AUTO_TEST_CASE(reglimit_test)
{
    RegLimit rf(ancp, true), rl(ancp, false);
    
    BOOST_CHECK_EQUAL(rf.region().to_attrstring(), "9:a46:-:4-6");
    BOOST_CHECK_EQUAL(rf.track_id(), 9);
    BOOST_CHECK(rf.is_first());
    BOOST_CHECK_EQUAL(rf.this_pos(), 4);
    BOOST_CHECK_EQUAL(rf.other_pos(), 6);
    
    BOOST_CHECK_EQUAL(rl.region().to_attrstring(), "9:a46:-:4-6");
    BOOST_CHECK_EQUAL(rl.track_id(), 9);
    BOOST_CHECK(!rl.is_first());
    BOOST_CHECK_EQUAL(rl.this_pos(), 6);
    BOOST_CHECK_EQUAL(rl.other_pos(), 4);
    
    BOOST_CHECK(rf < rl);
}

BOOST_AUTO_TEST_CASE(reglimitser_test)
{
    Tempfile tempfile;
    {
        // create 2 RegLimit objects referring to the same ancregion
        RegLimit rlf(ancp, true), rll(ancp, false);
        std::ofstream ofs(tempfile.name());
        boost::archive::text_oarchive oa(ofs);
        oa << rlf << rll;
    }
    
    std::cout << "## Reglimit archive contents ##" << std::endl;
    tempfile.print();
    
    {
        std::ifstream ifs(tempfile.name());
        boost::archive::text_iarchive ia(ifs);
        RegLimit inrlf, inrll;
        ia >> inrlf >> inrll;
        
        // The raw pointers should be the same...
        BOOST_CHECK_EQUAL(inrlf.const_raw_region_ptr(), inrll.const_raw_region_ptr());
        
        // first & last
        BOOST_CHECK_EQUAL(inrlf.region().to_attrstring(), "9:a46:-:4-6");
        BOOST_CHECK(inrlf.is_first());
        BOOST_CHECK_EQUAL(inrll.region().to_attrstring(), "9:a46:-:4-6");
        BOOST_CHECK(!inrll.is_first());
    }
}

BOOST_AUTO_TEST_CASE(deepcopy_test)
{
    RegLimit rl(ancp, true);
    RegLimit rlcopy = rl.deep_copy();
    
    // contents same, raw ptrs different
    BOOST_CHECK(rl.const_raw_region_ptr() != rlcopy.const_raw_region_ptr());
    BOOST_CHECK_EQUAL(rl.region().to_attrstring(), rlcopy.region().to_attrstring());
    BOOST_CHECK(rlcopy.is_first());
    
    // change the copy's name, the original must stay unaffected
    rlcopy.raw_region_ptr()->name("chgdname");
    BOOST_CHECK_EQUAL(rlcopy.region().name(), "chgdname");
    BOOST_CHECK_EQUAL(rl.region().name(), "a46");
}

BOOST_AUTO_TEST_SUITE_END()

