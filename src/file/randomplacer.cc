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
    _lengths(), _placed()
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

void RandomPlacer::random_placement(const FreeRegions& frees, UniformGen& rng)
{
    unsigned int i, N = _lengths.size();
    _placed.clear();
    
    // go through all region lengths, place them
    for (i=0; i<N; ++i)
    {
        unsigned int reglen = _lengths[i];
        const Region& free = frees.select_free_region(rng, reglen);
        unsigned int freelen = free.length();
        
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
