#define BOOST_TEST_MODULE multireglimittest
#include "boost/test/unit_test.hpp"

// -- Own headers --

#include "multireglimit.hh"
using namespace multovl;

// -- Standard headers --

#include <list>

// -- Boost headers --

#include "boost/shared_ptr.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/assign/list_of.hpp"
using namespace boost::assign;

// -- Fixture

struct MultiReglimitFixture
{
    MultiReglimitFixture():
        r15(1, 5, '+', "r15"),
        r15p(new AncestorRegion(r15, 9)),
        r46(4, 6, '-', "r46"),
        r46p(new AncestorRegion(r46, 17))
    {}
    
    Region r15, r46;
    boost::shared_ptr<AncestorRegion> r15p, r46p;
};

static
std::string reg_tostr(const Region& reg)
{
    std::string s = boost::lexical_cast<std::string>(reg.first()) + ":"
        + boost::lexical_cast<std::string>(reg.last()) + ":"
        + reg.strand() + reg.name();
    return s;
}

BOOST_FIXTURE_TEST_SUITE(multireglimitsuite, MultiReglimitFixture)

BOOST_AUTO_TEST_CASE(multireglimit_test)
{
    MultiRegLimit mrl(r15, 9);
    mrl.add(r46, 17);
    
    typedef std::list<RegLimit> rllist_t;
    rllist_t expl = list_of
        (RegLimit(r15p, true))(RegLimit(r46p, true))
        (RegLimit(r15p, false))(RegLimit(r46p, false));
    MultiRegLimit::reglim_t obs = mrl.reglim();
    MultiRegLimit::reglim_t::const_iterator obsit;
    rllist_t::const_iterator expit;
    for (obsit = obs.begin(), expit = expl.begin();
        obsit != obs.end() && expit != expl.end();
        ++obsit, ++expit)
    {
        
        BOOST_CHECK_EQUAL(obsit->region().to_attrstring(), expit->region().to_attrstring());
        BOOST_CHECK_EQUAL(obsit->is_first(), expit->is_first());
    }
}

// NOTE: serialization is tested elsewhere

BOOST_AUTO_TEST_SUITE_END()

