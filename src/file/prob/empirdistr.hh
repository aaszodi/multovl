#ifndef MULTOVL_PROB_EMPIRDISTR_HEADER
#define MULTOVL_PROB_EMPIRDISTR_HEADER

// ==== HEADER empirdistr.hh ====

// -- Boost headers --

#include "boost/accumulators/accumulators.hpp"
#include "boost/accumulators/statistics.hpp"
using namespace boost::accumulators;

// -- System headers --

#include <cmath>

namespace multovl {
namespace prob {

/// Objects of the EmpirDistr class implement univariate continuous empirical distributions
/// that are approximated by histograms. Samples can be added to an EmpirDistr object
/// and after data collection the approximate CDF can be queried.
class EmpirDistr
{
    // -- member classes
    
public:
    
    /// Exception class for the following situations:
    /// if an accessor is invoked on a "dirty" EmpirDistr object
    /// if an accessor is invoked on an empty EmpirDistr object,
    /// or if variance() or std_dev() are invoked on an EmpirDistr object
    /// containing less than 2 data points
    class Exception
    {
    public:
        explicit Exception(const std::string& msg):
            _message(msg) {}
        
        const std::string error_message() const { return _message; }
        
    private:
        std::string _message;
        
    };
    // END of member class Exception
    
    // -- methods
	    
    /**
     * Constructs an object so that it is ready to do a CDF approximation
     * with a number of cells
     * \param ncell the number of cells, default 100. If ncell=0, then 100 is used.
     */
    explicit EmpirDistr(unsigned int ncell = 0);

    /// adds a value 
    /// \param x a sample from the distribution to be approximated.
    EmpirDistr& add(double x);
    
    /// evaluate(): normalise the counts and make the CDF.
    void evaluate();
    
    /// \return the lowest value covered by the approximation.
    /// \throw Exception if evaluate() was not invoked beforehand
    /// or if there were no data
    double low() const throw(Exception);
    
    /// \return the highest value covered by the approximation.
    /// \throw Exception if evaluate() was not invoked beforehand
    /// or if there were no data
    double high() const throw(Exception);
    
    /// Interpolates the CDF.
    /// \param x the value at which the CDF is to be approximated.
    /// \return an interpolated value for the CDF, or 0.0
    /// if /x/ lies outside [low()..high()].
    /// \throw Exception if evaluate() was not invoked beforehand
    /// or if there were no data.
    double cdf(double x) const throw(Exception);
    
    /// \return the (estimated) mean of the distribution.
    /// \throw Exception if evaluate() was not invoked beforehand
    /// or if there were no data.
    double mean() const throw(Exception);
    
    /// \return the (estimated) variance of the distribution.
    /// \throw DirtyException if evaluate() was not invoked beforehand.
    /// or if less than 2 data points have been added to the calling object.
    double variance() const throw(Exception);
    
    /// \return the standard deviation (by taking the square root of variance()).
    double std_dev() const throw(Exception)
	{
		return std::sqrt(variance());
	}

    private: 
    
    typedef accumulator_set<double, 
        features< 
            tag::count, tag::min, tag::max, 
            tag::mean, tag::lazy_variance, 
            stats<tag::p_square_cumulative_distribution>
        >
    > acc_t;
    typedef std::pair<double, double> histogram_bin_t;
    typedef std::vector<histogram_bin_t>::iterator histogram_iter_t;
    typedef boost::iterator_range<histogram_iter_t> histogram_t;

    // Histogram object comparison class
    struct HistogramComp
    {
        bool operator()(const histogram_bin_t& it, double x)
        {
            return (it.first < x);
        }
    };

    // data
    acc_t _acc;
    histogram_t _histogram;
    double _low, _high;
    bool _dirty;     ///< true when a new value is added
    
};
// END OF CLASS EmpirDistr

} // namespace prob
} // namespace multovl

#endif   // MULTOVL_PROB_EMPIRDISTR_HEADER