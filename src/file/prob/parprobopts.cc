// == MODULE parprobopts.cc ==

// -- Own header --

#include "parprobopts.hh"
#include "multovl_config.hh"

// -- Boost headers --

#include "boost/thread.hpp"
#include "boost/lexical_cast.hpp"

// -- Standard headers --

#include <algorithm>

// == Implementation ==

namespace multovl {
namespace prob {

const unsigned int ParProbOpts::DEFAULT_THREADS = ParProbOpts::core_count();

unsigned int ParProbOpts::core_count()
{
    unsigned int corecount = boost::thread::hardware_concurrency();
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
		<< "Accepted input file formats: BED, GFF/GTF" 
#if USE_BAMTOOLS
        << ", BAM"
#endif
        << " (detected from extension)" << std::endl
		<< "Output goes to stdout" << std::endl;
	Polite::print_help(out);
	return out;
}

}   // namespace prob
}   // namespace multovl
