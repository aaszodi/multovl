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
#include "region.hh"
using namespace multovl;

struct LinereaderFixture
{
    LinereaderFixture():
        region(10, 20, '+', "region")
    {}
    
    Region region;
};

BOOST_FIXTURE_TEST_SUITE(linereadersuite, LinereaderFixture)

BOOST_AUTO_TEST_CASE(bedreader_test)
{
    io::BedLinereader blr;
    
    Region reg;
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
    
    Region reg;
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
