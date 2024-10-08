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
#define BOOST_TEST_MODULE shuffleovltest
#include "boost/test/unit_test.hpp"

// -- Standard headers --

#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
using namespace std;

// -- Own headers --

#include "multovl/prob/shuffleovl.hh"
using namespace multovl;
using namespace multovl::prob;

// -- Fixture --

// Test class that makes the `reglims()` member function public
class TestShuffleOvl: public ShuffleOvl {
    public:
    
    explicit TestShuffleOvl(const freeregvec_t& frees):
        ShuffleOvl(frees)
    {}
    
    /// \return const access to the RegLimit multiset inside
    const reglimset_t& reglims() const { return ShuffleOvl::reglims(); }

    /// print the reglims object
    void print_reglims() const {
        for (const auto& rl : reglims()) {
            std::cout << rl.this_pos()
            << ":" << rl.other_pos() 
            <<", track=" << rl.track_id() << std::endl;
        }
    }
};

// ad-hoc storage for expected results
// basically a string vector and a method to fill it up
struct ExpectedResult
{
    // poor man's MultiRegion->string conversion
    static
    std::string to_str(unsigned int first, unsigned int last, unsigned int mult, const string& ancstr)
    {
        ostringstream s;
        s << first << ',' << last << ',' << mult
            << ',' << ancstr;
        return s.str();
    }
    void add(unsigned int first, unsigned int last, unsigned int mult, const string& ancstr)
    {
        result.push_back(to_str(first, last, mult, ancstr));
    }
    vector<string> result;
};

struct ShuffleOvlFixture
{
    ShuffleOvlFixture():
    	frees{
            Region(50,650,'.',"free1"),
            Region(690,850,'.',"free2"),
            Region(900,1400,'.',"free3")
    	}, 
    	so3(frees), rng(42),		// use fixed seed for reproducibility
    	expres()
    {
        // add regions for triple overlaps
        // Track 3 will be "shufflable"
        so3.add(Region(100, 600, '+', "REGa"), 1, false);
        so3.add(Region(200, 500, '+', "REGb"), 2, false);
        so3.add(Region(300, 400, '+', "sREGc"), 3, true);

        so3.add(Region(700, 800, '+', "REGd"), 1, false);
        so3.add(Region(700, 800, '+', "REGe"), 2, false);
        so3.add(Region(700, 800, '+', "sREGf"), 3, true);
    }
    
    freeregvec_t frees;	// free regions
    TestShuffleOvl so3;
	UniformGen rng;
	ExpectedResult expres;
};

// little utility function for result checking using the GFF output string format
// regcnt: region count returned by the find_overlaps() methods
// expres: the expected result struct
// mrs: the multiregions generated
static void check_results(
    unsigned int regcnt,
    const ExpectedResult& exp,
    const vector<MultiRegion>& mrs
)
{
    unsigned int expsize = exp.result.size();
    BOOST_CHECK_EQUAL(regcnt, expsize);
    BOOST_CHECK_EQUAL(mrs.size(), expsize);
    for (unsigned int i = 0; i < expsize; i++)
    {
        BOOST_CHECK_EQUAL(
            exp.result[i], 
            ExpectedResult::to_str(
                mrs[i].first(), mrs[i].last(),
                mrs[i].multiplicity(), mrs[i].anc_str()
            )
        );
    }
}

// Predicate to find a given RegLimit in the reglim() output
class RegLimitPred
{
public:
	RegLimitPred(unsigned int pos, bool isfirst, unsigned int trackid):
		_pos(pos),
		_isfirst(isfirst),
		_trackid(trackid)
	{}

	bool operator()(const RegLimit& rl) const
	{
		return (_pos == rl.this_pos() &&
				_isfirst == rl.is_first() &&
				_trackid == rl.track_id());
	}

private:
	unsigned int _pos;
	bool _isfirst;
	unsigned int _trackid;
};

