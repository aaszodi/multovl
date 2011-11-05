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
    
    virtual ~MultiRegLimit() {}
    
    /// Adds a region /region/ identified by /trackid/
    /// to the calling object.
    /// \param region a genomic region to be stored
    /// \param trackid an arbitrary number identifying a track (collection of regions).
    void add(const Region& region, unsigned int trackid);

    /// \return const access to the RegLimit multiset inside
    const reglim_t& reglim() const { return _reglim; }
    
    private:
            
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
