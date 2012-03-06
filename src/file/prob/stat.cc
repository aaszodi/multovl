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
         double val, 
         bool is_actual)
{
    // which distr?
    diter_t dit = _distrs.find(multiplicity);
    if (dit != _distrs.end())
    {
        // found, just update
        dit->second.add(val, is_actual);
    } else {
        // multiplicity hasn't been seen so far
        Distr distr(val, is_actual);
        _distrs.insert(
            std::make_pair<unsigned int, Distr>(
                multiplicity, distr)
        );
    }
    
    // update min/max of multiplicities seen so far
    if (multiplicity < _minmult) _minmult = multiplicity;
    if (multiplicity > _maxmult) _maxmult = multiplicity;
}

void Stat::evaluate()
{
    for (diter_t dit = _distrs.begin(); dit != _distrs.end(); ++dit)
    {
        dit->second.evaluate();
    }
}

const Stat::Distr& Stat::distr(unsigned int multiplicity) const throw(NotfoundException)
{
    distrs_t::const_iterator dit = _distrs.find(multiplicity);
    if (dit == _distrs.end())
        throw(NotfoundException(multiplicity));
    return dit->second;
}

}   // namespace prob
}   // namespace multovl
