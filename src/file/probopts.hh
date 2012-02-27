#ifndef PROBOPTS_HEADER
#define PROBOPTS_HEADER

// == HEADER probopts.hh ==

/**
 * \file Command-line option handling for the 
 * 'multovprob' program
 * \author Andras Aszodi
 * \date 2012-02-26
 */

// -- Base class header --

#include "multovlopts.hh"

// == Classes ==

namespace multovl {
namespace prob {

/// Option handling for the file-based multovl tools.
class ProbOpts : public MultovlOptbase
{
	public:
	
    typedef std::vector<std::string> filenames_t;
    
    /// Initialize
	ProbOpts();
	
    /// \return the name of the free regions file
    const std::string& free_file() const { return _freefile; }
    
	/// \return a vector of input file names that are to be held "fixed" (no shuffling).
	const filenames_t& fixed_files() const { return _fixedfiles; }
    
    /// Convenience method to see whether fixed files have been defined at all.
    /// \return the number of fixed files, 0 if none has been specified
    unsigned int fixed_filecnt() const { return _fixedfiles.size(); }
	
	/// \return a vector of input file names that are to be reshuffled.
	const filenames_t& shuffle_files() const { return pos_opts(); }
    
    /// \return the number of reshufflings requested
    unsigned int reshufflings() const { return _reshufflings; }
    
    /// \return the random number generator seed
    unsigned int random_seed() const { return _randomseed; }
	
	virtual
    std::string param_str() const;
    
	virtual
	std::ostream& print_help(std::ostream& out) const;
	
	virtual
	std::ostream& version_info(std::ostream& out) const;
	
	protected:
	
	virtual
	bool check_variables();

	private:
	
    static const unsigned int DEFAULT_RESHUFFLINGS, DEFAULT_RANDOMSEED;

	std::string _freefile;
	filenames_t _fixedfiles;
    unsigned int _reshufflings, _randomseed;
};

} // namespace prob
} // namespace multovl

#endif	// PROBOPTS_HEADER

