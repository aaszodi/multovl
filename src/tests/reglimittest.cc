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
#define BOOST_TEST_MODULE reglimittest
#include "boost/test/unit_test.hpp"

// -- Own headers --

#include "multovl/reglimit.hh"
using namespace multovl;
#include "tempfile.hh"  // temporary file utility

// -- Boost headers --

#include "boost/lexical_cast.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"

// -- Standard headers --

#include <fstream>

// -- Fixture

struct ReglimitFixture
{
    ReglimitFixture():
        anc(4, 6, '-', "a46", 9)
    {}
    
    AncestorRegion anc;
};

BOOST_FIXTURE_TEST_SUITE(reglimitsuite, ReglimitFixture)

BOOST_AUTO_TEST_CASE(reglimit_test)
{
    RegLimit rf(anc, true), rl(anc, false);
    
    BOOST_CHECK_EQUAL(rf.region().to_attrstring(), "9:a46:-:4-6");
    BOOST_CHECK_EQUAL(rf.track_id(), 9);
    BOOST_CHECK(rf.is_first());
    BOOST_CHECK_EQUAL(rf.this_pos(), 4);
    BOOST_CHECK_EQUAL(rf.other_pos(), 6);
    
    BOOST_CHECK_EQUAL(rl.region().to_attrstring(), "9:a46:-:4-6");
    BOOST_CHECK_EQUAL(rl.track_id(), 9);
    BOOST_CHECK(!rl.is_first());
    BOOST_CHECK_EQUAL(rl.this_pos(), 6);
    BOOST_CHECK_EQUAL(rl.other_pos(), 4);
    
    BOOST_CHECK(rf < rl);
}

BOOST_AUTO_TEST_CASE(reglimit_extension_test)
{
    Region::set_extension(2);
    RegLimit rf(anc, true), rl(anc, false);
    
    BOOST_CHECK_EQUAL(rf.region().to_attrstring(), "9:a46:-:2-8");
    BOOST_CHECK_EQUAL(rf.this_pos(), 2);
    BOOST_CHECK_EQUAL(rf.other_pos(), 8);
    
    BOOST_CHECK_EQUAL(rl.region().to_attrstring(), "9:a46:-:2-8");
    BOOST_CHECK_EQUAL(rl.this_pos(), 8);
    BOOST_CHECK_EQUAL(rl.other_pos(), 2);
    BOOST_CHECK(rf < rl);
    Region::set_extension(0);
}

BOOST_AUTO_TEST_SUITE_END()

