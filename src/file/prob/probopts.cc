// == MODULE probopts.cc ==

// -- Own header --

#include "probopts.hh"
#include "multovl_config.hh"

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
		<< "Accepted input file formats: BED, GFF/GTF" 
#if USE_BAMTOOLS
        << ", BAM"
#endif
        << " (detected from extension)" << std::endl
		<< "Output goes to stdout" << std::endl;
	Polite::print_help(out);
	return out;
}

std::ostream& ProbOpts::version_info(std::ostream& out) const
{
    MultovlOptbase::version_info(out);
#if USE_BAMTOOLS
    out << "BAM file input supported" << std::endl;
#endif
	return out;
}

}   // namespace prob
}   // namespace multovl
