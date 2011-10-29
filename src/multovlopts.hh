#ifndef MULTOVLOPTS_HEADER
#define MULTOVLOPTS_HEADER

// == HEADER multovlopts.hh ==

/**
 * \file Command-line option handling base class
 * for the 'multovl' program and variants
 * \author Andras Aszodi
 * \date 2010-03-22
 */

// -- Polite header --

#include "polite.hh"

// == Classes ==

namespace multovl {

/// Base class for option handling in the multovl family of tools.
class MultovlOptbase : public virtual Polite
{
	public:
	
	MultovlOptbase();
	
    unsigned int minmult() const { return _minmult; }
    unsigned int maxmult() const { return _maxmult; }
    bool solitary() const { return _minmult == 1; }
    bool nointrack() const { return _nointrack; }
    unsigned int ovlen() const { return _ovlen; }
    bool uniregion() const { return _uniregion; }
    bool timing() const { return _timing; }
	
	virtual
	std::string param_str() const;
	
	virtual
	std::ostream& print_version(std::ostream& out) const;
	
	protected:
	
	virtual
	bool check_variables();
	
	virtual
	std::ostream& version_info(std::ostream& out) const;
	
	private:
	
	unsigned int _minmult, _maxmult, _ovlen, _copt;
	bool _uniregion, _nointrack, _timing;
};

} // namespace multovl

#endif	// MULTOVLOPTS_HEADER

