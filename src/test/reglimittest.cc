#define BOOST_TEST_MODULE reglimittest
#include "boost/test/unit_test.hpp"

// -- Own headers --

#include "reglimit.hh"
using namespace multovl;

// -- Standard headers --

// -- Boost headers --

#include "boost/lexical_cast.hpp"
#include "boost/make_shared.hpp"

// -- Fixture

struct ReglimitFixture
{
    ReglimitFixture():
        anc(4, 6, '-', "a46", 9)
    {
    }
    
    AncestorRegion anc;
};

BOOST_FIXTURE_TEST_SUITE(reglimitsuite, ReglimitFixture)

BOOST_AUTO_TEST_CASE(reglimit_test)
{
    boost::shared_ptr<AncestorRegion> ancp = boost::make_shared<AncestorRegion>(anc);
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

// NOTE: RegLimit serialization is tested elsewhere

BOOST_AUTO_TEST_SUITE_END()

