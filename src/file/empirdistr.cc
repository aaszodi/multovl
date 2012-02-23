// ==== METHODS empirdistr.cc ====

// -- Own header --

#include "empirdistr.hh"

// -- System headers --

#include <algorithm>

#ifdef DEBUGG
#include <iostream>
#endif

namespace multovl {
namespace prob {

// == Implementation ==

EmpirDistr::EmpirDistr(unsigned int ncell):
    _acc(tag::p_square_cumulative_distribution::num_cells = ((ncell==0)? 100: ncell)),
    _low(0.0), _high(0.0),
    _dirty(true)
{}

EmpirDistr& EmpirDistr::add(double x)
{
    _acc(x);
    _dirty = true;     // evaluation will be necessary
    return *this;
}

void EmpirDistr::evaluate()
{
    if (!_dirty) return;
    _low = (min)(_acc);
    _high = (max)(_acc);
    _histogram = p_square_cumulative_distribution(_acc);
    _dirty = false;
}

double EmpirDistr::cdf(double x) const throw(DirtyException)
{
    if (_dirty)
	    throw DirtyException("cdf(X)");
    
    if (x < _low) return 0.0;
    if (x > _high) return 1.0;
    
    // find the index for x
    // TODO
    return 0.5; // BS!
}

}   // namespace prob
}   // namespace multovl
