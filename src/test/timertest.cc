#define BOOST_TEST_MODULE timertest
#include "boost/test/unit_test.hpp"

// -- Standard headers --

#include <unistd.h>
#include <iostream>
#include <iomanip>
using namespace std;

#include "timer.hh"
using namespace multovl;

static void check_approx_duration(
    const boost::posix_time::time_duration& actualdur,
    double expecteddur, // in seconds
    double pctol=1.0    // percentage tolerance: 1.0% needed on Irix
)
{
    double ms = static_cast<double>(actualdur.total_milliseconds());
    BOOST_CHECK_CLOSE(ms, 1000.0*expecteddur, pctol);
}

BOOST_AUTO_TEST_CASE(timer_test)
{
    Timer timer;
    sleep(1);
    timer.add_timepoint();
    sleep(2);
    timer.add_timepoint();
    boost::posix_time::time_duration actual_duration = timer.interval(1);
    check_approx_duration(actual_duration, 1);
    actual_duration = timer.interval(2, 1);
    check_approx_duration(actual_duration, 2);
    actual_duration = timer.interval(2, 0);
    check_approx_duration(actual_duration, 3);
}
