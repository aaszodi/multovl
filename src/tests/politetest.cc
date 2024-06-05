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
#define BOOST_TEST_MODULE politetest
#include "boost/test/unit_test.hpp"

// -- Standard headers --

#include <string>
using namespace std;

// -- Boost headers --

#include "boost/lexical_cast.hpp"

// -- Library headers --

#include "multovl/polite.hh"
using namespace multovl;

// -- Derived classes --

// Because Polite is abstract, we need test classes.
// Both simple and multiple (virtual) inheritance is tested.

// simple inheritance
class DerPolite: public Polite
{
    public:
    
    DerPolite(): Polite(), _v(3)
    {
        add_option<unsigned int>("val", &_v, 3, "uint value, must be >= 3", 'v');
        add_bool_switch("switch", &_s, "Boolean switch", 's');
    }
    
    unsigned int v() const { return _v; }
    bool s() const { return _s; }
    
    /// make option_seen() publicly available & hence testable
    bool optseen(const std::string& param) const { return option_seen(param); }
    
    protected:
    
    virtual
    bool check_variables()
    {
        if (option_seen("val")) // non-mandatory
        {
            unsigned int _v = fetch_value<unsigned int>("val");
            if (_v < 3) 
            {
                add_error("val must be >= 3");
                return false;
            }
        }
        return true;
    }
    
    private:

    unsigned int _v; // some value
    bool _s;
    
};  // class DerPolite

// complex: diamond inheritance
class Aopt: public virtual Polite
{
    public:
    
    Aopt(): Polite("A options help"), _a(3)
    {
        add_option<int>("aaa", &_a, 3, "integer value, must be >= 3", 'a');
    }
    
    int a() const { return _a; }
    
    virtual
    ostream& print_version(ostream& out) const
    {
        Aopt::version_info(out);
        return Polite::version_info(out);
    }

    protected:
    
    virtual
    bool check_variables()
    {
        if (option_seen("aaa"))
        {
            int _a = fetch_value<int>("aaa");
            if (_a < 3) 
            {
                add_error("aaa must be >= 3");
                return false;
            }
            return true;
        }
        return false;
    }

    virtual
    ostream& version_info(ostream& out) const
    {
        out << "Version A" << endl;
        return out;
    }
    
    private:
    
    int _a;
};  // class Aopt

class Bopt: public virtual Polite
{
    public:
    
    Bopt(): Polite("B options help"), _b(4)
    {
        add_option<int>("bbb", &_b, 4, "integer value, must be >= 4", 'b');
    }
    
    int b() const { return _b; }
    
    virtual
    ostream& print_version(ostream& out) const
    {
        Bopt::version_info(out);
        return Polite::version_info(out);
    }

    protected:
    
    virtual
    bool check_variables()
    {
        if (option_seen("bbb"))
        {
            int _b = fetch_value<int>("bbb");
            if (_b < 4) 
            {
                add_error("bbb must be >= 4");
                return false;
            }
            return true;
        }
        return false;
    }

    virtual
    ostream& version_info(ostream& out) const
    {
        out << "Version B" << endl;
        return out;
    }
    
    private:
    
    int _b;
};  // class Bopt

class Copt: public Aopt, public Bopt
{
    public:
    
    Copt(): Polite("C options help"), Aopt(), Bopt()
    {
        add_option<int>("ccc", &_c, 5, "integer value, must be >= 5", 'c');
    }
    
    int c() const { return _c; }
    
    virtual
    ostream& print_version(ostream& out) const
    {
        Copt::version_info(out);
        Aopt::version_info(out);
        Bopt::version_info(out);
        return Polite::version_info(out);
    }

    protected:
    
    virtual
    bool check_variables()
    {
        bool oka = Aopt::check_variables(),
            okb = Bopt::check_variables();  // need side effects in both
        
        if (option_seen("ccc"))
        {
            int _c = fetch_value<int>("ccc");
            if (_c < 5) 
            {
                add_error("ccc must be >= 5");
                return false;
            }
            return oka && okb;
        }
        return false;
    }

    virtual
    ostream& version_info(ostream& out) const
    {
        out << "Version C" << endl;
        return out;
    }
    
    private:
    
    int _c;
};  // class Copt

