# Module to find the BAM tools library.
# This library supports reading and writing the BAM binary alignment format.
# Reference:-
# http://pezmaster31.github.com/bamtools
# CMake finder written by Andras Aszodi
# 2011-06-20

# This module supplies the following:-
# BAMTOOLS_FOUND => true if BAM tools is found
# BAMTOOLS_INCLUDE_DIRS => location of the BAMtools headers, used in include_directories(...)
# BAMTOOLS_LIBRARY_DIRS => location of the BAMtools library, used in link_directories(...)
# BAMTOOLS_LIBRARIES => the library plus "lz"
# these should go in target_link_libraries(...)
# Note that this module also tries to find the corresponding Zlib.
# 
# You can help if you define the BAMTOOLS_ROOT variable
# e.g. if the installation is at /foo/bar/bamtools/{include,lib}
# then specify -DBAMTOOLS_ROOT=/foo/bar/bamtools when invoking cmake

# You may set BAMTOOLS_USE_STATIC_LIBS if you want static linking

set(BAMTOOLS_FOUND 0)
find_path(BAMTOOLS_TOPLEVEL /include/api/BamReader.h ${BAMTOOLS_ROOT})
if (NOT BAMTOOLS_TOPLEVEL)
	message(STATUS "BamTools top-level directory not found")
	return()
endif()

set(BAMTOOLS_INCLUDE_DIRS ${BAMTOOLS_TOPLEVEL}/include) # use hash-include "api/XX.h"
set(BAMTOOLS_LIBRARY_DIRS ${BAMTOOLS_TOPLEVEL}/lib)

set(BAMTOOLS_CORE_NAME bamtools)
if( UNIX AND BAMTOOLS_USE_STATIC_LIBS )
	set(BAMTOOLS_CORE_NAME libbamtools.a)
endif()

# make sure Zlib is found in Unices
if (UNIX)
    find_package(ZLIB REQUIRED)
endif()

# MSVC-specific stuff, this is quite ad-hoc
if (MSVC)
	# there seems to be a Build or Release directory under lib/
	set(BAMTOOLS_LIBRARY_DIRS ${BAMTOOLS_TOPLEVEL}/lib/${CMAKE_BUILD_TYPE})
	# must find Zlib: help with ZLIB_ROOT
	set(ZLIB_ROOT "../../zlib/install" CACHE PATH "Top-level Zlib installation directory")
	find_package(ZLIB QUIET)
	if (ZLIB_FOUND)
		# link error, "cannot open input file z.lib", fix for static
		if (BAMTOOLS_USE_STATIC_LIBS)
			string(REPLACE "zlib.lib" "zlibstatic.lib" ZLIB_LIBRARIES ${ZLIB_LIBRARIES})
		endif()
		message(STATUS "Zlib found " ${ZLIB_LIBRARIES})
	else()
		message(STATUS "BamTools requires Zlib. Set ZLIB_ROOT properly")
		return()
	endif()
	include_directories(${ZLIB_INCLUDE_DIRS})
	if( BAMTOOLS_USE_STATIC_LIBS )
		set(BAMTOOLS_CORE_NAME libbamtools.lib)
	endif()
endif()

find_library(BAMTOOLS_CORE_LIBRARY NAMES ${BAMTOOLS_CORE_NAME} PATHS ${BAMTOOLS_LIBRARY_DIRS})

if (BAMTOOLS_CORE_LIBRARY)
	set(BAMTOOLS_FOUND 1)
	set(BAMTOOLS_LIBRARIES ${BAMTOOLS_CORE_LIBRARY} ${ZLIB_LIBRARIES})
	message(STATUS "BAM tools found at ${BAMTOOLS_TOPLEVEL}, libraries: ${BAMTOOLS_LIBRARIES}")
endif()
MARK_AS_ADVANCED( BAMTOOLS_FOUND )

