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

#include <cmath>
#include <iostream>
#include <string>

// -- Fixtures... --

// NOTE: these tolerances have been titrated manually, they are quite generous,
// but there's no exact way of testing random sampling...
// this test is meant to filter out really horrible errors.
const double PCT_TOL=2.5,   // percentage tolerance for floating-point checks
    ABS_TOL=0.01;          // ... and absolute tolerance for values that should be 0.0

static void check_0(const std::string& what, double x, double tol = ABS_TOL)
{
    std::cerr << what << ": " << x << std::endl;
    BOOST_CHECK(std::fabs(x) < tol);
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
    EmpirDistr ed(1.0); // dirty state
    BOOST_CHECK_THROW(ed.low(), EmpirDistr::DirtyException);
    BOOST_CHECK_THROW(ed.high(), EmpirDistr::DirtyException);
    BOOST_CHECK_THROW(ed.mean(), EmpirDistr::DirtyException);
    BOOST_CHECK_THROW(ed.variance(), EmpirDistr::DirtyException);
    BOOST_CHECK_THROW(ed.pdf(1.0), EmpirDistr::DirtyException);
    BOOST_CHECK_THROW(ed.cdf(0.5), EmpirDistr::DirtyException);
    BOOST_CHECK_THROW(ed.inv_cdf(0.5), EmpirDistr::DirtyException);
}

// uniform distribution with values 1,2,..,6
BOOST_AUTO_TEST_CASE(dice_test)
{
    EmpirDistr ed(0.1);
    for (unsigned int i=0; i<dice.size(); ++i)
        ed.add(dice[i]);
    ed.evaluate();
    BOOST_CHECK_CLOSE(ed.low(), 0.95, PCT_TOL); // within PCT_TOL %
    BOOST_CHECK_CLOSE(ed.high(), 6.05, PCT_TOL);
    BOOST_CHECK_CLOSE(ed.mean(), 3.5, PCT_TOL);
    BOOST_CHECK_CLOSE(ed.variance(), 2.92, PCT_TOL);    // Mathematica says 7/2 though...
}

// uniform distribution coming from a random number generator
// this is a really approximate test... 
// note that it tests the uniform Rng in randomgen.hh as well
BOOST_AUTO_TEST_CASE(unirnd_test)
{
    // set up the RNG
    const double UNIMIN = 7.0, UNIMAX = 12.0, UNIWIDTH = UNIMAX-UNIMIN;
    UniformGen uni(42u, UNIMIN, UNIMAX);

    const double BINWIDTH = 0.1;
    EmpirDistr ed(BINWIDTH);
    for (unsigned int i=0; i<10000; ++i)
    {
        double x = uni();
        ed.add(x);
    }
    ed.evaluate();
    
    BOOST_CHECK_CLOSE(ed.low(), UNIMIN-BINWIDTH, PCT_TOL);
    BOOST_CHECK_CLOSE(ed.high(), UNIMAX+BINWIDTH, PCT_TOL);
    BOOST_CHECK_CLOSE(ed.mean(), (UNIMIN+UNIMAX)/2.0, PCT_TOL);
    BOOST_CHECK_CLOSE(ed.variance(), (UNIWIDTH*UNIWIDTH)/12.0, PCT_TOL);
    
    // test PDF and CDF approximations
    // at UNIMIN and UNIMAX positions, the PDF should be half of the uniform values
    // within the interval due to the "trapezoidal" approximation, hence 0.5/UNIWIDTH
    BOOST_CHECK_CLOSE(ed.pdf(UNIMIN), 0.5/UNIWIDTH, PCT_TOL);
    check_0("CDF(UNIMIN)", ed.cdf(UNIMIN));
    BOOST_CHECK_CLOSE(ed.inv_cdf(0.0), UNIMIN-BINWIDTH, PCT_TOL);
    const unsigned int STEPNO=7;
    const double XSTEP = UNIWIDTH/STEPNO;
    double x = UNIMIN;
    for (unsigned int xi = 1; xi < STEPNO; ++xi)
    {
        x += XSTEP;
        BOOST_CHECK_CLOSE(ed.pdf(x), 1.0/UNIWIDTH, PCT_TOL);
        double yfrac = static_cast<double>(xi)/STEPNO;
        BOOST_CHECK_CLOSE(ed.cdf(x), yfrac, PCT_TOL);
        BOOST_CHECK_CLOSE(ed.inv_cdf(yfrac), x, PCT_TOL);
    }
    BOOST_CHECK_CLOSE(ed.pdf(UNIMAX), 0.5/UNIWIDTH, PCT_TOL);
    BOOST_CHECK_CLOSE(ed.cdf(UNIMAX), 1.0, PCT_TOL);
    BOOST_CHECK_CLOSE(ed.inv_cdf(1.0), UNIMAX+BINWIDTH, PCT_TOL);
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

    const double BINWIDTH = 0.1;
    EmpirDistr ed(BINWIDTH);
    for (unsigned int i=0; i<10000; ++i)
    {
        double x = norm();
        ed.add(x);
    }
    ed.evaluate();
    
    // cannot reliably test low() and high()
    BOOST_CHECK_CLOSE(ed.mean(), EMEAN, PCT_TOL);
    BOOST_CHECK_CLOSE(ed.variance(), EDEV*EDEV, PCT_TOL);
}

BOOST_AUTO_TEST_SUITE_END()