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
// == MODULE basepipeline.cc ==

// -- Own headers --

#include "multovl/basepipeline.hh"
#include "multovl/region.hh"
#include "multovl/timer.hh"

// -- Standard headers --

#include <iostream>

// == Implementation ==

namespace multovl {

bool BasePipeline::set_optpimpl(MultovlOptbase* optp)
{
    if (_optpimpl.get() == nullptr)
    {
        _optpimpl.reset(optp);
        return true;
    }
    else
        return false;
}

bool BasePipeline::run()
{
    Timer timer;    // start with current time as 0-th time point
    
    // read the regions
    unsigned int trackcnt = read_input();
    if (trackcnt == 0 || !errors().perfect())
    {
        std::cerr << "! Problems encountered during input" << std::endl;
        errors().print(std::cerr);
        if (!errors().ok())
            return false;  // stop only on serious errors
        else
            clear_errors();
    }
    if (opt_ptr()->timing())
        timer.add_timepoint();  // [1]-st time point is the end of input
    
    // detect overlaps
    detect_overlaps();
    if (!errors().perfect())
    {
        std::cerr << "! Problems encountered during overlap detection" << std::endl;
        errors().print(std::cerr);
        if (!errors().ok())
            return false;  // stop on errors only
        else
            clear_errors();
    }
    if (opt_ptr()->timing())
    {
        timer.add_timepoint();  // [2]-nd timepoint is the end of the multioverlap calc

        // no overlap output is produced, just the
        // timing results are printed to stdout
        std::cout << "Parameters: " << opt_ptr()->param_str() 
            << ", Timing: input = " << timer.interval(1) 
            << " ms, multovl = " << timer.interval(2,1)
            << " ms, total = " << timer.interval(2) << " ms" << std::endl;
        return true;    // OK
    }
    
    // generate output: this can hardly go wrong, but who knows...
    write_output();
    if (!errors().perfect())
    {
        std::cerr << "! Problems encountered during output" << std::endl;
        if (!errors().ok())
            return false;   // stop on errors only
        else
            clear_errors();
    }
        
    return true;    // all is fine
}

}   // namespace multovl
