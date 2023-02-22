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
// == MODULE parprobopts.cc ==

// -- Own header --

#include "parprobopts.hh"
#include "thirdparty.h"

// -- Boost headers --

#include "boost/lexical_cast.hpp"

// -- Standard headers --

#include <algorithm>
#include <thread>

// == Implementation ==

namespace multovl {
namespace prob {

const unsigned int ParProbOpts::DEFAULT_THREADS = ParProbOpts::core_count();

unsigned int ParProbOpts::core_count()
{
    unsigned int corecount = std::thread::hardware_concurrency();
    if (corecount == 0)
        corecount = 1;  // could not find out core count, assume 1
    return corecount;
}

ParProbOpts::ParProbOpts():
	ProbOpts(),
    _threads(DEFAULT_THREADS)
{
	add_option<unsigned int>("threads", &_threads, DEFAULT_THREADS, 
		"Number of threads, default to use all cores", 'T');
}

bool ParProbOpts::check_variables()
{
	ProbOpts::check_variables();
    
    if (_threads == 0)
        _threads = DEFAULT_THREADS;
	
	return (!error_status());
}

std::string ParProbOpts::param_str() const 
{
    std::string outstr = ProbOpts::param_str();
    outstr += " -T " + boost::lexical_cast<std::string>(_threads);
    return outstr;
}

std::ostream& ParProbOpts::print_help(std::ostream& out) const
{
	out << "Multiple Region Overlap Probabilities, parallel version" << std::endl
		<< "Usage: parmultovlprob [options] file1 [file2...]" << std::endl
        << "file1, file2, ... will be reshuffled, there must be at least one" << std::endl
        << "Reshuffling can be done in parallel on multicore machines" << std::endl
        << "Default number of threads on this machine is " << DEFAULT_THREADS << std::endl
		<< "Accepted input file formats: BED, GFF/GTF" ;
    if (config_have_bamtools()) {
        out << ", BAM";
    }
    out << " (detected from extension)" << std::endl
		<< "Output goes to stdout" << std::endl;
	Polite::print_help(out);
	return out;
}

}   // namespace prob
}   // namespace multovl
