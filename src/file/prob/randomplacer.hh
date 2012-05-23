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
#ifndef MULTOVL_PROB_RANDOMPLACER_HEADER
#define MULTOVL_PROB_RANDOMPLACER_HEADER

// == Header randomplacer.hh ==

/// \file Places a set of regions randomly within a prescribed set
/// of free regions.
/// \date 2012-02-21
/// \author Andras Aszodi

// -- Library headers --

#include "freeregions.hh"

// -- System headers --

#include <vector>

namespace multovl {
namespace prob {

// == Classes ==

/**
 * A RandomPlacer object is responsible for placing a set of regions
 * randomly within a prescribed set of free regions.
 */
class RandomPlacer
{
    public:
    
    /// Init to empty
    RandomPlacer():
        _placed(),
        _lengths()
    {}
    
    /// Inits with regions to be placed randomly within free regions.
    /// \param Regions the regions to be shuffled. Note that only the lengths of the regions will be used.
    explicit RandomPlacer(const std::vector<Region>& Regions);
    
    /// Adds the length of a region to the calling object.
    /// \param len the region length, should be > 0
    /// \return true on success, false if /len/ == 0 (which will not be added)
    bool add(unsigned int len);
    
    /// Performs a shuffling.
    /// \param frees a FreeRegions object containing the free regions
    /// \param rng a uniform [0,1) random number generator
    void random_placement(const FreeRegions& frees, UniformGen& rng);
    
    /// Returns the randomly placed regions after a random_placement(...) call.
    const std::vector<Region>& get_regions() const { return _placed; }
    
    private:
            
    // data   
    std::vector<Region> _placed;
    std::vector<unsigned int> _lengths;
};

}   // namespace prob
}   // namespace multovl

#endif  // MULTOVL_PROB_RANDOMPLACER_HEADER
