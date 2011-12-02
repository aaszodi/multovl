#ifndef MULTOVL_MULTIREGION_HEADER
#define MULTOVL_MULTIREGION_HEADER

// == Header multiregion.hh ==

/// \file 
/// \brief Region representing multiple overlaps for file-based "multovl".
/// \author Andras Aszodi
/// \date 2010-08-11

// -- Library headers --

#include "ancregion.hh"     // pulls in region.hh 

// -- Boost headers --

#include "boost/serialization/set.hpp"  // other serialization headers come from [anc]region.hh

// == CLASSES ==

namespace multovl {

/**
 * Multiple regions (overlaps) which constitute the output type of the multioverlap operations.
 * Use this for file-based "multovl" where it is important that the objects can be
 * written to a text file.
 */
class MultiRegion: public Region
{
    public:
    
    /// Init to empty (to be used by STL containers etc)  
    MultiRegion();
    
    /** 
     * Inits to a region between [first..last] to contain
     * a (multi)set of regions as its "ancestry". Note that multiregions
     * have unspecified ('.') strands and a name "overlap" by default,
     * but these can be changed using the strand(char) and name(const std::string&) methods
     * inherited from Region.
     * \param first start of the multiregion
     * \param last end of the multiregion
     * \param ancestors multiset of AncestorRegion objects, empty if omitted
     * \param mult the maximal multiplicity of the overlap, if 0 (default) then
     * ancestors.size() will be used.
     */
    MultiRegion(unsigned int first, unsigned int last, 
        const ancregset_t& ancestors = ancregset_t(), unsigned int mult = 0);
    
    /// Adds an ancestor to the calling object.
    /// \param anc the ancestor to be added.
    void add_ancestor(const AncestorRegion& anc);

    /// Returns the multiplicity of the overlap, ie. how many tracks
    /// contributed to this region. 
    /// Not the same as the number of ancestors when "union overlaps" are generated.
    unsigned int multiplicity() const { return _mult; }
    
    /// Returns the set of ancestors.
    const ancregset_t& ancestors() const { return _ancestors; }
    
    /// Return the track IDs of the ancestors in a vector.
    std::vector<int> ancestor_trackids() const;
    
    /**
     * Returns /true/ if this is a 'solitary' multiregion,
     * ie. it has multiplicity()==1 and its ancestor does not overlap
     * with anyone else
     */
    bool solitary() const { return _solitary; }
    
    /// Returns the "ancestor std::string" which is a '|'-separated list of
    /// what AncestorRegion::to_attrstring()-formatted ancestor descriptors.
    /// Can be used e.g. als GFF attributes.
    std::string anc_str() const;
    
    private:
    
    bool update_solitary();
    
    ancregset_t _ancestors; // ancestors
    bool _solitary;
    unsigned int _mult;
    
    // serialization
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<Region>(*this);
        ar & _ancestors & _solitary & _mult;
    }
    
};  // class MultiRegion
	
}   // namespace multovl

#endif // MULTOVL_MULTIREGION_HEADER
