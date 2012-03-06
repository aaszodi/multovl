// == PROGRAM multovlprob.cc ==

/** \file multovlprob.cc
 * \brief Program to calculate multiple overlap probabilities among genomic regions.
 *
 * This program first calculates the actual multiple overlaps,
 * and then permutes the regions in those tracks that were allowed to be "shuffled"
 * several times to estimate the null distribution of multiple overlaps.
 * All regions must fall into a predefined set of "free regions"
 * that corresponds to the segments of a genome "visible" to a given
 * experimental methodology (microarrays or HT sequencing).
 *
 * \author Andras Aszodi
 * \date 2012-02-28
 */

// -- Standard headers --

#include <cstdlib>
#include <stdexcept>

// -- Own headers --

#include "probpipeline.hh"
using namespace multovl;
using namespace prob;

// == MAIN ==

int main(int argc, char *argv[])
{
    try
    {
        // set up and run pipeline
        ProbPipeline pipeline(argc, argv);
        bool ok = pipeline.run();
        if (!ok)
        {
            pipeline.errors().print(std::cerr);
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

// == END OF PROGRAM multovlprob.cc ==
