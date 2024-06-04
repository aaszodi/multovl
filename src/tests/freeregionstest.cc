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
        frees{ 
            {100, 199, '+', "reg100"},  // len=100
            {200, 249, '+', "reg50"},  // len=50
            {300, 329, '+', "reg30"},  // len=30
            {400, 419, '+', "reg20"}  // len=20
        },
        fr(frees),
        counts{
            {"reg100", 0},
            {"reg50", 0},
            {"reg30", 0},
            {"reg20", 0},
        },
        total(0)
    { }
    
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
    std::vector<BaseRegion> frees;
    FreeRegions fr;
    std::map<std::string, unsigned int> counts;
    unsigned int total;
};

BOOST_FIXTURE_TEST_SUITE(freeregionssuite, FreeRegionsFixture)

BOOST_AUTO_TEST_CASE(maxlen_test)
{
    BOOST_CHECK_EQUAL(fr.max_freelen(), 100);
    // will not select free region if the length is too much
    try {
        const auto& nosuch = fr.select_free_region(rng, 9965);
        BOOST_CHECK(false); // must not land here
    } catch(const std::length_error& err) {
        BOOST_CHECK_EQUAL(err.what(), "Region will not fit");
    }
}

BOOST_AUTO_TEST_CASE(fit_test)
{
    BOOST_CHECK(fr.fit(BaseRegion(110, 120, '+', "fit1")));
    BOOST_CHECK(fr.fit(BaseRegion(230, 240, '+', "fit2")));
    BOOST_CHECK(fr.fit(BaseRegion(300, 329, '+', "xfit3")));  // "exact" fit, i.e. equality
    BOOST_CHECK(fr.fit(BaseRegion(401, 410, '+', "fit4")));

    BOOST_CHECK(!fr.fit(BaseRegion(10, 8500, '+', "too_long")));
    BOOST_CHECK(!fr.fit(BaseRegion(10, 20, '+', "before")));
    BOOST_CHECK(!fr.fit(BaseRegion(50, 120, '+', "beforeovl")));
    BOOST_CHECK(!fr.fit(BaseRegion(130, 220, '+', "twoovl")));
    BOOST_CHECK(!fr.fit(BaseRegion(270, 280, '+', "between")));
    BOOST_CHECK(!fr.fit(BaseRegion(410, 430, '+', "afterovl")));
    BOOST_CHECK(!fr.fit(BaseRegion(500, 600, '+', "after")));
}

BOOST_AUTO_TEST_CASE(nominlen_test)
{
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
