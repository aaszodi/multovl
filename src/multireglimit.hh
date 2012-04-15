#ifndef MULTOVL_MULTIREGLIMIT_HEADER
#define MULTOVL_MULTIREGLIMIT_HEADER

// == Header multireglimit.hh ==

/// \file 
/// \brief Collection of multiple region limits.
/// \author Andras Aszodi
/// \date 2011-11-05

// -- System headers --
    
// -- Boost headers --

#include "boost/serialization/set.hpp"

// -- Own headers --

#include "reglimit.hh"

// == Classes ==

namespace multovl {

/**
 * Objects of the MultiRegLimit class store genomic regions encoded
 * by the regions' first and last positions.
 * Internally, this is implemented by a multiset storing RegLimit objects.
 */
class MultiRegLimit
{
public: 
	
    typedef std::multiset<RegLimit> reglim_t;
    
	// -- methods --
           
    /// Init to empty 
    MultiRegLimit(): _reglim() {}
    
    /// Init to contain a region and trackid 
    MultiRegLimit(const Region& region, unsigned int trackid): 
        _reglim()
    {
        add(region, trackid);
    }
    
    /// Copy ctor
    /// Implements a deep copy so that the AncestorRegion objects under the RegLimits
    /// will be completely independent from those under /other/
    MultiRegLimit(const MultiRegLimit& other);
    
    /// Assignment
    /// Implements a deep copy of the underlying AncestorRegion objects just like the copy ctor
    MultiRegLimit& operator=(const MultiRegLimit& rhs);
    
    virtual ~MultiRegLimit() {}
    
    /// Adds a region /region/ identified by /trackid/
    /// to the calling object.
    /// \param region a genomic region to be stored
    /// \param trackid an arbitrary number identifying a track (collection of regions).
    void add(const Region& region, unsigned int trackid);

    /// \return const access to the RegLimit multiset inside
    const reglim_t& reglim() const { return _reglim; }
    
    /// Merges another MultiRegLimit object into the calling object.
    /// \param rhs the other MultiRegLimit object.
    /// \return the calling object
    MultiRegLimit& operator+=(const MultiRegLimit& rhs);
    
protected:
    
    /// \return non-const access to the RegLimit multiset inside
    reglim_t& nonconst_reglim() { return _reglim; }
    
private:
    
    static
    void deep_copy(const reglim_t& source, reglim_t& target);
    
    // data 
    reglim_t _reglim;

    // serialization
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & _reglim;
    }
    
};

} // namespace multovl

#endif  // MULTOVL_MULTIREGLIMIT_HEADER
