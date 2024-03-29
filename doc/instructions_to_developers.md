﻿# INSTRUCTIONS FOR DEVELOPERS

## IDE setup

### NetBeans

NetBeans 6.8 and above can automatically import CMake-controlled projects.
I tried NetBeans 7.1.1 on OS X 10.6.8 and SunStudio 12.3 on Solaris 11 (X86),
which is essentially the same.

0) In Preferences, specify the CMake location because NetBeans won't find it, even 
if it is in your $PATH. Also prepare an empty build directory.
1) File: New Project...: C/C++: C/C++ Project with Existing Sources. Hit Next
2) Browse for the folder with your source files and CMakeList.txt file. 
3) For the "Select Configuration Mode", choose the Custom radio button. Hit Next
4) Configure Arguments: here you need to fill in all the `-DBOOST_ROOT, -DBAMTOOLS_ROOT,
    -DPQXX_ROOT` variables in addition to everything else NetBeans has filled in. 
5) Click "Run configure script in subfolder", and specify the empty build directory
you created in Step 0. Hit Next
6) Build Tools: may add `-jN` (N is the number of cores you'd like to use) to do parallel builds. Hit Next
7) Click through "Source Files", "Code Assistance", "Project Name and Location", then Finish

This should do it.

Problems:
1) Only a few standard targets are taken from CMake, you'd need to add e.g. unit_tests manually.
2) Git handling is clumsy, you need to select the files you'd like to commit together.
3) Code completion does not seem to be working. No on-the-fly syntax checking.
4) The silly spellchecker cannot be switched off, you must deactivate the corresponding plugin.

### Eclipse

Use the CMake Eclipse CDT generator. Run it in an out-of-source build directory. 
Example:

```bash
cmake -DCMAKE_BUILD_TYPE=Debug \
    -DBOOST_ROOT=/usr/local/boost/1.47/ \
    -DBAMTOOLS_ROOT=~/DEV/EXTERNSOFT/bamtools/current \
    -DPQXX_ROOT=/usr/local/pqxx/current \
    -G "Eclipse CDT4 - Unix Makefiles" \
    -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE \ # <== from CMake 2.8.7 on
    ../../multovl/
```

Then in Eclipse, click “File” –> “Import” –> “General” –> “Existing Projects into Workspace”.

### Xcode

Use the CMake XCode generator. Run it in an out-of-source build directory.
The usual cmake settings will suffice, just add `-G Xcode`. Then open the project with XCode.
All CMake targets are available, which is nice.

Problems:
1) XCode 4.2 told me she cannot/will not refactor C++ code. WTF?
2) Header files are not visible, you have to jump to them from the source files.
3) If the project structure is rearranged (e.g. new files are added), there's no
straightforward way to reflect this in the XCode project setup. When pulling from a remote repo,
the new files do not get included in the source groups, you have to put them in manually.
The XCode project file cannot be put under version control because it contains 
absolute pathnames.

### Visual Studio 2013

Boost 1.57, binary distribution from the Boost website. Debug libraries have the `_ITERATOR_DEBUG_LEVEL=2` vs `0` problem.
Could be fixed by locally compiling the whole lot. I made the Release version of Multovl instead.
Compile everything statically, including the runtime so that users don't have to install VS2013.
Important Cmake settings:

```
BOOST_ROOT=C:\local\boost_1_57_0
BOOST_LIBRARYDIR=C:\local\boost_1_57_0\lib32-msvc-12.0
Boost_USE_STATIC_LIBS=ON
Boost_USE_STATIC_RUNTIME=ON
```

#### 64-bit Windows version

The CMake GUI does not provide a 64-bit MSVC generator. Run CMake from the 
Visual Studio 64-bit command prompt. This you can find in the Start menu on Windows7 by
looking for the All Programs > Visual Studio 2013 > Visual Studio Tools. I made shortcuts to the 
X86 and X64 versions on the desktop.

Invoke CMake from the 64-bit VS command prompt 
in the build directory like this (substitute the `BOOST_ROOT`
and `BOOST_LIBRARYDIR` hints as appropriate): 

```
cmake -G "Visual Studio 12 Win64" -DBOOST_ROOT=C:\path\to\boost \
	-DBOOST_LIBRARYDIR=C:\path\to\boost\64-bit-libraries 
	-DCMAKE_BUILD_TYPE=Release ..
```

Then open the solution in VS. Note that even though CMake was told to generate a Release,
it will open as Debug. Change to Release configuration and then build.

#### BAMtools on Windows (V2.3.0)

For some reason, the stdint.h constants are redefined in `include/shared/bamtools_global.h`.
This causes a `typedef` redefinition conflict when compiling Multovl. Fix from Line 65
by setting `ifdef _MSC_VER` to `if 0`:

