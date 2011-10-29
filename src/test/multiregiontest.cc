#define BOOST_TEST_MODULE multiregiontest
#include "boost/test/unit_test.hpp"

// -- Standard headers --

#include <vector>
#include <algorithm>    // for equal
using namespace std;

// -- Own headers --

#include "multiregion.hh"
#include "region.hh"
using namespace multovl;

struct MultiRegionFixture
{
    MultiRegionFixture():
        a1(Region(1, 5, '+', "a15")),   // track id = 0
        a2(4, 6, '-', "a46", 9)
    {
        aset1.insert(a1);
        aset2.insert(a1); aset2.insert(a2);
    }
    
    AncestorRegion a1, a2;
    ancregset_t aset1, aset2;
};

BOOST_FIXTURE_TEST_SUITE(multiregionsuite, MultiRegionFixture)

BOOST_AUTO_TEST_CASE(multiregion_test)
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
    
    // this is non-solitary: ancestor has different coords
    MultiRegion nonsol(2, 5, aset1);
    BOOST_CHECK(!nonsol.solitary());
    
    // ancestor add/retrieve
    nonsol.add_ancestor(a2);
    BOOST_CHECK_EQUAL(nonsol.multiplicity(), 2);
    BOOST_CHECK_EQUAL(nonsol.anc_str(), "0:a15:+:1-5|9:a46:-:4-6");
    const ancregset_t& as = nonsol.ancestors();
    BOOST_CHECK(equal(as.begin(), as.end(), aset2.begin()));
    
    // ancestor IDs
    const int ancids[] = {0,9,9};
    const vector<int> ais = nonsol.ancestor_trackids();
    BOOST_CHECK(equal(ancids, ancids+2, ais.begin()));  // compare to {0,9}

    // add ancestor a2 again
    nonsol.add_ancestor(a2);
    BOOST_CHECK_EQUAL(nonsol.multiplicity(), 3);
    BOOST_CHECK_EQUAL(nonsol.anc_str(), "0:a15:+:1-5|2*9:a46:-:4-6");
    const vector<int> ais3 = nonsol.ancestor_trackids();
    BOOST_CHECK(equal(ancids, ancids+3, ais3.begin())); // compare to {0,9,9}
}

BOOST_AUTO_TEST_SUITE_END()
