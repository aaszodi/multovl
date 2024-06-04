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

