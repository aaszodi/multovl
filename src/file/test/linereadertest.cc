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
#define BOOST_TEST_MODULE linereadertest
#include "boost/test/unit_test.hpp"

// Tests the Linereader ==> region read functionality.
// Consider running as linereadertest --log_level=message
// to see which class is being tested.
// For line parsing, see 'lineparsertest.cc'
// 2011-01-28 AA

#include <string>
#include <limits>

#include "linereader.hh"
using namespace multovl;

struct LinereaderFixture
{
    LinereaderFixture():
        region(10, 20, '+', "region")
    {}
    
    BaseRegion region;
};

BOOST_FIXTURE_TEST_SUITE(linereadersuite, LinereaderFixture)

BOOST_AUTO_TEST_CASE(bedreader_test)
{
    io::BedLinereader blr;
    
    BaseRegion reg;
    io::Linereader::Status status;
    bool ok;

    status = blr.parse("chr1\t10\t20\tregion\t0.8\t+");
    BOOST_CHECK(status == io::Linereader::DATA);
    BOOST_CHECK_EQUAL(blr.chrom(), "chr1");
    ok = blr.read_into(reg);
    BOOST_CHECK(ok);
    BOOST_CHECK(region == reg);
    
    // some parsing errors
    // these are the same for GFF, tested only here
    status = blr.parse("chr1\tFOO\t20\tregion\t0.8\t+");
    BOOST_CHECK(status == (io::Linereader::ERROR | io::Linereader::DATA));
    BOOST_CHECK_EQUAL(blr.error_msg(), "\"FOO\": cannot parse to unsigned int");
    
    status = blr.parse("chr1\t-99\t20\tregion\t0.8\t+");
    BOOST_CHECK(status == (io::Linereader::ERROR | io::Linereader::DATA));
    BOOST_CHECK_EQUAL(blr.error_msg(), "\"-99\": must not be negative");
}

BOOST_AUTO_TEST_CASE(gffreader_test)
{
    io::GffLinereader glr;
    
    BaseRegion reg;
    io::Linereader::Status status;
    bool ok;

    status = glr.parse("chr1\tsrc\tregion\t10\t20\t0.8\t+");
    BOOST_CHECK(status == io::Linereader::DATA);
    BOOST_CHECK_EQUAL(glr.chrom(), "chr1");
    ok = glr.read_into(reg);
    BOOST_CHECK(ok);
    BOOST_CHECK(region == reg);
}

BOOST_AUTO_TEST_SUITE_END()
