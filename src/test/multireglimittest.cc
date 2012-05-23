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
#define BOOST_TEST_MODULE multireglimittest
#include "boost/test/unit_test.hpp"

// -- Own headers --

#include "multireglimit.hh"
using namespace multovl;
#include "tempfile.hh"  // temporary file utility

// -- Boost headers --

#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/assign/list_of.hpp"
using namespace boost::assign;

// -- Standard headers --

#include <list>
#include <fstream>

// -- Fixture

struct MultiReglimitFixture
{
    typedef std::list<RegLimit> rllist_t;
    
    MultiReglimitFixture():
        expl()
    {}
    
    // adds a region to /mrl/, and adds 2 RegLimit entries to the internal "expected list"
    void add_region(
        MultiRegLimit& mrl,
        unsigned int first, unsigned int last, char strand, const std::string& nm,
        unsigned int trackid
    )
    {
        Region r(first, last, strand, nm);
        mrl.add(r, trackid);
        boost::shared_ptr<AncestorRegion> rp(new AncestorRegion(r, trackid));
        expl.push_back(RegLimit(rp, true));
        expl.push_back(RegLimit(rp, false));
    }
    
    // sorts the internal "expected list"
    // and then compares it to what is in mrl.reglim()
    // this works only if mrl was filled up with add_region(...) above
    void checker(const MultiRegLimit& mrl)
    {
        expl.sort();
        
        const MultiRegLimit::reglim_t& obs = mrl.reglim();
        MultiRegLimit::reglim_t::const_iterator obsit;
        rllist_t::const_iterator expit;
        for (obsit = obs.begin(), expit = expl.begin();
            obsit != obs.end() && expit != expl.end();
            ++obsit, ++expit)
        {
            
            BOOST_CHECK_EQUAL(obsit->region().to_attrstring(), expit->region().to_attrstring());
            BOOST_CHECK_EQUAL(obsit->is_first(), expit->is_first());
        }
    }
    
    rllist_t expl;
};

BOOST_FIXTURE_TEST_SUITE(multireglimitsuite, MultiReglimitFixture)

BOOST_AUTO_TEST_CASE(multireglimit_ctor_test)
{
    static const unsigned int TRACKID = 9;
    Region r(1, 5, '+', "r15");
    MultiRegLimit mrl(r, TRACKID);
    
    boost::shared_ptr<AncestorRegion> rp(new AncestorRegion(r, TRACKID));
    rllist_t expl = list_of
        (RegLimit(rp, true))(RegLimit(rp, false));
    checker(mrl);
}

BOOST_AUTO_TEST_CASE(multireglimit_test)
{
    MultiRegLimit mrl;
    add_region(mrl, 1, 5, '+', "r15", 9);
    add_region(mrl, 4, 6, '-', "r46", 17);
    checker(mrl);
    
    // copy ctor and assignment tests
    // NOTE: these are deep copies so the proper test would be
    // to modify the sources and be happy if the targets don't change
    MultiRegLimit mrl2(mrl);
    checker(mrl2);
    
    MultiRegLimit mrl3;
    add_region(mrl3, 7, 9, '-', "r79", 13);
    mrl3 = mrl;
    checker(mrl3);
}

BOOST_AUTO_TEST_CASE(multireglimit_merge_test)
{
    MultiRegLimit mrl;
    add_region(mrl, 1, 5, '+', "r15", 9);
    add_region(mrl, 4, 6, '-', "r46", 17);
    
    // the regions go into /other/, but the reglimits into the same /expl/
    MultiRegLimit other;
    add_region(other, 9, 11, '+', "r911", 19);
    add_region(other, 5, 7, '-', "r57", 37);
    
    mrl += other;
    checker(mrl);
}

BOOST_AUTO_TEST_CASE(multireglimitser_test)
{
    Tempfile tempfile;
    {
        Region r(1, 5, '+', "r15");
        MultiRegLimit mrl(r, 9);
        std::ofstream ofs(tempfile.name());
        boost::archive::text_oarchive oa(ofs);
        oa << mrl;
    }
    
    {
        std::ifstream ifs(tempfile.name());
        boost::archive::text_iarchive ia(ifs);
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

