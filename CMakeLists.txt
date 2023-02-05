# Top-level CMake file for the Multovl tools
# 2015-03-28 Andras Aszodi
#
# USAGE: you should invoke cmake in your build directory like this:-
# cmake -DCMAKE_BUILD_TYPE=(Debug|Release) \ <-- define for Makefile targets, not for Visual Studio or XCode
#   -DBOOST_ROOT=/path/to/boost \
#	-DBOOST_LIBRARYDIR=/path/to/boost/libs \ <-- optional if not found automatically
#   -DUSE_SYSTEM_ZLIB \ <-- optional, default ON under UNIX, OFF under Windows
#	-DZLIB_ROOT=/path/to/zlib <- location of Zlib, ignored if USE_SYSTEM_ZLIB is OFF
#   ..
#
# The Debug build will contain the unit tests but no INSTALL or PACKAGE targets.
# The Release build will not contain the unit tests but will provide INSTALL and PACKAGE.
# Generators for multi-configuration IDEs will enable both.
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
cmake_minimum_required(VERSION 3.1)

project(multovl C CXX)

# Directory layout:-
# multovl					<== ${CMAKE_SOURCE_DIR}: top-level
# ├── cmake					<== extra CMake includes (finders, OS-specific hacks...)
# ├── thirdparty			<== Third-party support libraries
# │   ├── bamtools			<== BamTools V2.3.1 (AA) modified version
# │   └── zlib				<== Zlib V1.2.8 to be compiled on Windows
# ├── doc					<== ${DOCDIR}: Documentation
# │   ├── doxygen			<== Doxygen input
# │   └── html				<== HTML docs under version control, e.g. user guide
# ├── containers            <== Containerization setup
# │   ├── docker     
# │   └── singularity
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

# C++ language standard: C++11
set(CMAKE_CXX_STANDARD 11)

# Version string for the project
set(MULTOVL_VERSION_MAJOR 1)
set(MULTOVL_VERSION_MINOR 3)

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

# extra detection scripts
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

# the installation directory 
set(MULTOVL_DESTDIR "multovl-${MULTOVL_VERSION_MAJOR}.${MULTOVL_VERSION_MINOR}")

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
    ${SRCDIR}/config
    ${Boost_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ${BAMTOOLS_INCLUDE_DIRS}
)

# -- END OF LIBRARIES --

# -- SOURCE TREE --

# we collect the app targets in this "truly global" variable
# which can be updated from subdirectories
# thanks to Robert J Maynard,
# http://stackoverflow.com/questions/4372512/global-variables-in-cmake-for-dependency-tracking
set(app_targets CACHE INTERNAL "app targets" FORCE)

if(MULTICONFIG OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(CTest)
    enable_testing()
    
    # "truly global" variables
    set(utestlibs 
        ${BAMTOOLS_LIBRARIES}
        ${Boost_DATE_TIME_LIBRARY}
        ${Boost_PROGRAM_OPTIONS_LIBRARY}
        ${Boost_REGEX_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SERIALIZATION_LIBRARY}
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    CACHE INTERNAL "unit test libs" FORCE
    )
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
if(MULTICONFIG OR CMAKE_BUILD_TYPE STREQUAL "Debug")
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

if(MULTICONFIG OR CMAKE_BUILD_TYPE STREQUAL "Release")
    # apps are installed from their respective subdirs
    # see corresponding CMakeLists.txt files
    
    # shared libraries
    if(NOT MULTOVL_USE_STATIC_LIBS)
    	install(FILES 
    		${CMAKE_BINARY_DIR}/bamtools/libbamtools.so.${BAMTOOLS_VERSION}
    		${CMAKE_BINARY_DIR}/bin/libmovl.so
    		${CMAKE_BINARY_DIR}/bin/file/libmovl_fileio.so
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE 
				GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
			DESTINATION ${MULTOVL_DESTDIR}/lib
    	)
    	# BamTools shared object symlink
		install(CODE "execute_process( \
			COMMAND ${CMAKE_COMMAND} -E create_symlink \
			${CMAKE_BINARY_DIR}/bamtools/libbamtools.so.${BAMTOOLS_VERSION} \
			${CMAKE_BINARY_DIR}/bamtools/libbamtools.so   \
			)"
		)
	endif()
    
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
    set(CPACK_SOURCE_IGNORE_FILES "build/;debug/;containers/singularity/sandboxes;release/;\\\\.DS_Store;\\\\.project;\\\\.git")
    include(CPack)
endif()