```
#  if 0 /* def _MSC_VER */
     typedef char                 int8_t;
     typedef unsigned char       uint8_t;
     typedef short               int16_t;
     typedef unsigned short     uint16_t;
     typedef int                 int32_t;
     typedef unsigned int       uint32_t;
     typedef long long           int64_t;
     typedef unsigned long long uint64_t;
#  else
#    include <stdint.h>
#  endif
```

Needs the Zlib library. See below. Once Zlib is built, do the following:

Edit `CMakeLists.txt` so that it invokes `FindZLIB`. Pass `ZLIB_ROOT` if necessary.
The edit looks like this:

```
set(ZLIB_ROOT "/Users/aszodi/Source/zlib/install" CACHE PATH "Top-level Zlib installation directory")
find_package(ZLIB REQUIRED)
if (ZLIB_FOUND)
	message(STATUS "Zlib found")
else()
	message(FATAL_ERROR "Cannot build BamTools without Zlib. Set ZLIB_ROOT properly")
endif()
include_directories(${ZLIB_INCLUDE_DIRS})
```

In `src/api/CMakeLists.txt`, comment out the following:

```
# if( WIN32 )
    # set( APILibs z ws2_32 )
# else()
    # set( APILibs z )
# endif()
```

In the `bamtools` root dir:
`mkdir x64` (this is going to be the build dir, can call it anything you like)
Then `cd` to this in the VS 64-bit command prompt, run the following CMake command:

`cmake -G "Visual Studio 12 Win64" -DCMAKE_BUILD_TYPE=Release ..`

This sets up the VS2013 64-bit solution called "BamTools". Open it in VS. It contains 11 projects.
Build the project BamTools-static, switch to Release in the GUI before!
this will put the library in `bamtools-2.3.0\lib\Release\libbamtools.lib`

#### Zlib on Windows

Download the source from `www.zlib.net`. Expand, it will create `zlib-x.y.z`.
Create a `build` and an `install`
directory at the same level. In the 64-bit Visual Studio command line:

```
cd build
cmake -G "Visual Studio 12 Win64" -DCMAKE_INSTALL_PREFIX=..\install ..\zlib-x.y.z
```

Then open the solution `build/zlib.sln`, switch to Release in VS2013, build `INSTALL`. 
This will put everything under the `install` directory which you can use later.

### Visual Studio 2015, Windows 10

Building Zlib 1.2.8, 32-bit

We build a "debug" and "release" version separately. First create the installation directories:

```
mkdir c:\local\zlib-1.2.8\debug
mkdir c:\local\zlib-1.2.8\release
```

Then in the VS command prompt:

`cd (to the level containing zlib-1.2.8)`

(make the directories build/debug, build/release)

```
cd build\debug
cmake -G "Visual Studio 14" -DCMAKE_INSTALL_PREFIX=c:\local\zlib-1.2.8\debug ..\zlib-1.2.8
```

Open the VS solution `zlib.sln`, make sure the overall settings are "Debug" and "32 bit". 
Go to the Properties of the `zlibstatic` target and make sure the C/C++ runtime switch is `/MTd`
instead of `/MDd`.
Then build the `INSTALL` target. In the debug installation directory there will be
a `lib\zlibstaticd.lib`, a `lib\zlibd.lib` and a `bin\zlibd.dll`. Remove the trailing `d`-s
from the library file basenames.

```
cd ..\release
cmake -G "Visual Studio 14" -DCMAKE_INSTALL_PREFIX=c:\local\zlib-1.2.8\release ..\zlib-1.2.8
```

(note the change in `CMAKE_INSTALL_PREFIX`!)

Open the solution again, make sure the overall settings are this time "Release" and "32 bit",
then build the `INSTALL` target. In the release installation directory there will be
a `lib\zlibstatic.lib`, a `lib\zlib.lib` and a `bin\zlib.dll`.

This beats the purpose of a multiconfig CMake setup. The idea is that when you want to 
build MULTOVL as Debug, then you pass `CMake` the `-DZLIB_ROOT=\path\to\zlib-1.2.8\debug` so that the
Debug-compiled Zlib versions are found. There must be a better way, but for now this is sufficient
to build the Multovl unit tests and the apps.

Todo: fix the Zlib compilation/installation so that the libraries are distinguished by appropriate
suffixes like the Boost libraries are.

## Coverage testing

Currently `gcov` with the Gnu compilers is supported. 
Important note: For some strange reason Apple's default LLVM G++ compiler as supplied
with Xcode 4 (tested with 4.2) does not produce the coverage files needed for analysis. 
The workaround is to use the clang++ compiler. You get a warning when constructing
a Debug build with CMake.
