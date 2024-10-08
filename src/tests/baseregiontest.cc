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
#define BOOST_TEST_MODULE baseregiontest
#include "boost/test/unit_test.hpp"

// -- Own headers --

#include "multovl/baseregion.hh"
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

struct BaseRegionFixture
{
    BaseRegionFixture():
        r_empty(), 
        r15(1, 5, '+', "r15"),
        r46(6, 4, '-', "r46")  // checks limit swapping
    {}
    
    BaseRegion r_empty, r15, r46;
};

static
std::string reg_tostr(const BaseRegion& reg)
{
    std::string s = boost::lexical_cast<std::string>(reg.first()) + ":"
        + boost::lexical_cast<std::string>(reg.last()) + ":"
        + reg.strand() + reg.name();
    return s;
}

BOOST_FIXTURE_TEST_SUITE(baseregionsuite, BaseRegionFixture)

BOOST_AUTO_TEST_CASE(empty_test)
{
    BOOST_CHECK_EQUAL(r_empty.first(), 0);
    BOOST_CHECK_EQUAL(r_empty.last(), 0);
    BOOST_CHECK_EQUAL(r_empty.strand(), '.');
    BOOST_CHECK_EQUAL(r_empty.name(), "");
    BOOST_CHECK(r_empty.is_empty());
}

BOOST_AUTO_TEST_CASE(length_test)
{
    BOOST_CHECK_EQUAL(r_empty.length(), 0);
    BOOST_CHECK_EQUAL(r15.length(), 5);
}

// Instead of separately testing the < operator,
// we just build a vector of Coord objects in descending order,
// then sort into ascending order and then test.
BOOST_AUTO_TEST_CASE(coordsort_test)
{
    std::vector<BaseRegion> desc;
    boost::assign::push_back(desc)
        (BaseRegion(3, 5, '-',"z"))
        (BaseRegion(3, 6, '-', "foo"))
        (BaseRegion(3, 6, '-', "aardvark"))
        (BaseRegion(2, 3, '.',"z"))
        (BaseRegion(2, 3, '-',"z"))
        (BaseRegion(2, 3, '+',"z"))
        (BaseRegion(1, 2, '+',"z"));
    std::vector<BaseRegion> asc = desc;
    std::sort(asc.begin(), asc.end());
    
    std::vector<BaseRegion>::iterator fwdit = asc.begin();
    std::vector<BaseRegion>::reverse_iterator revit = desc.rbegin();
    for ( ; fwdit != asc.end() && revit != desc.rend(); fwdit++, revit++)
    {
        BOOST_CHECK_EQUAL( reg_tostr(*fwdit), reg_tostr(*revit) );
    }
}

BOOST_AUTO_TEST_CASE(name_test)
{
    std::string oldname = r15.name("newr15");
    BOOST_CHECK_EQUAL(oldname, "r15");
    BOOST_CHECK_EQUAL("newr15", r15.name());
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
        BaseRegion inr;
        ia >> inr;
        BOOST_CHECK_EQUAL(reg_tostr(inr), reg_tostr(r15));
    }
}

BOOST_AUTO_TEST_SUITE_END()
