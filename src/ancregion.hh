/* <LICENSE>
License for the MULTOVL multiple genomic overlap tools

Copyright (c) 2007-2012, Dr Andras Aszodi, 
Campus Science Support Facilities GmbH (CSF),
Dr-Bohr-Gasse 3, A-1030 Vienna, Austria, Europe.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of the Campus Science Support Facilities GmbH
      nor the names of its contributors may be used to endorse
      or promote products derived from this software without specific prior
      written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS
AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
</LICENSE> */
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

#include <boost/serialization/base_object.hpp>

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
    const unsigned int& track_id() const { return _trackid; }
    
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
    
    // "split" serialization
    friend class boost::serialization::access;
    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << boost::serialization::base_object<Region>(*this); 
        ar << _trackid;
    }
    
    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
        ar >> boost::serialization::base_object<Region>(*this);
        ar >> _trackid;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
    
};  // class AncestorRegion

/// Collection of ancestors for a descendant region.
/// Under certain conditions the same region may occur multiple times in the input,
/// therefore a multiset is used to hold the ancestors of an overlap.
typedef std::multiset<AncestorRegion> ancregset_t;

}   // namespace multovl

#endif  // MULTOVL_ANCREGION_HEADER
