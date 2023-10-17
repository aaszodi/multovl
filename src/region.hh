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
#ifndef MULTOVL_REGION_HEADER
#define MULTOVL_REGION_HEADER

// == HEADER region.hh ==

/** \file 
 * \brief Genomic regions with coordinates and a name
 * The region limits may be extended.
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

// -- Own headers --

#include "baseregion.hh"

namespace multovl {

/**
 * \brief Instances of the Region class represent regions on a sequence.
 * The coordinates may be "extended" symmetrically in all Region instances
 * by invoking the `set_extension()` static method.
 */
class Region: public BaseRegion {
    public:
    
    /// Inits to empty (makes sense when using as input buffer)
    Region(): 
        BaseRegion()
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
    Region(unsigned int f, unsigned int l,
        char s, const std::string& nm):
        BaseRegion(f, l, s, nm)
    {}
    
    /// Inits with base class object
    Region(const BaseRegion& r):
        BaseRegion(r)
    {}
    
    // -- Getters --
    
    /// Returns the extension
    static
    const unsigned int& extension() { return _extension; }
    
    /// Returns the (possibly extended) first coordinate.
    virtual 
    unsigned int first() const override final;
    
    /// Returns the (possibly extended) last coordinate.
    virtual
    unsigned int last() const override final;
    
    // -- Setters --
    
    /// Sets the region coordinate extension.
    static
    void set_extension(unsigned int ext) { _extension = ext; }
    
    private:
    
    // data
    static unsigned int _extension; // region limits are artificially "extendable"
    
    // serialization
    // no new members, use base class serialization
    friend class boost::serialization::access;
    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << boost::serialization::base_object<BaseRegion>(*this);
    }
    
    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
        ar >> boost::serialization::base_object<BaseRegion>(*this);
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
};  // class Region

}   // namespace multovl

#endif  // MULTOVL_REGION_HEADER
