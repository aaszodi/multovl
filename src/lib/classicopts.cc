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
// == MODULE classicopts.cc ==

// -- Standard headers --

#include <algorithm>

// -- Own headers --

#include "multovl/classicopts.hh"
#include "multovl/io/fileformat.hh"

// -- Boost headers --

#include "boost/algorithm/string/case_conv.hpp"

// == Implementation ==

namespace multovl {

ClassicOpts::ClassicOpts():
	MultovlOptbase()
{
	add_option<std::string>("source", &_source, "multovl", 
		"Source field in GFF output", 's');
	add_option<std::string>("output", &_output, "", 
		"Output file, format BED or GFF, auto-detected from extension, standard output in GFF format if omitted", 'o');
	add_option<std::string>("save", &_saveto, "", 
		"Save program data to archive file, default: do not save");
	add_option<std::string>("load", &_loadfrom, "", 
		"Load program data from archive file, default: do not load");
}

bool ClassicOpts::check_variables()
{
	MultovlOptbase::check_variables();
	
	// figure out the output format: currently BED and GFF are accepted
	_outformat = "GFF"; // default
	if (_output != "") {
	    // file name specified
	    auto fmt = io::Fileformat::from_filename(_output);
	    if (fmt == io::Fileformat::Kind::BED) {
	        _outformat = "BED";
	        // anything else will be set to GFF by default
	    }
	}
	
    // there must be at least 1 positional param
    // unless --load has been set in which case
    // all input comes from the archive
    unsigned int filecnt = pos_opts().size();
    if (_loadfrom == "" && filecnt < 1)
    {
        add_error("Must specify at least one input file");
    }
	return (!error_status());
}

std::string ClassicOpts::param_str() const 
{
    std::string outstr = MultovlOptbase::param_str();
    outstr += " -s " + _source + " -f " + _outformat;
    if (_loadfrom != "") outstr += " --load " + _loadfrom;
    if (_saveto != "") outstr += " --save " + _saveto;
    if (_output != "") outstr += " -o " + _output;
    return outstr;
}

std::ostream& ClassicOpts::print_help(std::ostream& out) const
{
	out << "Multiple Chromosome / Multiple Region Overlaps" << std::endl
		<< "Usage: multovl [options] [<infile1> [ <infile2> ... ]]" << std::endl
		<< "Accepted input file formats: BED, GFF/GTF, BAM (detected from extension)" << std::endl
        << "<infileX> arguments are ignored if --load is set" << std::endl
		<< "Output goes to stdout in GFF format by default, specify output file with the -o option" << std::endl;
	Polite::print_help(out);
	return out;
}

}   // namespace multovl
