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
    
    private:
    
    // data
    FreeRegions _freeregions;
    typedef std::map<unsigned int, RandomPlacer> rpm_t; // track ID => RandomPlacer for the "shufflable tracks"
    rpm_t _rpm;

    // serialization TODO???
#if 0
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<multovl::MultiOverlap>(*this);
        // TODO ar & _freeregions & _randomplacer; ???
    }
#endif
};

}   // namespace prob
}   // namespace multovl

#endif  // MULTOVL_PROB_SHUFFLEOVL_HEADER

