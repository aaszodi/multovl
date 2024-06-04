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
#ifndef TOOLS_TIMER_HEADER
#define TOOLS_TIMER_HEADER

// == Tools header timer.hh ==

// -- Standard headers --

#include <vector>
#include <chrono>

// == Class ==

namespace multovl {

// This class implements a stopwatch that keeps track of a vector of time points internally
// and then evaluates the durations.
class Timer
{
    public:
    
    // ctor: adds the 0-th timepoint at time of creation
    Timer()
    {
        add_timepoint();
    }
    
    // Adds the current time as a new timepoint
    void add_timepoint();

    // Returns the time difference between the /later/-th and /earlier/-th timepoints
    // as the number of milliseconds.
    // Throws std::invalid_argument if these indices are invalid.
    unsigned int interval(unsigned int later, unsigned int earlier = 0) const;
    
    private:
    
    typedef std::chrono::time_point<std::chrono::system_clock> timepoint_t;
    typedef std::vector<timepoint_t> timevec_t;
    timevec_t _timepoints;
    
};  // class Timer

}   // namespace multovl

#endif // TOOLS_TIMER_HEADER
