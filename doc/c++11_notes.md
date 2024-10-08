# Moving MULTOVL to C++11

2023-02-05

Version 1.4 of MULTOVL will require C++11 language features
(V1.3 was still C++98). This transition will make some of the Boost
library dependencies obsolete.

This SO link `https://stackoverflow.com/a/8852421` provides a good overview
of which Boost libraries can be replaced by C++11 standard libraries.

## Version 1.3 Boost dependencies

Detect them with:

`find src -type f \( -name '*.hh' -o -name '*.cc' \) -exec fgrep '#include "boost/' {} \; | sort | uniq`

This is the Version 1.3/dev result on 2023-02-05:

```
#include "boost/accumulators/accumulators.hpp"
#include "boost/accumulators/statistics.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/case_conv.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/assign/list_inserter.hpp"
#include "boost/assign/list_inserter.hpp"   // for insert() ()()()...()
#include "boost/assign/list_of.hpp"
#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/math/special_functions/next.hpp"    // minimal floating-point distance
#include "boost/operators.hpp"
#include "boost/program_options.hpp"
#include "boost/progress.hpp"
#include "boost/progress.hpp"   // deprecated but pretty ASCII progress display
#include "boost/serialization/access.hpp"
#include "boost/serialization/library_version_type.hpp"
#include "boost/serialization/set.hpp"
#include "boost/serialization/set.hpp"  // other serialization headers come from [anc]region.hh
#include "boost/serialization/shared_ptr.hpp"
#include "boost/serialization/split_member.hpp"
#include "boost/serialization/string.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/serialization/version.hpp"
#include "boost/test/floating_point_comparison.hpp"
#include "boost/test/unit_test.hpp"
#include "boost/utility.hpp"    // for noncopyable
#include "boost/version.hpp"
```

## Boost dependencies replaced by standard libraries

I move the replaced dependencies from the list above here.
This is very much "work in progress".
Current state on 2023-03-11

```
#include "boost/foreach.hpp"
#include "boost/regex.hpp"
#include "boost/thread.hpp"
#include "boost/static_assert.hpp"
#include "boost/type_traits.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"  // needed by GFF output header
#include "boost/make_shared.hpp"
#include "boost/math/special_functions/erf.hpp"
#include "boost/noncopyable.hpp"
#include "boost/random/mersenne_twister.hpp"
#include "boost/random/normal_distribution.hpp"
#include "boost/random/uniform_real.hpp"
#include "boost/random/variate_generator.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
```

## Boost V 1.74 deprecation messages

```
/usr/include/boost/progress.hpp:23:1: note: ‘#pragma message: This header is deprecated. Use the facilities in <boost/timer/timer.hpp> or <boost/timer/progress_display.hpp> instead.’
   23 | BOOST_HEADER_DEPRECATED( "the facilities in <boost/timer/timer.hpp> or <boost/timer/progress_display.hpp>" )
```
2024-04-14: fixed, see below.

```
                 from /home/andras/DEV/WORK/multovl/src/file/prob/test/empirdistrtest.cc:36:
/usr/include/boost/test/floating_point_comparison.hpp:14:1: note: ‘#pragma message: This header is deprecated. Use <boost/test/tools/floating_point_comparison.hpp> instead.’
   14 | BOOST_HEADER_DEPRECATED( "<boost/test/tools/floating_point_comparison.hpp>" )
```

## Boost V 1.83 deprecation issues

The `boost/timer.hpp` header is not allowed any longer. The error can still be
converted to a warning with the following setting:

```
#include "boost/version.hpp"
#if BOOST_VERSION >= 108300
#define BOOST_TIMER_ENABLE_DEPRECATED
#endif
```
This I added to `probpipeline.cc` and `parprobpipeline.hh` as a stopgap measure
on 2024-02-04. 
**UPDATE** 2024-04-14: Replaced `#include "boost/progress.hpp"` with
`#include "boost/timer/progress_display.hpp"`, compiles fine with Boost V1.74

## Traditional `for` loops

2024-04-14

Decided to replace the `for (i=0; i<n; ++i)` - style `for` loops 
with `for( const auto&x : ctr)` loops as much as possible.

Detected with the following command:

`find src -name '*.cc' -exec egrep -l 'for.?\(.+;' {} \;`

Finds the following source files:

```
src/multioverlap.cc <-FIXED
src/multiregion.cc <-NOFIX_NEEDED
src/exercise/inputfiles.cc <-FIXED
src/errors.cc <-FIXED
src/test/multioverlaptest.cc <-NOFIX_NEEDED
src/test/baseregiontest.cc <-NOFIX_NEEDED
src/polite.cc <-FIXED
src/file/test/fileiotest.cc <-FIXED
src/file/classicpipeline.cc <-FIXED
src/file/prob/test/shuffleovltest.cc <-NOFIX_NEEDED
src/file/prob/test/empirdistrtest.cc <-NOFIX_NEEDED
src/file/prob/test/stattest.cc <-NOFIX_NEEDED
src/file/prob/test/freeregionstest.cc <-NOFIX_NEEDED
src/file/prob/probopts.cc <-NOFIX_NEEDED
src/file/prob/probpipeline.cc <-FIXED
src/file/prob/stat.cc <-FIXED
src/file/prob/shuffleovl.cc <-NOFIX_NEEDED
src/file/prob/parprobpipeline.cc <-NOFIX_NEEDED
src/file/prob/freeregions.cc <-NOFIX_NEEDED
```

Those files in which the `for` loops were updated are marked with "FIXED".
In some cases the "old-style" `for` loop is semantically correct (e.g.
because it was an integer-iteration-variable loop), the annotation above is "NOFIX_NEEDED".
