#ifndef MULTOVL_PROB_STAT_HEADER
#define MULTOVL_PROB_STAT_HEADER

// == HEADER stat.hh ==

/// \file Overlap length statistics collection.
/// \date 2012-03-01
/// \author Andras Aszodi

// -- Library headers --

#include "empirdistr.hh"

// -- Boost headers --

// -- Standard headers --

#include <map>

// == CLASSES ==

namespace multovl {
namespace prob {

/// Utility to collect actual overlap lengths
/// and the null distribution estimates (obtained by shuffling)
/// grouped by multiplicity.
class Stat
{
public:
    
    /// Init to default (empty).
    Stat():
        _lendistrs()
    {}
    
    /// Adds a total overlap length to the calling object.
    /// \param multiplicity the multiplicity of the overlap for which
    /// the total overlap length is provided. Should be >= 2
    /// \param ovlen the overlap length to be added. Should be >= 0
    /// \param is_actual true if /ovlen/ is an actual overlap length,
    /// ie. before shuffling. false (the default) if /ovlen/
    /// is the total overlap length obtained in one of the shuffling rounds.
    void add(unsigned int multiplicity, 
             unsigned int ovlen, 
             bool is_actual = false);

    /// Calculates the "two-sided p-values"
    /// for all multiplicities.
    void calc_pvalues();  

private:
    
    // utility to keep together the actual total overlap length
    // and the empirically estimated null distribution
    // for a given multiplicity
    struct LenDistr
    {
        // Init. For the time being the default /ncell/ will be used
        explicit LenDistr(unsigned int ncell = 0):
            actual(0),
            nulldistr(ncell),
            pvalue(0.0)
        {}
        
        // Init straight away by invoking add(ovlen, is_actual)
        LenDistr(unsigned int ovlen, bool is_actual)
        {
            add(ovlen, is_actual);
        }
        
        // Adds an overlap length
        // \param ovlen the overlap len to be added
        // \param act if true, then the /actual/ member
        // will be set to /ovlen/, otherwise it will be added
        // to the /nulldistr/ estimate
        void add(unsigned int ovlen, bool is_actual)
        {
            if (is_actual) actual = ovlen;
            else nulldistr.add(ovlen);
        }
        
        // Calculates a "two-sided p-value" based on the 
        // position of the internal actual value
        // relative to the null distribution.
        double calc_pvalue()
        {
            nulldistr.evaluate();
            double c = nulldistr.cdf(actual);
            pvalue = (c >= 0.5)? 1.0 - c: c;
            return pvalue;
        }
        
        // data
        unsigned int actual;
        EmpirDistr nulldistr;
        double pvalue;
    };
    
    // multiplicity => total overlap length distribution
    typedef std::map<unsigned int, LenDistr> lendistrs_t;
    typedef lendistrs_t::iterator lditer_t;
    
    lendistrs_t _lendistrs;
};

} // namespace prob
} // namespace multovl

#endif  // MULTOVL_PROB_STAT_HEADER

