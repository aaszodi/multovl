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
#ifndef MULTOVL_PROB_RANDOMGEN_HEADER
#define MULTOVL_PROB_RANDOMGEN_HEADER

// ==== HEADER randomgen.hh ====

// -- Standard headers --

#include <random>

// -- Boost headers --

namespace multovl {
namespace prob {

/// Non-copyable onvenience class that wraps a uniform RND generator.
class UniformGen
{
    public:
    
    /// Constructor
    /// \param seed random seed to initialise
    /// \param lower the lower bound of the uniform distribution, default 0.0
    /// \param upper the upper bound of the uniform distribution, default 1.0
    UniformGen(unsigned int seed, double lower=0.0, double upper=1.0):
        _rng(seed),
        _unidistr(lower, upper)
    {}
    
    // non-copyable
    UniformGen(const UniformGen&) = delete;
    UniformGen& operator=(const UniformGen&) = delete;
    
    /// Generates a random variate.
    /// \return a uniformly distributed variate between /lower/ and /upper/
    /// as set in the ctor.
    double operator()()
    {
        return _unidistr(_rng);
    }
    
    private:
    
    std::mt19937 _rng;    ///< Mersenne twister
    std::uniform_real_distribution<> _unidistr;  ///< continuous uniform distribution
};

}   // namespace prob
}   // namespace multovl

#endif  // MULTOVL_PROB_RANDOMGEN_HEADER
