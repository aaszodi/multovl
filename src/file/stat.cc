// == MODULE stat.cc ==

/// \file
/// \date 2012-03-01
/// \author Andras Aszodi

// -- Own header --

#include "stat.hh"

// -- Boost headers --

// -- Standard headers --

#include <utility>

// == Implementation ==

namespace multovl {
namespace prob {

void Stat::add(unsigned int multiplicity, 
         unsigned int ovlen, 
         bool is_actual)
{
    // which lendistr?
    lditer_t ldit = _lendistrs.find(multiplicity);
    if (ldit != _lendistrs.end())
    {
        // found, just update
        ldit->second.add(ovlen, is_actual);
    } else {
        // multiplicity hasn't been seen so far
        LenDistr ld(ovlen, is_actual);
        _lendistrs.insert(
            std::make_pair<unsigned int, LenDistr>(
                multiplicity, ld)
        );
    }
}

void Stat::calc_pvalues()
{
    for (lditer_t ldit = _lendistrs.begin(); ldit != _lendistrs.end(); ++ldit)
    {
        ldit->second.calc_pvalue();
    }
}
    
}   // namespace prob
}   // namespace multovl
