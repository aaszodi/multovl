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
// == Implementation randomplacer.cc ==

// -- Own header --

#include "randomplacer.hh"

// -- System headers --

#include <cmath>
#include <cassert>
#include <algorithm>

#ifdef DEBUGG
#include <iostream>
#define NDEBUG
#endif

namespace multovl {
namespace prob {

// -- RandomPlacer implementation --

RandomPlacer::RandomPlacer(const std::vector<Region>& Regions):
    _placed(), _lengths() 
{
    const unsigned int REGSIZE = Regions.size();
    _lengths.reserve(REGSIZE);
    _placed.reserve(REGSIZE);

    unsigned int i;    
    // store only the lengths of the regions to be randomly placed
    for (i=0; i<REGSIZE; ++i)
    {
        _lengths.push_back(Regions[i].length());
    }
}

bool RandomPlacer::add(unsigned int len)
{
    if (len == 0)
        return false;
    _lengths.push_back(len);
    return true;
}

void RandomPlacer::random_placement(const FreeRegions& frees, UniformGen& rng)
{
    unsigned int i, N = _lengths.size();
    _placed.clear();
    
    // go through all region lengths, place them
    for (i=0; i<N; ++i)
    {
        unsigned int reglen = _lengths[i];
        const Region& free = frees.select_free_region(rng, reglen);
        
        // place current region within selected free region
        // generate a random offset from free region begin
        unsigned int free1 = free.first(),
            freeN = free.last() - reglen;
        double rnd = rng();
        unsigned int beg = static_cast<unsigned int>(std::floor((freeN-free1)*rnd)) + free1;
        #ifdef DEBUGG
        std::cerr << "free1=" << free1 << ", freeN=" << freeN
            << ", rnd=" << rnd << ", beg=" << beg << ", end=" << (beg + reglen - 1) << std::endl;
        #endif
        Region Ranreg(beg, beg + reglen - 1, '.', "shuffled");
        _placed.push_back(Ranreg);  // remember successfully placed region
    }
}

}   // namespace prob
}   // namespace multovl
