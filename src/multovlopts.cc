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
// == MODULE multovlopts.cc ==

// -- Standard headers --

#include <algorithm>

// -- Own header --

#include "multovlopts.hh"
#include "multovl_config.hh"

// == Implementation ==

namespace multovl {

// -- MultovlOptbase methods --

MultovlOptbase::MultovlOptbase():
	Polite("Options")
{
	add_option<unsigned int>("common-mult", &_copt, 0, 
		"Find exactly arg-fold overlaps (default 2), -c 1 detects 'solitary regions', overrides -m, -M", 'c');
	add_option<unsigned int>("ovlen", &_ovlen, 1, 
		"Minimal overlap length (default 1), -c 1 forces -L 1", 'L');
	add_option<unsigned int>("minmult", &_minmult, 2, 
		"Minimal multiplicity of the overlaps (default 2), 1 detects solitary regions,too", 'm');
	add_option<unsigned int>("maxmult", &_maxmult, 0, 
		"Maximal multiplicity of the overlaps, 0 (the default) means no upper limit", 'M');
    add_bool_switch("uniregion", &_uniregion, 
        "List union of overlapping regions instead of each overlap, ignored if -c 1 is set", 'u');
    add_bool_switch("nointrack", &_nointrack, 
        "Do not detect overlaps within the same track, ignored if -c 1 or -u is set", 'n');
    add_bool_switch("timing", &_timing,
        "List execution times only, no region output", 't');
}

std::string MultovlOptbase::param_str() const 
{
    std::string outstr = " -L ";
    outstr += boost::lexical_cast<std::string>(ovlen());
    if (uniregion()) outstr += " -u";
    if (nointrack()) outstr += " -n";
    if (option_seen("common-mult"))
        outstr += " -c " + boost::lexical_cast<std::string>(_copt);
    else
        outstr += " -m " + boost::lexical_cast<std::string>(minmult())
            + " -M " + boost::lexical_cast<std::string>(maxmult());
    return outstr;
}
    
bool MultovlOptbase::check_variables()
{
    // if -c has been seen, override -m, -M, -n
    if (option_seen("common-mult"))
    {
        if (_copt < 1) _copt = 1;
        _minmult = _maxmult = _copt;
        if (_copt == 1)
        {
            // -c 1 overrides -L, -u
            _ovlen = 1;
            _uniregion = false;
            _nointrack = false;
        }
    }
    else
    {
        // no -c option
        if (_minmult <= 0) _minmult = 2;
        if (_maxmult > 0 && _minmult > _maxmult)
            add_error(
                "The combination -m " + boost::lexical_cast<std::string>(_minmult) +
                " -M " + boost::lexical_cast<std::string>(_maxmult) +
                " makes no sense"
            );
        
        // solitary overrides -L
        if (_minmult == 1) _ovlen = 1;
        
        // -u overrides -n
        if (_uniregion) _nointrack = false;
    }
    
	return (!error_status());
}

std::ostream& MultovlOptbase::print_version(std::ostream& out) const
{
    version_info(out);
    Polite::version_info(out);
    return out;
}

std::ostream& MultovlOptbase::version_info(std::ostream& out) const
{
	out << "Multovl version " << MULTOVL_VER() << std::endl;
    out << MULTOVL_BUILD() << std::endl;
	return out;
}

}   // namespace multovl
