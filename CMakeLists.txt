# Top-level CMake file for the Multovl tools
# 2015-03-28 Andras Aszodi
#
# USAGE: you should invoke cmake in your build directory like this:-
# cmake -DCMAKE_BUILD_TYPE=(Debug|Release) \ <-- define for Makefile targets, not for Visual Studio or XCode
#   -DBOOST_ROOT=/path/to/boost \ <-- default is "/usr/local/boost"
#   -DBoost_INCLUDE_DIR=/path/to/boost/include \ <-- optional if not found automatically, note the "Boost_..." spelling
#   -DBOOST_LIBRARYDIR=/path/to/boost/libs \ <-- optional if not found automatically
#   -DUSE_SYSTEM_ZLIB \ <-- optional, default ON under UNIX, OFF under Windows
#   -DZLIB_ROOT=/path/to/zlib <- location of Zlib, ignored if USE_SYSTEM_ZLIB is OFF
#   ..
#
# The Debug build will contain the unit tests but no INSTALL or PACKAGE targets.
# The Release build will not contain the unit tests but will provide INSTALL and PACKAGE.
# Generators for multi-configuration IDEs will enable both.
#
# Static vs shared/dynamic linking:-
#   By default everything will be linked statically. To link everything dynamically, specify
#   -DMULTOVL_USE_STATIC_LIBS:BOOL=OFF
#   You can specify dynamic linkage for each of the 3rd party libraries (not recommended)
#   by setting 
#   -D{Boost,BAMTOOLS}_USE_STATIC_LIBS:BOOL=OFF as appropriate.
#
# Optional parameters:-
#   -DTOOLCHAIN=<toolchain> for choosing C and C++ compilers, see 'cmake/toolchain.cmake'
#   -DCMAKE_CXX_COMPILER=<compiler> for using a C++ compiler other than the system default
#   -DCMAKE_C_COMPILER=<compiler> for using a C compiler other than the system default
#   -DCMAKE_INSTALL_PREFIX=/inst/path for defining the installation root,
#       makes sense only when CMAKE_BUILD_TYPE=Release
#
# You may run cmake -i which will then ask for the parameters interactively.
#
# By convention I put all the various build directories under multovl/build.
#
cmake_minimum_required(VERSION 3.20.5)

# Directory layout:-
# multovl                   <== ${CMAKE_SOURCE_DIR}: top-level
# ├── cmake                 <== extra CMake includes (finders, OS-specific hacks...)
# ├── thirdparty            <== Third-party support libraries
# │   ├── bamtools          <== BamTools V2.3.1 (AA) modified version
# │   └── zlib              <== Zlib V1.2.8 to be compiled on Windows
# ├── doc                   <== ${DOCDIR}: Documentation
# │   ├── doxygen           <== Doxygen input
# │   └── html              <== HTML docs under version control, e.g. user guide
# ├── containers            <== Containerization setup
# │   ├── docker     
# │   └── singularity
# └── src                   <== ${SRCDIR}: application main source files
#     ├── exercise          <== timing tests
#     ├── incl              <== top-level header file directory: -I this
#     │   └── multovl       <== Multovl headers (*.hh) in `multovl` namespace
#     │       ├── io        <== Input-output headers in `multovl::io` namespace
#     │       └── prob      <== Probability calculation headers in `multovl::prob` namespace
#     ├── lib               <== Multovl library implementation sources (*.cc)
#     │   ├── io            <== Input-output implementation sources
#     │   └── prob          <== Probability implementation sources
#     ├── scripts           <== ${SCRIPTS}: helper scripts
#     └── tests             <== unit tests
#         └── data          <== input files for unit tests

# Extra Find... modules, include files
set(MYCMAKE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_MODULE_PATH ${MYCMAKE_PATH})

# this must come before the project() setting
option(TOOLCHAIN "Compiler toolchain" FALSE)
if (TOOLCHAIN)
    include("${MYCMAKE_PATH}/toolchain.cmake")
