/* <LICENSE>
License for the MULTOVL multiple genomic overlap tools

Copyright (c) 2007-2012, Dr Andras Aszodi, 
Campus Science Support Facilities GmbH (CSF),
Dr-Bohr-Gasse 3, A-1030 Vienna, Austria, Europe.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of the Campus Science Support Facilities GmbH
      nor the names of its contributors may be used to endorse
      or promote products derived from this software without specific prior
      written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS
AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
</LICENSE> */
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

