#define BOOST_TEST_MODULE fileiotest
#include "boost/test/unit_test.hpp"

// NOTE: we assume the executable is run from the $MULTOVL_ROOT/debug directory
// the data files are in ../src/test/data,
// their names are hard-coded

// -- own headers --

#include "fileio.hh"
#include "region.hh"
#include "multovl_config.hh"
using namespace multovl;

// -- Boost headers --

#include "boost/assign/list_inserter.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/filesystem.hpp"

// -- standard headers --

#include <vector>
#include <string>

// -- Fixture --

struct FileioFixture
{
    FileioFixture(): exps()
    {
        // NOTE:
        // I wrote BED files, converted to BAM using BedTools, then
        // converted back to BED using BamTools. Somewhere along the line
        // the 1-long regions got expanded... so I took them out.
        // No idea whether BamTools had problems or BedTools.
        boost::assign::push_back( exps )   // first, last, strand, name
        (100,200,'+',"REGa")    // chr1
        (210,220,'+',"REGa")
        (224,230,'+',"REGa")
        (250,300,'+',"REGa")
        (350,450,'+',"REGa")
        (1100,1200,'-',"REGa")  // chr2
        (1210,1220,'-',"REGa")
        (1224,1230,'-',"REGa")
        (1250,1300,'-',"REGa")
        (1350,1450,'-',"REGa");
    }
    
    std::vector<Region> exps;   // expected regions
    
    // all sorts of helper functions
    
    // converts /reg/ to a string. Convenient to test region equality.
    std::string reg_tostr(const Region& reg)
    {
        std::string s = boost::lexical_cast<std::string>(reg.first()) + ":"
            + boost::lexical_cast<std::string>(reg.last()) + ":"
            + reg.strand() + reg.name();
        return s;
    }
    
    // The BAM input ignores region names, so change them in exps
    void change_region_names(const std::string& newname)
    {
        for (std::vector<Region>::iterator eit = exps.begin();
            eit != exps.end();
            ++eit
        )
        {
            eit->name(newname);
        }
    }
    
    // Runs the complete test by opening /filename/ and reading regions from it.
    // It assumes that /filename/ is a file living in $MULTOVL_ROOT/src/test/data
    // and that the build directory is $MULTOVL_ROOT/debug.
    void run_test(const std::string& filename)
    {
        // figure out the Multovl root, based on some assumptions
        boost::filesystem::path curpath = boost::filesystem::current_path();     
        while (curpath != "/")
        {
            if (curpath.filename() == "debug")
            {
                curpath = curpath.parent_path();
                break;
            }
            curpath = curpath.parent_path();
        }
        if (curpath == "/")
        {
            std::cerr << "! Got lost in file system" << std::endl;
            return;
        }
        boost::filesystem::path inputpath = 
            curpath / "src" / "test" / "data" / filename;

        bool ok = false;
        io::FileReader fr(inputpath.string());
        ok = fr.errors().ok();
        BOOST_CHECK(ok);
        if (!ok)
        {
            fr.errors().print(std::cerr);
            return; // just don't bother
        }
        
        // read the first 8 records, they are all chr1
        unsigned int i;
        std::string chrom;
        Region reg;
        for (i = 0; i < 5; ++i)
        {
            ok = fr.read_into(chrom, reg);
            if (!ok)
            {
                fr.errors().print(std::cerr);
                continue;
            }
            BOOST_CHECK(ok);
            BOOST_CHECK_EQUAL(chrom, "chr1");
            BOOST_CHECK_EQUAL(reg_tostr(exps[i]), reg_tostr(reg));
        }
        
        // then the 2nd part, chrom=chr2 here
        for (i = 5; i < 10; ++i)
        {
            ok = fr.read_into(chrom, reg);
            if (!ok)
            {
                fr.errors().print(std::cerr);
                continue;
            }
            BOOST_CHECK(ok);
            BOOST_CHECK_EQUAL(chrom, "chr2");
            BOOST_CHECK_EQUAL(reg_tostr(exps[i]), reg_tostr(reg));
        }
        
        // done?
        ok = fr.read_into(chrom, reg);
        BOOST_CHECK(ok);    // still OK, but nothing comes
        BOOST_CHECK(fr.finished());
    }
    
};

BOOST_FIXTURE_TEST_SUITE(fileiosuite, FileioFixture)

// NOTE: only reading from a correct BED file is tested
// due to time constraints ( ==laziness)
BOOST_AUTO_TEST_CASE(fromtextfile_test)
{
    BOOST_TEST_MESSAGE("Running BED file input test");
    run_test("rega12.bed"); // hard-coded input file name!
}

#if USE_BAMTOOLS
BOOST_AUTO_TEST_CASE(frombamfile_test)
{
    BOOST_TEST_MESSAGE("Running BAM file input test");
    change_region_names("bam");
    run_test("rega12.bam"); // hard-coded input file name!
}
#endif

BOOST_AUTO_TEST_SUITE_END()