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
// == PROGRAM parmultovlprob.cc ==

/** \file parmultovlprob.cc
 * \brief Parallelised program to calculate multiple overlap probabilities among genomic regions.
 *
 * This program first calculates the actual multiple overlaps,
 * and then permutes the regions in those tracks that were allowed to be "shuffled"
 * several times to estimate the null distribution of multiple overlaps.
 * These shufflings are executed in parallel on multicore machines.
 * All regions must fall into a predefined set of "free regions"
 * that corresponds to the segments of a genome "visible" to a given
 * experimental methodology (microarrays or HT sequencing).
 *
 * \author Andras Aszodi
 * \date 2012-04-14
 */

// -- Standard headers --

#include <cstdlib>
#include <stdexcept>

// -- Own headers --

#include "multovl/prob/parprobpipeline.hh"

// == MAIN ==

int main(int argc, char *argv[])
{
    try
    {
        // set up and run pipeline
        multovl::prob::ParProbPipeline pipeline(argc, argv);
        bool ok = pipeline.run();
        if (!ok)
        {
            std::exit(EXIT_FAILURE);
        }
        std::exit(EXIT_SUCCESS);
    }
    catch (const std::exception& exc)
    {
        std::cerr << "! " << exc.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }
    catch (...)
    {
        std::cerr << "! Unexpected exception caught, exiting" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

// == END OF PROGRAM parmultovlprob.cc ==
