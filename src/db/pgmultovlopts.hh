#ifndef PGMULTOVLOPTS_HEADER
#define PGMULTOVLOPTS_HEADER

// == HEADER pgmultovlopts.hh ==

/**
 * \file Command-line option handling for the 'pgmultovl' program
 * that reads/writes tracks from/to Postgres databases.
 * \author Andras Aszodi
 * \date 2011-06-21
 */

// -- Base class header --

#include "multovlopts.hh"

// == Classes ==

namespace multovl {

/// Option handling for the Postgres-based multovl tool.
class PgMultovlOpts : public MultovlOptbase
{
	public:
	
	PgMultovlOpts();
	
	const std::string& config_file() const { return _configfile; }
	std::vector<std::string> input_tracks() const { return pos_opts(); }
	const std::string& output_track() const { return _outtrack; }
	
	virtual
    std::string param_str() const;
    
	virtual
    std::ostream& version_info(std::ostream& out) const;

	virtual
	std::ostream& print_help(std::ostream& out) const;
	
	protected:
	
	virtual
	bool check_variables();

	private:
	
	std::string _configfile, _outtrack;
};

} // namespace multovl

#endif	// PGMULTOVLOPTS_HEADER

