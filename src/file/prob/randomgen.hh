#ifndef MULTOVL_PROB_RANDOMGEN_HEADER
#define MULTOVL_PROB_RANDOMGEN_HEADER

// ==== HEADER randomgen.hh ====

// -- Boost headers --

#include "boost/utility.hpp"    // for noncopyable
#include "boost/random/mersenne_twister.hpp"
#include "boost/random/uniform_real.hpp"
#include "boost/random/variate_generator.hpp"

namespace multovl {
namespace prob {

/// Convenience class that wraps a uniform RND generator.
class UniformGen: boost::noncopyable
{
    public:
    
    /// Constructor
    /// \param seed random seed to initialise
    /// \param lower the lower bound of the uniform distribution, default 0.0
    /// \param upper the upper bound of the uniform distribution, default 1.0
    UniformGen(unsigned int seed, double lower=0.0, double upper=1.0):
        _rng(seed),
        _unidistr(lower, upper),
        _unigen(_rng, _unidistr)
    {}
    
    /// Generates a random variate.
    /// \return a uniformly distributed variate between /lower/ and /upper/
    /// as set in the ctor.
    double operator()()
    {
        return _unigen();
    }
    
    private:
    
    typedef boost::mt19937 rng_t;    ///< Mersenne twister type
    typedef boost::uniform_real<> unidistr_t;  ///< uniform distribution type
    typedef boost::variate_generator<rng_t&, unidistr_t > unigen_t; ///< uniform generator type
    
    rng_t _rng;
    unidistr_t _unidistr;
    unigen_t _unigen;
};

}   // namespace prob
}   // namespace multovl

#endif  // MULTOVL_PROB_RANDOMGEN_HEADER
