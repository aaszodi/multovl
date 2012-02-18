#ifndef RANDOMPLACER_HEADER
#define RANDOMPLACER_HEADER

// == Header randomplacer.hh ==

// Places a set of regions randomly within a prescribed set
// of free regions.
// 14-Mar-2007. Andras Aszodi

// -- System headers --

#include <vector>
using namespace std;

// -- Simulib headers --

#include "PsdesGenerator.hh"

// -- Regions headers --

#include "regions.hh"

// -- Classes --

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
    explicit FreeRegions(const vector<Region>& frees);
    
    /// Picks one of the free regions randomly with a probability
    /// corresponding to the regions' lengths
    /// \param rng A random number generator from the Simulib framework
    /// \param minlen The selected region must be at least this long
    /// (this is how we make sure a region would fit into the selected free region)
    /// \return const ref to a randomly selected free region
    const Region& select_free_region(Simulib::RandomGenerator& rng, unsigned int minlen) const;
    
    private:
    
    // data
    vector<Region> _frees;
    vector<float> _roulette_sectors;
};

/**
 * A RandomPlacer object is responsible for placing a set of regions
 * randomly within a prescribed set of free regions.
 */
class RandomPlacer
{
    public:
            
    /// Inits with regions to be placed randomly within free regions.
    /// \param Regions the regions to be shuffled.
    explicit RandomPlacer(const vector<Region>& Regions);
    
    /// Performs a shuffling.
    /// \param frees a FreeRegions object containing the free regions
    /// \param rng a Simulib random number generator
    void random_placement(const FreeRegions& frees, Simulib::RandomGenerator& rng);
    
    /// Returns the randomly placed regions after a random_placement(...) call.
    const vector<Region>& get_regions() const { return _placed; }
    
    private:
            
    // data   
    vector<Region> _placed;
    vector<unsigned int> _lengths;
};

#endif  // RANDOMPLACER_HEADER
