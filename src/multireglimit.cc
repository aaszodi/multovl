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
// == Module multireglimit.cc ==

// -- Own header --

#include "multireglimit.hh"

// -- Boost headers --

// -- System headers --

#include <algorithm>

// == Implementation ==

namespace multovl {

MultiRegLimit::MultiRegLimit(const MultiRegLimit& other)
{
    deep_copy(other.reglim(), _reglim);
}

MultiRegLimit& MultiRegLimit::operator=(const MultiRegLimit& rhs)
{
    if (this == &rhs) return *this;
    reglim_t newreglim;
    deep_copy(rhs.reglim(), newreglim);
    this->_reglim.swap(newreglim);
    return *this;
}

void MultiRegLimit::deep_copy(const reglim_t& source, reglim_t& target)
{
    // for each AncestorRegion there are two RegLimits in the reglim_t multiset
    // so we need to deep-copy only the "first" RegLimit in /source/
    // and the corresponding "last" RegLimit can be shallow-copied
    for (reglim_t::const_iterator rlit = source.begin(); rlit != source.end(); ++rlit)
    {
        if (!rlit->is_first())  // skip "last" limit
            continue;
        RegLimit limfirst = rlit->deep_copy();
        target.insert(limfirst);
        RegLimit limlast(limfirst); // shallow copy
        limlast.first(false);
        target.insert(limlast);
    }
}

void MultiRegLimit::add(const Region& region, unsigned int trackid)
{
    // Store /region/ twice in the region limit map /_reglim/
    // through smart pointers
    boost::shared_ptr<AncestorRegion> regp(new AncestorRegion(region, trackid));
    // once as a "first position"
    RegLimit limfirst(regp, true);
    _reglim.insert(limfirst);
    // ... and then as "last position"
    RegLimit limlast(regp, false);
    _reglim.insert(limlast);
}

MultiRegLimit& MultiRegLimit::operator+=(const MultiRegLimit& rhs)
{
    reglim_t result;
    std::merge(
        this->reglim().begin(), this->reglim().end(),
        rhs.reglim().begin(), rhs.reglim().end(),
        std::inserter(result, result.begin())
    );
    this->_reglim.swap(result);
    return *this;
}

}   // namespace multovl
