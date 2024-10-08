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
#ifndef MULTOVL_BASEREGION_HEADER
#define MULTOVL_BASEREGION_HEADER

// == HEADER baseregion.hh ==

/** \file 
 * \brief Base class representing genomic regions with coordinates and a name.
 * \author Andras Aszodi
 * \date 2023-10-15.
 */

// -- Standard headers --

#include <string>
#include <iostream>

// -- Boost headers --

#include "boost/operators.hpp"
#include "boost/serialization/access.hpp"
#include "boost/serialization/string.hpp"
#include "boost/serialization/version.hpp"
#include "boost/serialization/split_member.hpp"
#include "boost/serialization/base_object.hpp"

namespace multovl {

/**
 * \brief Instances of the BaseRegion class represent regions on a sequence.
 * They have first and last coordinates, strand information and a name.
 * This class serves as the base class for all other classes that represent genomic regions.
 */
class BaseRegion:
    boost::less_than_comparable< BaseRegion,
    boost::equality_comparable<BaseRegion> >
{
    public:
    
    /// Inits to empty (makes sense when using as input buffer)
    BaseRegion(): 
        _first(0), _last(0),
        _strand('.'), _name("")
    {}
    
    /**
     * Inits to contain the genomic region with positions [f..l],
     * strand /s/, name /nm/.
     * Enforces the invariance f<=l
     * If f>l, then the values are swapped silently.
     * \param f the first position of the region
     * \param l the last position of the region
     * \param s the strand indicator. Anything else than '+' or '-' will be
     * converted to '.'
     * \param nm the feature name for the region
     * Note: the parameters f,l,s will be fixed for the lifetime of the object.
     */
    BaseRegion(unsigned int f, unsigned int l,
        char s, const std::string& nm);
    
    /// Copy ctor
    BaseRegion(const BaseRegion&) = default;
    
    virtual
    ~BaseRegion() = default;
    
    // -- Getters --
    
    /// Returns the first coordinate.
    virtual
    unsigned int first() const { return _first; }
    
    /// Returns the last coordinate.
    virtual
    unsigned int last() const { return _last; }
    
    /// Returns the length
    unsigned int length() const { return is_empty()? 0: last() - first() + 1; }
    
    /// Empty region (0,0)
    bool is_empty() const { return (first() == 0 && last() == 0); }
    
    /// Returns the strand information.
    const char& strand() const { return _strand; }
    
    /// Returns the name of the region.
    const std::string& name() const { return _name; }
    
    /// \return true if the positions of /*this/ and /other/
    /// are the same, strand information is not considered.
    bool equal_pos(const BaseRegion& rhs) const
    {
        return (this->first() == rhs.first() && this->last() == rhs.last());
    }
    
    /// Equality. Positions and strands must be equal.
    bool operator==(const BaseRegion& rhs) const
    {
        return (
            equal_pos(rhs) && 
            this->strand() == rhs.strand() &&
            this->name() == rhs.name()
        );
    }
        
    /// Ordering.
    /// Sorted on first,last coords, strand ('+' < '-' < '.') and the name (lexicographically).
    bool operator<(const BaseRegion& rhs) const;
    
    // -- Setters --
    
    /// Sets the coordinates. Enforces f<=l.
    /// This method is virtual, will be overridden in `Region`
    /// to handle optional region limit extensions.
    /// \param f The first coordinate
    /// \param l the last coordinate
    virtual
    void set_coords(unsigned int f, unsigned int l);
    
    /// Sets the strand
    void strand(char s);
    
    /// Changes the name of the region
    /// \return old name.
    std::string name(const std::string& nm);
    
    private:
    
    unsigned int _first, _last;
    char _strand;
    std::string _name;

    // "split" serialization
    friend class boost::serialization::access;
    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << _first << _last 
            << _strand << _name;
    }
    
    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
        unsigned int first, last;
        ar >> first >> last;
        this->set_coords(first, last);  // sets _length, too
        ar >> _strand;
        ar >> _name;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};  // class BaseRegion

}   // namespace multovl

#endif  // MULTOVL_BASEREGION_HEADER
