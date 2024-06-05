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
// == MODULE polite.cc ==

// -- Standard headers --

#include <cstdlib>
using namespace std;

// -- Own headers --

#include "multovl/polite.hh"
#include "thirdparty.h"

namespace multovl {

// == Implementation ==

// -- Public methods --

Polite::Polite(const string& descr):
	_desc(descr), _err(false), _parsed(false)
{
	_desc.add_options()
		("help,h", "Print this help and exit")
	;
	_desc.add_options()
		("version", "Print version information and exit")
	;
	_hidden.add_options()
		("posopts", bpo::value< vector<string> >(), "positional arguments")
	;
	_posdesc.add("posopts", -1);	// always have one, even if unused
}

void Polite::process_commandline(int argc, char* argv[])
{
    bool parseok = parse_check(argc, argv);
    if (version_needed())
    {
    	print_version(cout);
    	exit(EXIT_SUCCESS);
    }
    if (help_needed())
    {
    	print_help(cout);
    	exit(EXIT_SUCCESS);
    }
    if (! parseok)
    {
    	cerr << error_messages() << endl;
        print_help(cerr);
        exit(EXIT_FAILURE);
    }
}

void Polite::add_bool_switch(const string& name, bool* varp,
    const string& descr, const char nm)
{
    string oname = (nm == '\0')? name: name + ',' + nm;
    _desc.add_options()
        (oname.c_str(), 
        bpo::bool_switch(varp),
        descr.c_str());
}

bool Polite::parse_check(int argc, char* argv[])
{
    bool ok = perform_parsing(argc, argv);
    if (ok)
        ok = check_variables(); // this should be implemented in derived classes
    return ok;
}

vector<string> Polite::pos_opts() const
{
	if (_vm.count("posopts"))
		return _vm["posopts"].as< vector<string> >();
	else
		return vector<string>();	// just empty
}

ostream& Polite::print_help(ostream& out) const
{
	out << _desc << endl;
	out << "(c) Andras Aszodi 2011. All rights reserved." << endl;
	return out;
}

// -- Protected methods --

bool Polite::perform_parsing(int argc, char* argv[])
{
	if (_parsed)
		return false;
	_parsed = true;
	
	error_reset();
	bpo::options_description cmdline;
	cmdline.add(_desc);
	cmdline.add(_hidden);
	try {
		_vm.clear();
		bpo::parsed_options parsed = bpo::command_line_parser(argc, argv).
          options(cmdline).allow_unregistered().positional(_posdesc).run();
		bpo::store(parsed, _vm);
		bpo::notify(_vm);
		
		// check if there have been unrecognised options, store them
		vector<string> unknown = bpo::collect_unrecognized(
			parsed.options, bpo::exclude_positional);
		if (unknown.size() > 0)
		{
			for (const auto& unk : unknown) {
                add_error("Unknown parameter: " + unk);
			}
			return false;
		}
		else return true;
	} catch (bpo::error& err) {
	    add_error(err.what());
		return false;
	}
}

ostream& Polite::version_info(ostream& out) const
{
    out << config_boost_version() << endl
        << config_bamtools() << endl;
	return out;
}

void Polite::add_error(const string& msg)
{
    _errors << "ERROR: " << msg << endl;
    _err = true;
}

void Polite::error_reset()
{
	_errors.clear();
	_errors.str("");
	_err = false;
}

} // namespace multovl
