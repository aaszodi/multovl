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
{
    
}

unsigned int ShuffleOvl::shuffle(UniformGen& rng)
{
    // remove all RegLimits referring to the reshufflable tracks
    for (rpm_t::const_iterator rcit = _rpm.begin(); rcit != _rpm.end(); ++rcit)
    {
        unsigned int rtid = rcit->first;    // reshufflable track ID
        for (reglim_t::iterator rlit = reglim().begin(); rlit != reglim().end(); ++rlit)
        {
            if (rtid == rlit->track_id())
                reglim().erase(rtid);
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

