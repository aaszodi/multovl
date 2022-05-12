# Multiple Overlap of Genomic Regions

by András Aszódi

_Version 1.3_

This program is licensed under the terms of the BSD Open Source license.
See the file `LICENSE` for details.

Pre-compiled binaries are provided for some popular architectures.
If you want to build Multovl from scratch, keep on reading:

## Building instructions

See `doc/html/guide.html` for installation instructions
and a detailed documentation.

### Prerequisites

You will need `cmake` Version 3.1 or above, a C++98-compliant compiler, 
and the Boost libraries (1.44 or above) to build Multovl.

### For the very impatient

Run the `quickbuild.sh` script in the top-level Multovl directory.
This will compile the package using static linkage, assuming a Linux
platform, Boost libraries installed in the usual system location.
You have to specify the installation prefix by providing a path as a command line argument.
E.g. `quickbuild.sh $HOME/foo/bar` will install the Multovl executables into
`$HOME/foo/bar/multovl-1.3/bin`.

### For the moderately impatient

By default all these external libraries will be linked statically.
To link all of them dynamically instead, specify the CMake option
`-DMULTOVL_USE_DYNAMIC_LIBS:BOOL=ON`

The basic steps if you use the default system compilers:

```
mkdir build 
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT=/path/to/boost \
    -DCMAKE_INSTALL_PREFIX=/path/to/installdir ..
make
```

Or you can spell out what make all does:

```
make apps
make doc    # this generates Doxygen documentation if you have Doxygen installed
```

And then finally:

`make install` (this you may need to run as root, depending on installdir permissions)

The tools should now be installed in `${CMAKE_INSTALL_PREFIX}/multovl-1.3/bin`.

Additional Cmake settings are listed in the top-level `CMakeLists.txt`, or you may run
`cmake -i` for interactive options, or `ccmake`.
