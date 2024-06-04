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
// == MODULE errors.cc ==

// -- Own header --

#include "errors.hh"

// == Implementation ==

namespace multovl {

Errors::Errors(
    const std::string& errprefix,
    const std::string& warnprefix
):
    _errors(), _warnings(),
    _errprefix(errprefix + ": "),
    _warnprefix(warnprefix + ": ")
{
}

void Errors::add_error(const std::string& msg)
{
    if (msg != "")
        _errors.push_back(_errprefix + msg);
}

void Errors::add_warning(const std::string& msg)
{
    if (msg != "")
        _warnings.push_back(_warnprefix + msg);
}

Errors& Errors::operator+=(const Errors& another)
{
    _errors.insert(
        _errors.end(),
        another._errors.begin(),
        another._errors.end()
    );
    _warnings.insert(
        _warnings.end(),
        another._warnings.begin(),
        another._warnings.end()
    );
    return *this;
}

std::ostream& Errors::print(std::ostream& outf, bool warnings) const
{
    for (const auto& err: _errors) {
        outf << err << std::endl;
    }
    if (error_count() > 0) {
        outf << _errprefix << "total = " << error_count() << std::endl;
    }
    
    if (warnings) {
        for (const auto& warn: _warnings) {
            outf << warn << std::endl;
        }
        if (warning_count() > 0) {
            outf << _warnprefix << "total = " << warning_count() << std::endl;
        }
    }
    return outf;
}
    
void Errors::clear()
{
    _errors.clear();
    _warnings.clear();
}

}   // namespace multovl

