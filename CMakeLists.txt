# Top-level CMake file for the Multovl tools
# 2015-03-28 Andras Aszodi
#
# USAGE: you should invoke cmake in your build directory like this:-
# cmake -DCMAKE_BUILD_TYPE=(Debug|Release) \
#   -DBOOST_ROOT=/path/to/boost \
#	-DBOOST_LIBRARYDIR=/path/to/boost/libs \ # <-- optional if not found automatically
#   ..
#
# The Debug build will contain the unit tests but no INSTALL or PACKAGE targets.
# The Release build will not contain the unit tests but will provide INSTALL and PACKAGE.
#
# Static vs shared/dynamic linking:-
#	By default everything will be linked statically. To link everything dynamically, specify
#	-DMULTOVL_USE_STATIC_LIBS:BOOL=OFF
#	You can specify dynamic linkage for each of the 3rd party libraries (not recommended)
#	by setting 
#	-D{Boost,BAMTOOLS}_USE_STATIC_LIBS:BOOL=OFF as appropriate.
#
# Optional parameters:-
#   -DCMAKE_CXX_COMPILER=<compiler> for using a C++ compiler other than the system default
#	-DCMAKE_C_COMPILER=<compiler> for using a C compiler other than the system default
#   -DCMAKE_INSTALL_PREFIX=/inst/path for defining the installation root,
#       makes sense only when CMAKE_BUILD_TYPE=Release
#
# You may run cmake -i which will then ask for the parameters interactively.
#
# By convention I put all the various build directories under multovl/build.
#
cmake_minimum_required(VERSION 2.8)

project(multovl C CXX)

# Directory layout:-
# multovl					<== ${CMAKE_SOURCE_DIR}: top-level
# ├── cmake					<== extra CMake includes (finders, OS-specific hacks...)
# ├── bamtools2				<== BamTools subtree
# ├── doc					<== ${DOCDIR}: Documentation
# │   ├── doxygen			<== Doxygen input
# │   └── html				<== HTML docs under version control, e.g. user guide
# └── src					<== ${SRCDIR}: generic multovl components
#     ├── config			<== C source for the config/build information library
#     ├── exercise			<== timing tests
#     ├── file				<== file-based: "classic" multovl
#     │   ├── prob			<== overlap probabilities: [par]multovlprob
#     │   │   └── test		<== unit tests for multovlprob components
#     │   └── test			<== unit tests for file-based multovl components (mainly I/O)
#     ├── scripts			<== ${SCRIPTS}: helper scripts
#     └── test				<== unit tests for generic multovl components
#         └── data			<== input files for unit tests

# Brief system/compiler description
message(STATUS "System = ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler = ${CMAKE_CXX_COMPILER_ID}")

# Version string for the project
set(MULTOVL_VERSION_MAJOR 1)
set(MULTOVL_VERSION_MINOR 3)

# the source directory
# The top-level dir will be CMAKE_SOURCE_DIR because this CMakeLists.txt is there
# the "real sources" are in SRCDIR underneath
set(SRCDIR "${CMAKE_SOURCE_DIR}/src")
set(SCRIPTDIR "${SRCDIR}/scripts")
set(DOCDIR "${PROJECT_SOURCE_DIR}/doc")

# extra detection scripts
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

# the installation directory 
set(MULTOVL_DESTDIR "multovl-${MULTOVL_VERSION_MAJOR}.${MULTOVL_VERSION_MINOR}")

# make sure my DEBUG macro is defined if not release (don't rely on NDEBUG)
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")

# so that we see what goes wrong:-
set(CMAKE_VERBOSE_MAKEFILE ON)

# -- LIBRARIES WE DEPEND ON --

# The cached variables below should be set e.g. from ccmake or cmake -i or
# via -D switches

# if you want to link all extra libraries dynamically, set this to OFF
# this will set the defaults for the individual static/dynamic linker settings
set(MULTOVL_USE_STATIC_LIBS ON CACHE BOOL "Shall all Multovl programs be linked statically?")

# Boost
set(BOOST_ROOT "/usr/local/boost" CACHE PATH "Top-level Boost directory")
set(Boost_USE_STATIC_LIBS ${MULTOVL_USE_STATIC_LIBS} 
    CACHE BOOL "Shall the Boost libraries be linked statically?")
