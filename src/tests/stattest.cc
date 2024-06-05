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

#include "multovl/prob/stat.hh"
using namespace multovl;
using namespace prob;

// -- standard headers --

// NOTE: we boldly assume <cmath> provides the erf() function as per ISO/IEC 9899:1999(E).
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

const double PCT_TOL=2.5;   // percentage tolerance for floating-point checks
const unsigned int NTRIAL=10000;    // number of random trials

static double norm_cdf(double m, double s, double x)
{
    static const double SQRT2 = std::sqrt(2.0);
    double t = (x - m)/(SQRT2*s);
    return ((1.0 + std::erf(t))/2.0);
}

BOOST_AUTO_TEST_SUITE(statsuite)

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
    // set up the RNG
    std::mt19937 rng(42u);
    const double EMEAN=3.2, EDEV=1.7;
    std::normal_distribution<> normdistr(EMEAN, EDEV);

    Stat st;
    // fill up nulldistr for multiplicities 3 and 5
    for (unsigned int i=0; i<NTRIAL; ++i)
    {
        double x;
        x = normdistr(rng);
        st.add(3, x, false);
        x = normdistr(rng);
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
