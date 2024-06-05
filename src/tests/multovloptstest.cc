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
#define BOOST_TEST_MODULE multovloptstest
#include "boost/test/unit_test.hpp"

// -- Standard headers --

#include <string>
#include <iostream>
using namespace std;

// -- Boost headers --

#include "boost/lexical_cast.hpp"

// -- Own header --

#include "multovl/multovlopts.hh"
using namespace multovl;

// gets rid of annoying "deprecated conversion from string constant blah blah" warning
#pragma GCC diagnostic ignored "-Wwrite-strings"

// -c option
BOOST_AUTO_TEST_CASE(c_test)
{
    const int ARGC = 9;
    char *ARGV[] = { "multovloptstest", "-c", "2", "-L", "50", "-u", "-M", "3", "-n" };
    
    MultovlOptbase opt;
    bool ok = opt.parse_check(ARGC, ARGV);
    BOOST_CHECK(ok);
    BOOST_CHECK_EQUAL(opt.minmult(), 2);
    BOOST_CHECK_EQUAL(opt.maxmult(), 2);
    BOOST_CHECK_EQUAL(opt.ovlen(), 50);
    BOOST_CHECK(opt.uniregion());
    BOOST_CHECK(opt.nointrack());
}

// -c 1 solitary
BOOST_AUTO_TEST_CASE(c1_test)
{
    const int ARGC = 6;
    char *ARGV[] = { "multovloptstest", "-c", "1", "-L", "50", "-u" };
    
    MultovlOptbase opt;
    bool ok = opt.parse_check(ARGC, ARGV);
    BOOST_CHECK(ok);
    BOOST_CHECK_EQUAL(opt.minmult(), 1);
    BOOST_CHECK_EQUAL(opt.maxmult(), 1);
    BOOST_CHECK_EQUAL(opt.ovlen(), 1);
    BOOST_CHECK(!opt.uniregion());
}

// -m -M defaults
BOOST_AUTO_TEST_CASE(M0_test)
{
    const int ARGC = 6;
    char *ARGV[] = { "multovloptstest", "-M", "0", "-L", "50", "-n" };
    
    MultovlOptbase opt;
    bool ok = opt.parse_check(ARGC, ARGV);
    BOOST_CHECK(ok);
    BOOST_CHECK_EQUAL(opt.minmult(), 2);
    BOOST_CHECK_EQUAL(opt.maxmult(), 0);    // i.e. up to anything
    BOOST_CHECK_EQUAL(opt.ovlen(), 50);
    BOOST_CHECK(opt.nointrack());
}

// -m -M general case
BOOST_AUTO_TEST_CASE(mM_test)
{
    const int ARGC = 5;
    char *ARGV[] = { "multovloptstest", "-M", "7", "-m", "3" };
    
    MultovlOptbase opt;
    bool ok = opt.parse_check(ARGC, ARGV);
    BOOST_CHECK(ok);
    BOOST_CHECK_EQUAL(opt.minmult(), 3);
    BOOST_CHECK_EQUAL(opt.maxmult(), 7);
    BOOST_CHECK_EQUAL(opt.ovlen(), 1);  // default
}

// silly -m -M combination
BOOST_AUTO_TEST_CASE(bad_mM_test)
{
    const int ARGC = 5;
    char *ARGV[] = { "multovloptstest", "-M", "3", "-m", "7" };
    
    MultovlOptbase opt;
    bool ok = opt.parse_check(ARGC, ARGV);
    BOOST_CHECK(!ok);
    BOOST_CHECK_EQUAL(
        opt.error_messages(),
        "ERROR: The combination -m 7 -M 3 makes no sense\n"
    );
}

#pragma GCC diagnostic pop
