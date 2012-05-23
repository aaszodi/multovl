/* <LICENSE>
License for the MULTOVL multiple genomic overlap tools

Copyright (c) 2007-2012, Dr Andras Aszodi, 
Campus Science Support Facilities GmbH (CSF),
Dr-Bohr-Gasse 3, A-1030 Vienna, Austria, Europe.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of the Campus Science Support Facilities GmbH
      nor the names of its contributors may be used to endorse
      or promote products derived from this software without specific prior
      written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS
AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
</LICENSE> */
// ==== METHODS empirdistr.cc ====

// -- Own header --

#include "empirdistr.hh"

// -- Boost headers --

#include "boost/math/special_functions/next.hpp"    // minimal floating-point distance

// -- System headers --

#include <algorithm>
#include <functional>
#include <limits>

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

double EmpirDistr::std_dev() const throw(Exception)
{
    double var = variance();
    if (var < std::numeric_limits<double>::epsilon())
        return 0.0; // because the devil never sleeps
    else
        return std::sqrt(var);
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
        // covers single-bin case
        xlow = _low; xhigh = hit->first; 
        ylow = 0.0; yhigh = hit->second;
    }
    else if (hit == _histogram.end())
    {
        // shouldn't really happen, but if it does, then...
        return 1.0;
    }
    else
    {
        // must have >=2 bins
        histogram_iter_t lit = hit--;
        xlow = lit->first; ylow = lit->second;
        xhigh = hit->first; yhigh = hit->second;
    }
    
    // xlow, xhigh are too close
    int ulpdist = boost::math::float_distance(xlow, xhigh);
    if (ulpdist == 0)
        return (ylow + yhigh)/2.0;
    
    // simple linear interpolation btw the two bins
    double xdelta = xhigh - xlow, ydelta = yhigh - ylow;
    double y = ylow + (ydelta*(x - xlow))/xdelta;
    return y;
}

}   // namespace prob
}   // namespace multovl
