// ==== METHODS empirdistr.cc ====

// -- Own header --

#include "empirdistr.hh"

// -- System headers --

#include <algorithm>
#include <functional>

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
    
    // find the histogram bins around x
    // hit will point to the first bin with abscissa >= x
    histogram_iter_t hit = std::lower_bound(_histogram.begin(), _histogram.end(), x, HistogramComp());
    double xlow, xhigh, ylow, yhigh;
    if (hit == _histogram.begin())
    {
        xlow = _low; ylow = 0.0;
        xhigh = hit->first; yhigh = hit->second;
    }
    else if (hit == _histogram.end())
    {
        xlow = hit->first; ylow = hit->second;
        xhigh = _high; yhigh = 1.0;
    }
    else
    {
        histogram_iter_t lit = hit--;
        xlow = lit->first; ylow = lit->second;
        xhigh = hit->first; yhigh = hit->second;
    }
    
    // simple linear interpolation btw the two bins
    double xdelta = xhigh - xlow, ydelta = yhigh - ylow;
    double y = ylow + (ydelta*(x - xlow))/xdelta;
    return y;
}

}   // namespace prob
}   // namespace multovl
