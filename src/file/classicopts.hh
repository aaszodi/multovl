#ifndef CLASSICOPTS_HEADER
#define CLASSICOPTS_HEADER

// == HEADER classicopts.hh ==

/**
 * \file Command-line option handling for the 
 * classic file-based 'multovl' program and variants
 * \author Andras Aszodi
 * \date 2010-03-22
 */

// -- Base class header --

#include "multovlopts.hh"

// == Classes ==

namespace multovl {

/// Option handling for the file-based multovl tools.
class ClassicOpts : public MultovlOptbase
{
	public:
	
	ClassicOpts();
	
	const std::string& source() const { return _source; }
	const std::string& outformat() const { return _outformat; }
	
	std::vector<std::string> input_files() const { return pos_opts(); }
	
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
	
	std::string _source, _outformat;
};

} // namespace multovl

#endif	// CLASSICOPTS_HEADER

