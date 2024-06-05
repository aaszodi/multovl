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
// == MODULE baseregion.cc ==

// -- Own header --

#include "multovl/baseregion.hh"

// -- Implementation --

namespace multovl {

// -- BaseRegion methods

BaseRegion::BaseRegion(unsigned int f, unsigned int l,
    char s, const std::string& nm): _name(nm)
{
    set_coords(f, l);
    strand(s);
}

std::string BaseRegion::name(const std::string& nm)
{
    std::string oldname = _name;
    _name = nm;
    return oldname;
}

bool BaseRegion::operator<(const BaseRegion& rhs) const
{
    if (this->first() < rhs.first()) return true;
    if (this->first() == rhs.first())
    {
        if (this->last() > rhs.last()) return true;
        if (this->last() == rhs.last())
        {
            if (this->strand() < rhs.strand()) return true;
            if (this->strand() == rhs.strand())
            {
                return (this->name() < rhs.name());
            }
            else return false;
        }
        else return false;
    }
    return false;
}

void BaseRegion::set_coords(unsigned int f, unsigned int l)
{
    if (f <= l)
    {
        _first = f;
        _last = l;
    }
    else
    {
        // swap silently
        _first = l;
        _last = f;
    }
}

void BaseRegion::strand(char s)
{
    if (s == '+' || s == '-')
        _strand = s;
    else
        _strand = '.';
}

}   // namespace multovl
