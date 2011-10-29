#ifndef TOOLS_TIMER_HEADER
#define TOOLS_TIMER_HEADER

// == Tools header timer.hh ==

// -- Standard headers --

#include <vector>

// -- Boost header --

#include "boost/date_time/posix_time/posix_time.hpp"

// == Class ==

namespace multovl {

// This is a simple wrapper around Boost's POSIX time functionality,
// basically a stopwatch that keeps track of a vector of time points internally
// and then evaluates the durations.
class Timer
{
    public:
    
    // ctor: adds the 0-th timepoint at time of creation
    Timer()
    {
        add_timepoint();
    }
    
    // Adds the current time with usec resolution as a new timepoint
    void add_timepoint();

    // Returns the time difference between the /later/-th and /earlier/-th timepoints.
    // Returns /not_a_date_time/ if these indices are invalid.
    boost::posix_time::time_duration interval(unsigned int later, unsigned int earlier = 0) const;
    
    private:
    
    typedef std::vector<boost::posix_time::ptime> timevec_t;
    timevec_t _timepoints;
    
};  // class Timer

}   // namespace multovl

#endif // TOOLS_TIMER_HEADER
