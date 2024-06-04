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
#define BOOST_TEST_MODULE fileformattest
#include "boost/test/unit_test.hpp"

// -- own header --

#include "fileformat.hh"
using namespace multovl::io;

BOOST_AUTO_TEST_SUITE(fileformatsuite)

BOOST_AUTO_TEST_CASE(fromfile_test)
{
    Fileformat::Kind f;
    
    f = Fileformat::from_filename("FiLe.bEd");
    BOOST_CHECK(f == Fileformat::BED);
    BOOST_CHECK_EQUAL(Fileformat::to_string(f), "BED");

    f = Fileformat::from_filename("file.gff");
    BOOST_CHECK(f == Fileformat::GFF);
    BOOST_CHECK_EQUAL(Fileformat::to_string(f), "GFF");
    
    f = Fileformat::from_filename("file.bam");
    BOOST_CHECK(f == Fileformat::BAM);
    BOOST_CHECK_EQUAL(Fileformat::to_string(f), "BAM");
    
    f = Fileformat::from_filename("file.blabla");
    BOOST_CHECK(f == Fileformat::UNKNOWN);
    BOOST_CHECK_EQUAL(Fileformat::to_string(f), "UNKNOWN");
}

BOOST_AUTO_TEST_CASE(knownextension_test)
{
    std::string knownext(".bed, .gff, .gtf, .bam");
    BOOST_CHECK_EQUAL(Fileformat::known_extensions(), knownext);
}

BOOST_AUTO_TEST_SUITE_END()
