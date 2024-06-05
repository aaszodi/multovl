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
// == MODULE stat.cc ==

/// \file
/// \date 2012-03-01
/// \author Andras Aszodi

// -- Own header --

#include "multovl/prob/stat.hh"

// -- Boost headers --

// -- Standard headers --

#include <utility>

// == Implementation ==

namespace multovl {
namespace prob {

void Stat::add(unsigned int multiplicity, 
         double val, 
         bool is_actual)
{
    // which distr?
    diter_t dit = _distrs.find(multiplicity);
    if (dit != _distrs.end())
    {
        // found, just update
        dit->second.add(val, is_actual);
    } else {
        // multiplicity hasn't been seen so far
        Distr distr(val, is_actual);
        _distrs.insert(
            std::make_pair(multiplicity, distr)
        );
    }
    
    // update min/max of multiplicities seen so far
    if (multiplicity < _minmult) _minmult = multiplicity;
    if (multiplicity > _maxmult) _maxmult = multiplicity;
}

void Stat::evaluate()
{
    for (auto& distr : _distrs) {
        distr.second.evaluate();
    }
}

const Stat::Distr& Stat::distr(unsigned int multiplicity) const
{
    distrs_t::const_iterator dit = _distrs.find(multiplicity);
    if (dit == _distrs.end())
        throw NotfoundException(multiplicity);
    return dit->second;
}

}   // namespace prob
}   // namespace multovl
