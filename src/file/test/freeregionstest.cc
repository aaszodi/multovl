#define BOOST_TEST_MODULE freeregionstest
#include "boost/test/unit_test.hpp"
#include "boost/test/floating_point_comparison.hpp"

// -- own headers --

#include "freeregions.hh"
using namespace multovl;
using namespace prob;

// -- standard headers --

#include <map>

// -- Fixtures... --

const double PCT_TOL=2.5;   // percentage tolerance for floating-point checks
const unsigned int NTRIAL=100000;

struct FreeRegionsFixture
{
    FreeRegionsFixture():
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

BOOST_FIXTURE_TEST_SUITE(freeregionssuite, FreeRegionsFixture)

BOOST_AUTO_TEST_CASE(fit_test)
{
    FreeRegions fr(frees);
    BOOST_CHECK(fr.fit(Region(110, 120, '+', "fit1")));
    BOOST_CHECK(fr.fit(Region(230, 240, '+', "fit2")));
    BOOST_CHECK(fr.fit(Region(300, 329, '+', "xfit3")));  // "exact" fit, i.e. equality
    BOOST_CHECK(fr.fit(Region(401, 410, '+', "fit4")));

    BOOST_CHECK(!fr.fit(Region(10, 20, '+', "before")));
    BOOST_CHECK(!fr.fit(Region(50, 120, '+', "beforeovl")));
    BOOST_CHECK(!fr.fit(Region(130, 220, '+', "twoovl")));
    BOOST_CHECK(!fr.fit(Region(270, 280, '+', "between")));
    BOOST_CHECK(!fr.fit(Region(410, 430, '+', "afterovl")));
    BOOST_CHECK(!fr.fit(Region(500, 600, '+', "after")));
}

BOOST_AUTO_TEST_CASE(nominlen_test)
{
    FreeRegions fr(frees);
    for (unsigned int i=0; i<NTRIAL; ++i)
    {
        std::string regname = fr.select_free_region(rng, 0).name();
        count(regname);
    }
    BOOST_CHECK_CLOSE(frac("reg100"), 0.5, PCT_TOL);
    BOOST_CHECK_CLOSE(frac("reg50"), 0.25, PCT_TOL);
    BOOST_CHECK_CLOSE(frac("reg30"), 0.15, PCT_TOL);
    BOOST_CHECK_CLOSE(frac("reg20"), 0.1, PCT_TOL);
}

BOOST_AUTO_TEST_CASE(minlen_test)
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

BOOST_AUTO_TEST_SUITE_END()
