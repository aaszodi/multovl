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
// == MODULE shuffleovl.cc ==

/// \file
/// \date 2012-02-26
/// \author Andras Aszodi

// -- Own header --

#include "shuffleovl.hh"

// -- Boost headers --

// -- Standard headers --

#include <algorithm>

// == Implementation ==

namespace multovl {
namespace prob {

ShuffleOvl::ShuffleOvl(const freeregvec_t& frees):
    MultiOverlap(),
    _freeregions(frees),
    _shuffleables(),
    _shufflecount(0)
{}

unsigned int ShuffleOvl::shuffle_overlaps(UniformGen& rng,
        unsigned int ovlen, unsigned int minmult, unsigned int maxmult, 
        unsigned int ext, bool intrack)
{
    // Extend the region limits if required
    Region::set_extension(ext);
    
    // shuffle the movable regions
    shuffle(rng);
    
    // generate the overlaps
    unsigned int regcount = generate_overlaps(ovlen, minmult, maxmult, intrack);
    
    // reset the extensions
    Region::set_extension(0);
    return regcount;
}

unsigned int ShuffleOvl::shuffle_unionoverlaps(UniformGen& rng,
        unsigned int ovlen, unsigned int minmult, unsigned int maxmult,
        unsigned int ext)
{
    // Extend the region limits if required
    Region::set_extension(ext);
    
    // shuffle the movable regions
    shuffle(rng);
    
    // generate the union overlaps
    unsigned int regcount = generate_unionoverlaps(ovlen, minmult, maxmult);
        
    // reset the extensions
    Region::set_extension(0);
    return regcount;
}

#ifndef NDEBUG
namespace debug {
    // ad-hoc debug functions
    std::ostream& operator<<(std::ostream& ostr, const Region& reg) {
        ostr << "*** " << reg.first() << "-" << reg.last() << ":" << reg.name();
        return ostr;
    }
    std::ostream& operator<<(std::ostream& ostr, const RegLimit& rl) {
        ostr << "*** this_pos()=" << rl.this_pos()
            << " --> " << rl.region();
        return ostr;
    }
    // end of ad-hoc debug functions
}
#endif

// Shuffle the "shufflable" tracks
// \param rng a uniform[0,1) random number generator
// \return the new shuffle count
// Private
unsigned int ShuffleOvl::shuffle(UniformGen& rng)
{
#ifndef NDEBUG
    using namespace debug;
    std::cerr << "** The contents of _ancregions upon entering shuffle:" << std::endl;
    for (const auto& reg : ancregions()) {
        std::cerr << reg << std::endl;
    }
#endif

    // cache the shuffleable regions
    if (_shuffleables.size() == 0) {
        for (const auto& ancreg : ancregions()) {
            if (ancreg.is_shuffleable()) {
                _shuffleables.push_back(ancreg);
            }
        }
    }
#ifndef NDEBUG
    std::cerr << "** The contents of _shuffleables upon entering shuffle:" << std::endl;
    for (const auto& reg : _shuffleables) {
        std::cerr << reg << std::endl;
    }
#endif

    // remove all RegLimit-s referring to the regions in the reshufflable tracks
    for (auto rlit = nonconst_reglims().begin(); rlit != nonconst_reglims().end(); ) {
        if ( rlit->region().is_shuffleable() ) {
            // RegLimit of reshufflable AncRegion, remove
            rlit = nonconst_reglims().erase(rlit);
        } else {
            ++rlit;
        }
    }
    
    // shuffle the reshufflable regions
    // and add their changed limits to reglimits()
    for (auto& sreg : _shuffleables) {
        if (!place_randomly(rng, sreg))
            continue;
        add_reglimit(sreg);
    }
    
#ifndef NDEBUG
    std::cerr << "** Reglims contents after adding reshuffled limits:" << std::endl;
    for (const auto& rl : reglims()) {
        std::cerr << rl << std::endl;
    }
#endif

    return ++_shufflecount;
}

// Shifts the coordinates of an AncestorRegion randomly so that it still fits
// into one of the randomly picked free regions.
// \param rng Uniform random number generator
// \param sreg An AncestorRegion. Its coordinates will be randomly shifted.
// \return /true/ if the shift operation was successful, /false/ otherwise.
// Private
bool ShuffleOvl::place_randomly(UniformGen& rng, AncestorRegion& sreg) const {
    try {
        unsigned int reglen = sreg.length();
        const auto& free = _freeregions.select_free_region(rng, reglen);
        unsigned int free1 = free.first(), freeN = free.last() - reglen;
        double rnd = rng();
        int newbeg = static_cast<unsigned int>(std::floor((freeN-free1)*rnd)) + free1,
            newend = newbeg + reglen - 1;
#ifndef NDEBUG
        std::cerr << "** free1=" << free1 << ", freeN=" << freeN
            << ", rnd=" << rnd << ", beg=" << newbeg << ", end=" << newend << std::endl;
#endif
        sreg.set_coords(newbeg, newend);
        return true;
    } catch(const std::length_error&) {
        return false;
    }
}

}   // namespace prob
}   // namespace multovl

