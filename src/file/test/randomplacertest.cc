#define BOOST_TEST_MODULE randomplacertest
#include "boost/test/unit_test.hpp"
#include "boost/test/floating_point_comparison.hpp"

// -- own headers --

#include "randomplacer.hh"
using namespace multovl;
using namespace prob;

// -- standard headers --

#include <map>

// -- Fixtures... --

const double PCT_TOL=2.5;   // percentage tolerance for floating-point checks
const unsigned int NTRIAL=100000;

struct RandomPlacerFixture
{
    RandomPlacerFixture():
        rng(42u),    // uniform [0,1)
        total(0)
    {
        frees.push_back(Region(100, 199, '+', "reg100"));  // len=100
        frees.push_back(Region(200, 249, '+', "reg50"));  // len=50
        frees.push_back(Region(300, 329, '+', "reg30"));  // len=30
        frees.push_back(Region(400, 419, '+', "reg20"));  // len=20
        
        counts["reg100"] = 0;
        counts["reg50"] = 0;
        counts["reg30"] = 0;
        counts["reg20"] = 0;
    }
    
    void count(const std::string& regname)
    {
        counts[regname]++;
        ++total;
    }
    
    double frac(const std::string& regname) const
    {
        double cnt = static_cast<double>(counts.find(regname)->second);
        return cnt/total;
    }
    
    UniformGen rng;
    std::vector<Region> frees;
    std::map<std::string, unsigned int> counts;
    unsigned int total;
};

BOOST_FIXTURE_TEST_SUITE(randomplacersuite, RandomPlacerFixture)

// no idea how to test this properly...
#if 0
BOOST_AUTO_TEST_CASE(randomplacer_test)
{
    const unsigned int MINLEN = 40;
    
    FreeRegions fr(frees);
    for (unsigned int i=0; i<NTRIAL; ++i)
    {
        std::string regname = fr.select_free_region(rng, MINLEN).name();
        count(regname);
    }
    BOOST_CHECK_CLOSE(frac("reg100"), 2.0/3.0, PCT_TOL);
    BOOST_CHECK_CLOSE(frac("reg50"), 1.0/3.0, PCT_TOL);
    BOOST_CHECK_CLOSE(frac("reg30"), 0.0, PCT_TOL);     // these are too short now...
    BOOST_CHECK_CLOSE(frac("reg20"), 0.0, PCT_TOL);
}
#endif

BOOST_AUTO_TEST_SUITE_END()
