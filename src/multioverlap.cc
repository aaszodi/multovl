#ifndef MULTOVL_MULTIOVERLAP_IMPL
#define MULTOVL_MULTIOVERLAP_IMPL

// == Module multioverlap.cc ==

// -- Own header --

#include "multioverlap.hh"

// -- System headers --

#include <sstream>
#include <algorithm>
#include <iterator>

// do not use this unless something goes seriously wrong
#undef MULTOVL_DEBUG

// == Implementation ==

namespace multovl {

// -- MultiOverlap

void MultiOverlap::add(const Region& region, unsigned int trackid)
{
    // Store /region/ twice in the region limit map /_reglim/
    // through smart pointers
    boost::shared_ptr<AncestorRegion> regp(new AncestorRegion(region, trackid));
    // once as a "first position"
    RegLimit limfirst(regp, true);
    _reglim.insert(limfirst);
    // ... and then as "last position"
    RegLimit limlast(regp, false);
    _reglim.insert(limlast);
}

unsigned int MultiOverlap::find_overlaps(
        unsigned int ovlen, unsigned int minmult, unsigned int maxmult, bool intrack)
{
    // set up filter params with solitary checking
    Filter filter(ovlen, minmult, maxmult, true, intrack);
    
    unsigned int pos = 0, mrstart = 0, mrend = 0, mult = 0, regcount = 0;
    bool istempthere = false;
    ancregset_t ancestors;  // set of ancestors
    _multiregions.clear();
    
    // iterate over the region limit multimap
    reglim_t::const_iterator lowbound = _reglim.begin(), upbound;
    while (_reglim.end() != lowbound)
    {
        pos = lowbound->this_pos(); // current region limit position
        upbound = _reglim.upper_bound(*lowbound);

        // the lowbound/upbound pair corresponds to the STL range
        // of all RegLimit objects in _reglim
        // that were at position pos        
        // iterate over them all
        for (reglim_t::const_iterator rmiter = lowbound;
                rmiter != upbound; ++rmiter)
        {
#ifdef MULTOVL_DEBUG
            std::cerr << "** pos = " << pos << ", other = " << rmiter->other_pos()
                <<": track = " << rmiter->track_id() << " isfirst = " << rmiter->is_first() << ": ";
            for (std::set<AncestorRegion>::const_iterator ait = ancestors.begin();
                    ait != ancestors.end(); ++ait)
            {
                std::cerr << ait->to_attrstring() << '|';
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
                        MultiRegion newregion(mrstart, mrend, ancestors, mult);
                        _multiregions.push_back(newregion);
                        ++regcount;
                    }
                }
                mrstart = pos;
                ancestors.insert(rmiter->region());   // save ancestor
#ifdef MULTOVL_DEBUG
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
                        MultiRegion newregion(mrstart, mrend, ancestors, mult);
                        _multiregions.push_back(newregion);
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
        unsigned int ovlen, unsigned int minmult, unsigned int maxmult)
{
    // set up filter params without solitary checking
    // intra-track overlaps are always allowed
    Filter filter(ovlen, minmult, maxmult, false);

    unsigned int pos = 0, mrstart = 0, mrend = 0, mult = 0, multmax = 0, regcount = 0;
    ancregset_t ancestors;  // set of ancestors
    _multiregions.clear();
    
    // iterate over the region limit multimap
    reglim_t::const_iterator lowbound = _reglim.begin(), upbound;
    while (_reglim.end() != lowbound )
    {
        pos = lowbound->this_pos();    // this is the new position
        upbound = _reglim.upper_bound(*lowbound);

        // the lowbound/upbound pair corresponds to the STL range
        // of all RegLimit objects in _reglim
        // that were at position pos
        // iterate over them all
        for (reglim_t::const_iterator rmiter = lowbound;
                rmiter!= upbound; ++rmiter)
        {
#ifdef MULTOVL_DEBUG
            std::cerr << "** pos = " << pos << ", other = " << rmiter->other_pos()
                <<": track = " << rmiter->track_id() << " isfirst = " << rmiter->is_first() << ": ";
            for (std::set<AncestorRegion>::const_iterator ait = ancestors.begin();
                    ait != ancestors.end(); ++ait)
            {
                std::cerr << ait->to_attrstring() << '|';
            }
            std::cerr << std::endl;
#endif
            if (rmiter->is_first())
            {
                // Region starts here
                if (mult == 0)   // remember if a new union region is started here
                    mrstart = pos;
                ancestors.insert(rmiter->region());   // save ancestor
#ifdef MULTOVL_DEBUG
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
    for (multiregvec_t::const_iterator mrit = _multiregions.begin();
        mrit != _multiregions.end(); ++mrit
    )
    {
        counter.count(*mrit);
    }
}


// == Private ==

// -- Filter methods --

MultiOverlap::Filter::Filter(unsigned int ovlen, unsigned int minmult, 
        unsigned int maxmult, bool checksoli, bool intrack):
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

bool MultiOverlap::Filter::accept_new_region(unsigned int mrstart, unsigned int mrend,
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

/**
 * Count the distinct tracks that make up an ancestry.
 * \param ancestors the set of ancestors of a candidate multiregion.
 * \return the number of distinct tracks in the ancestry. If there were no intra-track
 * overlaps, then this is equal to ancestors.size(), otherwise it is less because
 * some tracks occur in the ancestor set more than once.
 * Private static
 */
unsigned int MultiOverlap::Filter::distinct_track_count(const ancregset_t& ancestors)
{
    std::set<unsigned int> trackidset;
    for (ancregset_t::const_iterator ancit = ancestors.begin();
        ancit != ancestors.end(); ++ancit)
    {
        unsigned int trackid = ancit->track_id();
        trackidset.insert(trackid);
    }
    return trackidset.size();
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
    for (histo_t::const_iterator otherit = other._histo.begin();
        otherit != other._histo.end(); ++otherit)
    {
        std::string otherkey = otherit->first;
        histo_t::iterator thisit = _histo.find(otherkey);
        if (thisit == _histo.end())
        {
            // otherkey is not in this, so insert
            _histo[otherkey] = otherit->second;
        }
        else
        {
            // otherkey already present, increase count
            thisit->second += otherit->second;
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

// -- Private MultiOverlap methods --

/**
 * Scans the internally stored position/region limit multimap
 * and returns a vector of uint pairs. The first in a pair is a position,
 * the second is the number of overlapping regions at that position.
 * The vector entries correspond to the positions where the number of
 * overlapping regions just changed. Implicitly, there is a <0,0> pair
 * at the beginning but this is not stored.
 * Private
 */
MultiOverlap::uintpairvec_t MultiOverlap::overlap_counts() const
{
    uintpairvec_t ovlapcounts;
    ovlapcounts.reserve(_reglim.size()); // cannot grow longer than this
    
    int ocount = 0;   // keeps track of how many regions overlap
    unsigned int pos = 0;
    reglim_t::const_iterator lowbound = _reglim.begin(), upbound;
    while (_reglim.end() != lowbound)
    {
        pos = lowbound->this_pos();    // this is the new position
        upbound = _reglim.upper_bound(*lowbound);

        // the lowbound/upbound pair corresponds to the STL range
        // of all RegLimit objects in _reglim
        // that were at position pos
        // count all "first" and "last" limits to figure out by what amount
        // the overlap count needs to jump
        for (reglim_t::const_iterator rmiter = lowbound;
                rmiter != upbound; ++rmiter)
        {
#ifdef MULTOVL_DEBUG
            std::cerr << "** pos = " << rmiter->this_pos() << 
                ": track = " << rmiter->track_id() << " isfirst = " << 
                rmiter->is_first() << std::endl;
#endif
            if (rmiter->is_first())
                ++ocount;   // region starts here, increase the count
            else
                --ocount;   // region ends here, decrease the count
#ifdef MULTOVL_DEBUG
            std::cerr << "** ocount = " << ocount << std::endl;
#endif
        }
            
        ovlapcounts.push_back(std::make_pair(pos, static_cast<unsigned int>(ocount)));
        ++pos;  // move on
    }
    
    return ovlapcounts;
}

} // namespace multovl

#endif  // MULTOVL_MULTIOVERLAP_IMPL
