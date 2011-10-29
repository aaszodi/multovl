// == MODULE timer.cc ==

// set this from CMakeLists.txt 
#ifndef CAN_USE_BOOST_DATETIME_MICROSEC_CLOCK
#include <time.h>
#endif

#include "timer.hh"

// == Implementation ==

namespace multovl {

void Timer::add_timepoint()
{
    // NOTE: for some strange reason I have no time to investigate,
    // the boost::posix_time::microsec_clock::universal_time() function and its "second" counterpart
    // started causing segfaults (Snow Leopard, GNU-compiled Boost 1.42)
    // but the stdlibc time() function seems OK
    #ifdef CAN_USE_BOOST_DATETIME_MICROSEC_CLOCK
    boost::posix_time::ptime justnow(boost::posix_time::microsec_clock::universal_time());
    #else
    boost::posix_time::ptime justnow(boost::posix_time::from_time_t(time(NULL)));
    #endif
    _timepoints.push_back(justnow);
}

boost::posix_time::time_duration Timer::interval(unsigned int later, unsigned int earlier) const
{
    if (later >= _timepoints.size() || earlier >= _timepoints.size() || later < earlier)
        return boost::posix_time::not_a_date_time;  // invalid
    return (_timepoints[later] - _timepoints[earlier]);
}
    
}   // namespace multovl
