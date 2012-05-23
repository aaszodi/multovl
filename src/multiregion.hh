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
