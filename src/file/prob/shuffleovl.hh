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
#ifndef MULTOVL_PROB_SHUFFLEOVL_HEADER
#define MULTOVL_PROB_SHUFFLEOVL_HEADER

// == HEADER shuffleovl.hh ==

/// \file Module to calculate multiple overlaps repeatedly after reshuffling some of the tracks.
/// \date 2012-02-25
/// \author Andras Aszodi

// -- Library headers --

#include "multioverlap.hh"
#include "randomplacer.hh"

// -- Boost headers --

// -- Standard headers --

#include <map>

namespace multovl {
namespace prob {

// == Classes ==

/// Class for calculating multiple overlaps repeatedly after reshuffling some of the tracks.
class ShuffleOvl: public MultiOverlap 
{
public:
        
    /// Init with free regions
    /// \param frees a vector of free regions into which all fixed and shufflable regions must fall
    explicit ShuffleOvl(const freeregvec_t& frees);
    
    /// Checks whether a given region "fits" into one of the free regions
    /// (all regions must be contained in a free region).
    /// If a region does not fit, then it must not be considered for overlap shuffling.
    /// Note that the pipeline input operation takes care of this.
    /// \param reg The region to be tested
    /// \return true if /reg/ fits, false otherwise.
    bool fit_into_frees(const Region& reg) const { return _freeregions.fit(reg); }
    
    /// Adds the length of a "shufflable" region in a track to the internal trackid => RandomPlacer map.
    /// \param reglen the length of the region, should be > 0
    /// \param trackid the track ID 
    /// \return true on success, false if reglen == 0
    bool add_randomplacer(unsigned int reglen, unsigned int trackid);
    
    /// Shuffle the "shufflable" tracks
    /// \param rng a uniform[0,1) random number generator
    /// \return the new shuffle count
    unsigned int shuffle(UniformGen& rng);
    
private:
    
    // data
    FreeRegions _freeregions;
    typedef std::map<unsigned int, RandomPlacer> rpm_t; // track ID => RandomPlacer for the "shufflable tracks"
    rpm_t _rpm;
    unsigned int _shufflecount;

#if 0
    // serialization LATER (if at all)
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<multovl::MultiOverlap>(*this);
        // TODO ar & _freeregions & _rpm; ???
    }
#endif
};

}   // namespace prob
}   // namespace multovl

#endif  // MULTOVL_PROB_SHUFFLEOVL_HEADER

