// == Module multireglimit.cc ==

// -- Own header --

#include "multireglimit.hh"

// -- Boost headers --

// -- System headers --

#include <algorithm>

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

MultiRegLimit& MultiRegLimit::operator+=(const MultiRegLimit& rhs)
{
    reglim_t result;
    std::merge(
        this->reglim().begin(), this->reglim().end(),
        rhs.reglim().begin(), rhs.reglim().end(),
        std::inserter(result, result.begin())
    );
    this->_reglim.swap(result);
    return *this;
}

}   // namespace multovl
