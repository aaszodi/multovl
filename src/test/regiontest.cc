#define BOOST_TEST_MODULE regiontest
#include "boost/test/unit_test.hpp"

// -- Own headers --

#include "region.hh"
using namespace multovl;

// -- Standard headers --

#include <vector>
#include <algorithm>
#include <string>

// -- other Boost headers --

#include "boost/assign/list_inserter.hpp"   // for insert() ()()()...()
#include "boost/lexical_cast.hpp"

// -- Fixture

struct RegionFixture
{
    RegionFixture():
        r_empty(), 
        r15(1, 5, '+', "r15"),
        r46(6, 4, '-', "r46")  // checks limit swapping
    {}
    
    Region r_empty, r15, r46;
};

static
std::string reg_tostr(const Region& reg)
{
    std::string s = boost::lexical_cast<std::string>(reg.first()) + ":"
        + boost::lexical_cast<std::string>(reg.last()) + ":"
        + reg.strand() + reg.name();
    return s;
}

BOOST_FIXTURE_TEST_SUITE(regionsuite, RegionFixture)

BOOST_AUTO_TEST_CASE(empty_test)
{
    BOOST_CHECK(r_empty.first() == 0);
    BOOST_CHECK(r_empty.last() == 0);
    BOOST_CHECK(r_empty.is_empty());
}

BOOST_AUTO_TEST_CASE(length_test)
{
    BOOST_CHECK(r_empty.length() == 0);
    BOOST_CHECK(r15.length() == 5);
}

// Instead of separately testing the < operator,
// we just build a vector of Coord objects in descending order,
// then sort into ascending order and then test.
BOOST_AUTO_TEST_CASE(coordsort_test)
{
    std::vector<Region> desc;
    boost::assign::push_back(desc)
        (Region(3, 5, '-',"z"))
        (Region(3, 6, '-', "foo"))
        (Region(3, 6, '-', "aardvark"))
        (Region(2, 3, '.',"z"))
        (Region(2, 3, '-',"z"))
        (Region(2, 3, '+',"z"))
        (Region(1, 2, '+',"z"));
    std::vector<Region> asc = desc;
    std::sort(asc.begin(), asc.end());
    
    std::vector<Region>::iterator fwdit = asc.begin();
    std::vector<Region>::reverse_iterator revit = desc.rbegin();
    for ( ; fwdit != asc.end() && revit != desc.rend(); fwdit++, revit++)
    {
        BOOST_CHECK_EQUAL( reg_tostr(*fwdit), reg_tostr(*revit) );
    }
}

BOOST_AUTO_TEST_CASE(name_test)
{
    std::string oldname = r15.name("newr15");
    BOOST_CHECK_EQUAL(oldname, "r15");
    BOOST_CHECK_EQUAL("newr15", r15.name());
}

BOOST_AUTO_TEST_SUITE_END()
