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
#ifndef MULTOVL_PIPELINE_HEADER
#define MULTOVL_PIPELINE_HEADER

// == Header pipeline.hh ==

// -- Own headers --

#include "basepipeline.hh"
#include "multioverlap.hh"

// -- Boost headers --

// -- Standard headers --

#include <map>
#include <string>

namespace multovl {

// -- Classes --

/// Abstract class that provides a framework for detecting multiple overlaps using the MultiOverlap class.
class Pipeline: public BasePipeline
{
    
protected:
    
    /// There is one MultiOverlap object for each chromosome
    typedef std::map<std::string, MultiOverlap> chrom_multovl_map;

    /// Detects the overlaps.
    /// The default implementation provided here uses 1 CPU core.
    /// \return the total number of overlaps found, including solitary regions.
    virtual
    unsigned int detect_overlaps() override;
    
    /// \return const access to the chromosome=>multiple overlap map
    const chrom_multovl_map& cmovl() const { return _cmovl; }

    /// \return non-const access to the chromosome=>multiple overlap map
    chrom_multovl_map& cmovl() { return _cmovl; }

private:

    chrom_multovl_map _cmovl;   ///< chromosome ==> MultiOverlap map
};

}   // namespace multovl

#endif  // MULTOVL_PIPELINE_HEADER
