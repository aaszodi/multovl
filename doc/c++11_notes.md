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
#include "boost/date_time/gregorian/gregorian.hpp"  // needed by GFF output header
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/make_shared.hpp"
#include "boost/math/special_functions/erf.hpp"
#include "boost/math/special_functions/next.hpp"    // minimal floating-point distance
#include "boost/noncopyable.hpp"
#include "boost/operators.hpp"
#include "boost/program_options.hpp"
#include "boost/progress.hpp"
#include "boost/progress.hpp"   // deprecated but pretty ASCII progress display
#include "boost/random/mersenne_twister.hpp"
#include "boost/random/normal_distribution.hpp"
#include "boost/random/uniform_real.hpp"
#include "boost/random/variate_generator.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/serialization/access.hpp"
#include "boost/serialization/library_version_type.hpp"
#include "boost/serialization/set.hpp"
#include "boost/serialization/set.hpp"  // other serialization headers come from [anc]region.hh
#include "boost/serialization/shared_ptr.hpp"
#include "boost/serialization/split_member.hpp"
#include "boost/serialization/string.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/serialization/version.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/test/floating_point_comparison.hpp"
#include "boost/test/unit_test.hpp"
#include "boost/utility.hpp"    // for noncopyable
#include "boost/version.hpp"
```

## Boost dependencies replaced by standard libraries

I move the replaced dependencies from the list above here.
This is very much "work in progress".

```
#include "boost/foreach.hpp"
#include "boost/regex.hpp"
#include "boost/thread.hpp"
#include "boost/static_assert.hpp"
#include "boost/type_traits.hpp"
```

## Boost V 1.74 deprecation messages

```
/usr/include/boost/progress.hpp:23:1: note: ‘#pragma message: This header is deprecated. Use the facilities in <boost/timer/timer.hpp> or <boost/timer/progress_display.hpp> instead.’
   23 | BOOST_HEADER_DEPRECATED( "the facilities in <boost/timer/timer.hpp> or <boost/timer/progress_display.hpp>" )
```

```
                 from /home/andras/DEV/WORK/multovl/src/file/prob/test/empirdistrtest.cc:36:
/usr/include/boost/test/floating_point_comparison.hpp:14:1: note: ‘#pragma message: This header is deprecated. Use <boost/test/tools/floating_point_comparison.hpp> instead.’
   14 | BOOST_HEADER_DEPRECATED( "<boost/test/tools/floating_point_comparison.hpp>" )
```