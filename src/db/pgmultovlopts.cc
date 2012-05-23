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
// == MODULE pgmultovlopts.cc ==

// -- Standard headers --

#include <algorithm>

// -- Own header --

#include "pgmultovlopts.hh"
#include "multovl_config.hh"

// -- PQXX version --

#include "pqxx/version"

// == Implementation ==

namespace multovl {

PgMultovlOpts::PgMultovlOpts():
	MultovlOptbase()
{
	add_option<std::string>("configfile", &_configfile, "./pgmultovl.cfg", 
		"Configuration file, default is \"./pgmultovl.cfg\"", 'C');
	add_mandatory_option<std::string>("outtrack", 
		"Output track name", 'o');
}

bool PgMultovlOpts::check_variables()
{
	MultovlOptbase::check_variables();
    // there must be at least 1 positional param
    unsigned int filecnt = pos_opts().size();
    if (filecnt < 1)
    {
        add_error("Must specify at least one input track name");
    }
    if (option_seen("outtrack"))
    {
        _outtrack = fetch_value<std::string>("outtrack");
    } else {
        add_error("Must specify output track name");
    }
	return (!error_status());
}

std::string PgMultovlOpts::param_str() const 
{
    std::string outstr = MultovlOptbase::param_str();
    outstr += " -o " + _outtrack;
    if (option_seen("configfile"))
        outstr += " -C " + _configfile;
    return outstr;
}

std::ostream& PgMultovlOpts::version_info(std::ostream& out) const
{
    MultovlOptbase::version_info(out);
	out << "Built with libpqxx " << PQXX_VERSION << std::endl;
	return out;
}

std::ostream& PgMultovlOpts::print_help(std::ostream& out) const
{
	out << "Multiple Region Overlaps in a PostgreSQL Database" << std::endl
		<< "Usage: pgmultovl [options] <trackname1> [ <trackname2> ... ]" << std::endl
		<< "Output will be inserted into the same database." << std::endl;
	Polite::print_help(out);
	return out;
}

}   // namespace multovl
