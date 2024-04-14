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
// == PROGRAM inputfiles.cc ==

/**
 * \file Makes a set of synthetic input files
 * that can be used for (timing) tests on the multovl variants.
 * \author Andras Aszodi
 * \date 2010-03-13
 */

// -- Standard headers --

#include <cstdlib>

#ifdef _WIN32
#include "wgetopt.h"
#else
#if defined(__sun) && defined(__SUNPRO_CC)
#define __EXTENSIONS__
#include <unistd.h>	// Solaris/SunPro getopt() is here...
#else
#include <getopt.h> // Linux, MacOS
#endif
#endif

#include <iostream>
#include <sstream>
#include <vector>

// -- Boost headers --

#include "boost/lexical_cast.hpp"
#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"

// -- Own headers --

#include "region.hh"
#include "linewriter.hh"

// -- Typedefs --

typedef std::vector<multovl::Region> regvec_t;
typedef std::vector<regvec_t> trackvec_t;

// -- Prototypes --

static std::ostream& print_help(std::ostream &out);
static unsigned int parse_uintparam(const char* optarg, unsigned int defval);
static trackvec_t make_regvecs(
    unsigned int delta, unsigned int reglen, unsigned int groupgap,
    unsigned int trackcnt, unsigned int groupcnt
);

// -- Default values --

static const unsigned int CHROMCNT = 2, DELTA = 10, TRACKCNT = 3, GROUPCNT = 4, 
    REGLEN = TRACKCNT*DELTA, GROUPGAP = 100;

// == MAIN ==

int main(int argc, char *argv[])
{
    extern char* optarg;
    signed char optch;
	unsigned int chromcnt = CHROMCNT,  // number of chromosomes
	    delta = DELTA, // amount of shift between regions within an overlap group
	    trackcnt = TRACKCNT,   // number of tracks
	    groupcnt = GROUPCNT,    // number of overlap groups
	    reglen = REGLEN, // length of each region
	    groupgap = GROUPGAP;    // distance between neighbouring overlap groups
	boost::filesystem::path outdir;
	bool outdirseen = false;
	
	// process command-line options
    static const char OPTCHARS[] = "c:d:D:t:g:l:L:h";
    while((optch = getopt(argc, argv, OPTCHARS)) != -1)
    {
        switch(optch)
        {
            case 'c':
                chromcnt = parse_uintparam(optarg, CHROMCNT);
                if (chromcnt == 0)
                    chromcnt = CHROMCNT;
                break;
            case 'd':
                delta = parse_uintparam(optarg, DELTA);
                if (delta == 0)
                    delta = DELTA;
                break;
            case 'D':
            	try {
					outdir = boost::filesystem::path(optarg);
					outdirseen = true;
				} catch (const boost::filesystem::filesystem_error& err) {
					std::cerr << "! Silly directory path \"" << optarg <<
						"\": " << err.what() << std::endl;
					outdirseen = false;
				}
                break;
            case 't':
                trackcnt = parse_uintparam(optarg, TRACKCNT);
                if (trackcnt < 2)
                    trackcnt = 2;
                break;
            case 'g':
                groupcnt = parse_uintparam(optarg, GROUPCNT);
                if (groupcnt == 0)
                    groupcnt = GROUPCNT;
                break;
            case 'l':
                reglen = parse_uintparam(optarg, REGLEN);
                if (reglen == 0)
                    reglen = REGLEN;
                break;
            case 'L':
                groupgap = parse_uintparam(optarg, GROUPGAP);
                if (groupgap == 0)
                    groupgap = GROUPGAP;
                break;
            case 'h':
            case '?':
                print_help(std::cout);
                std::exit(EXIT_FAILURE);
		}
	}
	if (!outdirseen)
	{
	    std::cerr << "! You must define an output directory using the -D <dir> option." << std::endl;
	    print_help(std::cerr);
	    std::exit(EXIT_FAILURE);
	}
	
	// create data directory if necessary
	if (outdir != ".")
	{
		try
		{
			boost::filesystem::remove_all(outdir);
			boost::filesystem::create_directories(outdir);
		} catch (const boost::filesystem::filesystem_error& err) {
			std::cerr << "! Failed to create output dir \"" << outdir << "\": " <<
				err.what() << std::endl;
			std::exit(EXIT_FAILURE);
		}
	}
	
	// generate settings string
	std::ostringstream sets;
	sets << "made by inputfiles -c " << chromcnt << " -d " << delta << " -t " << trackcnt
	    << " -g " << groupcnt << " -l " << reglen << " -L " << groupgap;
    std::string setstr = sets.str();
    
	// generate and save the tracks
	unsigned int i;
	trackvec_t regvecs = make_regvecs(delta, reglen, groupgap, trackcnt, groupcnt);
	
	// we could just have numbered chromosomes, but
	// let's simulate the usual case with "chrX"-style names
	std::vector<std::string> chroms;
	for (i=0; i<chromcnt; ++i)
		chroms.push_back("chr" + boost::lexical_cast<std::string>(i+1));
	
	boost::filesystem::path outpath(outdir);
	for (i=0; i<trackcnt; ++i)
	{
	    // create and open the track file
		std::string filenm = "track_" + boost::lexical_cast<std::string>(i) + ".bed";
		boost::filesystem::path filepath = outpath / filenm;
        boost::filesystem::ofstream out(filepath);	    
	    if (!out)
	    {
	        std::cerr << "? Cannot not open \"" << filepath.string() << "\", skipped" << std::endl;
	        continue;
	    }
	    out << "# Synthetic input " << filenm << " " << setstr << std::endl;
	    
		for (unsigned int ch = 0; ch < chromcnt; ++ch)
		{
		    multovl::io::BedLinewriter blw(chroms[ch]);
		    for (const auto& reg : regvecs[i]) {
		        out << blw.write(reg) << std::endl;
            }
		}
		out.close();
	}
	
	// add a "free.bed" as free regions for [par]multovlprob
	// this is very simple, just one region per chromosome
	// that spans all input regions
	// we write it 'by hand'
	boost::filesystem::path freepath = outpath / "free.bed";
    boost::filesystem::ofstream out(freepath);	    
    if (out)
    {
        const unsigned int FREEMAX = (reglen + (trackcnt-1)*delta) * groupcnt 
            + groupgap*(groupcnt -1) + 2*delta;
        out << "# Synthetic free regions file free.bed " << setstr << std::endl;
		for (unsigned int ch = 1; ch <= chromcnt; ++ch)
		{
		    out << "chr" << ch << "\t1\t" << FREEMAX << "\tfree" << ch << "\t0\t." << std::endl;
        }
        out.close();
    }
    else
    {
        std::cerr << "? Cannot not open \"" << freepath.string() << "\", skipped" << std::endl;
    }



	std::exit(EXIT_SUCCESS);
}

