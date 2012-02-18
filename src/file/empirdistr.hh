#ifndef MULTOVL_PROB_EMPIRDISTR_HEADER
#define MULTOVL_PROB_EMPIRDISTR_HEADER

// ==== HEADER empirdistr.hh ====

// -- System headers --

#include <math.h>

#include <string>
#include <list>
#include <vector>

// -- Library headers --

namespace multovl {
namespace prob {

/// Objects of the EmpirDistr class implement univariate continuous empirical distributions
/// that are approximated by histograms. Samples can be added to an EmpirDistr object
/// and after data collection the approximate PDF, CDF and inverse CDF can be queried.
class EmpirDistr
{
    // -- member classes
    
    public:
    
    /// A DirtyException is thrown if one of the accessors
    /// is invoked on a "dirty" EmpirDistr object.
    class DirtyException
    {
        public:
        explicit DirtyException(const std::string& throwname):
            _tnm(throwname) {}
        const std::string error_message() const 
        {
            return _tnm + " invoked on dirty EmpirDistr";
        }
        
        private:
        std::string _tnm;
        
    };
    // END of member class DirtyException
    
    private:
	    
    /**
     * A Bin object represents the i:th histogram bin.
     * All bins are Width wide (see enclosing class for details).
     */
    class Bin
    {
        // methods
        public:

        /// Constructs the i:th bin (default 0)
        explicit Bin(int i=0): _idx(i), _count(0) {}

        /// Returns the index of the bin (may be negative).
        int index() const { return _idx; }

        /// \return the no. of samples in the bin.
        unsigned int count() const { return _count; }

        /// Comparison according to index
        bool operator<(const Bin& other) { return index() < other.index(); }

        /// Prefix increment: adds 1 to the bin count.
        Bin& operator++() { ++_count; return *this; }

        // data
        private:

        int _idx;	///< the bin index (can be negative)
        unsigned int _count; 	///< no. of samples falling into the bin
    };
    // END of member class Bin
    
    // shorthands
    typedef std::list<Bin> binlist_t;
    typedef binlist_t::iterator biniter_t;
    
    // methods
    public:
	    
    /**
     * Constructs an object so that it is ready to do an approximation
     * with a bin width w.
     * \param w the bin width. If w<0.0, then its absolute value is used.
     */
    explicit EmpirDistr(double w) { reset(w); }

    /**
     * Resets the calling object so that it is ready to do an approximation
     * with a bin width w. All previous data are lost.
     * \param w the bin width. If w<0.0, then its absolute value is used.
     * If w==0.0, then nothing happens.
     */
    void reset(double w);
    
    /**
     * clears the calling object. All data are lost except the histogram bin width.
     */
    void clear();
    
    /// adds a value 
    /// \param x a sample from the distribution to be approximated.
    EmpirDistr& add(double x);
    
    /// evaluate(): normalise the counts and make the PDF and CDF.
    void evaluate();
    
    /**
     * \return the lowest value covered by the approximation.
     * \throw DirtyException if evaluate() was not invoked beforehand.
     */
    double low() const throw(DirtyException)
    {
    	if (_dirty)
            throw DirtyException("low()");
        return _low;
    }
    
    /**
     * \return the highest value covered by the approximation.
     * \throw DirtyException if evaluate() was not invoked beforehand.
     */
    double high() const throw(DirtyException)
    {
        if (_dirty)
            throw DirtyException("high()");
        return _high;
    }
    
    /**
     * Interpolates the PDF.
     * \param x the value at which the PDF is to be approximated.
     * \return a linearly interpolated value for the PDF, or 0.0
     * if /x/ lies outside [low()..high()].
     * \throw DirtyException if evaluate() was not invoked beforehand.
     */
    double pdf(double x) const throw(DirtyException);
    
    /**
     * Interpolates the CDF.
     * \param x the value at which the CDF is to be approximated.
     * \return a linearly interpolated value for the CDF, or 0.0
     * if /x/ lies outside [low()..high()].
     * \throw DirtyException if evaluate() was not invoked beforehand.
     */
    double cdf(double x) const throw(DirtyException);
    
    /** 
     * Interpolates the inverse CDF.
     * \param y the probability value, must be in the range 0.0 .. 1.0
     * \return an interpolated value for the inverse CDF, x where CDF(x)=y.
     * Returns low() if y<=0, high() if y>=1.0
     * \throw DirtyException if evaluate() was not invoked beforehand.
     */
    double inv_cdf(double Y) const throw(DirtyException);
    
    /**
     * \return the (estimated) mean of the distribution.
     * \throw DirtyException if evaluate() was not invoked beforehand.
     */
    double mean() const throw(DirtyException)
    {
        if (_dirty)
            throw DirtyException("mean()");
        return _mean;
    }
    
    /**
     * \return the (estimated) variance of the distribution.
     * \throw DirtyException if evaluate() was not invoked beforehand.
     */
    double variance() const throw(DirtyException)
    {
        if (_dirty)
            throw DirtyException("variance()");
        return _variance;
    }
    
    private: 
    // hidden methods
    int bin_index(double x) const
    {
        return static_cast<int>(floor(x/_width + 0.5));
    }
    int locate_index(double x) const;
    double bin_midpoint(int idx) const { return (_width*(idx+0.5)+_low); }
    
    // data
    binlist_t _binlist;     ///< histogram bins sorted in increasing center order
    double _width;  ///< bin width
    std::vector<double> _pdf;     ///< approximation of PDF
    std::vector<double> _cdf;     ///< approximation of CDF
    unsigned int _binno;     ///< length of Pdf and Cdf
    double _low, _high;	    ///< lowest and highest X value covered by the bins
    double _mean, _variance;  ///< estimated mean and variance
    unsigned int _total;     ///< total number of X values added
    bool _dirty;     ///< true when a new value is added
    
};
// END OF CLASS EmpirDistr

} // namespace prob
} // namespace multovl

#endif   // MULTOVL_PROB_EMPIRDISTR_HEADER
