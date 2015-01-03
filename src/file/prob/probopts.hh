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
    
    /// \return true if /filename/ is a fixed file
    bool file_is_fixed(const std::string& filename) const;

    /// \return a vector of input file names that are to be reshuffled.
    filenames_t shuffle_files() const { return pos_opts(); }
    
    /// \return the number of reshufflings requested
    unsigned int reshufflings() const { return _reshufflings; }
    
    /// \return the random number generator seed
    unsigned int random_seed() const { return _randomseed; }
    
    /// \return true if the user requested an ASCII progress bar display
    bool progress() const { return _progress; }
	
	virtual
    std::string param_str() const;
    
	virtual
	std::ostream& print_help(std::ostream& out) const;
	
	protected:
	
	virtual
	bool check_variables();

	private:
	
    static const unsigned int DEFAULT_RESHUFFLINGS, DEFAULT_RANDOMSEED;

	std::string _freefile;
	filenames_t _fixedfiles;
    unsigned int _reshufflings, _randomseed;
    bool _progress;
};

} // namespace prob
} // namespace multovl

#endif	// PROBOPTS_HEADER

