#define BOOST_TEST_MODULE multioverlaptest
#include "boost/test/unit_test.hpp"

// -- Standard headers --

#include <vector>
#include <algorithm>
#include <sstream>
using namespace std;

#include "multioverlap.hh"
#include "multiregion.hh"
using namespace multovl;

struct MultovlFixture
{
    MultovlFixture() 
    {
        // regions for pairwise overlap tests (hence mo2)
        mo2.add(Region(100, 200, '+', "REGa"), 1);
        mo2.add(Region(210, 220, '+', "REGa"), 1);
        mo2.add(Region(222, 222, '+', "REGa"), 1);
        mo2.add(Region(224, 230, '+', "REGa"), 1);
        mo2.add(Region(250, 300, '+', "REGa"), 1);
        mo2.add(Region(333, 333, '+', "REGa"), 1);
        mo2.add(Region(340, 340, '+', "REGa"), 1);
        mo2.add(Region(350, 450, '+', "REGb"), 1);   // <- all other REGb's are on track 2!
        
        mo2.add(Region(50, 150, '-', "REGb"), 2);
        mo2.add(Region(223, 223, '-', "REGb"), 2);
        mo2.add(Region(250, 300, '-', "REGb"), 2);
        mo2.add(Region(340, 340, '-', "REGb"), 2);
        mo2.add(Region(400, 410, '-', "REGb"), 2);
        mo2.add(Region(415, 415, '-', "REGb"), 2);
        mo2.add(Region(420, 430, '-', "REGb"), 2);
        mo2.add(Region(440, 500, '-', "REGb"), 2);
        
        // regions for triple overlaps (hence mo3)
        mo3.add(Region(100, 600, '+', "REGa"), 1);
        mo3.add(Region(200, 500, '+', "REGb"), 2); 
        mo3.add(Region(300, 400, '+', "REGc"), 3);
    
        mo3.add(Region(700, 800, '+', "REGa"), 1);
        mo3.add(Region(700, 800, '+', "REGb"), 2);
        mo3.add(Region(700, 800, '+', "REGc"), 3);
        
        // in track 2, there is an intra-track overlap
        mo3.add(Region(1000, 1200, '-', "REGbi"), 2);
        mo3.add(Region(1100, 1300, '-', "REGbi"), 2);
    }
    
