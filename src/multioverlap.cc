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
// == Module multioverlap.cc ==

// -- Own header --

#include "multioverlap.hh"

// -- Boost headers --

#include "boost/lexical_cast.hpp"

// -- System headers --

#include <sstream>
#include <algorithm>
#include <iterator>

// == Implementation ==

namespace multovl {

namespace impl {
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
                unsigned int maxmult, bool checksoli, bool intrack=true):
            _ovlen((ovlen<1)? 1: ovlen),
            _minmult((minmult<1)? 1: minmult),
            _maxmult(maxmult), _intrack(intrack)
        {
            // silent swapping: note _maxmult == 0 means _maxmult == infinity
            if (_maxmult > 0 && _minmult > _maxmult) std::swap(_minmult, _maxmult);
    
            // do we detect solitary regions?
            _solitary = checksoli && minmult == 1;
            if (_solitary)
            {
                _ovlen = 1;   // longer ovlen wouldn't make sense
                _intrack = true;   // do check intra-track overlaps
            }
        }

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
            const ancregset_t& ancestors, unsigned int& mult) const
        {
            // solitary region required
            // accept if there is only one ancestor with equal position
            if (_solitary && ancestors.size() == 1)
            {
                const Region& anc = *ancestors.begin();
                return (anc.first() == mrstart && anc.last() == mrend);
            }
    
            // generic non-solitary case
            if (!_intrack)
            {
                unsigned int distrcnt = distinct_track_count(ancestors);
                if (distrcnt == 1)
                {
                    // do not accept overlaps within the same track only
                    return false;
                }
                else
                {
                    // override the multiplicity: use the number of distinct tracks
                    // contributing to the overlap
                    // this is a lame workaround for track-filtered complex overlaps
                    mult = distrcnt;
                }
            }
    
            unsigned int mlen = mrend - mrstart + 1;
            return (_minmult <= mult && mlen >= _ovlen && 
                (_maxmult == 0 || mult <= _maxmult));
        }
            
    private:
        
        // Count the distinct tracks that make up an ancestry.
        // \param ancestors the set of ancestors of a candidate multiregion.
        // \return the number of distinct tracks in the ancestry. If there were no intra-track
        // overlaps, then this is equal to ancestors.size(), otherwise it is less because
        // some tracks occur in the ancestor set more than once.
        static
        unsigned int distinct_track_count(const ancregset_t& ancestors)
        {
            std::set<unsigned int> trackidset;
            for (const auto& anc : ancestors) {
                unsigned int trackid = anc.track_id();
                trackidset.insert(trackid);
            }
            return trackidset.size();
        }

        unsigned int _ovlen, _minmult, _maxmult;
        bool _solitary, _intrack;
        
    };  // class Filter

}   // namespace impl

typedef std::pair<unsigned int, unsigned int> uintpair_t;
typedef std::vector<uintpair_t> uintpairvec_t;

void MultiOverlap::setup_reglims() {
    _reglims.clear();
    
    // Stores ancregions twice in the region limit map `_reglims`
    for (const auto& ancreg : _ancregions) {
        add_reglimit(ancreg);
    }
}

void MultiOverlap::add_reglimit(const AncestorRegion& ancreg) {
    // add once as a "first position"
    RegLimit limfirst(ancreg, true);
    _reglims.insert(limfirst);
    // ... and then as "last position"
    RegLimit limlast(ancreg, false);
    _reglims.insert(limlast);
}

unsigned int MultiOverlap::find_overlaps(
        unsigned int ovlen, unsigned int minmult, unsigned int maxmult, 
        unsigned int ext, bool intrack)
{
    // Extend the region limits if required
    Region::set_extension(ext);
    
    // Set up the region limits based on the current contents of `_ancregions`
    setup_reglims();
    
    // generate the overlaps
    unsigned int regcount = generate_overlaps(ovlen, minmult, maxmult, intrack);
    
    // reset the extensions
    Region::set_extension(0);
    return regcount;
}