endif()
project(multovl C CXX)
if(TOOLCHAIN MATCHES "oneapi")
    find_package(IntelSYCL REQUIRED) # for heterogeneous apps, etc.
endif()

# Brief system/compiler description
message(STATUS "System = ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler = ${CMAKE_CXX_COMPILER_ID}")

# C++ language standard: C++17
set(CMAKE_CXX_STANDARD 17)

# Version string for the project
# The major and minor versions need not be numbers but must begin with a digit 0..9,
# then optionally dashes plus some lowercase letters or digits
# see https://stackoverflow.com/a/47084079
file(READ "${CMAKE_SOURCE_DIR}/VERSION" ver)
string(REGEX MATCH "VERSION_MAJOR ([0-9]+[-0-9a-z]*)" _ ${ver})
set(MULTOVL_VERSION_MAJOR ${CMAKE_MATCH_1})
string(REGEX MATCH "VERSION_MINOR ([0-9]+[-0-9a-z]*)" _ ${ver})
set(MULTOVL_VERSION_MINOR ${CMAKE_MATCH_1})
set(MULTOVL_VERSION ${MULTOVL_VERSION_MAJOR}.${MULTOVL_VERSION_MINOR})

# Git revision
include(${MYCMAKE_PATH}/gitrev.cmake)

# Multi-config environments (CMAKE_BUILD_TYPE not used)
set(MULTICONFIG OFF)
if (CMAKE_GENERATOR MATCHES "Visual Studio" OR CMAKE_GENERATOR MATCHES "Xcode")
    message(STATUS "Multi-config environment: ${CMAKE_GENERATOR}")
    set(MULTICONFIG ON)
else()
    # Set a default build type if none was specified in a single-config environment
    if(NOT CMAKE_BUILD_TYPE)
      message(STATUS "Setting build type to 'Debug' as none was specified.")
      set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
      # Set the possible values of build type for cmake-gui
      set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
        "MinSizeRel" "RelWithDebInfo")
    endif()
endif()

# the source directory
# The top-level dir will be CMAKE_SOURCE_DIR because this CMakeLists.txt is there
# the "real sources" are in SRCDIR underneath
set(SRCDIR "${CMAKE_SOURCE_DIR}/src")
set(SCRIPTDIR "${SRCDIR}/scripts")
set(DOCDIR "${PROJECT_SOURCE_DIR}/doc")

# the installation directory 
set(MULTOVL_DESTDIR "multovl/${MULTOVL_VERSION}")

# so that we see what goes wrong:-
set(CMAKE_VERBOSE_MAKEFILE ON)

# platform-specific settings
include("${CMAKE_SOURCE_DIR}/cmake/platformspec.cmake")

# -- LIBRARIES WE DEPEND ON --

# The cached variables below should be set e.g. from ccmake or cmake -i or
# via -D switches

# if you want to link all extra libraries dynamically, set this to OFF
# this will set the defaults for the individual static/dynamic linker settings
set(MULTOVL_USE_STATIC_LIBS ON CACHE BOOL "Shall all Multovl programs be linked statically?")

# Visual Studio compile flags, runtime selection etc.
if (MSVC)
    if (MULTOVL_USE_STATIC_LIBS)
        replace_cflags("/MD" "/MT")
    else()
        replace_cflags("/MT" "/MD")
    endif()
endif()

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
set(BOOST_MINIMUM_VERSION "1.74.0")

find_package( Boost ${BOOST_MINIMUM_VERSION}
    COMPONENTS unit_test_framework program_options serialization
)

# list of Boost libraries needed by apps
set(MULTOVL_BOOST_LIBS ${Boost_DATE_TIME_LIBRARY} 
    ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SERIALIZATION_LIBRARY}
)

# -- Third-party libraries --

