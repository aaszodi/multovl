#ifndef MULTOVL_REGLIMIT_HEADER
#define MULTOVL_REGLIMIT_HEADER

// == Header reglimit.hh ==

/// \file 
/// \brief Helper class to keep track of the limits (first, last position)
/// of regions to be overlapped.
/// \author Andras Aszodi
/// \date 2011-11-02

// -- System headers --
    
#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>

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
    
    RegLimit(): _regp(), _isfirst(false) {}
    
    explicit RegLimit(const boost::shared_ptr<AncestorRegion>& regp, 
        bool isfirst=true)
    : _regp(regp), _isfirst(isfirst) {}
    
    const AncestorRegion& region() const
    {
        return *_regp;
    }
    
    bool is_first() const
    {
        return _isfirst;
    }
    
    unsigned int track_id() const
    {
        return _regp->track_id();   // convenience function
    }
    
    unsigned int this_pos() const
    {
        return (is_first()? _regp->first(): _regp->last());
    }
    
    unsigned int other_pos() const
    {
        return (is_first()? _regp->last(): _regp->first());
    }
    
    /// Ordering according to position, or first before last if the same position.
    bool operator<(const RegLimit& other) const;
    
    #ifdef DEBUG
        const AncestorRegion* raw_region_ptr() const
        {
            return _regp.get();
        }
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
