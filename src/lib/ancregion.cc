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
// == MODULE ancregion.cc ==

// -- Standard headers --

#include <vector>
#include <sstream>

// -- Boost headers --

#include "boost/algorithm/string.hpp"

// -- Own header --

#include "multovl/ancregion.hh"

// == Implementation ==

namespace multovl {

// -- AncestorRegion methods --
    
unsigned int AncestorRegion::track_id(unsigned int trackid)
{
    unsigned int oldid = _trackid;
    _trackid = trackid;
    return oldid;
}

bool AncestorRegion::operator==(const AncestorRegion& rhs) const
{
    return (static_cast<Region>(*this) == static_cast<Region>(rhs) &&
        this->track_id() == rhs.track_id() &&
        this->is_shuffleable() == rhs.is_shuffleable());
}

bool AncestorRegion::operator<(const AncestorRegion& rhs) const 
{
    return track_id() < rhs.track_id()
        ||
        (track_id() == rhs.track_id() && static_cast<Region>(*this) < static_cast<Region>(rhs));
}

std::string AncestorRegion::to_attrstring() const
{
    std::ostringstream ostr;
    ostr << track_id() << ':' << name() << ':' << strand()
        << ':' << first() << '-' << last();
    return ostr.str();
}

}   // namespace multovl
