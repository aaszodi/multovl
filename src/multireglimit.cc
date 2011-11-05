// == Module multireglimit.cc ==

// -- Own header --

#include "multireglimit.hh"

// -- Boost headers --

// -- System headers --

// == Implementation ==

namespace multovl {

void MultiRegLimit::add(const Region& region, unsigned int trackid)
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

}   // namespace multovl
