// == PROGRAM pgmultovl.cc ==

/** \file pgmultovl.cc
 * \brief Program to detect multiple overlaps among genomic regions on several chromosomes.
 *
 * Detection of multiple overlaps among a set of genomic regions.
 * This is the "database" variant: 
 * fetches tracks from a Postgres DB, writes the result to the same DB,
 * runs on 1 CPU only.
 *
 * \author Andras Aszodi
 * \date 2011-06-24
 */

// -- Standard headers --

#include <cstdlib>
#include <stdexcept>

// -- Own headers --

#include "pgpipeline.hh"
using namespace multovl;

// == MAIN ==

int main(int argc, char *argv[])
{
    try
    {
        // set up and run pipeline
        PgPipeline pipeline(argc, argv);
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

// == END OF PROGRAM pgmultovl.cc ==
