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
#ifndef MULTOVL_PROB_FREEREGIONS_HEADER
#define MULTOVL_PROB_FREEREGIONS_HEADER

// == Header freeregions.hh ==

///\file Manages a set of genomic regions into which shuffled regions can be placed.
/// Represents regions in which one can sequence (i.e. non-repeats) or which have
/// been covered by probes.
/// \date 2012-02-21
/// \author Andras Aszodi

// -- Own headers --

#include "region.hh"
#include "randomgen.hh"

// -- System headers --

#include <vector>

namespace multovl {
namespace prob {

// == Classes ==

/// A FreeRegions object holds the genomic regions within which
/// a shuffled actual region may be placed, and can select one randomly
/// with a probability proportional to the free region's length if asked.
/// Free regions are those genomic regions in which it is possible
/// to detect "peaks". For microarrays, these are regions containing
/// probes. For ChIP-Seq or RNA-Seq, repeat-free regions into which
/// reads can be mapped.
class FreeRegions
{
    public:
    
    /// Inits with a vector of free regions.
    /// All must be on the same chromosome of course.
    /// They should not overlap (not tested).
    /// \param frees vector of free regions
    explicit FreeRegions(const std::vector<Region>& frees);
    
    /// Checks if a given region fits into one of the free regions.
    /// \param reg the region to be tested
    /// \return true if /reg/ fits, false if not.
    bool fit(const Region& reg) const;
    
    /// Picks one of the free regions randomly with a probability
    /// corresponding to the regions' lengths
    /// \param rng A uniform random number generator
    /// \param minlen The selected region must be at least this long
    /// (this is how we make sure a region would fit into the selected free region)
    /// \return const ref to a randomly selected free region
    const Region& select_free_region(UniformGen& rng, unsigned int minlen) const;
    
    private:
    
    // data
    std::vector<Region> _frees;
    std::vector<float> _roulette_sectors;
};

}   // namespace prob
}   // namespace multovl

#endif  // MULTOVL_PROB_FREEREGIONS_HEADER
