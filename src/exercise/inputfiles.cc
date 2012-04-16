// == PROGRAM inputfiles.cc ==

/**
 * \file Makes a set of synthetic input files
 * that can be used for (timing) tests on the multovl variants.
 * \author Andras Aszodi
 * \date 2010-03-13
 */

// -- Standard headers --

#include <cstdlib>

#if defined(__sun) && defined(__SUNPRO_CC)
#define __EXTENSIONS__
#include <unistd.h>	// Solaris/SunPro getopt() is here...
#else
#include <getopt.h> // Linux, MacOS
#endif

#include <iostream>
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
static trackvec_t make_regvecs(unsigned int delta, unsigned int trackcnt, 
	unsigned int groupcnt);

// -- Default values --

static const unsigned int CHROMCNT=2, DELTA=10, TRACKCNT=3, GROUPCNT=4;

// == MAIN ==

int main(int argc, char *argv[])
{
    extern char* optarg;
    signed char optch;
	unsigned int chromcnt=CHROMCNT, delta=DELTA, trackcnt=TRACKCNT, groupcnt=GROUPCNT;
	boost::filesystem::path outdir;
	bool outdirseen = false;
	
	// process command-line options
    static const char OPTCHARS[] = "c:d:D:t:g:h";
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
	
	// generate and save the tracks
	unsigned int i;
	trackvec_t regvecs = make_regvecs(delta, trackcnt, groupcnt);
	
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
	    out << "# Synthetic input file made by inputfiles" << std::endl;
	    
		for (unsigned int ch = 0; ch < chromcnt; ++ch)
		{
		    multovl::io::BedLinewriter blw(chroms[ch]);
		    for (regvec_t::const_iterator rit = regvecs[i].begin();
		        rit != regvecs[i].end(); ++rit
		    )
		    {
		        out << blw.write(*rit) << std::endl;
            }
		}
		out.close();
	}
	
	// add a "free.bed" as free regions for [par]multovlprob
	// this is very simple, just one region per chromosome
	// we write it 'by hand'
	boost::filesystem::path freepath = outpath / "free.bed";
    boost::filesystem::ofstream out(freepath);	    
    if (out)
    {
        const unsigned int FREEMAX = 2 * delta * trackcnt * groupcnt;
        out << "# Synthetic free regions file made by inputfiles" << std::endl;
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
    out << "\t-h|-?: print this help and exit" << std::endl;
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

static trackvec_t make_regvecs(unsigned int delta, unsigned int trackcnt, 
	unsigned int groupcnt)
{
	// generate track names
	unsigned int t;
	std::vector<std::string> tracknames;
	for (t = 1; t <= trackcnt; ++t)
		tracknames.push_back("trk" + boost::lexical_cast<std::string>(t));
	
	trackvec_t regvecs(trackcnt);
	
	unsigned int pos = delta;	// starting position
	unsigned int reglen = trackcnt * delta;	// length of one region
	for (unsigned int g = 0; g < groupcnt; g++)
	{
		for (t = 0; t < trackcnt; ++t)
		{
			regvecs[t].push_back(
				multovl::Region(pos, pos+reglen, '.', tracknames[t])
			);
			pos += delta;
		}
		pos += reglen;
	}
	
	return regvecs;
}

// == END OF PROGRAM inputfiles.cc ==
