# - Finds OpenMP support for a C++ compiler only
# based on A.R. Brodtkorb's generic C/C++ OpenMP script
# the problem with it was that if C was not among the enabled languages,
# the script failed even though the C++ flag was found.
# I simply took out all C-related stuff and kept the C++ pieces.
# 2010-03-02 Andras Aszodi

# This module can be used to detect OpenMP support in a compiler.
# If the compiler supports OpenMP, the flags required to compile with
# openmp support are set.  
#
# The following variables are set:
#   OpenMP_CXX_FLAGS - flags to add to the CXX compiler for OpenMP support
#   OPENMP_FOUND - true if openmp is detected
#
# Supported compilers can be found at http://openmp.org/wp/openmp-compilers/


# Copyright 2008, 2009 <André Rigland Brodtkorb> Andre.Brodtkorb@ifi.uio.no
#
# Redistribution AND use is allowed according to the terms of the New 
# BSD license. 
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include(CheckCXXSourceRuns)
include(FindPackageHandleStandardArgs)

# AA: if we know the compiler, no need to go through all these tests
# limit the flag candidates
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(OpenMP_CXX_FLAG_CANDIDATES "-fopenmp")
elseif ( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")	# Microsoft Visual C++
	set(OpenMP_CXX_FLAG_CANDIDATES "/openmp")
elseif ( CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
	set(OpenMP_CXX_FLAG_CANDIDATES "-Qopenmp" "-openmp")	# windows, Unix
elseif ( CMAKE_CXX_COMPILER_ID STREQUAL "HP")
	set(OpenMP_CXX_FLAG_CANDIDATES "+Oopenmp")
elseif ( CMAKE_CXX_COMPILER_ID STREQUAL "XL")	# IBM
	set(OpenMP_CXX_FLAG_CANDIDATES "-qsmp")
elseif ( CMAKE_CXX_COMPILER_ID STREQUAL "PGI")	# Portland Group
	set(OpenMP_CXX_FLAG_CANDIDATES "-mp")
elseif ( CMAKE_CXX_COMPILER_ID STREQUAL "MIPSpro")	# SGI Irix
	set(OpenMP_CXX_FLAG_CANDIDATES "-mp")
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "SunPro" )
	# The SunStudio compilers set the OpenMP flag based on the optimization level
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(OpenMP_CXX_FLAG_CANDIDATES "-xopenmp=noopt")
    else (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(OpenMP_CXX_FLAG_CANDIDATES "-xopenmp=parallel")
    endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
else()
	set(OpenMP_CXX_FLAG_CANDIDATES "-openmp")	# ...anything else, won't work...
endif()

# sample openmp source code to test
# this should return the year part of _OPENMP if defined
# 2005 -> Version 2.5, 2008 -> Version 3.0
# we require Version >= 3.0
set(OpenMP_CXX_TEST_SOURCE 
"
#include <omp.h>
int main() { 
#ifdef _OPENMP
  if (_OPENMP/100 >= 2008)
    return 0;   // OK
  else
    return 1;   // not OK
#else
  breaks_on_purpose
#endif
}
")
# if these are set then do not try to find them again,
# by avoiding any try_compiles for the flags
if(DEFINED OpenMP_CXX_FLAGS)
  set(OpenMP_CXX_FLAG_CANDIDATES)
endif(DEFINED OpenMP_CXX_FLAGS)

# check cxx compiler
foreach(FLAG ${OpenMP_CXX_FLAG_CANDIDATES})
  set(SAFE_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
  set(CMAKE_REQUIRED_FLAGS "${FLAG}")
  # Cmake 2.6.2 doesn't understand this
  unset(OpenMP_FLAG_DETECTED CACHE)
  message(STATUS "Try OpenMP CXX flag = [${FLAG}]")
  check_cxx_source_runs("${OpenMP_CXX_TEST_SOURCE}" OpenMP_FLAG_DETECTED)
  set(CMAKE_REQUIRED_FLAGS "${SAFE_CMAKE_REQUIRED_FLAGS}")
  if(OpenMP_FLAG_DETECTED)
    set(OpenMP_CXX_FLAGS_INTERNAL "${FLAG}")
    break()
  endif(OpenMP_FLAG_DETECTED)
endforeach(FLAG ${OpenMP_CXX_FLAG_CANDIDATES})

set(OpenMP_CXX_FLAGS "${OpenMP_CXX_FLAGS_INTERNAL}"
  CACHE STRING "C++ compiler flags for OpenMP parallization")
# handle the standard arguments for find_package
find_package_handle_standard_args(OpenMP DEFAULT_MSG OpenMP_CXX_FLAGS )

mark_as_advanced(
  OpenMP_CXX_FLAGS
)
