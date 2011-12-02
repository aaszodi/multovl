#define BOOST_TEST_MODULE multiregiontest
#include "boost/test/unit_test.hpp"

// -- Own headers --

#include "multiregion.hh"
#include "region.hh"
using namespace multovl;
#include "tempfile.hh"  // temporary file utility

// -- Boost headers --

#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"

// -- Standard headers --

#include <vector>
#include <algorithm>    // for equal
#include <fstream>

// -- Fixture --

struct MultiRegionFixture
{
    MultiRegionFixture():
        a1(Region(1, 5, '+', "a15")),   // track id = 0
        a2(4, 6, '-', "a46", 9),
        mr(2, 5)
    {
        aset1.insert(a1);
        aset2.insert(a1); aset2.insert(a2);
        mr.add_ancestor(a1);
    }
    
    AncestorRegion a1, a2;
    ancregset_t aset1, aset2;
    MultiRegion mr;
};

BOOST_FIXTURE_TEST_SUITE(multiregionsuite, MultiRegionFixture)

BOOST_AUTO_TEST_CASE(special_multiregion_test)
{
    // test empty
    MultiRegion empty;
    BOOST_CHECK_EQUAL(empty.multiplicity(), 0);
    BOOST_CHECK(!empty.solitary());
    BOOST_CHECK_EQUAL(empty.anc_str(), "");
    BOOST_CHECK_EQUAL(empty.strand(), '.');
    BOOST_CHECK_EQUAL(empty.name(), "overlap");
    
    // test solitary
    MultiRegion sol(1, 5, aset1);
    BOOST_CHECK_EQUAL(sol.multiplicity(), 1);
    BOOST_CHECK(sol.solitary());
    BOOST_CHECK_EQUAL(sol.anc_str(), "0:a15:+:1-5");
}

BOOST_AUTO_TEST_CASE(multiregion_test)
{
    // mr is non-solitary: ancestor has different coords
    BOOST_CHECK(!mr.solitary());
    
    // ancestor add/retrieve
    mr.add_ancestor(a2);
    BOOST_CHECK_EQUAL(mr.multiplicity(), 2);
    BOOST_CHECK_EQUAL(mr.anc_str(), "0:a15:+:1-5|9:a46:-:4-6");
    const ancregset_t& as = mr.ancestors();
    BOOST_CHECK(std::equal(as.begin(), as.end(), aset2.begin()));
    
    // ancestor IDs
    const int ancids[] = {0,9,9};
    const std::vector<int> ais = mr.ancestor_trackids();
    BOOST_CHECK(std::equal(ancids, ancids+2, ais.begin()));  // compare to {0,9}

    // add ancestor a2 again
    mr.add_ancestor(a2);
    BOOST_CHECK_EQUAL(mr.multiplicity(), 3);
    BOOST_CHECK_EQUAL(mr.anc_str(), "0:a15:+:1-5|2*9:a46:-:4-6");
    const std::vector<int> ais3 = mr.ancestor_trackids();
    BOOST_CHECK(std::equal(ancids, ancids+3, ais3.begin())); // compare to {0,9,9}
}

BOOST_AUTO_TEST_CASE(multiregion_serialization_test)
{
    mr.add_ancestor(a2);    // 2 ancestors
    Tempfile tempfile;
    {
        std::ofstream ofs(tempfile.name());
        boost::archive::text_oarchive oa(ofs);
        oa << mr;
    }
    
    {
        std::ifstream ifs(tempfile.name());
        boost::archive::text_iarchive ia(ifs);
        MultiRegion inmr;
        ia >> inmr;
        BOOST_CHECK_EQUAL(inmr.first(), 2);
        BOOST_CHECK_EQUAL(inmr.last(), 5);
        BOOST_CHECK_EQUAL(inmr.multiplicity(), 2);
        BOOST_CHECK_EQUAL(inmr.anc_str(), "0:a15:+:1-5|9:a46:-:4-6");
    }
    
}

BOOST_AUTO_TEST_SUITE_END()
