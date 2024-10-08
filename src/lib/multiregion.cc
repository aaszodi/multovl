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
// == MODULE multiregion.cc ==

// -- Standard headers --

#ifndef NDEBUG
#include <iostream>
#endif

// -- Own header --

#include "multovl/multiregion.hh"

// -- Boost headers --

// == Implementation ==

namespace multovl {

MultiRegion::MultiRegion():
    BaseRegion(0, 0, '.', "overlap"),
    _ancestors(),
    _solitary(false),
    _mult(0)
{}

MultiRegion::MultiRegion(unsigned int first, unsigned int last, 
    const ancregset_t& ancestors, unsigned int mult):
    BaseRegion(first, last, '.', "overlap"), 
    _ancestors(ancestors),
    _mult(mult>0? mult: ancestors.size())
{
    update_solitary();
#ifndef NDEBUG
    std::cerr << "MultiRegion: anclen = " << _ancestors.size() << ", ancstr = \"" << anc_str() << "\"" << std::endl;
#endif
}

void MultiRegion::add_ancestor(const AncestorRegion& anc)
{
    _ancestors.insert(anc);
    _mult++;
    update_solitary();
}

std::vector<int> MultiRegion::ancestor_trackids() const
{
    std::vector<int> ancids;
    ancids.reserve(ancestors().size());
    for (const auto& anc : ancestors()) {
        ancids.push_back(anc.track_id());
    }
    return ancids;
}

std::string MultiRegion::anc_str() const
{
    if (ancestors().size() == 0) {
        // only relevant for empty
        return "";
    }
    std::string ancstr = "";
    auto ait = ancestors().begin();
    auto nextit = ait;
    while(ait != ancestors().end()) {
        // compose the ancestor attribute string
        if (ait != ancestors().begin()) ancstr += '|';
        // find first element not equal to *ait
        unsigned int cnt;
        for(cnt = 0; nextit != ancestors().end() && *ait == *nextit; ++cnt, nextit++);
        if (cnt > 1) {
            // ancestor attribute string gets a prefix "<cnt>*"
            ancstr += std::to_string(cnt);
            ancstr += "*"; // avoid temporaries with +=
        }
        ancstr += ait->to_attrstring();
        ait = nextit;   // stepper
    }
    return ancstr;
}

// a MultiRegion is solitary if it has only one single ancestor
// that is not part of any overlap: in this case the coords
// of the ancestor are the same as those of the present region
// private
bool MultiRegion::update_solitary()
{
    if (ancestors().size() == 1) {
        const auto& ancit = ancestors().begin(); // points to the one and only ancestor
        _solitary = (ancit->first() == this->first() && ancit->last() == this->last());
    } else {
        _solitary = false;
    }
    return _solitary;
}

}   // namespace multovl

