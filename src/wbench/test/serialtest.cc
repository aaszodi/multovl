#define BOOST_TEST_MODULE serialtest
#include "boost/test/unit_test.hpp"

// -- Own headers --

#include "wbench/serial.hh" // pulls in [anc]region.hh
#include "multireglimit.hh" // pulls in reglimit.hh
using namespace multovl;

// -- Standard headers --

#include <vector>
#include <algorithm>
#include <string>
#include <fstream>

// -- other Boost headers --

#include "boost/lexical_cast.hpp"
#include "boost/filesystem.hpp"
#include "boost/make_shared.hpp"

// play with the archive type
#define USE_TEXT_ARCHIVE
#if defined(USE_TEXT_ARCHIVE)
#define IARCHIVE text_iarchive
#define OARCHIVE text_oarchive
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#else
#define IARCHIVE binary_iarchive
#define OARCHIVE binary_oarchive
#include "boost/archive/binary_iarchive.hpp"
#include "boost/archive/binary_oarchive.hpp"
#endif

//// #include "boost/assign/list_inserter.hpp"   // for insert() ()()()...()

// -- Fixture

struct SerialFixture
{
    SerialFixture():
        r15(1, 5, '+', "r15"),
        a2(4, 6, '-', "a46", 9)
    {
        // some temp file
        tmpfilepath = boost::filesystem::temp_directory_path() /
            boost::filesystem::unique_path();
    }
    
    #if defined(USE_TEXT_ARCHIVE)
    void print_text_archive()
    {
        std::cout << "** RegLimit text archive:" << std::endl;
        std::ifstream ifs(tmpfilepath.string().c_str());
        std::string buf;
        while (std::getline(ifs, buf))
            std::cout << buf << std::endl;
    }
    #endif

    ~SerialFixture()
    {
        boost::filesystem::remove(tmpfilepath);
    }
    
    boost::filesystem::path tmpfilepath;
    Region r15;
    AncestorRegion a2;
};

static
std::string reg_tostr(const Region& reg)
{
    std::string s = boost::lexical_cast<std::string>(reg.first()) + ":"
        + boost::lexical_cast<std::string>(reg.last()) + ":"
        + reg.strand() + reg.name();
    return s;
}

BOOST_FIXTURE_TEST_SUITE(serialsuite, SerialFixture)

BOOST_AUTO_TEST_CASE(regionser_test)
{
    {
        std::ofstream ofs(tmpfilepath.string().c_str());
        boost::archive::OARCHIVE oa(ofs);
        oa << r15;
    }
    
    {
        std::ifstream ifs(tmpfilepath.string().c_str());
        boost::archive::IARCHIVE ia(ifs);
        Region inr;
        ia >> inr;
        BOOST_CHECK_EQUAL(reg_tostr(inr), reg_tostr(r15));
    }
}

BOOST_AUTO_TEST_CASE(ancregionser_test)
{
    {
        std::ofstream ofs(tmpfilepath.string().c_str());
        boost::archive::OARCHIVE oa(ofs);
        oa << a2;
    }
    
    {
        std::ifstream ifs(tmpfilepath.string().c_str());
        boost::archive::IARCHIVE ia(ifs);
        AncestorRegion inar;
        ia >> inar;
        std::string out = inar.to_attrstring();
        BOOST_CHECK_EQUAL(out, "9:a46:-:4-6");
    }
}

BOOST_AUTO_TEST_CASE(reglimitser_test)
{
    {
        // create 2 RegLimit objects referring to the same ancregion
        boost::shared_ptr<AncestorRegion> a2p = boost::make_shared<AncestorRegion>(a2);
        RegLimit rlf(a2p, true), rll(a2p, false);
        std::ofstream ofs(tmpfilepath.string().c_str());
        boost::archive::OARCHIVE oa(ofs);
        oa << rlf << rll;
    }
    
    #if defined(USE_TEXT_ARCHIVE)
        print_text_archive();
    #endif
    
    {
        std::ifstream ifs(tmpfilepath.string().c_str());
        boost::archive::IARCHIVE ia(ifs);
        RegLimit inrlf, inrll;
        ia >> inrlf >> inrll;
        
        // The raw pointers should be the same...
        BOOST_CHECK_EQUAL(inrlf.raw_region_ptr(), inrll.raw_region_ptr());
        
        // first & last
        BOOST_CHECK_EQUAL(inrlf.region().to_attrstring(), "9:a46:-:4-6");
        BOOST_CHECK(inrlf.is_first());
        BOOST_CHECK_EQUAL(inrll.region().to_attrstring(), "9:a46:-:4-6");
        BOOST_CHECK(!inrll.is_first());
    }
}

BOOST_AUTO_TEST_CASE(multireglimitser_test)
{
    {
        MultiRegLimit mrl(r15, 9);
        std::ofstream ofs(tmpfilepath.string().c_str());
        boost::archive::OARCHIVE oa(ofs);
        oa << mrl;
    }
    
    {
        std::ifstream ifs(tmpfilepath.string().c_str());
        boost::archive::IARCHIVE ia(ifs);
        MultiRegLimit inmrl;
        ia >> inmrl;
        
        MultiRegLimit::reglim_t reglim = inmrl.reglim();
        BOOST_CHECK_EQUAL(reglim.size(), 2);
        MultiRegLimit::reglim_t::const_iterator rlit = reglim.begin();
        BOOST_CHECK_EQUAL(rlit->region().to_attrstring(), "9:r15:+:1-5");
        BOOST_CHECK(rlit->is_first());
        ++rlit;
        BOOST_CHECK_EQUAL(rlit->region().to_attrstring(), "9:r15:+:1-5");
        BOOST_CHECK(!rlit->is_first());
    }
}

BOOST_AUTO_TEST_SUITE_END()
