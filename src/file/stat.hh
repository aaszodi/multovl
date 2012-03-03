#ifndef MULTOVL_PROB_STAT_HEADER
#define MULTOVL_PROB_STAT_HEADER

// == HEADER stat.hh ==

/// \file Overlap statistics collection.
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

/// Utility to collect actual overlap data
/// and the corresponding null distribution estimates (obtained by shuffling)
/// grouped by multiplicity.
/// The values collected are of type double although in most cases
/// they are actually integers, but let's stay generic.
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
    class Distr
    {
    public:
        
        // Init. For the time being the default /ncell/ will be used
        explicit Distr(unsigned int ncell = 0):
            _actual(0.0),
            _nulldistr(ncell),
            _pvalue(0.0),
            _zscore(0.0)
        {}
        
        // Init straight away by invoking add(val, is_actual)
        Distr(double val, bool is_actual)
        {
            add(val, is_actual);
        }
        
        // Adds a value
        // \param val the value to be added
        // \param act if true, then the /actual/ member
        // will be set to /val/, otherwise it will be added
        // to the /nulldistr/ estimate
        void add(double val, bool is_actual)
        {
            if (is_actual) _actual = val;
            else _nulldistr.add(val);
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
        double actual() const { return _actual; }
        const EmpirDistr& nulldistr() const { return _nulldistr; }
        double p_value() const { return _pvalue; }
        double z_score() const { return _zscore; }
        
    private:
        // data
        double _actual;
        EmpirDistr _nulldistr;
        double _pvalue;
        double _zscore;
    };
    
    /// Init to default (empty).
    Stat():
        _distrs()
    {}
    
    /// Adds a value to the calling object.
    /// \param multiplicity the multiplicity of the overlap for which
    /// the total overlap length is provided. Should be >= 2
    /// \param val the value to be added
    /// \param is_actual true if /value/ is an actual value,
    /// ie. before shuffling. false (the default) if /val/
    /// is a value obtained in one of the shuffling rounds.
    void add(unsigned int multiplicity, 
             double val, 
             bool is_actual = false);

    /// Evaluates the distributions inside
    void evaluate();  

    /// Look up the Distr object belonging to a given multiplicity.
    /// \param multiplicity is the multiplicity
    /// \return the Distr object belonging to /multiplicity/
    /// \throw Stat::NotfoundException if /multiplicity/ has not been seen
    const Distr& distr(unsigned int multiplicity) const throw(NotfoundException);
    
private:
    
   // multiplicity => data distribution
    typedef std::map<unsigned int, Distr> distrs_t;
    typedef distrs_t::iterator diter_t;
    
    distrs_t _distrs;
};

} // namespace prob
} // namespace multovl

#endif  // MULTOVL_PROB_STAT_HEADER

