#define BOOST_TEST_MODULE multovloptstest
#include "boost/test/unit_test.hpp"

// -- Standard headers --

#include <string>
#include <iostream>
using namespace std;

// -- Boost headers --

#include "boost/lexical_cast.hpp"

// -- Own header --

#include "multovlopts.hh"
using namespace multovl;

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
