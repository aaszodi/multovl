#define BOOST_TEST_MODULE empirdistrtest
#include "boost/test/unit_test.hpp"
#include "boost/test/floating_point_comparison.hpp"

// -- own headers --

#include "empirdistr.hh"
#include "randomgen.hh" // note: this is implicitly tested here
using namespace multovl;
using namespace prob;

// -- Boost headers --

// we test with normal random deviates in addition to what is provided in "randomgen.hh"
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
const unsigned int NCELL=50,    // number of approximation cells in an EmpirDistr object
    NTRIAL=10000;    // number of random trials

static double norm_cdf(double m, double s, double x)
{
    static const double SQRT2 = std::sqrt(2.0);
    double t = (x - m)/(SQRT2*s);
    return ((1.0+erf(t))/2.0);
}

struct EmpirDistrFixture
{
    EmpirDistrFixture()
    {
        for (unsigned int i=0; i<6; ++i)
            dice.push_back(1.0*(i+1));
    }
    
    std::vector<double> dice;
};

BOOST_FIXTURE_TEST_SUITE(empirdistrsuite, EmpirDistrFixture)

BOOST_AUTO_TEST_CASE(empty_test)
{
    EmpirDistr ed; // dirty & empty state
    BOOST_CHECK_THROW(ed.low(), EmpirDistr::Exception);
    BOOST_CHECK_THROW(ed.high(), EmpirDistr::Exception);
    BOOST_CHECK_THROW(ed.mean(), EmpirDistr::Exception);
    BOOST_CHECK_THROW(ed.variance(), EmpirDistr::Exception);
    BOOST_CHECK_THROW(ed.cdf(0.5), EmpirDistr::Exception);
    
    // clean but empty
    ed.evaluate();
    try {
        ed.mean();
    } catch (const EmpirDistr::Exception& exc) {
        BOOST_CHECK_EQUAL(exc.error_message(), "mean(): empty");
    }
    
    // adding one value is still not enough for variance calc
    ed.add(3.14);
    ed.evaluate();
    BOOST_CHECK_NO_THROW(ed.mean());
    try {
        ed.variance();
    } catch (const EmpirDistr::Exception& exc) {
        BOOST_CHECK_EQUAL(exc.error_message(), "variance(): not enough data");
    }
    
    // adding another is now OK
    ed.add(2.71);
    ed.evaluate();
    BOOST_CHECK_NO_THROW(ed.variance());
}

// uniform distribution with values 1,2,..,6
BOOST_AUTO_TEST_CASE(dice_test)
{
    EmpirDistr ed(0);    // ncell will be set to 100
    for (unsigned int i=0; i<dice.size(); ++i)
        ed.add(dice[i]);
    ed.evaluate();
    BOOST_WARN_CLOSE(ed.low(), 1.0, PCT_TOL); // within PCT_TOL %
    BOOST_WARN_CLOSE(ed.high(), 6.0, PCT_TOL);
    BOOST_WARN_CLOSE(ed.mean(), 3.5, PCT_TOL);
    BOOST_WARN_CLOSE(ed.variance(), 2.92, PCT_TOL);    // Mathematica says 7/2 though...
}

// uniform distribution coming from a random number generator
// this is a really approximate test... 
// note that it tests the uniform Rng in randomgen.hh as well
BOOST_AUTO_TEST_CASE(unirnd_test)
{
    // set up the RNG
    const double UNIMIN = 7.0, UNIMAX = 12.0, UNIWIDTH = UNIMAX-UNIMIN;
    UniformGen uni(42u, UNIMIN, UNIMAX);

    EmpirDistr ed(NCELL);
    for (unsigned int i=0; i<NTRIAL; ++i)
    {
        double x = uni();
        ed.add(x);
    }
    ed.evaluate();
    
    BOOST_WARN_CLOSE(ed.low(), UNIMIN, PCT_TOL);
    BOOST_WARN_CLOSE(ed.high(), UNIMAX, PCT_TOL);
    BOOST_WARN_CLOSE(ed.mean(), (UNIMIN+UNIMAX)/2.0, PCT_TOL);
    BOOST_WARN_CLOSE(ed.variance(), (UNIWIDTH*UNIWIDTH)/12.0, PCT_TOL);
    
    // test CDF approximations
    BOOST_WARN_SMALL(ed.cdf(UNIMIN), ABS_TOL);
    const unsigned int STEPNO=17;
    const double XSTEP = UNIWIDTH/STEPNO;
    double x = UNIMIN;
    for (unsigned int xi = 1; xi < STEPNO; ++xi)
    {
        x += XSTEP;
        double yfrac = static_cast<double>(xi)/STEPNO;
        BOOST_WARN_CLOSE(ed.cdf(x), yfrac, PCT_TOL);
    }
    BOOST_WARN_CLOSE(ed.cdf(UNIMAX), 1.0, PCT_TOL);
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

    EmpirDistr ed(NCELL);
    for (unsigned int i=0; i<NTRIAL; ++i)
    {
        double x = norm();
        ed.add(x);
    }
    ed.evaluate();
    
    // cannot reliably test low() and high()
    BOOST_WARN_CLOSE(ed.mean(), EMEAN, PCT_TOL);
    BOOST_WARN_CLOSE(ed.variance(), EDEV*EDEV, PCT_TOL);
    BOOST_WARN_CLOSE(ed.std_dev(), EDEV, PCT_TOL);
    
    // scan a range of +/- Z S.D of the CDF in 0.1 S.D. steps
    const double Z = 2.0, XSTEP = 0.1*EDEV;
    for (double x = EMEAN - Z*EDEV; x <= EMEAN + Z*EDEV; x+=XSTEP)
    {
        double yexp = norm_cdf(EMEAN, EDEV, x);
        BOOST_WARN_CLOSE(ed.cdf(x), yexp, PCT_TOL);
    }
}

BOOST_AUTO_TEST_SUITE_END()
