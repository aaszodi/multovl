// == MODULE polite.cc ==

// -- Standard headers --

#include <cstdlib>
using namespace std;

// -- Own headers --

#include "polite.hh"
#include "multovl_config.hh"

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
			for (vector<string>::const_iterator uit = unknown.begin();
				uit != unknown.end(); uit++)
            {
                add_error("Unknown parameter: " + *uit);
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
    out << "Built with Boost " << BOOST_VERSION_MAJOR << '.' 
        << BOOST_VERSION_MINOR << '.' << BOOST_PATCHLEVEL << endl;
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
