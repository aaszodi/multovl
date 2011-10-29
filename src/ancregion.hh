#ifndef MULTOVL_ANCREGION_HEADER
#define MULTOVL_ANCREGION_HEADER

// == HEADER ancregion.hh ==

/** \file 
 * \brief Ancestor regions representing the regions giving rise to overlaps.
 * \author Andras Aszodi
 * \date 2011-05-28.
 */

// -- Standard headers --

#include <string>
#include <set>
#include <vector>

// -- Boost headers --

#include "boost/operators.hpp"

// -- Library headers --

#include "region.hh"    // base class for AncestorRegion

namespace multovl {

/**
 * \brief AncestorRegion objects represent the regions that give rise to overlaps.
 * They are essentially Region-s, with the additional track ID attribute (an uint)
 * that indicates which track they belong to. Track IDs are assigned by
 * client programs and may indirectly refer to files or database objects.
 */
class AncestorRegion: public Region,
    boost::less_than_comparable< AncestorRegion,
    boost::equality_comparable<AncestorRegion> >
{
    public:
    
    /// Init to empty
    AncestorRegion(): Region(), _trackid(0) {}

    /// Init with base class /region/ and /trackid/ (default 0)
    explicit AncestorRegion(const Region& region, unsigned int trackid = 0): 
        Region(region), _trackid(trackid) {}
    
    /**
     * Inits to contain the genomic region with positions [f..l],
     * strand /s/, name /nm/, track ID /trackid/.
     */
    AncestorRegion(unsigned int f, unsigned int l,
        char s, const std::string& nm, unsigned int trackid = 0): 
        Region(f, l, s, nm), _trackid(trackid) {}
    
    /// Returns the current track ID.
    unsigned int track_id() const { return _trackid; }
    
    /// Sets the track ID to /trackid/. Returns old track ID.
    unsigned int track_id(unsigned int trackid);
    
    /// Equality. All fields must be equal.
    bool operator==(const AncestorRegion& rhs) const;
    
    /// Ordering.
    /// Sorted on the track ID, then on the coordinates and the name (lexicographically).
    bool operator<(const AncestorRegion& rhs) const;
    
    /// Makes a string that describes the ancestor region
    /// so that it can be put in a GFF-style ANCESTORS attribute
    /// when Multovl prints the overlaps.
    /// The format is "<trackid>:<name>:<strand>:<first>-<last>"
    std::string to_attrstring() const;
    
    private:
    
    unsigned int _trackid;
};  // class AncestorRegion

/// Collection of ancestors for a descendant region.
/// Under certain conditions the same region may occur multiple times in the input,
/// therefore a multiset is used to hold the ancestors of an overlap.
typedef std::multiset<AncestorRegion> ancregset_t;

}   // namespace multovl

#endif  // MULTOVL_ANCREGION_HEADER