# Multovl now brings its own, slightly edited version of the BamTools library
# and the source of Zlib V1.2.8 for platforms that don't have it by default
# (most notably Windows)
# The inofficial BamTools version number is V2.3.1
set(BAMTOOLS_VERSION "2.3.1")
set(USE_SYSTEM_ZLIB ${UNIX} CACHE BOOL "Use the system Zlib if available")
set(THIRDPARTY_DIR "${CMAKE_SOURCE_DIR}/thirdparty")
include("${THIRDPARTY_DIR}/CMakeLists.txt")

# All the include directories
include_directories(
    ${SRCDIR}
    ${SRCDIR}/incl
    ${Boost_INCLUDE_DIR} # On MacOS, both seem to be needed
    ${Boost_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ${BAMTOOLS_INCLUDE_DIRS}
)

# -- END OF LIBRARIES --

# -- SOURCE TREE --

# Unit tests
if (MULTICONFIG OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(CTest)
    enable_testing()
    
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

# Doxygen documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    message(STATUS "Doxygen found, API documentation can be made")
    add_subdirectory(${DOCDIR}/doxygen)
else(DOXYGEN_FOUND)
    message(WARNING "Doxygen not found, API documentation will not be created")
endif(DOXYGEN_FOUND)

# -- INSTALLATION --

if(MULTICONFIG OR CMAKE_BUILD_TYPE STREQUAL "Release")
    # apps
    set(_applications multovl multovlprob parmultovlprob)
    install(TARGETS ${_applications}
        RUNTIME DESTINATION ${MULTOVL_DESTDIR}/bin)
    
    # libraries
    set(_libraries movl movl_prob ${BAMTOOLS_CORE_LIBRARY}) # without Zlib
    if(NOT ZLIB_FOUND)
        # not provided by system, install "our own"
        set(_libraries ${_libraries} ${ZLIB_LIBRARIES})
    endif()
    if(MULTOVL_USE_STATIC_LIBS)
        install(TARGETS  ${_libraries}
            ARCHIVE DESTINATION ${MULTOVL_DESTDIR}/lib)
    else()
        # `NAMELINK_COMPONENT` takes care of BamTools .so symlink
        install(TARGETS ${_libraries}
            LIBRARY NAMELINK_COMPONENT DESTINATION ${MULTOVL_DESTDIR}/lib)
    endif()
    
    # library headers
    install(DIRECTORY ${SRCDIR}/incl/
        DESTINATION ${MULTOVL_DESTDIR}/include
        FILES_MATCHING PATTERN "*.hh")
    
    # scripts
    # Note: install(PROGRAMS... wouldn't add the exec permissions)
    install(FILES "${SCRIPTDIR}/anctrack.awk" "${SCRIPTDIR}/bed2gff.sh" "${SCRIPTDIR}/multovltest.sh"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE 
            GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        DESTINATION ${MULTOVL_DESTDIR}/bin)
    
    # Dockerfile
    # The installation directory ${MULTOVL_DESTDIR} will be the Docker build context
    set(_dockerdir "${PROJECT_SOURCE_DIR}/containers/docker")
    install(FILES "${_dockerdir}/Dockerfile" DESTINATION ${MULTOVL_DESTDIR})
    
    # Singularity-related stuff not installed
    
    # documentation
    install(FILES "${PROJECT_SOURCE_DIR}/README.md" "${PROJECT_SOURCE_DIR}/LICENSE" 
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
    set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
    set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
    
    # Select package generators
    if(WIN32)
        set(CPACK_GENERATOR "ZIP")
        set(CPACK_SOURCE_GENERATOR "ZIP")
    else()
        # UNIX
        set(CPACK_GENERATOR "TGZ")
        set(CPACK_SOURCE_GENERATOR "TGZ")
    endif()
    
    # source package    
    set(CPACK_SOURCE_IGNORE_FILES
        build/
        debug/
        release/
        containers/singularity/sandboxes/
        \\.git
        \\.DS_Store
    )
    # Executable package
    set(CPACK_IGNORE_FILES
        ${CPACK_SOURCE_IGNORE_FILES}
        cmake/
        src/
    )
    include(CPack)
endif()
