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
// == MODULE shuffleovl.cc ==

/// \file
/// \date 2012-02-26
/// \author Andras Aszodi

// -- Own header --

#include "shuffleovl.hh"

// -- Boost headers --

// -- Standard headers --

// == Implementation ==

namespace multovl {
namespace prob {

ShuffleOvl::ShuffleOvl(const freeregvec_t& frees):
    _freeregions(frees),
    _rpm(),
    _shufflecount(0)
{}

bool ShuffleOvl::add_randomplacer(unsigned int reglen, unsigned int trackid)
{
    if (reglen == 0)
        return false;
    bool ok = false;
    
    // have we seen this track ID before?
    rpm_t::iterator rit = _rpm.find(trackid);
    if (rit != _rpm.end())
    {
        // track ID already there
        ok = rit->second.add(reglen);
    }
    else
    {
        // set up for this track ID
        RandomPlacer rp;
        ok = rp.add(reglen);
        _rpm[trackid] = rp;
    }
    return ok;
}

unsigned int ShuffleOvl::shuffle(UniformGen& rng)
{
    // remove all RegLimits referring to the reshufflable tracks
    for (rpm_t::const_iterator rcit = _rpm.begin(); rcit != _rpm.end(); ++rcit)
    {
        unsigned int rtid = rcit->first;    // reshufflable track ID
        reglims_t::const_iterator rlit = reglims().begin();
        while(rlit != reglims().end())
        {
            if (rtid == rlit->track_id())
            {
                reglims_t::iterator me = rlit;   // rlit still valid
                ++rlit;                         // so increment it...
                nonconst_reglims().erase(me);    // me is now invalid
            }
            else ++rlit;
        }
    }
    
    // shuffle the reshufflable tracks
    for (rpm_t::iterator rit = _rpm.begin(); rit != _rpm.end(); ++rit)
    {
        rit->second.random_placement(_freeregions, rng);
    }
    
    // add their limits again
    for (rpm_t::const_iterator rcit = _rpm.begin(); rcit != _rpm.end(); ++rcit)
    {
        unsigned int rtid = rcit->first;
        const std::vector<Region>& vr = rcit->second.get_regions();
        for (std::vector<Region>::const_iterator vrcit = vr.begin(); vrcit != vr.end(); ++vrcit)
        {
            add(*vrcit, rtid);
        }
    }
    return ++_shufflecount;
}

}   // namespace prob
}   // namespace multovl

