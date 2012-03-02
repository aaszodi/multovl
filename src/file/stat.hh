#ifndef MULTOVL_PROB_STAT_HEADER
#define MULTOVL_PROB_STAT_HEADER

// == HEADER stat.hh ==

/// \file Overlap length statistics collection.
/// \date 2012-03-01
/// \author Andras Aszodi

// -- Library headers --

#include "empirdistr.hh"

// -- Boost headers --

#include "boost/lexical_cast.hpp"

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
    
    // -- Member classes --
    
    /// A NotfoundException is thrown if a method tries to access
    /// a multiplicity that has not been seen before.
    class NotfoundException
    {
    public:
        explicit NotfoundException(
            unsigned int multiplicity
        ):
        _msg("Multiplicity " + 
            boost::lexical_cast<std::string>(multiplicity) +
            " not found"
        ) {}
        const std::string error_message() const 
        {
            return _msg;
        }
        
    private:
        std::string _msg;
        
    };
    // END of member class NotFoundException

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
            _pvalue(0.0),
            _zscore(0.0)
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
        
        // Evaluates the nulldistr and the actual/nulldistr
        // comparison quantities such as p-value, z-score...
        void evaluate()
        {
            _nulldistr.evaluate();
            double c = _nulldistr.cdf(_actual);
            _pvalue = (c >= 0.5)? 1.0 - c: c;
            _zscore = (_actual - _nulldistr.mean())/_nulldistr.std_dev();
        }
        
        // accessors
        unsigned int actual() const { return _actual; }
        const EmpirDistr& nulldistr() const { return _nulldistr; }
        double p_value() const { return _pvalue; }
        double z_score() const { return _zscore; }
        
    private:
        // data
        unsigned int _actual;
        EmpirDistr _nulldistr;
        double _pvalue;
        double _zscore;
    };
    
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

    /// Evaluates the distributions inside
    void evaluate();  

    /// Look up the Lendistr object belonging to a given multiplicity.
    /// \param multiplicity is the multiplicity
    /// \return the Lendistr object belonging to /multiplicity/
    /// \throw Stat::NotfoundException if /multiplicity/ has not been seen
    const LenDistr& lendistr(unsigned int multiplicity) const throw(NotfoundException);
    
private:
    
   // multiplicity => total overlap length distribution
    typedef std::map<unsigned int, LenDistr> lendistrs_t;
    typedef lendistrs_t::iterator lditer_t;
    
    lendistrs_t _lendistrs;
};

} // namespace prob
} // namespace multovl

#endif  // MULTOVL_PROB_STAT_HEADER

