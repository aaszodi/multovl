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
    explicit ShuffleOvl(const std::vector<Region>& frees);
    
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

