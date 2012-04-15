#ifndef PARPROBOPTS_HEADER
#define PARPROBOPTS_HEADER

// == HEADER parprobopts.hh ==

/**
 * \file Command-line option handling for the 
 * 'multovprob' program
 * \author Andras Aszodi
 * \date 2012-04-14
 */

// -- Base class header --

#include "probopts.hh"

// == Classes ==

namespace multovl {
namespace prob {

/// Option handling for the file-based parallel overlap probability tool.
class ParProbOpts : public ProbOpts
{
public:
	
    /// Initialize
	ParProbOpts();
	
    /// \return the number of threads requested
    unsigned int threads() const { return _threads; }
    
	virtual
    std::string param_str() const;
    
	virtual
	std::ostream& print_help(std::ostream& out) const;
	
protected:
	
	virtual
	bool check_variables();

private:
	
    static
    unsigned int core_count();
    static const unsigned int DEFAULT_THREADS;
    unsigned int _threads;
};

} // namespace prob
} // namespace multovl

#endif	// PROBOPTS_HEADER

