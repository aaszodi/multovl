# Moving Multovl to C++17

## BAM library fixes

The C++17 standard library now requires const correctness from comparison objects,
i.e. `bool operator()(...) const` is the correct signature. Fixed parts of the
BAM library. Note that BAM 2.5 is already available but I stick to "my" 2.3.1-AA
modifications for the time being.
 
## Use the standard filesystem 

2024-06-08

Detect the source files that include `boost/filesystem`:

`find src -type f \( -name '*.hh' -o -name '*.cc' \) -exec fgrep -l '#include "boost/filesystem' {} \;`

Files to be modified:

```
src/tests/fileiotest.cc
src/tests/tempfile.hh
src/exercise/inputfiles.cc
src/lib/io/fileformat.cc
```

Replace `#include "boost/filesystem.hpp"` with `#include <filesystem>`
and `boost::filesystem::` with `std::filesystem`.
