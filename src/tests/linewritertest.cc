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
#define BOOST_TEST_MODULE linewritertest
#include "boost/test/unit_test.hpp"

// 2011-01-26 AA

#include <string>

#include "multovl/io/linewriter.hh"
#include "multovl/multiregion.hh"
using namespace multovl;

struct LinewriterFixture
{
    LinewriterFixture():
        reg(80, 90, '+', "region"),
        mreg(10, 20),
        chrom("chr1")
    {
        // two simple ancestors
        mreg.add_ancestor(AncestorRegion(1, 5, '+', "a15"));   // track id = 0
        mreg.add_ancestor(AncestorRegion(4, 6, '-', "a46", 9)); // track id = 9
    }
    
    BaseRegion reg;
    MultiRegion mreg;
    std::string chrom;
};

BOOST_FIXTURE_TEST_SUITE(linewritersuite, LinewriterFixture)

BOOST_AUTO_TEST_CASE(bedwriter_test)
{
    io::BedLinewriter blw(chrom);
    std::string exps, obss;
    
    exps = "chr1\t80\t90\tregion\t0\t+";
    obss = blw.write(reg);
    BOOST_CHECK_EQUAL(exps, obss);
    
    exps = "chr1\t10\t20\t0:a15:+:1-5|9:a46:-:4-6\t2\t.";
    obss = blw.write(mreg);
    BOOST_CHECK_EQUAL(exps, obss);
}

BOOST_AUTO_TEST_CASE(gffwriter_test)
{
    io::GffLinewriter glw("src", 1, chrom);   // version fixed to 2
    BOOST_CHECK_EQUAL(glw.version(), 2);
    std::string exps, obss;

    // GFF version 2
    
    exps = "chr1\tsrc\tregion\t80\t90\t.\t+\t.\t.";
    obss = glw.write(reg);
    BOOST_CHECK_EQUAL(exps, obss);
    
    exps = "chr1\tsrc\toverlap\t10\t20\t2\t.\t.\tANCESTORS 0:a15:+:1-5|9:a46:-:4-6";
    obss = glw.write(mreg);
    BOOST_CHECK_EQUAL(exps, obss);
    
    // GFF version 3
    io::GffLinewriter glw3("src", 4, chrom);   // version fixed to 3
    BOOST_CHECK_EQUAL(glw3.version(), 3);
    
    // note: don't bother with glw3.write(reg), it is the same as Version 2
    exps = "chr1\tsrc\toverlap\t10\t20\t2\t.\t.\tANCESTORS=0:a15:+:1-5|9:a46:-:4-6";
    obss = glw3.write(mreg);
    BOOST_CHECK_EQUAL(exps, obss);
}

BOOST_AUTO_TEST_SUITE_END()
