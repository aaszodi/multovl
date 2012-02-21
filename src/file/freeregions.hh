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
