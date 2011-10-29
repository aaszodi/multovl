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

BOOST_AUTO_TEST_SUITE_END()