// helper function to detect the presence of a reglimit in a reglims_t multiset
static
bool is_present(const MultiOverlap::reglimset_t& reglims,
		unsigned int first, unsigned int last, unsigned int trackid)
{
	MultiOverlap::reglimset_t::const_iterator rlcit;
	rlcit = std::find_if(reglims.begin(), reglims.end(), RegLimitPred(first, true, trackid));
	if (rlcit == reglims.end()) return false;
	rlcit = std::find_if(reglims.begin(), reglims.end(), RegLimitPred(last, false, trackid));
	return (rlcit != reglims.end());
}

BOOST_FIXTURE_TEST_SUITE(multioverlapsuite, ShuffleOvlFixture)

// shuffle test without extending the ancestor regions
BOOST_AUTO_TEST_CASE(noext_shuffle_test)
{
    expres.add(200, 299, 2, "1:REGa:+:100-600|2:REGb:+:200-500");
    expres.add(300, 400, 3, "1:REGa:+:100-600|2:REGb:+:200-500|3:sREGc:+:300-400");
    expres.add(401, 500, 2, "1:REGa:+:100-600|2:REGb:+:200-500");
    expres.add(700, 800, 3, "1:REGd:+:700-800|2:REGe:+:700-800|3:sREGf:+:700-800");

    // overlaps without reshuffling
    unsigned int regcnt = so3.find_overlaps(1, 2, 0, 0, false); // up to any overlap
    check_results(regcnt, expres, so3.overlaps());
    std::cout << "Reglims after overlapping:" << std::endl;
    so3.print_reglims();

    // now we reshuffle and overlap once
    regcnt = so3.shuffle_overlaps(rng, 1, 2, 0, 0, false);

    // check if the fixed tracks remained fixed (ID=1,2)
    const MultiOverlap::reglimset_t& reglims = so3.reglims();
    BOOST_CHECK(is_present(reglims, 100, 600, 1));
    BOOST_CHECK(is_present(reglims, 200, 500, 2));
    BOOST_CHECK(is_present(reglims, 700, 800, 1));
    BOOST_CHECK(is_present(reglims, 700, 800, 2));

    // chances are _very_ slim that the reshuffled track 3 regions stayed in place
    BOOST_CHECK(!is_present(reglims, 300, 400, 3));
    BOOST_CHECK(!is_present(reglims, 700, 800, 3));
    
    std::cout << "Reglims after reshuffling:" << std::endl;
    so3.print_reglims();
}

// shuffle test with extended AncestorRegion coordinates
BOOST_AUTO_TEST_CASE(ext_shuffle_test)
{
    static const unsigned int EXT = 20;
    expres.add(180, 279, 2, "1:REGa:+:100-600|2:REGb:+:200-500");
    expres.add(280, 420, 3, "1:REGa:+:100-600|2:REGb:+:200-500|3:sREGc:+:300-400");
    expres.add(421, 520, 2, "1:REGa:+:100-600|2:REGb:+:200-500");
    expres.add(680, 820, 3, "1:REGd:+:700-800|2:REGe:+:700-800|3:sREGf:+:700-800");

    // overlaps without reshuffling
    unsigned int regcnt = so3.find_overlaps(1, 2, 0, EXT, false); // up to any overlap
    check_results(regcnt, expres, so3.overlaps());
    std::cout << "Reglims after overlapping:" << std::endl;
    so3.print_reglims();

    // now we reshuffle and overlap once
    regcnt = so3.shuffle_overlaps(rng, 1, 2, 0, EXT, false);

    // check if the fixed tracks remained fixed (ID=1,2)
    const MultiOverlap::reglimset_t& reglims = so3.reglims();
    BOOST_CHECK(is_present(reglims, 100, 600, 1));
    BOOST_CHECK(is_present(reglims, 200, 500, 2));
    BOOST_CHECK(is_present(reglims, 700, 800, 1));
    BOOST_CHECK(is_present(reglims, 700, 800, 2));

    // chances are _very_ slim that the reshuffled track 3 regions stayed in place
    BOOST_CHECK(!is_present(reglims, 300, 400, 3));
    BOOST_CHECK(!is_present(reglims, 700, 800, 3));
    
    std::cout << "Reglims after reshuffling:" << std::endl;
    so3.print_reglims();
}

BOOST_AUTO_TEST_SUITE_END()
