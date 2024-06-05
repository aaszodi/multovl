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

#include "multovl/polite.hh"

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
    unsigned int extension() const { return _extension; }
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
	
	unsigned int _minmult, _maxmult, _ovlen, _extension, _copt;
	bool _uniregion, _nointrack, _timing;
};

} // namespace multovl

#endif	// MULTOVLOPTS_HEADER

