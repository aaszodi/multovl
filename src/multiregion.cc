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

#ifdef MULTOVL_DEBUG
#include <iostream>
#endif

// -- Own header --

#include "multiregion.hh"

// -- Boost headers --

#include "boost/lexical_cast.hpp"

// == Implementation ==

namespace multovl {

MultiRegion::MultiRegion():
    Region(0, 0, '.', "overlap"),
    _ancestors(),
    _solitary(false),
    _mult(0)
{}

MultiRegion::MultiRegion(unsigned int first, unsigned int last, 
    const ancregset_t& ancestors, unsigned int mult):
    Region(first, last, '.', "overlap"), 
    _ancestors(ancestors),
    _mult(mult>0? mult: ancestors.size())
{
    update_solitary();
#ifdef MULTOVL_DEBUG
    std::cerr << "anclen = " << ancestors().size() << ", ancstr = \"" << ancstr() << "\"" << std::endl;
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
    for (ancregset_t::const_iterator ait = ancestors().begin(); ait != ancestors().end(); ++ait)
    {
        ancids.push_back(ait->track_id());
    }
    return ancids;
}

std::string MultiRegion::anc_str() const
{
    std::string ancstr = "";
    for (ancregset_t::const_iterator ait = _ancestors.begin(); ait != _ancestors.end(); ++ait)
    {
        // compose the ancestor attribute
        if (ait != _ancestors.begin()) ancstr += '|';
        unsigned int cnt = _ancestors.count(*ait);  // multiset, multiple element
        if (cnt > 1)
        {
            // ancestor gets a prefix "<cnt>*"
            ancstr += boost::lexical_cast<std::string>(cnt) + '*'
                + ait->to_attrstring();
            for (; cnt > 1; --cnt, ++ait); // skip the multiple occurrences
        }
        else
        {
            // ancestor occurs only once
            ancstr += ait->to_attrstring();
        }
    }
    return ancstr;
}

// a MultiRegion is solitary if it has only one single ancestor
// that is not part of any overlap: in this case the coords
// of the ancestor are the same as those of the present region
// private
bool MultiRegion::update_solitary()
{
    if (ancestors().size() == 1)
    {
        const ancregset_t::const_iterator& ancit = ancestors().begin();
        _solitary = (
            _ancestors.count(*ancit) == 1 &&
            ancit->first() == this->first() && 
            ancit->last() == this->last()
        );
    }
    else
        _solitary = false;
    return _solitary;
}



}   // namespace multovl

