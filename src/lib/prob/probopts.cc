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
// == MODULE probopts.cc ==

// -- Own header --

#include "multovl/prob/probopts.hh"

// -- Boost headers --

#include "boost/algorithm/string/case_conv.hpp"

// -- Standard headers --

#include <algorithm>

// == Implementation ==

namespace multovl {
namespace prob {

const unsigned int ProbOpts::DEFAULT_RESHUFFLINGS = 100,
    ProbOpts::DEFAULT_RANDOMSEED = 42u;

ProbOpts::ProbOpts():
	MultovlOptbase(),
    _freefile(""),
    _fixedfiles(),
    _reshufflings(DEFAULT_RESHUFFLINGS),
    _randomseed(DEFAULT_RANDOMSEED),
    _progress(false)
{
    add_mandatory_option<std::string>("free", "Free regions (mandatory)", 'F');
    add_option<filenames_t>("fixed", &_fixedfiles, filenames_t(), "Filenames of fixed tracks", 'f');
	add_option<unsigned int>("reshufflings", &_reshufflings, DEFAULT_RESHUFFLINGS, 
		"Number of reshufflings, default 100", 'r');
	add_option<unsigned int>("seed", &_randomseed, DEFAULT_RANDOMSEED, 
                             "Random number generator seed, default 42", 's');
    add_bool_switch("progress", &_progress,
        "Display ASCII progress bar on stderr");
}

bool ProbOpts::check_variables()
{
	MultovlOptbase::check_variables();
    
    if (option_seen("free"))
    {
        _freefile = fetch_value<std::string>("free");
    }
    else
    {
        add_error("Must specify free regions file");
    }
	
    // there must be at least 1 positional param for the track files to be reshuffled
    unsigned int filecnt = pos_opts().size();
    if (filecnt < 1)
    {
        add_error("Must specify at least one shuffle track file");
    }
    
    if (_reshufflings == 0) _reshufflings = DEFAULT_RESHUFFLINGS;
	
	return (!error_status());
}

bool ProbOpts::file_is_fixed(const std::string& filename) const
{
	filenames_t::const_iterator fit = std::find(_fixedfiles.begin(), _fixedfiles.end(), filename);
	return (fit != _fixedfiles.end());
}

std::string ProbOpts::param_str() const 
{
    std::string outstr = MultovlOptbase::param_str();
    outstr += " -F " + _freefile + 
            " -r " + boost::lexical_cast<std::string>(_reshufflings) +
            " -s " + boost::lexical_cast<std::string>(_randomseed);
    for (unsigned int i = 0; i < _fixedfiles.size(); ++i)
    {
        outstr += " -f " + _fixedfiles[i];
    }
    return outstr;
}

std::ostream& ProbOpts::print_help(std::ostream& out) const
{
	out << "Multiple Region Overlap Probabilities" << std::endl
		<< "Usage: multovlprob [options] file1 [file2...]" << std::endl
        << "file1, file2, ... will be reshuffled, there must be at least one" << std::endl
		<< "Accepted input file formats: BED, GFF/GTF, BAM (detected from extension)" << std::endl
		<< "Output goes to stdout" << std::endl;
	Polite::print_help(out);
	return out;
}

}   // namespace prob
}   // namespace multovl
