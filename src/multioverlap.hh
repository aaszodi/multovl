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

#include "boost/shared_ptr.hpp"
#include "boost/operators.hpp"

// -- Own headers --

#include "multiregion.hh"
#include "multovlopts.hh"

// == Classes ==

namespace multovl {

/**
 * Objects of the MultiOverlap class keep track of multiple overlaps of genomic regions.
 * The idea is that for each region we keep track where the region begins or ends.
 * Internally, this is implemented by a multimap that is keyed with the limits
 * (first or last position) of the regions, and the stored value is an internal class
 * that remembers which track it came from and whether it was a first (ie. beginning of region)
 * or a non-first=last (ie. end of the region). Because the multimap is sorted on its keys,
 * after all regions from all tracks are added, it stores all positions where a region began
 * or ended. One just needs to iterate along and look where a change occurred.
 */
class MultiOverlap
{
	// -- internal classes --
	
	private:
	
    // Each ancestor region to be overlapped is stored twice in MultiRegion's lookup;
    // once as a "first pos", and once as a "last pos".
    // The RegLimit class implements these "region limit" objects.
    class RegLimit
    {
        public:
        
        RegLimit(const boost::shared_ptr<AncestorRegion>& regp, 
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
        
        bool operator<(const RegLimit& other) const
        {
            if (this->this_pos() < other.this_pos())
                return true;
            else if (this->this_pos() == other.this_pos())
                return (this->is_first() && !(other.is_first()));
            else
                return false;
        }
        
        private:
        
        boost::shared_ptr<AncestorRegion> _regp;
        bool _isfirst;
    };  // class RegLimit
    
    typedef std::multiset<RegLimit> reglim_t;
    
    typedef std::pair<unsigned int, unsigned int> uintpair_t;
    typedef std::vector<uintpair_t> uintpairvec_t;
    
    /**
     * Encapsulates the parameters according to which the generated multiregions
     * should be filtered: the minimal overlap length, the minimal and maximal
     * multiplicity. Provides a method to filter nascent multiregions.
     */
    class Filter
    {
        public:
        
        /**
         * Constructs a Filter.
         * \param ovlen must be >=1, adjusted silently
         * \param minmult minimal multiplicity, must be >=1 
         * \param maxmult must be >=0, 0 means any multiplicity >= minmult is accepted; 
         * if /minmult/ > /maxmult/ then they are swapped silently
         * \param checksoli if /true/, then only solitary regions will be accepted
         * if /minmult/ == 1
         * (this sets /intrack/ to /true/, and /ovlen/ to 1)
         * \param intrack if /true/, then overlaps within the same track are accepted as well,
         * otherwise overlaps within the same track only are filtered out.
         */
        Filter(unsigned int ovlen, unsigned int minmult, 
                unsigned int maxmult, bool checksoli, bool intrack = true);
        
        /**
         * Checks whether a "nascent" multiregion (not yet constructed)
         * should be accepted as new multiregion.
         * \param mrstart the first position of the new multiregion
         * \param mrend the last position of the new multiregion
         * \param ancestors the set of ancestor regions
         * \param mult the desired multiplicity of the new multiregion.
         * This is usually ancestors.size(), but you need to specify a different value
         * for union regions. Moreover, this method may override /mult/ if
         * intra-track filtering is switched on.
         * \return /true/ if the multiregion may be accepted.
         */
        bool accept_new_region(unsigned int mrstart, unsigned int mrend,
            const ancregset_t& ancestors, unsigned int& mult) const;
            
        private:
        
        static unsigned int distinct_track_count(const ancregset_t& ancestors);
        
        unsigned int _ovlen, _minmult, _maxmult;
        bool _solitary, _intrack;
        
    };  // class Filter
    
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
    
	typedef std::vector<MultiRegion> multiregvec_t;
	
	// -- methods --
           
    /// Init to empty 
    MultiOverlap(): _reglim(), _multiregions() {}
    
    /// Init to contain a region and trackid 
    MultiOverlap(const Region& region, unsigned int trackid): 
        _reglim(), _multiregions()
    {
        add(region, trackid);
    }
    
    /**
     * Adds a region /region/ identified by /trackid/
     * to the calling object. Invoke this method for all regions
     * in all the tracks that are to be overlapped.
     */
    void add(const Region& region, unsigned int trackid);

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
     * \param intrack if /true/ (default), then overlaps within the same track are accepted
     * \return the total count of the overlaps found
     */
    unsigned int find_overlaps(unsigned int ovlen, 
        unsigned int minmult = 1, unsigned int maxmult = 0, bool intrack = true);

    /**
     * Finds 'unions' of overlaps.
     * For instance, the region [50,150] and the region [100, 200] will give rise
     * to the union region [50,200] with maximal multiplicity 2 (because in the middle
     * the original regions overlap).
     * The parameters and the return value have the same meaning as with /find_overlaps/.
     * However, /minmult/ must be >=2 as it makes no sense to detect solitaries here.
     * Also note that there is no /intrack/ parameter because I cannot understand
     * what inter-track region unions could be good for.
     */
    unsigned int find_unionoverlaps(unsigned int ovlen,
        unsigned int minmult = 2, unsigned int maxmult = 0);
    
    /// Returns the multiple overlaps found by the last 
    /// find_overlaps or find_unionoverlaps operation.
    /// \return a vector of MultiOverlap objects.
    const multiregvec_t& overlaps() const { return _multiregions; }
    
    /// Generates some overlap statistics using the overlaps found by the last
    /// find_overlaps or find_unionoverlaps operation.
    /// \param counter a Counter object with track histogram data which will be updated.
    void overlap_stats(Counter& counter) const;
    
    private:
            
    // -- data 
    
    reglim_t _reglim;
    multiregvec_t _multiregions;
};

} // namespace multovl

#endif  // MULTOVL_MULTIOVERLAP_HEADER
