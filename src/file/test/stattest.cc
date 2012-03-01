#define BOOST_TEST_MODULE empirdistrtest
#include "boost/test/unit_test.hpp"
#include "boost/test/floating_point_comparison.hpp"

// -- own headers --

#include "stat.hh"
using namespace multovl;
using namespace prob;

// -- Boost headers --

// we test with normal random deviates
#include "boost/random/normal_distribution.hpp"

// -- standard headers --

// NOTE: we boldly assume <cmath> provides the erf() function as per ISO/IEC 9899:1999(E).
#include <cmath>
#ifdef __SUNPRO_CC
#include <math.h>
#endif
#include <iostream>
#include <string>

// -- Consts, useful functions, fixture... --

// NOTE: these tolerances have been titrated manually, they are quite generous,
// but there's no exact way of testing random sampling...
// this test is meant to filter out really horrible errors.
// Run this test with the cmdline setting --log_level=warning
// to see the discrepancies.

const double PCT_TOL=2.5,   // percentage tolerance for floating-point checks
    ABS_TOL=0.01;          // ... and absolute tolerance for values that should be 0.0
const unsigned int NTRIAL=10000;    // number of random trials

static double norm_cdf(double m, double s, double x)
{
    static const double SQRT2 = std::sqrt(2.0);
    double t = (x - m)/(SQRT2*s);
    return ((1.0+erf(t))/2.0);
}

struct StatFixture
{
    StatFixture()
    {
        for (unsigned int i=0; i<6; ++i)
            dice.push_back(1.0*(i+1));
    }
    
    std::vector<double> dice;
};

BOOST_FIXTURE_TEST_SUITE(statsuite, StatFixture)

BOOST_AUTO_TEST_CASE(actual_test)
{
    // TODO
}

BOOST_AUTO_TEST_CASE(normrnd_test)
{
    // set up the RNG: spell out everything
    typedef boost::random::mt19937 rng_t;
    typedef boost::normal_distribution<double> normdistr_t;
    rng_t rng(42u);
    const double EMEAN=3.2, EDEV=1.7;
    normdistr_t normdistr(EMEAN, EDEV);
    boost::variate_generator<rng_t&, normdistr_t > norm(rng, normdistr);

    Stat st;
    for (unsigned int i=0; i<NTRIAL; ++i)
    {
        double x;
        x = norm();
        // TODO
        st.add(x);
    }
    ed.evaluate();
    
    // cannot reliably test low() and high()
    BOOST_WARN_CLOSE(ed.mean(), EMEAN, PCT_TOL);
    BOOST_WARN_CLOSE(ed.variance(), EDEV*EDEV, PCT_TOL);
    
    // scan a range of +/- Z S.D of the CDF in 0.1 S.D. steps
    const double Z = 2.0, XSTEP = 0.1*EDEV;
    for (double x = EMEAN - Z*EDEV; x <= EMEAN + Z*EDEV; x+=XSTEP)
    {
        double yexp = norm_cdf(EMEAN, EDEV, x);
        BOOST_WARN_CLOSE(ed.cdf(x), yexp, PCT_TOL);
    }
}

BOOST_AUTO_TEST_SUITE_END()