// gets rid of annoying "deprecated conversion from string constant blah blah" warning (StackOverflow tip)
#pragma GCC diagnostic ignored "-Wwrite-strings"

BOOST_AUTO_TEST_CASE(simple_test)
{
    const int ARGC = 4;
    char *ARGV[] = { "politetest", "-v", "4", "--switch" };
    
    DerPolite dp;
    bool ok = dp.parse_check(ARGC, ARGV);
    BOOST_CHECK(ok);
    BOOST_CHECK_EQUAL(dp.v(), 4);
    BOOST_CHECK(dp.optseen("val"));   // -v option was given
    BOOST_CHECK(dp.s());
    
    const int ARGC2 = 3;
    char *ARGV2[] = { "politetest", "-v", "2" };
    DerPolite dp2;  // need new, can't parse twice :-(
    ok = dp2.parse_check(ARGC2, ARGV2);
    BOOST_CHECK(!ok);
    BOOST_CHECK(dp2.optseen("val"));   // -v option was given
    BOOST_CHECK_EQUAL(dp2.error_messages(), "ERROR: val must be >= 3\n");

    const int ARGC3 = 2;
    char *ARGV3[] = { "politetest", "-s" };
    DerPolite dp3;  // need new again...
    ok = dp3.parse_check(ARGC3, ARGV3);
    BOOST_CHECK(ok);
    BOOST_CHECK(!dp3.optseen("val"));   // -v option was not given
    BOOST_CHECK(dp3.optseen("switch")); // -s option was given
    BOOST_CHECK(!dp3.optseen("nosuch")); // some bogus option...
}

BOOST_AUTO_TEST_CASE(diamond_test)
{
    const int ARGC = 7;
    char *ARGV[] = { "politetest", "-a", "6", "--bbb", "7", "--ccc", "8" };
    
    Copt cp;
    bool ok = cp.parse_check(ARGC, ARGV);
    BOOST_CHECK(ok);
    BOOST_CHECK_EQUAL(cp.a(), 6);
    BOOST_CHECK_EQUAL(cp.b(), 7);
    BOOST_CHECK_EQUAL(cp.c(), 8);
    
    // not a real test
    cerr << "=== Help and version string tests ===" << endl;
    Aopt ap;
    ap.print_help(cerr);
    ap.print_version(cerr);
    
    Bopt bp;
    bp.print_help(cerr);
    bp.print_version(cerr);
    
    cp.print_help(cerr);
    cp.print_version(cerr);
    
}

#if 0
BOOST_AUTO_TEST_CASE(parse_test)
{
    const int ARGC = 11;
    char *ARGV[] = {"prg", "-H","somehost","--user","Joe","-W","pwd","-D","db","--port","55432"};
    
    // this is the most generic setup, must specify user and pwd
    DbOpts opts(DbOpts::OTHER);
    bool retval = opts.parse(ARGC, ARGV);
    BOOST_CHECK(retval);
    BOOST_CHECK_EQUAL(opts.host(), "somehost");
    BOOST_CHECK_EQUAL(opts.user(), "Joe");
    BOOST_CHECK_EQUAL(opts.password(), "pwd");
    BOOST_CHECK_EQUAL(opts.database(), "db");
    BOOST_CHECK(opts.port() == 55432);
    BOOST_CHECK_EQUAL(opts.conn_str(), "host=somehost dbname=db user=Joe password=pwd port=55432");
    
    // readwrite user: needs password
    DbOpts rwopts(DbOpts::READWRITE);
    char *RWARGV[] = {"prg", "-H","somehost","-W","rwpwd","-D","db","--port","55432"};
    retval = rwopts.parse(9, RWARGV);
    BOOST_CHECK_EQUAL(rwopts.user(), "readwrite");
    BOOST_CHECK_EQUAL(rwopts.password(), "rwpwd");
    
    // readonly user, no password needed
    DbOpts roopts;    // default DbOpts::READONLY
    char *ROARGV[] = {"prg", "-H","somehost","-D","db","--port","55432"};
    retval = roopts.parse(7, ROARGV);
    BOOST_CHECK_EQUAL(roopts.user(), "readonly");
    BOOST_CHECK_EQUAL(roopts.password(), "rpass");
}
#endif
#pragma GCC diagnostic pop

