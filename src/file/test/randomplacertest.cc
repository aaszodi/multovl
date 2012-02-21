#define BOOST_TEST_MODULE randomplacertest
#include "boost/test/unit_test.hpp"
#include "boost/test/floating_point_comparison.hpp"

// -- own headers --

#include "randomplacer.hh"
using namespace multovl;
using namespace prob;

// -- Boost headers --

#include "boost/foreach.hpp"

// -- standard headers --

#include <map>

// -- Fixtures... --

const double PCT_TOL=2.5;   // percentage tolerance for floating-point checks
const unsigned int NTRIAL=100000;

// NOTE: this could have been derived from FreeRegionsFixture
struct RandomPlacerFixture
{
    RandomPlacerFixture():
        rng(42u),    // uniform [0,1)
        total(0)
    {
        // set up the free regions
        frees.push_back(Region(100, 199, '+', "free100"));  // len=100
        frees.push_back(Region(200, 249, '+', "free50"));  // len=50
        frees.push_back(Region(300, 329, '+', "free30"));  // len=30
        frees.push_back(Region(400, 419, '+', "free20"));  // len=20
        
        counts["free100"] = 0;
        counts["free50"] = 0;
        counts["free30"] = 0;
        counts["free20"] = 0;
        
        // these will fit in the "free100" and "free50", but not in the shorter ones
        regs.push_back(Region(500, 539, '+', "reg40"));
        regs.push_back(Region(600, 634, '+', "reg35"));
    }
    
    void count(const Region& reg)
    {
        unsigned int rf = reg.first(), rl = reg.last();
        bool found = false;
        BOOST_FOREACH(Region free, frees)
        {
            unsigned int ff = free.first(), fl = free.last();
            if (ff <= rf && rl <= fl)
            {
                // within one of the free regions, found, hooray!
                counts[free.name()]++;
                ++total;
                found = true;
                break;
            }
        }
        BOOST_CHECK_MESSAGE(found, "rf=" << rf << ",rl=" << rl);
    }
    
    double frac(const std::string& regname) const
    {
        double cnt = static_cast<double>(counts.find(regname)->second);
        return cnt/total;
    }
    
    UniformGen rng;
    std::vector<Region> frees, regs;
    std::map<std::string, unsigned int> counts;
    unsigned int total;
};

BOOST_FIXTURE_TEST_SUITE(randomplacersuite, RandomPlacerFixture)

BOOST_AUTO_TEST_CASE(randomplacer_test)
{
    FreeRegions fr(frees);
    RandomPlacer rp(regs);
    for (unsigned int i=0; i<NTRIAL; ++i)
    {
        rp.random_placement(fr, rng);
        BOOST_FOREACH(Region r, rp.get_regions())
        {
            count(r);
        }
    }
    BOOST_CHECK_EQUAL(total, 2*NTRIAL);   // make sure all shuffled regions fell in one of the frees...
    BOOST_CHECK_CLOSE(frac("free100"), 2.0/3.0, PCT_TOL);
    BOOST_CHECK_CLOSE(frac("free50"), 1.0/3.0, PCT_TOL);
    BOOST_CHECK_CLOSE(frac("free30"), 0.0, PCT_TOL);     // these are too short now...
    BOOST_CHECK_CLOSE(frac("free20"), 0.0, PCT_TOL);
}

BOOST_AUTO_TEST_SUITE_END()
