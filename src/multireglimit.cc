// == Module multireglimit.cc ==

// -- Own header --

#include "multireglimit.hh"

// -- Boost headers --

// -- System headers --

#include <algorithm>

// == Implementation ==

namespace multovl {

MultiRegLimit::MultiRegLimit(const MultiRegLimit& other)
{
    deep_copy(other.reglim(), _reglim);
}

MultiRegLimit& MultiRegLimit::operator=(const MultiRegLimit& rhs)
{
    if (this == &rhs) return *this;
    reglim_t newreglim;
    deep_copy(rhs.reglim(), newreglim);
    this->_reglim.swap(newreglim);
    return *this;
}

void MultiRegLimit::deep_copy(const reglim_t& source, reglim_t& target)
{
    // for each AncestorRegion there are two RegLimits in the reglim_t multiset
    // so we need to deep-copy only the "first" RegLimit in /source/
    // and the corresponding "last" RegLimit can be shallow-copied
    for (reglim_t::const_iterator rlit = source.begin(); rlit != source.end(); ++rlit)
    {
        if (!rlit->is_first())  // skip "last" limit
            continue;
        RegLimit limfirst = rlit->deep_copy();
        target.insert(limfirst);
        RegLimit limlast(limfirst); // shallow copy
        limlast.first(false);
        target.insert(limlast);
    }
}

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
