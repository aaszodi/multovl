#ifndef MULTOVL_REGLIMIT_HEADER
#define MULTOVL_REGLIMIT_HEADER

// == Header reglimit.hh ==

/// \file 
/// \brief Helper class to keep track of the limits (first, last position)
/// of regions to be overlapped.
/// \author Andras Aszodi
/// \date 2011-11-02

// -- System headers --
    
// -- Boost headers --

#include "boost/serialization/shared_ptr.hpp"

// -- Own headers --

#include "ancregion.hh"     // pulls in region.hh as well

// == Classes ==

namespace multovl {

/// Each ancestor region to be overlapped is stored twice in MultiRegion's lookup;
/// once as a "first pos", and once as a "last pos".
/// The RegLimit class implements these "region limit" objects.
/// It can also be serialized.
class RegLimit
{
public:
    
    /// Init to empty
    RegLimit(): _regp(), _isfirst(false) {}
    
    /// Init with a shared pointer to an ancestor region
    /// \param regp shared pointer to an ancestor region, usually created with boost::make_shared
    /// \param isfirst true if first position, false if last
    explicit RegLimit(const boost::shared_ptr<AncestorRegion>& regp, 
        bool isfirst=true)
    : _regp(regp), _isfirst(isfirst) {}
    
    // -- Accessors --
    
    /// \return the AncestorRegion underlying this RegLimit.
    const AncestorRegion& region() const
    {
        return *_regp;
    }
    
    /// \return /true/ if this limit is the "first" coordinate of the underlying AncestorRegion.
    bool is_first() const
    {
        return _isfirst;
    }
    
    /// Sets the first attribute of the calling object.
    void first(bool isfirst)
    {
        _isfirst = isfirst;
    }
    
    unsigned int track_id() const
    {
        return _regp->track_id();   // convenience function
    }
    
    /// \return the position of the calling object, depending on is_first().
    /// If is_first() == true, then the first position is returned, otherwise the last is returned.
    unsigned int this_pos() const
    {
        return (is_first()? _regp->first(): _regp->last());
    }
    
    /// \return the "other" position of the calling object, depending on is_first().
    /// If is_first() == true, then the last position is returned, otherwise the first is returned.
    unsigned int other_pos() const
    {
        return (is_first()? _regp->last(): _regp->first());
    }
    
    /// Ordering according to position, or first before last if the same position.
    bool operator<(const RegLimit& other) const;
    
    /// Deep copy operation.
    /// The default copy ctor returns a shallow copy in the sense that 
    /// only the internal shared pointer is copied but not the AncestorRegion it is pointing to,
    /// as it is to be expected from a boost::shared_ptr.
    /// \return a new RegLimit object whose internal shared pointer points to a completely
    /// separate copy of the AncestorRegion.
    RegLimit deep_copy() const
    {
        boost::shared_ptr<AncestorRegion> ancp(new AncestorRegion(this->region()));
        return RegLimit(ancp, this->is_first()); 
    }
    
    #ifdef DEBUG
        // these are used by the unit tests only
        const AncestorRegion* const_raw_region_ptr() const { return _regp.get(); }
        AncestorRegion* raw_region_ptr() { return _regp.get(); }
    #endif
    
private:
    
    // data
    boost::shared_ptr<AncestorRegion> _regp;
    bool _isfirst;
    
    // serialization
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & _regp;
        ar & _isfirst;
    }
    
};  // class RegLimit
    
}   // namespace multovl

#endif  // MULTOVL_REGLIMIT_HEADER
