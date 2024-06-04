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
#ifndef MULTOVL_POLITE_HEADER
#define MULTOVL_POLITE_HEADER

// == HEADER polite.hh ==

/**
 * \file 
 * \brief Wrapper around the overengineered boost::program_options
 * \author Andras Aszodi
 * \date 2010-03-20
 */
 
// -- Standard headers --

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// -- Boost headers --

#include "boost/program_options.hpp"
namespace bpo = boost::program_options;

// -- Classes --

namespace multovl {

/**
 * \brief Program Options LITE base class.
 * The Polite class provides a wrapper over boost::program_options
 * to make life a bit easier. Functionality includes parsing
 * the default --help and --version options, and access to
 * positional parameters. Apps using this are supposed to subclass Polite
 * and add their own extra options.
 */
class Polite
{
	public:
	
	/// Convenience method to process all command line arguments at once.
	/// This method invokes parse_check(argc, argv) and then does the following:-
	/// If version information was requested, invokes print_version() and then
	/// invokes exit(EXIT_SUCCESS). If help was requested, invokes print_help(std::cout)
	/// and then exit(EXIT_SUCCESS). If parsing fails, then the error
	/// messages are printed to std::cerr prefixed with "ERROR: ", then
	/// print_help(std::cerr) is invoked, then exit(EXIT_FAILURE) is invoked.
	/// If parsing was successful, then the method returns normally
	/// and the program can continue.
	/// \argc the number of commandline arguments as provided by main()
	/// \argv the commandline arguments in a C std::string array as provided by main()
    void process_commandline(int argc, char* argv[]);

    /// Parses the command line and checks the variables.
	/// \argc the number of commandline arguments as provided by main()
	/// \argv the commandline arguments in a C std::string array as provided by main()
	/// \return /true/ on success, /false/ on error. Upon success,
	/// you may access the variables' values.
    bool parse_check(int argc, char* argv[]);
    
	/// Returns the positional options as a std::string vector.
	/// \return a std::string vector or an empty vector if there were no arguments.
	std::vector<std::string> pos_opts() const;

	/// Error status
	/// \return /true/ if at least one error has been seen during parsing.
	bool error_status() const { return _err; }
	
	/// Error messages
	/// \return all error messages as a multiline std::string
	/// each line contains one message added with the (protected) add_error(msg) method.
	std::string error_messages() const { return _errors.str(); }
	
	/// Indicates whether version information should be printed.
	/// \return /true/ if the "--version" option has seen.
	bool version_needed() const { return _vm.count("version") > 0; }
	
	/// Prints the version information
	/// \param out this is the stream the version text is printed to.
	/// \return /out/ is returned here
	virtual
	std::ostream& print_version(std::ostream& out) const { return version_info(out); }
	
	/// Indicates whether the help should be printed.
	/// \return /true/ if the "--help" or "-h" option has seen,
	/// or if error(s) occurred.
	bool help_needed() const { return _vm.count("help") > 0; }
	
	/// Prints help, using the facilities provided by boost::program_options
	/// \param out this is the stream the help text is printed to.
	/// \return /out/ is returned here
	virtual
	std::ostream& print_help(std::ostream& out) const;
	
	virtual ~Polite() {}
	
	protected:
	
	/// Inits with a single option "--help" or "-h"
	/// and a single positional option.
	/// For simplicity's sake, the positionals are std::strings only,
	/// the name is fixed to "posopts",
	/// and they can be anywhere after the "real" options.
	/// \param descr the overall description of the visible options
	explicit Polite(const std::string& descr = "Options");
	
	/// Add a non-mandatory option.
	/// Derived classes should have members for each option variable,
	/// and then invoke this method in their constructors to set them up.
	/// \param name the 'long' option name like "--opt"
	/// \param optvarp pointer to the data member storing the option value
	/// \param defval default value for the option
	/// \param descr the description of the option
	/// \param nm the 'short' option name like "-o", can be omitted
	template <typename Opttype>
	void add_option(const std::string& name, Opttype* optvarp, const Opttype& defval,
		const std::string& descr, const char nm = '\0')
	{
		std::string oname = (nm == '\0')? name: name + ',' + nm;
		_desc.add_options()
			(oname.c_str(), 
			bpo::value<Opttype>(optvarp),
			/* NOTE: this used to be
			 * bpo::value<Opttype>(optvarp)->default_value(defval),
			 * but that caused _vm.count("name") > 0
			 * even if the parameter was not present on the commandline
			 */
			descr.c_str());
        *optvarp = defval;  // init default val here then
	}
	
