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

#include "multovl/multovlopts.hh"

// == Classes ==

namespace multovl {

/// Option handling for the file-based multovl tools.
class ClassicOpts : public MultovlOptbase
{
	public:
	
	ClassicOpts();
	
	/// \return the definition for the "source" column in the GFF output.
	const std::string& source() const { return _source; }
	
	/// \return output file path, "" means "write to standard output
	const std::string& output() const { return _output; }
	
	/// \return the desired output format. May be {BED,GFF}, case-insensitive, default GFF
	const std::string& outformat() const { return _outformat; }
	
	/// \return the archive file name to save (serialize) the status of the program to.
	/// Empty string by default meaning no status is to be saved.
	const std::string& save_to() const { return _saveto; }
	
	/// \return the archive file name to load (deserialize) the status of the program from.
	/// The file is to be created by a previous saving operation.
	/// If this option is chosen, then no track input files are read.
	/// Empty string by default, meaning tracks are read from files 
	/// given as positional arguments on the command line.
	const std::string& load_from() const { return _loadfrom; }
	
	/// \return a vector of input file names provided as positional arguments on the command line.
	std::vector<std::string> input_files() const { return pos_opts(); }
	
	virtual
    std::string param_str() const;
    
	virtual
	std::ostream& print_help(std::ostream& out) const;
	
	protected:
	
	virtual
	bool check_variables();

	private:
	
	std::string _source, _output, _outformat,
	    _saveto, _loadfrom;
};

} // namespace multovl

#endif	// CLASSICOPTS_HEADER

