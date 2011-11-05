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
    typedef std::list<RegLimit> rllist_t;
    
    MultiReglimitFixture():
        expl()
    {}
    
    // adds a region to /mrl/, and adds 2 RegLimit entries to the internal "expected list"
    void add_region(
        MultiRegLimit& mrl,
        unsigned int first, unsigned int last, char strand, const std::string& nm,
        unsigned int trackid
    )
    {
        Region r(first, last, strand, nm);
        mrl.add(r, trackid);
        boost::shared_ptr<AncestorRegion> rp(new AncestorRegion(r, trackid));
        expl.push_back(RegLimit(rp, true));
        expl.push_back(RegLimit(rp, false));
    }
    
    // sorts the internal "expected list"
    // and then compares it to what is in mrl.reglim()
    // this works only if mrl was filled up with add_region(...) above
    void checker(const MultiRegLimit& mrl)
    {
        expl.sort();
        
        const MultiRegLimit::reglim_t& obs = mrl.reglim();
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
    
    rllist_t expl;
};

BOOST_FIXTURE_TEST_SUITE(multireglimitsuite, MultiReglimitFixture)

BOOST_AUTO_TEST_CASE(multireglimit_ctor_test)
{
    static const unsigned int TRACKID = 9;
    Region r(1, 5, '+', "r15");
    MultiRegLimit mrl(r, TRACKID);
    
    boost::shared_ptr<AncestorRegion> rp(new AncestorRegion(r, TRACKID));
    rllist_t expl = list_of
        (RegLimit(rp, true))(RegLimit(rp, false));
    checker(mrl);
}

BOOST_AUTO_TEST_CASE(multireglimit_test)
{
    MultiRegLimit mrl;
    add_region(mrl, 1, 5, '+', "r15", 9);
    add_region(mrl, 4, 6, '-', "r46", 17);
    
    checker(mrl);
}

BOOST_AUTO_TEST_CASE(multireglimit_merge_test)
{
    MultiRegLimit mrl;
    add_region(mrl, 1, 5, '+', "r15", 9);
    add_region(mrl, 4, 6, '-', "r46", 17);
    
    // the regions go into /other/, but the reglimits into the same /expl/
    MultiRegLimit other;
    add_region(other, 9, 11, '+', "r911", 19);
    add_region(other, 5, 7, '-', "r57", 37);
    
    mrl += other;
    checker(mrl);
}

// NOTE: serialization is tested elsewhere

BOOST_AUTO_TEST_SUITE_END()

