// == MODULE shuffleovl.cc ==

/// \file
/// \date 2012-02-26
/// \author Andras Aszodi

// -- Own header --

#include "shuffleovl.hh"

// -- Boost headers --

// -- Standard headers --

// == Implementation ==

namespace multovl {
namespace prob {

ShuffleOvl::ShuffleOvl(const std::vector<Region>& frees):
    _freeregions(frees),
    _rpm(),
    _shufflecount(0)
{}

bool ShuffleOvl::add_randomplacer(unsigned int reglen, unsigned int trackid)
{
    if (reglen == 0)
        return false;
    bool ok = false;
    
    // have we seen this track ID before?
    rpm_t::iterator rit = _rpm.find(trackid);
    if (rit != _rpm.end())
    {
        // track ID already there
        ok = rit->second.add(reglen);
    }
    else
    {
        // set up for this track ID
        RandomPlacer rp;
        ok = rp.add(reglen);
        _rpm[trackid] = rp;
    }
    return ok;
}

unsigned int ShuffleOvl::shuffle(UniformGen& rng)
{
    // remove all RegLimits referring to the reshufflable tracks
    for (rpm_t::const_iterator rcit = _rpm.begin(); rcit != _rpm.end(); ++rcit)
    {
        unsigned int rtid = rcit->first;    // reshufflable track ID
        reglim_t::const_iterator rlit = reglim().begin();
        while(rlit != reglim().end())
        {
            if (rtid == rlit->track_id())
            {
                reglim_t::iterator me = rlit;   // rlit still valid
                ++rlit;                         // so increment it...
                nonconst_reglim().erase(me);    // me is now invalid
            }
            else ++rlit;
        }
    }
    
    // shuffle the reshufflable tracks
    for (rpm_t::iterator rit = _rpm.begin(); rit != _rpm.end(); ++rit)
    {
        rit->second.random_placement(_freeregions, rng);
    }
    
    // add their limits again
    for (rpm_t::const_iterator rcit = _rpm.begin(); rcit != _rpm.end(); ++rcit)
    {
        unsigned int rtid = rcit->first;
        const std::vector<Region>& vr = rcit->second.get_regions();
        for (std::vector<Region>::const_iterator vrcit = vr.begin(); vrcit != vr.end(); ++vrcit)
        {
            add(*vrcit, rtid);
        }
    }
    return ++_shufflecount;
}

}   // namespace prob
}   // namespace multovl