set(Boost_USE_MULTITHREADED ON)

# If Boost is linked statically, then the Visual Studio runtime must be static, too
if(MSVC AND Boost_USE_STATIC_LIBS)
    set(Boost_USE_STATIC_RUNTIME ON 
		CACHE BOOL "Shall the Boost libraries be linked against the static runtime?")
endif()

# The minimum Boost version
set(BOOST_MINIMUM_VERSION "1.44.0")

# Note that Boost_FOUND might be set to NOTFOUND 
# if the _optional_ library "chrono" is not found
# although it is required only from V1.50 onwards
find_package( Boost ${BOOST_MINIMUM_VERSION}
    COMPONENTS regex date_time filesystem system 
	unit_test_framework program_options serialization thread
	OPTIONAL_COMPONENTS chrono
)

# list of Boost libraries needed by apps
set(MULTOVL_BOOST_LIBS ${Boost_DATE_TIME_LIBRARY} 
    ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    ${Boost_REGEX_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY}
	${Boost_SERIALIZATION_LIBRARY} ${Boost_THREAD_LIBRARY}
)

# Cannot use the VERSION_GREATER comparison
# because FindBoost.cmake returns the 104900-style Boost_VERSION
# in some older installations
if (Boost_VERSION GREATER 104999)
    if (Boost_CHRONO_FOUND)
	    list(APPEND MULTOVL_BOOST_LIBS ${Boost_CHRONO_LIBRARY})
	else()
	    message(FATAL_ERROR "Boost Chrono needed but not found")
	endif()
else()
    message(STATUS "Boost Chrono library not required, ignore 'Could NOT find Boost' message")
endif()

# we always need the Boost headers
# and the configs
include_directories(
    ${SRCDIR}
    ${SRCDIR}/config
    ${Boost_INCLUDE_DIRS}
)

# BamTools now comes as a Multovl subtree
# The repo of AA's BamTools for is at: ssh://git@bitbucket.org:aa117sdf/bamtools2.git
# Note that its source is edited compared to the original V2.3.0
# BamTools subtree location
set(BAMTOOLS_TOPLEVEL "${CMAKE_SOURCE_DIR}/bamtools2")
set(BAMTOOLS_BUILDDIR "${BAMTOOLS_TOPLEVEL}/build")

# We need the BamTools API only
# "our" version's CMake setup understands this variable
set(BAMTOOLS_API_ONLY "YES")

set(BAMTOOLS_USE_STATIC_LIBS ${MULTOVL_USE_STATIC_LIBS} 
    CACHE BOOL "Shall the BamTools libraries be linked statically?")

# These are the variables delivered by the old `cmake/FindBamTools.cmake` include.
# During the transition we keep these variables but assign the "correct" values.
# BAMTOOLS_FOUND => true if BAM tools is found. TODO: Delete after transition
set(BAMTOOLS_FOUND 1)
# BAMTOOLS_INCLUDE_DIRS => location of the BAMtools API headers, used in include_directories(...):
# the MultOvl source files include "api/XXX.h"
set(BAMTOOLS_INCLUDE_DIRS "${BAMTOOLS_TOPLEVEL}/src")
# BAMTOOLS_LIBRARY_DIRS => location of the BAMtools library, used in link_directories(...)
set(BAMTOOLS_LIBRARY_DIRS "${BAMTOOLS_BUILDDIR}")

# make sure Zlib is found
if (UNIX)
    find_package(ZLIB REQUIRED)
endif()
# *** TODO *** MSVC

# build BamTools API
add_subdirectory(${BAMTOOLS_TOPLEVEL}/src/api ${BAMTOOLS_BUILDDIR})

if (MULTOVL_USE_STATIC_LIBS)
    set(BAMTOOLS_CORE_LIBRARY "${BAMTOOLS_LIBRARY_DIRS}/libbamtools.a") # UNIX only
else()
    set(BAMTOOLS_CORE_LIBRARY bamtools)    # UNIX only
endif()

