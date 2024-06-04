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
#define BOOST_TEST_MODULE multiregiontest
#include "boost/test/unit_test.hpp"

// -- Own headers --

#include "multiregion.hh"
#include "region.hh"
using namespace multovl;
#include "tempfile.hh"  // temporary file utility

// -- Boost headers --

#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"

// -- Standard headers --

#include <vector>
#include <algorithm>    // for equal
#include <fstream>

// -- Fixture --

struct MultiRegionFixture
{
    MultiRegionFixture():
        a1(Region(1, 5, '+', "a15")),   // track id = 0
        a2(4, 6, '-', "a46", 9),
        mr(2, 5)
    {
        aset1.insert(a1);
        aset2.insert(a1); aset2.insert(a2);
        mr.add_ancestor(a1);
    }
    
    AncestorRegion a1, a2;
    ancregset_t aset1, aset2;
    MultiRegion mr;
};

BOOST_FIXTURE_TEST_SUITE(multiregionsuite, MultiRegionFixture)

BOOST_AUTO_TEST_CASE(special_multiregion_test)
{
    // test empty
    MultiRegion empty;
    BOOST_CHECK_EQUAL(empty.multiplicity(), 0);
    BOOST_CHECK(!empty.solitary());
    BOOST_CHECK_EQUAL(empty.anc_str(), "");
    BOOST_CHECK_EQUAL(empty.strand(), '.');
    BOOST_CHECK_EQUAL(empty.name(), "overlap");
    
    // test solitary
    MultiRegion sol(1, 5, aset1);
    BOOST_CHECK_EQUAL(sol.multiplicity(), 1);
    BOOST_CHECK(sol.solitary());
    BOOST_CHECK_EQUAL(sol.anc_str(), "0:a15:+:1-5");
}

BOOST_AUTO_TEST_CASE(multiregion_test)
{
    // mr is non-solitary: ancestor has different coords
    BOOST_CHECK(!mr.solitary());
    
    // ancestor add/retrieve
    mr.add_ancestor(a2);
    BOOST_CHECK_EQUAL(mr.multiplicity(), 2);
    BOOST_CHECK_EQUAL(mr.anc_str(), "0:a15:+:1-5|9:a46:-:4-6");
    const ancregset_t& as = mr.ancestors();
    BOOST_CHECK(std::equal(as.begin(), as.end(), aset2.begin()));
    
    // ancestor IDs
    const int ancids[] = {0,9,9};
    const std::vector<int> ais = mr.ancestor_trackids();
    BOOST_CHECK(std::equal(ancids, ancids+2, ais.begin()));  // compare to {0,9}

    // add ancestor a2 again
    mr.add_ancestor(a2);
    BOOST_CHECK_EQUAL(mr.multiplicity(), 3);
    BOOST_CHECK_EQUAL(mr.anc_str(), "0:a15:+:1-5|2*9:a46:-:4-6");
    const std::vector<int> ais3 = mr.ancestor_trackids();
    BOOST_CHECK(std::equal(ancids, ancids+3, ais3.begin())); // compare to {0,9,9}
}

BOOST_AUTO_TEST_CASE(multiregion_serialization_test)
{
    mr.add_ancestor(a2);    // 2 ancestors
    Tempfile tempfile;
    {
        std::ofstream ofs(tempfile.name());
        boost::archive::text_oarchive oa(ofs);
        oa << mr;
    }
    
    {
        std::ifstream ifs(tempfile.name());
        boost::archive::text_iarchive ia(ifs);
        MultiRegion inmr;
        ia >> inmr;
        BOOST_CHECK_EQUAL(inmr.first(), 2);
        BOOST_CHECK_EQUAL(inmr.last(), 5);
        BOOST_CHECK_EQUAL(inmr.multiplicity(), 2);
        BOOST_CHECK_EQUAL(inmr.anc_str(), "0:a15:+:1-5|9:a46:-:4-6");
    }
    
}

BOOST_AUTO_TEST_SUITE_END()
