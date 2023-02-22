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
#define BOOST_TEST_MODULE timertest
#include "boost/test/unit_test.hpp"

// -- Standard headers --

#ifdef _WIN32
#include <Windows.h>
#define sleep Sleep
#else
#include <unistd.h>
#endif

#include <iostream>
#include <iomanip>
using namespace std;

#include "timer.hh"
using namespace multovl;

static void sleep_seconds(int sec)
#ifdef _WIN32
{
	// Windows takes millisecond arg
	Sleep(1000*sec);
}
#else
{
	// Unix takes second arg
	sleep(sec);
}
#endif

static void check_approx_duration(
    unsigned int actualdur,
    double expecteddur, // in seconds
    double pctol=2.0    // percentage tolerance: 1.0% needed on Irix, 2.0% on Windows
)
{
    double ms = static_cast<double>(actualdur);
    BOOST_CHECK_CLOSE(ms, 1000.0*expecteddur, pctol);
}

BOOST_AUTO_TEST_CASE(timer_test)
{
    Timer timer;
    sleep_seconds(1);
    timer.add_timepoint();
    sleep_seconds(2);
    timer.add_timepoint();
    unsigned int actual_duration = timer.interval(1);
    check_approx_duration(actual_duration, 1);
    actual_duration = timer.interval(2, 1);
    check_approx_duration(actual_duration, 2);
    actual_duration = timer.interval(2, 0);
    check_approx_duration(actual_duration, 3);
}
