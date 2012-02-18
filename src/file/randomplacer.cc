// == Implementation randomplacer.cc ==

// -- System headers --

#include <cmath>
#include <cassert>
#include <algorithm>

#ifdef DEBUGG
#include <iostream>
#define NDEBUG
#endif
using namespace std;

// -- Own header --

#include "randomplacer.hh"

// -- FreeRegions implementation --

FreeRegions::FreeRegions(const vector<Region>& frees)
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
        cerr << "_freelengths[" << i << "] = " << flen << endl;
        #endif
        flensum += flen;
        _roulette_sectors.push_back(flensum);
    }
    assert(flensum > 0);
    for (i=1; i<=FREESIZE; ++i)
    {
        _roulette_sectors[i] /= flensum;
        #ifdef DEBUGG
        cerr << "_roulette_sectors[" << i << "] = " << _roulette_sectors[i] << endl;
        #endif
    }
    // looks like 0, 0.15, 0.27, ... , 1.0 sorted
}

const Region& FreeRegions::select_free_region(Simulib::RandomGenerator& rng, unsigned int minlen) const
{
    unsigned int idx;
    do {
        float rnd = rng.std_uniform(); // 0.0 .. 1.0
        vector<float>::const_iterator roulit = 
            lower_bound(_roulette_sectors.begin(), _roulette_sectors.end(), rnd);
        idx = (roulit - _roulette_sectors.begin());
        if (idx > 0) --idx; // N free regions, N+1 sector elems
        #ifdef DEBUGG
        cerr << "Roulette selector: rnd = " << rnd << ", idx = " << idx << endl;
        #endif
    } while(_frees[idx].length() < minlen);
    return _frees[idx];
}

// -- RandomPlacer implementation --

RandomPlacer::RandomPlacer(const vector<Region>& Regions):
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

void RandomPlacer::random_placement(const FreeRegions& frees, Simulib::RandomGenerator& rng)
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
        float rnd = rng.std_uniform();
        unsigned int beg = static_cast<unsigned int>(floor((freeN-free1)*rnd)) + free1;
        #ifdef DEBUGG
        cerr << "free1=" << free1 << ", freeN=" << freeN
            << ", rnd=" << rnd << ", beg=" << beg << ", end=" << (beg + reglen - 1) <<endl;
        #endif
        Region Ranreg(beg, beg + reglen - 1);
        _placed.push_back(Ranreg);  // remember successfully placed region
    }
}

