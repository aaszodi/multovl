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
// == Module freeregions.cc ==

// -- System headers --

#include <cmath>
#include <cassert>
#include <algorithm>

#ifdef DEBUGG
#include <iostream>
#define NDEBUG
#endif

// -- Own header --

#include "freeregions.hh"

// == Implementation ==

namespace multovl {
namespace prob {

FreeRegions::FreeRegions(const std::vector<Region>& frees)
    : _frees(frees), _roulette_sectors()
{
    // store the "roulette wheel sections"
    const unsigned int FREESIZE = frees.size();
    _roulette_sectors.reserve(FREESIZE + 1);
    _roulette_sectors.push_back(0.0);   // [0]-th item
    float flensum = 0.0;
    unsigned int i;
    for (i=0; i<FREESIZE; ++i)
    {
        unsigned int flen = _frees[i].length();
        #ifdef DEBUGG
        std::cerr << "_freelengths[" << i << "] = " << flen << std::endl;
        #endif
        flensum += flen;
        _roulette_sectors.push_back(flensum);
    }
    assert(flensum > 0);
    for (i=1; i<=FREESIZE; ++i)
    {
        _roulette_sectors[i] /= flensum;
        #ifdef DEBUGG
        std::cerr << "_roulette_sectors[" << i << "] = " << _roulette_sectors[i] << std::endl;
        #endif
    }
    // looks like 0, 0.15, 0.27, ... , 1.0 sorted
}

bool FreeRegions::fit(const Region& reg) const
{
    // very simple linear algorithm
    for (unsigned int i = 0; i < _frees.size(); ++i)
    {
        if (_frees[i].first() <= reg.first() && reg.last() <= _frees[i].last())
            return true; // yes, it fits
    }
    return false;
}

const Region& FreeRegions::select_free_region(UniformGen& rng, unsigned int minlen) const
{
    unsigned int idx;
    do {
        float rnd = rng(); // 0.0 .. 1.0
        std::vector<float>::const_iterator roulit = 
            std::lower_bound(_roulette_sectors.begin(), _roulette_sectors.end(), rnd);
        idx = (roulit - _roulette_sectors.begin());
        if (idx > 0) --idx; // N free regions, N+1 sector elems
        #ifdef DEBUGG
        std::cerr << "Roulette selector: rnd = " << rnd << ", idx = " << idx << std::endl;
        #endif
    } while(_frees[idx].length() < minlen);
    return _frees[idx];
}

}   // namespace prob
}   // namespace multovl
