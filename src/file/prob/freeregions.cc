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