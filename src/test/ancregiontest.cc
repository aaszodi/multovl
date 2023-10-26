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
#define BOOST_TEST_MODULE ancregiontest
#include "boost/test/unit_test.hpp"

// -- Own headers --

#include "ancregion.hh"
using namespace multovl;
#include "tempfile.hh"  // temporary file utility

// -- Boost headers --

#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"

// -- Standard headers --

#include <fstream>

struct AncRegionFixture
{
    AncRegionFixture():
        a1(Region(1, 5, '+', "a15")),   // track id = 0
        a2(4, 6, '-', "a46", 9, false)  // non-default track ID and shuffleability
    {}
    
    AncestorRegion a1, a2;
};

BOOST_FIXTURE_TEST_SUITE(ancregionsuite, AncRegionFixture)

BOOST_AUTO_TEST_CASE(trackid_test)
{
    // testing the track ID changes
    unsigned int oldid = a1.track_id(17);
    BOOST_CHECK_EQUAL(oldid, 0);
    BOOST_CHECK_EQUAL(a1.track_id(), 17);
    BOOST_CHECK_EQUAL(a2.track_id(), 9);
}

BOOST_AUTO_TEST_CASE(shuffattr_test)
{
    BOOST_CHECK(a1.is_shuffleable());
    BOOST_CHECK(!a2.is_shuffleable());
}

BOOST_AUTO_TEST_CASE(io_test)
{
    // testing the I/O
    std::string out = a2.to_attrstring();
    BOOST_CHECK_EQUAL(out, "9:a46:-:4-6");
}

BOOST_AUTO_TEST_CASE(ancregionser_test)
{
    Tempfile tempfile;
    {
        std::ofstream ofs(tempfile.name());
        boost::archive::text_oarchive oa(ofs);
        oa << a2;
    }
    
    {
        std::ifstream ifs(tempfile.name());
        boost::archive::text_iarchive ia(ifs);
        AncestorRegion inar;
        ia >> inar;
        std::string out = inar.to_attrstring();
        BOOST_CHECK_EQUAL(out, "9:a46:-:4-6");
    }
}

BOOST_AUTO_TEST_SUITE_END()