// == FUNCTIONS ==

static std::ostream& print_help(std::ostream &out)
{
    out << "Input files for multovl testing " << std::endl;
    out << "Usage: inputfiles [options]" << std::endl << "Options:" << std::endl;
    out << "\t-c <N> use N chromosomes (default " << CHROMCNT << ")" << std::endl;
    out << "\t-d <N> regions within overlap groups are shifted by N (default " << 
    	DELTA << ")" << std::endl;
    out << "\t-D <outdir> output directory (mandatory)" << std::endl;
    out << "\t-t <N> generate N tracks (default " << TRACKCNT << ")" << std::endl;
    out << "\t-g <N> make N overlap groups (default " << GROUPCNT << ")" << std::endl;
    out << "\t-l <N> region length (default " << REGLEN << ")" << std::endl;
    out << "\t-L <N> gap between groups (default " << GROUPGAP << ")" << std::endl;
    out << "\t-h|-?: print this help and exit" << std::endl;
    out << "Writes the files track_{0,1,...}.bed and free.bed to output directory" << std::endl;
	out << "Example: regions generated for -c 1 -g 2 -t 3" << std::endl << std::endl;
	out << "            reglen" << std::endl;
	out << "         <---------->" << std::endl;
	out << "         :          :" << std::endl;
	out << "Track 1  |==========|                    |==========|" << std::endl;
	out << "Track 2  :   |==========|                :   |==========|    " << std::endl;
	out << "Track 3  :   :   |==========|            :       |==========|" << std::endl;
	out << "         :   :              :            :" << std::endl;
	out << "         <--->              <------------>" << std::endl;
	out << "         delta                 groupgap" << std::endl;

    return out;
}

static unsigned int parse_uintparam(const char* optarg, unsigned int defval)
{
    try
    {
        return boost::lexical_cast<unsigned int>(optarg);
    } catch (boost::bad_lexical_cast&) {
        return defval;
    }
}

static trackvec_t make_regvecs(
    unsigned int delta, unsigned int reglen, unsigned int groupgap,
    unsigned int trackcnt, unsigned int groupcnt
)
{
	// generate track names
	unsigned int t;
	std::vector<std::string> tracknames;
	for (t = 1; t <= trackcnt; ++t)
		tracknames.push_back("trk" + boost::lexical_cast<std::string>(t));
	
	trackvec_t regvecs(trackcnt);
	
	unsigned int pos = delta;	// starting position
	for (unsigned int g = 0; g < groupcnt; g++)
	{
		for (t = 0; t < trackcnt; ++t)
		{
			regvecs[t].push_back(
				multovl::Region(pos, pos+reglen, '.', tracknames[t])
			);
			pos += delta;
		}
		pos += reglen - delta + groupgap;
	}
	
	return regvecs;
}

// == END OF PROGRAM inputfiles.cc ==
