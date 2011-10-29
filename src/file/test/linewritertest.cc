#define BOOST_TEST_MODULE linewritertest
#include "boost/test/unit_test.hpp"

// 2011-01-26 AA

#include <string>

#include "linewriter.hh"
#include "multiregion.hh"
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
    
    Region reg;
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
    
    exps = "chr1\tsrc\tregion\t80\t90\t.\t+";
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
