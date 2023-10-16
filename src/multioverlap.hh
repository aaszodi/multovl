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
#ifndef MULTOVL_MULTIOVERLAP_HEADER
#define MULTOVL_MULTIOVERLAP_HEADER

// == Header multioverlap.hh ==

/// \file 
/// \brief Multiple genomic region overlaps.
/// \author Andras Aszodi
/// \date 2009-12-20

// -- System headers --
    
#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>

// -- Boost headers --

#include "boost/operators.hpp"
#include "boost/serialization/vector.hpp"

// -- Own headers --

#include "reglimit.hh"
#include "multiregion.hh"

// == Classes ==

namespace multovl {

/// Objects of the MultiOverlap class generate and store multiple overlaps of genomic regions.
class MultiOverlap
{
public: 
	
    /**
     * Counts how many overlaps were derived from regions in various track combinations.
     * E.g. if you overlap 4 tracks, you may wish to ask how many overlaps came from
     * tracks 1,3,4 or 2,3. 
     */
    class Counter
    {
    public:
        
        /// Default ctor
        Counter(): _histo(), _total(0) {}
        
        /// Count update based on the ancestor IDs in /mr/
        void count(const MultiRegion& mr);
        
        /// Return the total count
        unsigned int total() const { return _total; }
        
        /// Merge the contents of another Counter object into the calling object.
        Counter& operator+=(const Counter& other);
        
        /// Convert the counter to a string.
        /// The format is either "(empty)" or
        /// "id1,id2,...,idN:count1 id1,id2,...,idM:count2 ..."
        /// where "id1,id2,...idN" are the track IDs (uints).
        std::string to_string() const;
        
    private:
        
        static
        std::string get_key(
            const std::vector<int>::const_iterator& from,
            const std::vector<int>::const_iterator& to
        );
        
        typedef std::map<std::string, unsigned int> histo_t;
        histo_t _histo;
        unsigned int _total;
        
    };  // class Counter
    
    typedef std::multiset<RegLimit> reglims_t;
	typedef std::vector<MultiRegion> multiregvec_t;
	
	// -- methods --
           
    /// Init to empty 
    MultiOverlap(): _ancregions{}, _reglims{}, _multiregions{} {}
    
    /// Init to contain a region and trackid 
    MultiOverlap(const Region& region, unsigned int trackid): 
        MultiOverlap()
    { add(region, trackid); }
    
    // can serve as base class
    virtual
    ~MultiOverlap() = default;
    
    /**
     * Adds a region from a track.
     * \param region A Region object
     * \param trackid The track ID of the object
     */
    void add(const Region& region, unsigned int trackid) {
        _ancregions.emplace_back(region, trackid);
    }

    /**
     * Finds multiple overlaps. Whenever the multiplicity of the overlap
     * changes, there will be a new MultiRegion in the returned vector.
     * For instance, the region [50,150] and the region [100, 200] will give rise
     * to the following three multiregions: 
     * [50-99] with multiplicity 1;
     * [100-150] with multiplicity 2 (the actual overlap part);
     * [151-200] again with multiplicity 1.
     * \param multiregions the overlaps found are appended to this vector
     * \param ovlen the minimum overlap length (>=1) required
     * \param minmult the minimum multiplicity required, >=1,  if 0 --> solitaries also required
     * \param maxmult the maximum multiplicity required, >=2, or if 0 --> any multiplicity accepted
     * if /minmult/ > /maxmult/ then they are swapped silently, except when /maxmult/ == 0
     * \param ext "fake" extension of the input region boundaries, 0 by default
     * \param intrack if /true/ (default), then overlaps within the same track are accepted
     * \return the total count of the overlaps found
     */
    unsigned int find_overlaps(unsigned int ovlen, 
        unsigned int minmult = 1, unsigned int maxmult = 0, 
        unsigned int ext = 0, bool intrack = true);

    /**
     * Finds 'unions' of overlaps.
     * For instance, the region [50,150] and the region [100, 200] will give rise
     * to the union region [50,200] with maximal multiplicity 2 (because in the middle
     * the original regions overlap).
     * The parameters and the return value have the same meaning as with `find_overlaps`.
     * However, /minmult/ must be >=2 as it makes no sense to detect solitaries here.
     * Also note that there is no /intrack/ parameter because I cannot understand
     * what inter-track region unions could be good for.
     */
    unsigned int find_unionoverlaps(unsigned int ovlen,
        unsigned int minmult = 2, unsigned int maxmult = 0, unsigned int ext = 0);
    
    /// Returns the multiple overlaps found by the last 
    /// find_overlaps or find_unionoverlaps operation.
    /// \return a vector of MultiRegion objects.
    const multiregvec_t& overlaps() const { return _multiregions; }
    
    /// Generates some overlap statistics using the overlaps found by the last
    /// find_overlaps or find_unionoverlaps operation.
    /// \param counter a Counter object with track histogram data which will be updated.
    void overlap_stats(Counter& counter) const;
    
protected:

    /// \return const access to the RegLimit multiset inside
    const reglims_t& reglims() const { return _reglims; }

    /// \return non-const access to the RegLimit multiset inside
    reglims_t& nonconst_reglims() { return _reglims; }

private:
    
    void setup_reglims();
    
    // -- data 
    typedef std::vector<AncestorRegion> ancregions_t;
    ancregions_t _ancregions;
    reglims_t _reglims;
    multiregvec_t _multiregions;
    
    // serialization
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        // _reglims is NOT serialized because it changes with each `find_overlaps`
        ar & _ancregions & _multiregions;
    }
    
};

} // namespace multovl

#endif  // MULTOVL_MULTIOVERLAP_HEADER
