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
    // ctor
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
    
    // count in which free region a given region /reg/ falls
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
    
    // calculate the fraction of region counts relative to the total
    double frac(const std::string& regname) const
    {
        double cnt = static_cast<double>(counts.find(regname)->second);
        return cnt/total;
    }
    
    // tester function
    void tester(const FreeRegions& fr, RandomPlacer& rp)
    {
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
    
    // data
    UniformGen rng;
    std::vector<Region> frees, regs;
    std::map<std::string, unsigned int> counts;
    unsigned int total;
};

BOOST_FIXTURE_TEST_SUITE(randomplacersuite, RandomPlacerFixture)

BOOST_AUTO_TEST_CASE(randomplacer1_test)
{
    FreeRegions fr(frees);
    RandomPlacer rp;  // default init...
    
    // ...and then add the regions one by one
    BOOST_FOREACH(Region r, regs)
    {
        rp.add(r.length());
    }
    
    tester(fr, rp);
}

BOOST_AUTO_TEST_CASE(randomplacer2_test)
{
    FreeRegions fr(frees);
    RandomPlacer rp(regs);  // init with an array of regions
    
    tester(fr, rp);
}

BOOST_AUTO_TEST_SUITE_END()
