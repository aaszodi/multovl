// ==== METHODS EmpirDistr.cc ====

// -- Own header --

#include "EmpirDistr.hh"

// -- System headers --

#include <algorithm>

#ifdef DEBUGG
#include <iostream>
#endif

namespace multovl {
namespace prob {

// == METHODS ==

void EmpirDistr::reset(double w)
{
    if (w == 0.0) return;
    w=fabs(w);
    _width=w;
    clear();
}

void EmpirDistr::clear()
{
    _binlist.clear();
    _binno=0; _total=0;
    _low=_high=0.0;
    _pdf.clear(); _cdf.clear();
    _mean=_variance=0.0;
    _dirty=true;
}

EmpirDistr& EmpirDistr::add(double x)
{
    int xidx=bin_index(x);  // would fall into the Xidx:th bin
    Bin searchbin(xidx);
#ifdef DEBUGG
    std::cerr << "EmpirDistr::add(" << x << "): Xidx=" << xidx;
#endif
    biniter_t pos = std::lower_bound(_binlist.begin(), _binlist.end(), searchbin);
    if (pos->index() == xidx)
    {
#ifdef DEBUGG
        std::cerr << " found, increase count" << std::endl;
#endif
	    ++(*pos);   // found, increase count
	}
    else
    {
#ifdef DEBUGG
        std::cerr << " inserted" << std::endl;
#endif
    	_binlist.insert(pos, ++searchbin);	// with count 1
    }
    ++_total;        // one data point more
    _dirty=true;     // evaluation will be necessary
    return *this;
}

// evaluate(): normalise the counts and make the PDF and CDF.
void EmpirDistr::evaluate()
{
    if (!_dirty || _total == 0) return; // clean or empty
    
    double pdfnorm=1.0/(_width*_total), cdfnorm=1.0/_total,
	    pdfval, cdfval;
    unsigned int i, sum=0, pdfcnt;
    
    // set the low and high limits of X covered by the bins
    int frontidx=_binlist.front().index(), backidx=_binlist.back().index();
    _low = frontidx*_width-0.5*_width;
    _high = backidx*_width+0.5*_width;
    
    // set up the PDF and CDF arrays
    _binno = backidx - frontidx + 1;
    _pdf.reserve(_binno); std::fill_n(_pdf.begin(), _binno, 0.0);
    _cdf.reserve(_binno); std::fill_n(_cdf.begin(), _binno, 0.0);
    
    // normalisation
    biniter_t bp, bpnext;
    unsigned int idx, nextidx;
    for (idx=0, bp=_binlist.begin(); bp!=_binlist.end(); )
    {
        pdfcnt=bp->count();
        pdfval=pdfcnt*pdfnorm;
        sum+=pdfcnt;
        cdfval=sum*cdfnorm;
        _pdf[idx]=pdfval;
        _cdf[idx]=cdfval;
        bpnext=bp;
        ++bpnext;   // bp+1 is only for random access iterators
        if (bpnext!=_binlist.end())    // lookahead before last
        {
            // if nextidx>idx+1, there are empty bins coming!
            nextidx=idx+bpnext->index()-bp->index();
            for (i=idx+1; i<nextidx; ++i)
            {
                _pdf[i]=0.0; 	// emptiness is zero PDF
                _cdf[i]=cdfval;	// CDF stays constant over the empty bins
            }
            idx=nextidx;    // step
        }
        bp=bpnext;  // stepping bp, not in the for clause though
    }
    
#ifdef DEBUGG
    for (unsigned int ix=0; ix<_binno; ++ix)
        std::cerr<<"PDF["<<ix<<"]="<<_pdf[ix]<<", CDF="<<_cdf[ix]<<std::endl;
#endif
    
    // estimate the mean (histogram-based)
    double x;
    _mean=0.0;
    for (i=0, x=_low+0.5*_width; i<_binno; ++i, x+=_width)
        _mean+=_pdf[i]*x;
    _mean*=_width;
    
    // estimate the variance using the mean
    double xctr;    // centered by mean
    _variance=0.0;
    for (i=0, x=_low+0.5*_width; i<_binno; ++i, x+=_width)
    {
        xctr=x-_mean;
        _variance+=xctr*xctr*_pdf[i];
    }
    _variance*=_width;
    
    _dirty=false;
}

double EmpirDistr::pdf(double x) const throw(DirtyException)
{
    if (_dirty)
	    throw DirtyException("pdf(X)");
    
    if (x<=_low || x>=_high) return 0.0;
    
    register int idx=locate_index(x);
    register double xmid, xlow, xup, ylow, yup, delta;
    
    xmid=bin_midpoint(idx);	// midpoint abscissa of the bin of x
    
    // find the coordinates of the midpoints bracketing x
    if (x>=xmid)    // x is in upper half of bin
    {
        xlow=xmid; ylow=_pdf[idx];
        if (idx==_binno-1)   // last bin
        {
            xup=_high; yup=0.0; delta=_width/2.0;
        }
        else
        {
            xup=xlow+_width; yup=_pdf[idx+1]; delta=_width;
        }
    }
    else    	    // x is in lower half of bin
    {
        xup=xmid; yup=_pdf[idx];
        if (idx==0) 	// first bin
        {
            xlow=_low; ylow=0.0; delta=_width/2.0;
        }
        else
        {
            xlow=xup-_width; ylow=_pdf[idx-1]; delta=_width;
        }
    }
    
    // linear interpolation between (xlow,ylow)..x..(xup, yup)
    double y=(yup-ylow)*(x-xlow)/delta+ylow;
    return y;
}

double EmpirDistr::cdf(double x) const throw(DirtyException)
{
    if (_dirty)
	    throw DirtyException("cdf(X)");
    
    if (x<=_low) return 0.0;
    if (x>=_high) return 1.0;
    
    register int idx=locate_index(x);
    register double xmid, xlow, xup, ylow, yup;
    
    // the cdf value for a bin is reached at the "high end" of the bin
    // we have 3 cases: first bin, last bin, generic (in the middle)
    if (idx==0)
    {
        // first bin
        xlow=_low; ylow=0.0;
        xup=_low+_width; yup=_cdf[0];
    }
    else if (idx==_binno-1)
    {
        // last bin
        xlow=_high-_width; ylow=_cdf[idx-1];
        xup=_high; yup=1.0;
    }
    else
    {
        // generic case: bin is in the middle
        xlow=_low+idx*_width; ylow=_cdf[idx-1];
        xup=xlow+_width; yup=_cdf[idx];
    }
    
    // linear interpolation between (xlow,ylow)..x..(xup, yup)
    double y=(yup-ylow)*(x-xlow)/_width+ylow;
    return y;
}

double EmpirDistr::inv_cdf(double y) const throw(DirtyException)
{
    if (_dirty)
	throw DirtyException("inv_cdf(Y)");
    
    // y outside the allowed range [0.0 .. 1.0] is clamped silently
    if (y<=0.0) return _low;
    if (y>=1.0) return _high;
    
    unsigned int idxlow, idxup;
    double xlow, ylow, yup;
    
    // interval halving to find the cdf bin for y
    if (y<=_cdf[0])    // y in first half of first bin
    {
        xlow=_low; ylow=0.0;
        yup=_cdf[0];
    }
    else if (y>_cdf[_binno-1]) // y in second half of last bin
    {
        xlow=bin_midpoint(_binno-1); ylow=_cdf[_binno-1];
        yup=1.0;
    }
    else    // general case
    {
        unsigned int idxm;
        double ym;
        idxlow=0; idxup=_binno-1;	// first and last bins
        do
        {
            idxm=(idxlow+idxup)/2;
            ym=_cdf[idxm];
            if (ym>=y) idxup=idxm;  // new bracket from above
            else idxlow=idxm;  	    // or from below
        }
        while (idxup-idxlow>1);
        xlow=bin_midpoint(idxlow); ylow=_cdf[idxlow];
        yup=_cdf[idxup];
    }
    
    // interpolate x
    double x=_width*(y-ylow)/(yup-ylow)+xlow;
    return(x);
}

// -- aux --

// locate_index(X): returns the index of the PDF/CDF arrays containing X
// (in the range 0.._binno-1) or -1 if X is outside [_low.._high].
// Protected
int EmpirDistr::locate_index(double x) const
{
    int idx=-1;
    if (_low<=x && x<=_high)
    {
        idx=static_cast<int>(floor(_binno*(x-_low)/(_high-_low)));
        if (idx==_binno) idx--;
    }
    return idx;
}
// end of locate_index(x)

}   // namespace prob
}   // namespace multovl
