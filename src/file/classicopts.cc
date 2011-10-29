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
}

bool ClassicOpts::check_variables()
{
	MultovlOptbase::check_variables();
	
	// canonicalize the output format: currently BED and GFF are accepted
	boost::algorithm::to_upper(_outformat);
	if (_outformat != "BED" && _outformat != "GFF")
	    _outformat = "GFF"; // default
	
    // there must be at least 1 positional param
    unsigned int filecnt = pos_opts().size();
    if (filecnt < 1)
    {
        add_error("Must specify at least one input file");
    }
	return (!error_status());
}

std::string ClassicOpts::param_str() const 
{
    std::string outstr = MultovlOptbase::param_str();
    outstr += " -s " + _source + " -f " + _outformat;
    return outstr;
}

std::ostream& ClassicOpts::print_help(std::ostream& out) const
{
	out << "Multiple Chromosome / Multiple Region Overlaps" << std::endl
		<< "Usage: multovl [options] <infile1> [ <infile2> ... ]" << std::endl
		<< "Accepted input file formats: BED, GFF/GTF" 
#if USE_BAMTOOLS
        << ", BAM"
#endif
        << " (detected from extension)" << std::endl
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
