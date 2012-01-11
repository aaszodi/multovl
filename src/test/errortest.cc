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
