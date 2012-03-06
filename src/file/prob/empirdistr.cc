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

double EmpirDistr::low() const throw(Exception)
{
    if (_dirty)
        throw Exception("low(): dirty");
    if (count(_acc) < 1)
        throw Exception("low(): empty");
    return _low;
}

double EmpirDistr::high() const throw(Exception)
{
    if (_dirty)
        throw Exception("high(): dirty");
    if (count(_acc) < 1)
        throw Exception("high(): empty");
    return _high;
}
    
double EmpirDistr::mean() const throw(Exception)
{
    if (_dirty)
        throw Exception("mean(): dirty");
    if (count(_acc) < 1)
        throw Exception("mean(): empty");
    return boost::accumulators::mean(_acc);
}

double EmpirDistr::variance() const throw(Exception)
{
    if (_dirty)
        throw Exception("variance(): dirty");
    if (count(_acc) < 2)
        throw Exception("variance(): not enough data");
    return boost::accumulators::variance(_acc);
}

double EmpirDistr::cdf(double x) const throw(Exception)
{
    if (_dirty)
	    throw Exception("cdf(X)");
    if (count(_acc) < 1)
        throw Exception("cdf(): empty");    // N=1 makes also little sense...
    
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
