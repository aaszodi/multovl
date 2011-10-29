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
