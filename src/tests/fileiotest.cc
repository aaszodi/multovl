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
#define BOOST_TEST_MODULE fileiotest
#include "boost/test/unit_test.hpp"

// NOTE: we assume the executable is run from the $MULTOVL_ROOT/debug directory
// the data files are in ../src/test/data,
// their names are hard-coded

// -- own headers --

#include "fileio.hh"
#include "baseregion.hh"
#include "thirdparty.h"
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
        // This is the expected content of the "good" files.
        // NOTE: I wrote BED files, converted to BAM using BedTools, then
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
    
    std::vector<BaseRegion> exps;   // expected regions
    
    // all sorts of helper functions
    
    // converts /reg/ to a string. Convenient to test region equality.
    std::string reg_tostr(const BaseRegion& reg)
    {
        std::string s = boost::lexical_cast<std::string>(reg.first()) + ":"
            + boost::lexical_cast<std::string>(reg.last()) + ":"
            + reg.strand() + reg.name();
        return s;
    }
    
    // The BAM input ignores region names, so change them in exps
    void change_region_names(const std::string& newname)
    {
        for (auto& ex : exps) {
            ex.name(newname);
        }
    }
    
    // figure out the Multovl root, based on the assumption
    // that /filename/ is a file living in multovl/src/test/data
    // and that the build directory is somewhere below multovl as well
    // and then generate the full path to /filename/
    std::string locate_testfile(const std::string& filename)
    {
        boost::filesystem::path curpath = boost::filesystem::current_path();     
        while (curpath != "/")
        {
            if (curpath.filename() == "multovl")
            {
                break;
            }
            curpath = curpath.parent_path();
        }
        if (curpath == "/")
        {
            BOOST_ERROR("! Got lost in file system");
            // ...won't return...
        }
        boost::filesystem::path inputpath = 
            curpath / "src" / "test" / "data" / filename;
        return inputpath.string();
    }
    
    // Runs the complete test by opening /filename/ and reading regions from it.
    // It assumes that the input file is syntactically correct.
    void test_goodfile(const std::string& filename)
    {
        std::string inputname = locate_testfile(filename);
        bool ok = false;
        io::FileReader fr(inputname);   // noncopyable, must make here...
        ok = fr.errors().ok();
        BOOST_CHECK(ok);
        if (!ok)
        {
            fr.errors().print(std::cerr);
            return; // just don't bother
        }
        
        // read the first 5 records, they are all chr1
        unsigned int i;
        std::string chrom;
        BaseRegion reg;
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
// ...but see below, 2012-01-11...
BOOST_AUTO_TEST_CASE(fromtextfile_test)
{
    BOOST_TEST_MESSAGE("Running good BED file input test");
    test_goodfile("rega12.bed"); // hard-coded input file name!
}

BOOST_AUTO_TEST_CASE(frombamfile_test)
{
    if (config_have_bamtools()) {
        BOOST_TEST_MESSAGE("Running BAM file input test");  // these are expected to be OK
        change_region_names("bam");
        test_goodfile("rega12.bam"); // hard-coded input file name!
    } else {
        BOOST_TEST_MESSAGE("BAM file input test skipped");
        BOOST_CHECK(true);
    }
}

// NOTE: Johanna was right (again)
// the bad BED file needs to be tested, too!
BOOST_AUTO_TEST_CASE(badbed_test)
{
    BOOST_TEST_MESSAGE("Running bad BED file input test");
    std::string inputname = locate_testfile("bad.bed");
    bool ok = false;
    io::FileReader fr(inputname);   // noncopyable, must make here...
    ok = fr.errors().ok();
    BOOST_CHECK(ok);
    if (!ok)
    {
        fr.errors().print(std::cerr);
        return; // just don't bother
    }
    
    std::string chrom;
    BaseRegion reg;
    // the errors tested here must correspond to bad.bed's content
    std::vector<std::string> experrs;
    boost::assign::push_back( experrs )
        ("ERROR: At line 2: \"-100\": must not be negative")
        ("ERROR: At line 3: \"-220\": must not be negative")
        ("ERROR: At line 4: \"foo\": cannot parse to unsigned int")
        ("ERROR: At line 5: Too few fields: 2");
        
    for (unsigned int i = 0; i < 4; ++i)
    {
        ok = fr.read_into(chrom, reg);
        BOOST_CHECK(!ok);
        BOOST_CHECK_EQUAL(fr.errors().last_error(), experrs[i]);
    }
    
    // the last line is correct
    ok = fr.read_into(chrom, reg);
    BOOST_CHECK(ok);
    
    // ...and the whole thing should be finished by now
    ok = fr.read_into(chrom, reg);
    BOOST_CHECK(ok);
    BOOST_CHECK(fr.finished());
}

BOOST_AUTO_TEST_CASE(badformat_test)
{
	std::string inputname = "unknown_format.xyz";	// format recognition is extension-based
    bool ok = false;
    io::FileReader fr(inputname);   // noncopyable, must make here...
    ok = fr.errors().ok();
    BOOST_CHECK(!ok);
    BOOST_CHECK_EQUAL(fr.errors().last_error(), 
        "ERROR: unknown_format.xyz: Cannot deduce format from extension: use " +
        io::Fileformat::known_extensions()
    );
}

BOOST_AUTO_TEST_SUITE_END()
