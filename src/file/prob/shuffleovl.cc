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

// do not use this unless something goes seriously wrong
#define MULTOVL_DEBUG

// == Implementation ==

namespace multovl {
namespace prob {

ShuffleOvl::ShuffleOvl(const freeregvec_t& frees):
    MultiOverlap(),
    _freeregions(frees),
    _shufflecount(0)
{}

bool ShuffleOvl::add_randomplacer(const Region& reg, unsigned int trackid, bool shuffle)
{
    if (reg.length() == 0)
        return false;
    bool ok = false;
    
    // first add to the ancestors (pushes back)
    add(reg, trackid);
    
    if (shuffle) {
        // if the region is "shufflable" (ProbPipeline decides this)
        // then also add it to the RandomPlacer
        // the newly added region is the last ancestor seen so far
        _randomplacer.add(ancregions().back());
    }
}

unsigned int ShuffleOvl::shuffle_overlaps(UniformGen& rng,
        unsigned int ovlen, unsigned int minmult, unsigned int maxmult, 
        unsigned int ext, bool intrack)
{
    // Extend the region limits if required
    Region::set_extension(ext);
    
    // shuffle the movable regions
    // this just updates the region limits
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
    // this just updates the region limits
    shuffle(rng);
    
    // generate the union overlaps
    unsigned int regcount = generate_unionoverlaps(ovlen, minmult, maxmult);
        
    // reset the extensions
    Region::set_extension(0);
    return regcount;
}

#ifdef MULTOVL_DEBUG
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
    using namespace debug;
    
#ifdef MULTOVL_DEBUG
    std::cout << "** The contents of _ancregions upon entering shuffle:" << std::endl;
    for (const auto& reg : ancregions()) {
        std::cout << reg << std::endl;
    }
#endif

    // remove all RegLimits referring to the reshufflable tracks
    
    for (rpm_t::const_iterator rcit = _rpm.begin(); rcit != _rpm.end(); ++rcit)
    {
        unsigned int rtid = rcit->first;    // reshufflable track ID
        MultiOverlap::reglimset_t::const_iterator rlit = reglims().begin();
        while(rlit != reglims().end())
        {
            if (rtid == rlit->track_id())
            {
                MultiOverlap::reglimset_t::iterator me = rlit;   // rlit still valid
                ++rlit;                         // so increment it...
                nonconst_reglims().erase(me);    // me is now invalid
            }
            else ++rlit;
        }
    }
    
#ifdef MULTOVL_DEBUG
    std::cout << "** The contents of _ancregions after reglimit deletion:" << std::endl;
    for (const auto& reg : ancregions()) {
        std::cout << reg << std::endl;
    }
#endif

    // shuffle the reshufflable tracks
    for (auto& r : _rpm) {
        r.second.random_placement(_freeregions, rng);
    }
    
    // add their limits again
    for (const auto& r : _rpm) {
        const auto& shuffledregs = r.second.get_regions();
        for (const auto& sr : shuffledregs) {
            add_reglimit(sr);
            std::cout << "** Added limit " << sr << ", track " << rtid << std::endl;
        }
    }
#ifdef MULTOVL_DEBUG
    std::cout << "** Reglims contents after adding reshuffled limits:" << std::endl;
    for (const auto& rl : reglims()) {
        std::cout << rl << std::endl;
    }
    std::cout << "** The contents of _ancregions:" << std::endl;
    for (const auto& reg : ancregions()) {
        std::cout << reg << std::endl;
    }
#endif

    return ++_shufflecount;
}

}   // namespace prob
}   // namespace multovl

