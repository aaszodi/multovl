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