unsigned int MultiOverlap::generate_overlaps(
        unsigned int ovlen, unsigned int minmult, unsigned int maxmult, bool intrack)
{
    // set up filter params with solitary checking
    impl::Filter filter(ovlen, minmult, maxmult, true, intrack);
    
    unsigned int pos = 0, mrstart = 0, mrend = 0, mult = 0, regcount = 0;
    bool istempthere = false;
    ancregset_t ancestors;  // set of ancestors
    _multiregions.clear();
    
    // iterate over the region limits which have already been set up
    reglimset_t::const_iterator lowbound = reglims().begin(), upbound;
    while (reglims().end() != lowbound)
    {
        pos = lowbound->this_pos(); // current region limit position
        upbound = reglims().upper_bound(*lowbound);

        // the lowbound/upbound pair corresponds to the STL range
        // of all RegLimit objects in reglims()
        // that were at position pos        
        // iterate over them all
        for (reglimset_t::const_iterator rmiter = lowbound;
                rmiter != upbound; ++rmiter)
        {
#ifndef NDEBUG
            std::cerr << "** pos = " << pos << ", other = " << rmiter->other_pos()
                <<": track = " << rmiter->track_id() << " isfirst = " << rmiter->is_first() << ": ";
            for (const auto& anc : ancestors) {
                std::cerr << anc.to_attrstring() << '|';
            }
            std::cerr << std::endl;
#endif
            if (rmiter->is_first())
            {
                // Region starts here
                // finish prev region if applicable, 1 pos before current
                // and start new one at current pos
                if (istempthere && pos>mrstart)
                {
                    mrend = pos-1;
                    mult = ancestors.size();    // can be overwritten when filtering intra-track ovls
                    if (filter.accept_new_region(mrstart, mrend, ancestors, mult))
                    {
                        _multiregions.emplace_back(mrstart, mrend, ancestors, mult);
                        ++regcount;
                    }
                }
                mrstart = pos;
                ancestors.insert(rmiter->region());   // save ancestor
#ifndef NDEBUG
				std::cerr << "** mrstart = " << pos << std::endl;
				std::cerr << "** save ancestor: " << 
				    rmiter->region().to_attrstring() << std::endl;
				std::cerr << "** ancestorcnt = " << ancestors.size() << std::endl;
#endif
                istempthere = true;
            }
            else
            {
                // region ends here
                // finish prev region if applicable at current pos
                // start new one at current pos+1 if Idset is not empty
                if (istempthere && mrend<pos)
                {
                    mrend = pos;
                    mult = ancestors.size();
                    if (filter.accept_new_region(mrstart, mrend, ancestors, mult))
                    {
                        _multiregions.emplace_back(mrstart, mrend, ancestors, mult);
                        ++regcount;
                    }
                    mrstart = pos+1;
                }
                
                // remove the ancestor rmiter is pointing to
                // rmiter->is_first() == false, its _otherpos is the end pos
                // NOTE: remove_if() cannot be used on associative containers!
                // (my SGI STL description didn't tell me *that*...)
                ancestors.erase(rmiter->region());
                istempthere = !ancestors.empty();
            }
        }
        lowbound = upbound;  // move on
    }
    return regcount;
}

unsigned int MultiOverlap::find_unionoverlaps(
        unsigned int ovlen, unsigned int minmult, unsigned int maxmult,
        unsigned int ext)
{
    // Extend the region limits if required
    Region::set_extension(ext);
    
    // Set up the region limits
    setup_reglims();
    
    // generate the union overlaps
    unsigned int regcount = generate_unionoverlaps(ovlen, minmult, maxmult);
        
    // reset the extensions
    Region::set_extension(0);
    return regcount;
}

