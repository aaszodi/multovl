/* <LICENSE>
License for the MULTOVL multiple genomic overlap tools

Copyright (c) 2007-2012, Dr Andras Aszodi, 
Campus Science Support Facilities GmbH (CSF),
Dr-Bohr-Gasse 3, A-1030 Vienna, Austria, Europe.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of the Campus Science Support Facilities GmbH
      nor the names of its contributors may be used to endorse
      or promote products derived from this software without specific prior
      written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS
AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
</LICENSE> */
#define BOOST_TEST_MODULE empirdistrtest
#include "boost/test/unit_test.hpp"
#include "boost/test/floating_point_comparison.hpp"

// -- own headers --

#include "multovl/prob/empirdistr.hh"
#include "multovl/prob/randomgen.hh" // note: this is implicitly tested here
using namespace multovl;
using namespace prob;

// -- Boost headers --

// -- standard headers --

#include <cmath>
#ifdef __SUNPRO_CC
#include <math.h>
#endif

#include <iostream>
#include <string>
#include <random>

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
    return ((1.0 + std::erf(t))/2.0);
}

struct EmpirDistrFixture
{
    EmpirDistrFixture()
    {
        for (unsigned int i=0; i<6; ++i)
            dice.push_back(static_cast<double>(i+1));
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
    BOOST_CHECK_EQUAL(ed.low(), 3.14);
    BOOST_CHECK_EQUAL(ed.high(), 3.14);
    BOOST_CHECK_EQUAL(ed.cdf(3.1), 0.0);
    BOOST_CHECK_EQUAL(ed.cdf(3.14), 1.0);
    BOOST_CHECK_EQUAL(ed.cdf(3.2), 1.0);
    
    // adding another is now OK
    ed.add(2.71);
    ed.evaluate();
    BOOST_CHECK_NO_THROW(ed.variance());
    BOOST_CHECK_EQUAL(ed.low(), 2.71);
    BOOST_CHECK_EQUAL(ed.high(), 3.14);
    BOOST_CHECK_EQUAL(ed.cdf(2.0), 0.0);
    // gives around 0.86: BOOST_CHECK_EQUAL(ed.cdf(2.71), 0.0);
    BOOST_CHECK_EQUAL(ed.cdf(3.14), 1.0);
    BOOST_CHECK_EQUAL(ed.cdf(3.2), 1.0);
    
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
    // set up the RNG
    std::mt19937 rng(42u);
    const double EMEAN=3.2, EDEV=1.7;
    std::normal_distribution<> normdistr(EMEAN, EDEV);

    EmpirDistr ed(NCELL);
    for (unsigned int i=0; i<NTRIAL; ++i)
    {
        double x = normdistr(rng);
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
