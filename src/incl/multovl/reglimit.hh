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
#ifndef MULTOVL_REGLIMIT_HEADER
#define MULTOVL_REGLIMIT_HEADER

// == Header reglimit.hh ==

/// \file 
/// \brief Helper class to keep track of the limits (first, last position)
/// of regions to be overlapped.
/// \author Andras Aszodi
/// \date 2011-11-02

// -- System headers --

// -- Boost headers --

#include "boost/serialization/shared_ptr.hpp"   // for serializing `std::shared_ptr`

// -- Own headers --

#include "multovl/ancregion.hh"     // pulls in region.hh as well

// == Classes ==

namespace multovl {

/// Each ancestor region to be overlapped is stored twice in MultiRegion's lookup;
/// once as a "first pos", and once as a "last pos".
/// The RegLimit class implements these "region limit" objects.
class RegLimit
{
public:
    
    /// Init to empty
    RegLimit(): _regp(), _isfirst(false) {}
    
    /// Init with an ancestor region
    /// \param const reference to an ancestor region (managed by somebody else)
    /// \param isfirst true if first position, false if last
    explicit RegLimit(const AncestorRegion& reg, 
        bool isfirst=true)
    : _regp{&reg}, _isfirst{isfirst} {}
    
    // -- Accessors --
    
    /// \return the AncestorRegion underlying this RegLimit.
    const AncestorRegion& region() const
    {
        return *_regp;
    }
    
    /// \return /true/ if this limit is the "first" coordinate of the underlying AncestorRegion.
    bool is_first() const
    {
        return _isfirst;
    }
    
    /// Sets the first attribute of the calling object.
    void first(bool isfirst)
    {
        _isfirst = isfirst;
    }
    
    unsigned int track_id() const
    {
        return _regp->track_id();   // convenience function
    }
    
    /// \return the position of the calling object, depending on is_first().
    /// If is_first() == true, then the first position is returned, otherwise the last is returned.
    unsigned int this_pos() const
    {
        return (is_first()? _regp->first(): _regp->last());
    }
    
    /// \return the "other" position of the calling object, depending on is_first().
    /// If is_first() == true, then the last position is returned, otherwise the first is returned.
    unsigned int other_pos() const
    {
        return (is_first()? _regp->last(): _regp->first());
    }
    
    /// Ordering according to position, or first before last if the same position.
    bool operator<(const RegLimit& other) const;
    
private:
    
    // data
    const AncestorRegion* _regp;    // not supposed to modify underlying object ("view ptr")
    bool _isfirst;
    
};  // class RegLimit
    
}   // namespace multovl

#endif  // MULTOVL_REGLIMIT_HEADER