unsigned int MultiOverlap::generate_unionoverlaps(
        unsigned int ovlen, unsigned int minmult, unsigned int maxmult)
{
    // set up filter params without solitary checking
    // intra-track overlaps are always allowed
    impl::Filter filter(ovlen, minmult, maxmult, false);

    unsigned int pos = 0, mrstart = 0, mrend = 0, mult = 0, multmax = 0, regcount = 0;
    ancregset_t ancestors;  // set of ancestors
    _multiregions.clear();
    
    // iterate over the region limit multimap which was set up already
    reglimset_t::const_iterator lowbound = reglims().begin(), upbound;
    while (reglims().end() != lowbound )
    {
        pos = lowbound->this_pos();    // this is the new position
        upbound = reglims().upper_bound(*lowbound);

        // the lowbound/upbound pair corresponds to the STL range
        // of all RegLimit objects in reglims()
        // that were at position pos
        // iterate over them all
        for (reglimset_t::const_iterator rmiter = lowbound;
                rmiter!= upbound; ++rmiter)
        {
#ifndef NDEBUG
            std::cerr << "** pos = " << pos << ", other = " << rmiter->other_pos()
                <<": track = " << rmiter->track_id() << " isfirst = " << rmiter->is_first() << ": ";
            for (const auto& anc : ancestors) {
                std::cerr << anc.to_attrstring() << '|';
            }
            std::cerr << std::endl;
#endif
            if (rmiter->is_first())
            {
                // Region starts here
                if (mult == 0)   // remember if a new union region is started here
                    mrstart = pos;
                ancestors.insert(rmiter->region());   // save ancestor
#ifndef NDEBUG
				std::cerr << "** mrstart = " << pos << std::endl;
				std::cerr << "** save ancestor: " << 
				    rmiter->region().to_attrstring() << std::endl;
				std::cerr << "** save track: " << rmiter->track_id() << std::endl;
				std::cerr << "** ancestorcnt = " << ancestors.size() << std::endl;
#endif
                mult += 1;  // add to (maximal) multiplicity
                if (mult > multmax) multmax = mult;
            }
            else
            {
                // region ends here
                mult -= 1;
                // finish if there are no overlapping regions left
                if (mult == 0)
                {
                    mrend = pos;
                    
                    // check if this currently ended region needs to be saved
                    if (filter.accept_new_region(mrstart, mrend, ancestors, multmax))
                    {
                        MultiRegion newregion(mrstart, mrend, ancestors, multmax);
                        _multiregions.push_back(newregion);
                        ++regcount;
                    }
                    
                    // forget multiplicity, ancestors
                    multmax = 0;
                    ancestors.clear();
                }
            }
        }
        lowbound = upbound;  // move on
    }
    return regcount;
}

void MultiOverlap::overlap_stats(Counter& counter) const
{
    for (const auto& mr : _multiregions) {
        counter.count(mr);
    }
}

// -- Counter methods --

void MultiOverlap::Counter::count(const MultiRegion& mr)
{
    std::vector<int> ancids = mr.ancestor_trackids();    // local copy!
    std::sort(ancids.begin(), ancids.end());
    std::vector<int>::iterator ancidsend =      // new end after unique-ing
        std::unique(ancids.begin(), ancids.end());
    std::string key = get_key(ancids.begin(), ancidsend);
    
    histo_t::iterator it = _histo.find(key);
    if (it != _histo.end())
    {
        // val already present, increase count
        it->second++;
    } else {
        // add val with a count of 1
        _histo[key] = 1;
    }
    _total++;
}

MultiOverlap::Counter& MultiOverlap::Counter::operator+=(const Counter& other)
{
    this->_total += other.total();  // easy :-)
    
    // for each entry in /other/, check if the key is already available in this.
    // if yes, then add the count, if not, then insert
    for (const auto& oth : other._histo) {
        std::string otherkey = oth.first;
        histo_t::iterator thisit = _histo.find(otherkey);
        if (thisit == _histo.end())
        {
            // otherkey is not in this, so insert
            _histo[otherkey] = oth.second;
        }
        else
        {
            // otherkey already present, increase count
            thisit->second += oth.second;
        }
    }
    
    return *this;
}

std::string MultiOverlap::Counter::to_string() const
{
    if (_histo.empty()) return "(empty)";
    std::string str;
    for (histo_t::const_iterator cit = _histo.begin();
            cit != _histo.end(); ++cit)
    {
        if (cit != _histo.begin()) str += ' ';
        str += cit->first + ':' +
            boost::lexical_cast<std::string>(cit->second);
    }
    return str;
}

std::string MultiOverlap::Counter::get_key(
    const std::vector<int>::const_iterator& from,
    const std::vector<int>::const_iterator& to
)
{
    std::string keystr = "";
    for (std::vector<int>::const_iterator it = from;
        it != to; ++it)
    {
        if (it != from) keystr += ',';
        keystr += boost::lexical_cast<std::string>(*it);
    }
    return keystr;
}

} // namespace multovl
