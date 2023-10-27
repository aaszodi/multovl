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
#define BOOST_TEST_MODULE regiontest
#include "boost/test/unit_test.hpp"

// -- Own headers --

#include "region.hh"
using namespace multovl;
#include "tempfile.hh"  // temporary file utility

// -- Standard headers --

#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>

// -- other Boost headers --

#include "boost/assign/list_inserter.hpp"   // for insert() ()()()...()
#include "boost/lexical_cast.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"

// -- Fixture

struct RegionFixture
{
    RegionFixture():
        r_empty(), 
        r15(1, 5, '+', "r15"),
        r46(6, 4, '-', "r46")  // checks limit swapping
    {}
    
    Region r_empty, r15, r46;
};

static
std::string reg_tostr(const Region& reg)
{
    std::string s = boost::lexical_cast<std::string>(reg.first()) + ":"
        + boost::lexical_cast<std::string>(reg.last()) + ":"
        + reg.strand() + reg.name();
    return s;
}

BOOST_FIXTURE_TEST_SUITE(regionsuite, RegionFixture)

// same as BaseRegion
BOOST_AUTO_TEST_CASE(empty_test)
{
    BOOST_CHECK(r_empty.first() == 0);
    BOOST_CHECK(r_empty.last() == 0);
    BOOST_CHECK(r_empty.is_empty());
}

// same as BaseRegion
BOOST_AUTO_TEST_CASE(length_test)
{
    BOOST_CHECK(r_empty.length() == 0);
    BOOST_CHECK(r15.length() == 5);
}

BOOST_AUTO_TEST_CASE(extension_test)
{
    // by default the extension is 0
    BOOST_CHECK_EQUAL(Region::extension(), 0);
    // change it
    Region::set_extension(3);
    BOOST_CHECK_EQUAL(r15.first(), 0); // because extension > first
    BOOST_CHECK_EQUAL(r15.last(), 8);
    BOOST_CHECK_EQUAL(r15.length(), 9);
    BOOST_CHECK_EQUAL(r46.first(), 1); // here `extension` could be subtracted
    BOOST_CHECK_EQUAL(r46.last(), 9);
    BOOST_CHECK_EQUAL(r46.length(), 9);
    // change the extended coordinates
    // 5 -- 8 ======= 16 -- 19
    r46.set_coords(5, 19);
    BOOST_CHECK_EQUAL(r46.first(), 5);
    BOOST_CHECK_EQUAL(r46.last(), 19);
    // set extension back to 0
    Region::set_extension(0);
    BOOST_CHECK_EQUAL(r46.first(), 8);  // `set_coords` stored the non-extended "true" coordinates
    BOOST_CHECK_EQUAL(r46.last(), 16);
}

BOOST_AUTO_TEST_CASE(regionser_test)
{
    Tempfile tempfile;
    {
        std::ofstream ofs(tempfile.name());
        boost::archive::text_oarchive oa(ofs);
        oa << r15;
    }
    
    {
        std::ifstream ifs(tempfile.name());
        boost::archive::text_iarchive ia(ifs);
        Region inr;
        ia >> inr;
        BOOST_CHECK_EQUAL(reg_tostr(inr), reg_tostr(r15));
    }
}

BOOST_AUTO_TEST_SUITE_END()
