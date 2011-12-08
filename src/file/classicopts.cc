// == MODULE classicopts.cc ==

// -- Standard headers --

#include <algorithm>

// -- Own header --

#include "classicopts.hh"
#include "multovl_config.hh"

// -- Boost headers --

#include "boost/algorithm/string/case_conv.hpp"

// == Implementation ==

namespace multovl {

ClassicOpts::ClassicOpts():
	MultovlOptbase()
{
	add_option<std::string>("source", &_source, "multovl", 
		"Source field in GFF output", 's');
	add_option<std::string>("outformat", &_outformat, "GFF", 
		"Output format {BED,GFF}, case-insensitive, default GFF", 'f');
	add_option<std::string>("save", &_saveto, "", 
		"Save program data to archive file, default: do not save");
	add_option<std::string>("load", &_loadfrom, "", 
		"Load program data from archive file, default: do not load");
}

bool ClassicOpts::check_variables()
{
	MultovlOptbase::check_variables();
	
	// canonicalize the output format: currently BED and GFF are accepted
	boost::algorithm::to_upper(_outformat);
	if (_outformat != "BED" && _outformat != "GFF")
	    _outformat = "GFF"; // default
	
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
    return outstr;
}

std::ostream& ClassicOpts::print_help(std::ostream& out) const
{
	out << "Multiple Chromosome / Multiple Region Overlaps" << std::endl
		<< "Usage: multovl [options] [<infile1> [ <infile2> ... ]]" << std::endl
		<< "Accepted input file formats: BED, GFF/GTF" 
#if USE_BAMTOOLS
        << ", BAM"
#endif
        << " (detected from extension)" << std::endl
        << "<infileX> arguments are ignored if --load is set" << std::endl
		<< "Output goes to stdout, select format with the -f option" << std::endl;
	Polite::print_help(out);
	return out;
}

std::ostream& ClassicOpts::version_info(std::ostream& out) const
{
    MultovlOptbase::version_info(out);
#if USE_BAMTOOLS
    out << "BAM file input supported" << std::endl;
#endif
	return out;
}

}   // namespace multovl
