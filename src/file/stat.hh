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
    
    // utility to keep together the actual total overlap length
    // and the empirically estimated null distribution
    // for a given multiplicity
    class LenDistr
    {
    public:
        
        // Init. For the time being the default /ncell/ will be used
        explicit LenDistr(unsigned int ncell = 0):
            _actual(0),
            _nulldistr(ncell),
            _pvalue(0.0)
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
            if (is_actual) _actual = ovlen;
            else _nulldistr.add(ovlen);
        }
        
        // Calculates a "two-sided p-value" based on the 
        // position of the internal actual value
        // relative to the null distribution.
        double calc_pvalue()
        {
            _nulldistr.evaluate();
            double c = _nulldistr.cdf(_actual);
            _pvalue = (c >= 0.5)? 1.0 - c: c;
            return _pvalue;
        }
        
        unsigned int actual() const { return _actual; }
        const EmpirDistr& nulldistr() const { return _nulldistr; }
        double p_value() const { return _pvalue; }
        
    private:
        // data
        unsigned int _actual;
        EmpirDistr _nulldistr;
        double _pvalue;
    };
    
   // multiplicity => total overlap length distribution
    typedef std::map<unsigned int, LenDistr> lendistrs_t;

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

    const lendistrs_t& lendistrs() const { return _lendistrs; }
    
private:
    
    typedef lendistrs_t::iterator lditer_t;
    
    lendistrs_t _lendistrs;
};

} // namespace prob
} // namespace multovl

#endif  // MULTOVL_PROB_STAT_HEADER

