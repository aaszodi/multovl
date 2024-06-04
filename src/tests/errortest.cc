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
#define BOOST_TEST_MODULE errortest
#include "boost/test/unit_test.hpp"

// -- own header --

#include "errors.hh"
using namespace multovl;

// -- standard --

#include <sstream>

BOOST_AUTO_TEST_SUITE(errorsuite)

BOOST_AUTO_TEST_CASE(error_test)
{
    Errors err;
    BOOST_CHECK(err.ok());
    BOOST_CHECK(err.perfect());
    BOOST_CHECK_EQUAL(err.error_count(), 0);
    BOOST_CHECK_EQUAL(err.warning_count(), 0);
    
    err.add_warning("warned");
    err.add_warning("twice");
    BOOST_CHECK_EQUAL(err.warning_count(), 2);
    BOOST_CHECK(err.ok());
    BOOST_CHECK(!err.perfect());
    
    err.add_error("to err is human");
    BOOST_CHECK_EQUAL(err.error_count(), 1);
    BOOST_CHECK(!err.ok());
    BOOST_CHECK(!err.perfect());
    
    std::ostringstream outs;
    err.print(outs);
    BOOST_CHECK_EQUAL(
        outs.str(),
        "ERROR: to err is human\nERROR: total = 1\nWARNING: warned\nWARNING: twice\nWARNING: total = 2\n"
    );
    
    err.clear();
    BOOST_CHECK(err.ok());
    BOOST_CHECK(err.perfect());
    BOOST_CHECK_EQUAL(err.error_count(), 0);
    BOOST_CHECK_EQUAL(err.warning_count(), 0);
}

BOOST_AUTO_TEST_CASE(append_error_test)
{
    Errors err1, err2;
    
    err1.add_warning("careful");
    err1.add_error("mistake");
    err2.add_warning("beware");
    err2.add_error("problem");
    err1 += err2;
    
    std::ostringstream outs;
    err1.print(outs);
    BOOST_CHECK_EQUAL(
        outs.str(),
        "ERROR: mistake\nERROR: problem\nERROR: total = 2\nWARNING: careful\nWARNING: beware\nWARNING: total = 2\n"
    );
}
BOOST_AUTO_TEST_SUITE_END()
