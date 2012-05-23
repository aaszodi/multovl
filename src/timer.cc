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
