#define BOOST_TEST_MODULE empirdistrtest
#include "boost/test/unit_test.hpp"
#include "boost/test/floating_point_comparison.hpp"

// -- own headers --

#include "stat.hh"
using namespace multovl;
using namespace prob;

// -- Boost headers --

// we test with normal random deviates
#include "boost/random/mersenne_twister.hpp"
#include "boost/random/variate_generator.hpp"
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
    Stat st;
    
    st.add(2, 17, true);
    st.add(3, 112, true);
    st.add(5, 18, true);
    
    BOOST_CHECK_EQUAL(st.distr(2).actual(), 17);
    BOOST_CHECK_EQUAL(st.distr(3).actual(), 112);
    BOOST_CHECK_THROW(st.distr(4), Stat::NotfoundException);  // no such multiplicity
    BOOST_CHECK_EQUAL(st.distr(5).actual(), 18);
    
    // if we evaluate now, the results will be invalid
    st.evaluate();
    BOOST_CHECK(!st.distr(2).is_valid());
    BOOST_CHECK(!st.distr(3).is_valid());
    BOOST_CHECK(!st.distr(5).is_valid());
    
    // multiplicity range
    BOOST_CHECK_EQUAL(st.min_mult(), 2);
    BOOST_CHECK_EQUAL(st.max_mult(), 5);
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
    // fill up nulldistr for multiplicities 3 and 5
    for (unsigned int i=0; i<NTRIAL; ++i)
    {
        double x;
        x = norm();
        st.add(3, x, false);
        x = norm();
        st.add(5, x, false);
    }
    // add actuals -/+ 1 SD from mean
    st.add(3, EMEAN-EDEV, true);
    st.add(5, EMEAN+EDEV, true);
    st.evaluate();
    
    // enough data, Distr-s are valid
    BOOST_CHECK(st.distr(3).is_valid());
    BOOST_CHECK(st.distr(5).is_valid());
    
    // test "p-values" against the CDF
    BOOST_WARN_CLOSE(st.distr(3).p_value(), norm_cdf(EMEAN, EDEV, EMEAN-EDEV), PCT_TOL);
    BOOST_WARN_CLOSE(1.0 - st.distr(5).p_value(), norm_cdf(EMEAN, EDEV, EMEAN+EDEV), PCT_TOL);
   
    // test "Z-scores"
    BOOST_WARN_CLOSE(st.distr(3).z_score(), -1.0, PCT_TOL);
    BOOST_WARN_CLOSE(st.distr(5).z_score(), +1.0, PCT_TOL);
 }

BOOST_AUTO_TEST_SUITE_END()
