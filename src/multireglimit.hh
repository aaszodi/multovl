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
#ifndef MULTOVL_MULTIREGLIMIT_HEADER
#define MULTOVL_MULTIREGLIMIT_HEADER

// == Header multireglimit.hh ==

/// \file 
/// \brief Collection of multiple region limits.
/// \author Andras Aszodi
/// \date 2011-11-05

// -- System headers --
    
// -- Boost headers --

// fixing a Boost 1.74 bug
#include "boost/version.hpp"
#if BOOST_VERSION == 107400
#include "boost/serialization/library_version_type.hpp"
#endif
#include "boost/serialization/set.hpp"

// -- Own headers --

#include "reglimit.hh"

// == Classes ==

namespace multovl {

/**
 * Objects of the MultiRegLimit class store genomic regions encoded
 * by the regions' first and last positions.
 * Internally, this is implemented by a multiset storing RegLimit objects.
 */
class MultiRegLimit
{
public: 
	
    typedef std::multiset<RegLimit> reglim_t;
    
	// -- methods --
           
    /// Init to empty 
    MultiRegLimit(): _reglim() {}
    
    /// Init to contain a region and trackid 
    MultiRegLimit(const Region& region, unsigned int trackid): 
        _reglim()
    {
        add(region, trackid);
    }
    
    /// Copy ctor
    /// Implements a deep copy so that the AncestorRegion objects under the RegLimits
    /// will be completely independent from those under /other/
    MultiRegLimit(const MultiRegLimit& other);
    
    /// Assignment
    /// Implements a deep copy of the underlying AncestorRegion objects just like the copy ctor
    MultiRegLimit& operator=(const MultiRegLimit& rhs);
    
    virtual ~MultiRegLimit() {}
    
    /// Adds a region /region/ identified by /trackid/
    /// to the calling object.
    /// \param region a genomic region to be stored
    /// \param trackid an arbitrary number identifying a track (collection of regions).
    void add(const Region& region, unsigned int trackid);

    /// \return const access to the RegLimit multiset inside
    const reglim_t& reglim() const { return _reglim; }
    
    /// Merges another MultiRegLimit object into the calling object.
    /// \param rhs the other MultiRegLimit object.
    /// \return the calling object
    MultiRegLimit& operator+=(const MultiRegLimit& rhs);
    
protected:
    
    /// \return non-const access to the RegLimit multiset inside
    reglim_t& nonconst_reglim() { return _reglim; }
    
private:
    
    static
    void deep_copy(const reglim_t& source, reglim_t& target);
    
    // data 
    reglim_t _reglim;

    // serialization
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & _reglim;
    }
    
};

} // namespace multovl

#endif  // MULTOVL_MULTIREGLIMIT_HEADER