    MultiOverlap mo2, mo3;
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

BOOST_FIXTURE_TEST_SUITE(multioverlapsuite, MultovlFixture)

BOOST_AUTO_TEST_CASE(solitary_test)
{
    ExpectedResult exp;
    exp.add(210, 220, 1, "1:REGa:+:210-220"); // solitary
    exp.add(222, 222, 1, "1:REGa:+:222-222");
    exp.add(223, 223, 1, "2:REGb:-:223-223");
    exp.add(224, 230, 1, "1:REGa:+:224-230");
    exp.add(333, 333, 1, "1:REGa:+:333-333");
    
    unsigned int regcnt = mo2.find_overlaps(32799, 1, 1);    // solitaries ONLY, checks ovlen set to 1
    check_results(regcnt, exp, mo2.overlaps());
}

BOOST_AUTO_TEST_CASE(multioverlap2_test)
{
    
    // multioverlap
    ExpectedResult exp;
    exp.add(100, 150, 2, "1:REGa:+:100-200|2:REGb:-:50-150");
    exp.add(250, 300, 2, "1:REGa:+:250-300|2:REGb:-:250-300");
    exp.add(340, 340, 2, "1:REGa:+:340-340|2:REGb:-:340-340");
    exp.add(400, 410, 2, "1:REGb:+:350-450|2:REGb:-:400-410");
    exp.add(415, 415, 2, "1:REGb:+:350-450|2:REGb:-:415-415");
    exp.add(420, 430, 2, "1:REGb:+:350-450|2:REGb:-:420-430");
    exp.add(440, 450, 2, "1:REGb:+:350-450|2:REGb:-:440-500");
    
    unsigned int regcnt = mo2.find_overlaps(1, 2, 2); // checks minmult/maxmult correction
    check_results(regcnt, exp, mo2.overlaps());
}

BOOST_AUTO_TEST_CASE(unionoverlap2_test)
{
    // union overlap
    ExpectedResult exp;
    exp.add(50, 200, 2, "1:REGa:+:100-200|2:REGb:-:50-150");
    exp.add(250, 300, 2, "1:REGa:+:250-300|2:REGb:-:250-300");
    exp.add(340, 340, 2, "1:REGa:+:340-340|2:REGb:-:340-340");
    exp.add(350, 500, 2, "1:REGb:+:350-450|2:REGb:-:400-410|2:REGb:-:415-415|2:REGb:-:420-430|2:REGb:-:440-500");

    unsigned int regcnt = mo2.find_unionoverlaps(1, 2, 2);
    check_results(regcnt, exp, mo2.overlaps());
    MultiOverlap::Counter counter;
    mo2.overlap_stats(counter);
    BOOST_CHECK_EQUAL(counter.to_string(), "1,2:4");
}

BOOST_AUTO_TEST_CASE(multioverlap3_test)
{
    ExpectedResult exp;
    exp.add(200, 299, 2, "1:REGa:+:100-600|2:REGb:+:200-500");
    exp.add(300, 400, 3, "1:REGa:+:100-600|2:REGb:+:200-500|3:REGc:+:300-400");
    exp.add(401, 500, 2, "1:REGa:+:100-600|2:REGb:+:200-500");
    exp.add(700, 800, 3, "1:REGa:+:700-800|2:REGb:+:700-800|3:REGc:+:700-800");

    // overlaps without intra-track overlap
    unsigned int regcnt = mo3.find_overlaps(1, 2, 0, false); // up to any overlap
    check_results(regcnt, exp, mo3.overlaps());
    
    // intra-track overlap allowed by default
    exp.add(1100, 1200, 2, "2:REGbi:-:1000-1200|2:REGbi:-:1100-1300");
    regcnt = mo3.find_overlaps(1, 2, 0);
    check_results(regcnt, exp, mo3.overlaps());
}

BOOST_AUTO_TEST_CASE(unionoverlap3_test)
{
    ExpectedResult exp;
    exp.add(100, 600, 3, "1:REGa:+:100-600|2:REGb:+:200-500|3:REGc:+:300-400");
    exp.add(700, 800, 3, "1:REGa:+:700-800|2:REGb:+:700-800|3:REGc:+:700-800");

    vector<MultiRegion> urs;
    unsigned int regcnt = mo3.find_unionoverlaps(1, 3, 3); // 3-fold unions only
    check_results(regcnt, exp, mo3.overlaps());
}

// Here comes a rather complex test.
// Two tracks with intra-track overlaps.
// ASCII art illustration (not to scale):-
//
// 
//          /  100--------------140       200-----------------250           300---------330
// Track 1 -|      110--------------150           220-----------------270       310---------340
//          \                                                                       320---------350
// 
// Track 2 - /         120--130               210---------240
//           \                                       230---------260

BOOST_AUTO_TEST_CASE(complexintra_test)
{
    MultiOverlap mo;
    
    // Track 1
    mo.add(Region(100, 140, '+', "Track1"), 1);
    mo.add(Region(110, 150, '+', "Track1"), 1);
    mo.add(Region(200, 250, '+', "Track1"), 1);
    mo.add(Region(220, 270, '+', "Track1"), 1);
    mo.add(Region(300, 330, '+', "Track1"), 1);
    mo.add(Region(310, 340, '+', "Track1"), 1);
    mo.add(Region(320, 350, '+', "Track1"), 1);
    
    // Track 2
    mo.add(Region(120, 130, '-', "Track2"), 2);
    mo.add(Region(210, 240, '-', "Track2"), 2);
    mo.add(Region(230, 260, '-', "Track2"), 2);

    // exclude internal overlaps, allow track-pairwise only
    ExpectedResult exp;
    exp.add(120, 130, 2, "1:Track1:+:100-140|1:Track1:+:110-150|2:Track2:-:120-130");
    exp.add(210,219, 2, "1:Track1:+:200-250|2:Track2:-:210-240");
    exp.add(220,229, 2, "1:Track1:+:200-250|1:Track1:+:220-270|2:Track2:-:210-240");
    exp.add(230,240, 2, "1:Track1:+:200-250|1:Track1:+:220-270|2:Track2:-:210-240|2:Track2:-:230-260");
    exp.add(241,250, 2, "1:Track1:+:200-250|1:Track1:+:220-270|2:Track2:-:230-260");
    exp.add(251,260, 2, "1:Track1:+:220-270|2:Track2:-:230-260");
    
    // overlaps without intra-track overlap
    unsigned int regcnt = mo.find_overlaps(1, 2, 2, false); // pairwise overlap
    check_results(regcnt, exp, mo.overlaps());
}

// now relax... simple test for the MultiOverlap::Counter utility class
BOOST_AUTO_TEST_CASE(counter_test)
{
    MultiOverlap::Counter counter;
    BOOST_CHECK_EQUAL(counter.total(), 0);
    BOOST_CHECK_EQUAL(counter.to_string(), "(empty)");
    
    // triple overlaps without intra-track overlap
    // two 1,2 and two 1,2,3 overlaps
    // same as in multioverlap3_test
    unsigned int regcnt;
    regcnt = mo3.find_overlaps(1, 2, 0, false);
    BOOST_CHECK_EQUAL(regcnt, 4);
    mo3.overlap_stats(counter);
    BOOST_CHECK_EQUAL(counter.total(), 4);
    BOOST_CHECK_EQUAL(counter.to_string(), "1,2:2 1,2,3:2");
    
    // now we allow intra-track overlaps as well
    MultiOverlap::Counter counter2;
    regcnt = mo3.find_overlaps(1, 2, 0);
    BOOST_CHECK_EQUAL(regcnt, 5);
    mo3.overlap_stats(counter2);
    BOOST_CHECK_EQUAL(counter2.total(), 5);
    BOOST_CHECK_EQUAL(counter2.to_string(), "1,2:2 1,2,3:2 2:1");
    
    // test the summation
    counter += counter2;
    BOOST_CHECK_EQUAL(counter.total(), 9);
    BOOST_CHECK_EQUAL(counter.to_string(), "1,2:4 1,2,3:4 2:1");
}

BOOST_AUTO_TEST_SUITE_END()
