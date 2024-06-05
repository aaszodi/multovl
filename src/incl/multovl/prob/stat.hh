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
#ifndef MULTOVL_PROB_STAT_HEADER
#define MULTOVL_PROB_STAT_HEADER

// == HEADER stat.hh ==

/// \file Overlap statistics collection.
/// \date 2012-03-01
/// \author Andras Aszodi

// -- Library headers --

#include "multovl/prob/empirdistr.hh"

// -- Boost headers --

#include "boost/lexical_cast.hpp"

// -- Standard headers --

#include <limits>
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
            _zscore(0.0),
            _valid(false)
        {}
        
        // Init straight away by invoking add(val, is_actual)
        Distr(double val, bool is_actual):
            _actual(0.0),
            _nulldistr(),
            _pvalue(0.0),
            _zscore(0.0)
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
            _valid = false;
        }
        
        // Evaluates the nulldistr and the actual/nulldistr
        // comparison quantities such as p-value, z-score...
        // If there were enough data, then is_valid() will return /true/ afterwards
        void evaluate()
        {
            _nulldistr.evaluate();
            try {
                double c = _nulldistr.cdf(_actual);
                _pvalue = (c >= 0.5)? 1.0 - c: c;
                _zscore = (_actual - _nulldistr.mean())/_nulldistr.std_dev();
                _valid = true;
            } catch (const EmpirDistr::Exception&) {
                _valid = false;
            }
        }
        
        // accessors
        bool is_valid() const { return _valid; }
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
        bool _valid;
    };
    
    /// Init to default (empty).
    Stat():
        _distrs(),
        _minmult(std::numeric_limits<unsigned int>::max()),
        _maxmult(0)
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
    const Distr& distr(unsigned int multiplicity) const;
    
    /// \return the minimal multiplicity seen so far
    unsigned int min_mult() const { return _minmult; }
    
    /// \return the maximal multiplicity seen so far
    unsigned int max_mult() const { return _maxmult; }
    
private:
    
   // multiplicity => data distribution
    typedef std::map<unsigned int, Distr> distrs_t;
    typedef distrs_t::iterator diter_t;
    
    distrs_t _distrs;
    unsigned int _minmult, _maxmult;
};

} // namespace prob
} // namespace multovl

#endif  // MULTOVL_PROB_STAT_HEADER