# BAMTOOLS_LIBRARIES => the library plus "lz", used in target_link_libraries(...)
set(BAMTOOLS_LIBRARIES ${BAMTOOLS_CORE_LIBRARY} ${ZLIB_LIBRARIES})

# -- END OF LIBRARIES --

# platform-specific settings
include("${CMAKE_SOURCE_DIR}/cmake/platformspec.cmake")

# -- SOURCE TREE --

# we collect the app targets in this "truly global" variable
# which can be updated from subdirectories
# thanks to Robert J Maynard,
# http://stackoverflow.com/questions/4372512/global-variables-in-cmake-for-dependency-tracking
set(app_targets CACHE INTERNAL "app targets" FORCE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(CTest)
    enable_testing()
    
    # "truly global" variables
    set(utestlibs CACHE INTERNAL "unit test libs" FORCE)
    set(unit_test_targets CACHE INTERNAL "unit test targets" FORCE)
    
    # Code coverage
    # Currently only GNU compiler + gcov 
    include("${CMAKE_SOURCE_DIR}/cmake/coverage.cmake")
    if(GCOV_CAPABLE_COMPILER)
        set(CMAKE_CXX_FLAGS "-g -O0 -Wall ${GCOV_COMPILER_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCOV_COMPILER_FLAGS}")
        message(STATUS "Coverage tests were set up.")
    endif()
endif()

# Add the source directory and recursively everything under it
# (see respective CMakeLists.txt files therein)
add_subdirectory(${SRCDIR} bin)

# custom target to make all apps at once
add_custom_target(apps DEPENDS ${app_targets})

# ... and the unit tests
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_target(unit_tests DEPENDS ${unit_test_targets})
endif()

# Doxygen documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    message(STATUS "Doxygen found, API documentation can be made")
    add_subdirectory(${DOCDIR}/doxygen)
else(DOXYGEN_FOUND)
    message(WARNING "Doxygen not found, API documentation will not be created")
endif(DOXYGEN_FOUND)

# -- INSTALLATION --

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # apps are installed from their respective subdirs
    # see corresponding CMakeLists.txt files
    
    # scripts
    # Note: install(PROGRAMS... wouldn't add the exec permissions)
    install(FILES "${SCRIPTDIR}/anctrack.awk" "${SCRIPTDIR}/bed2gff.sh"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE 
            GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        DESTINATION ${MULTOVL_DESTDIR}/bin)
    
    # documentation
    install(FILES "${PROJECT_SOURCE_DIR}/README.txt" "${PROJECT_SOURCE_DIR}/LICENSE" 
        DESTINATION ${MULTOVL_DESTDIR}/doc)
    install(DIRECTORY "${DOCDIR}/html" DESTINATION ${MULTOVL_DESTDIR}/doc)
    if(DOXYGEN)
        install(DIRECTORY "${PROJECT_BINARY_DIR}/doc/doxygen/html" 
            DESTINATION ${MULTOVL_DESTDIR}/doc/doxygen)
    endif(DOXYGEN)

    # packaging: common settings
    set(CPACK_PACKAGE_VERSION_MAJOR "${MULTOVL_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${MULTOVL_VERSION_MINOR}")
    set(CPACK_PACKAGE_VERSION_PATCH "${GITREV_SHA1SHORT}")
    set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 1)
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Multiple Overlaps of Genomic Regions")
    set(CPACK_PACKAGE_VENDOR "Andras Aszodi")
    set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.txt")
    set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
    
    # Select package generators
    if (UNIX)
        set(CPACK_GENERATOR "TGZ")
        set(CPACK_SOURCE_GENERATOR "TGZ")
    elseif(WIN32)
        set(CPACK_GENERATOR "ZIP")
        set(CPACK_SOURCE_GENERATOR "ZIP")
    endif()
    
    # executable package (no source)
    set(CPACK_IGNORE_FILES "cmake/;src/;build/;debug/;release/;\\\\.project;\\\\.git;"/\\\\.hh$";"/\\\\.a$";\\\\.DS_Store")
    
    # source package    
    set(CPACK_SOURCE_IGNORE_FILES "build/;debug/;release/;\\\\.DS_Store;\\\\.project;\\\\.git")
    include(CPack)
endif()