	/// Add a non-mandatory Boolean switch option.
	/// \param name the 'long' option name like "--opt"
	/// \param varp pointer to the data member storing the option value
	/// \param descr the description of the option
	/// \param nm the 'short' option name like "-o", can be omitted
    void add_bool_switch(const std::string& name, bool* varp,
        const std::string& descr, const char nm = '\0');
    
	/// Add a mandatory option.
	/// Derived classes should have members for each option variable,
	/// and then invoke this method in their constructors to set them up.
	/// \param name the 'long' option name like "--opt"
	/// \param descr the description of the option
	/// \param nm the 'short' option name like "-o", can be omitted
	template <typename Opttype>
	void add_mandatory_option(const std::string& name, const std::string& descr, const char nm = '\0')
	{
		std::string oname = (nm == '\0')? name: name + ',' + nm;
		_desc.add_options()
			(oname.c_str(), 
			bpo::value<Opttype>(),
			descr.c_str());
	}
	
    /// Actually parses the command line.
    /// NOTE: You may invoke this method only once, because otherwise
    /// strange things happen (seem to be a "feature" of boost::program_options).
    /// \param argc the length of the argument array, must be >=1
    /// \param argv the arguments, argv[0] is the program name
    /// \return /true/ on success, /false/ on error, including repeated parsing.
	bool perform_parsing(int argc, char* argv[]);
	
	/**
	 * Derived classes must override this method so that first an appropriate base class
	 * version is invoked to process the base class' commandline variables
	 * and then additional processing must be done on the derived class' variables
	 * if /true/ was returned, like this:-
	 * \verbatim
	  bool DerivedOpts::check_variables()
	  {
	  	bool ok = BaseOpts::check_variables();  // don't invoke Polite::check_variables() !
	  	if (ok) {
	  		// e.g. check some variables here
	  		if (option_seen("opt") > 0)
	  		{
	  			int opt = fetch_value<int>("opt");
	  			if (opt < 1) opt = 1;
	  			// ...
	  			ok = true;
	  		}
	  		else
	  		{
	  		    add_error("opt must be present");
	  		    ok = false;
            }
	  	}
	  	return ok;
	  }
	  \endverbatim
	 */
	virtual
	bool check_variables() = 0;
	
	/// Checks whether an option has been seen.
	/// \param optnm the name of the option.
	/// \return true if the option has been seen on the command line.
	bool option_seen(const std::string& optnm) const { return (_vm.count(optnm) > 0); }
	
	/// Fetches the value of an option.
	/// \param optnm the name of the option.
	/// \return the value of the option.
	template <typename Opttype>
	Opttype fetch_value(const std::string& optnm) const { return (_vm[optnm].as<Opttype>()); }
	
	/// Adds an error message, sets error status.
	/// This method may be called more than once during parsing,
	/// the error messages will be "remembered".
	/// \param msg a descriptive error message.
	void add_error(const std::string& msg);
	
	/// Reset the error status to "no error".
	void error_reset();
	
	/// This method prints version information relevant to the current class.
	/// It is invoked by the print_version() method by default.
	/// Do not invoke the base class version(s) here, just specify what is relevant
	/// to your current class, and override print_version() so that the base class infos
	/// are combined correctly (important when doing diamond-style inheritance).
	/// \param out the stream the version information is printed to
	/// \return out
	virtual
	std::ostream& version_info(std::ostream& out) const;
	
	private:
	
	bpo::options_description _desc, _hidden;
	bpo::positional_options_description _posdesc;
	bpo::variables_map _vm;
	bool _err;
	std::ostringstream _errors;
	bool _parsed;	// apparently you may parse only once
};

}   // namespace multovl

#endif	// MULTOVL_POLITE_HEADER
