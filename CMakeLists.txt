# Top-level CMake file for the Multovl tools
# 2010-08-10 Andras Aszodi
#
# NOTE: you should invoke cmake in your build directory like this:-
# cmake -DCMAKE_BUILD_TYPE=(Debug|Release) \
#   -DCMAKE_CXX_COMPILER=your_favourite_compiler \
#   -DBOOST_ROOT=/path/to/boost \
#   -DBAMTOOLS_ROOT=/path/to/bamtools \
#   -DPQXX_ROOT=/path/to/pqxx \
#   ..
#
# Optional parameters:-
#   -DCMAKE_CXX_COMPILER=<compiler> for using a C++ compiler other than the system default
#   -DCMAKE_INSTALL_PREFIX=/inst/path for defining the installation root,
#       makes sense only when CMAKE_BUILD_TYPE=Release
#   -D{Boost,BAMTOOLS,POSTGRES,PQXX}_USE_STATIC_LIBS: if you want static linking (all ON by default)
#   -DMULTOVL_USE_DYNAMIC_LIBS: links all libraries dynamically if ON,
#       overrides the *_USE_STATIC_LIBS settings if set (unset by default)
#   Note that the Multovl-internal utility libraries are always statically linked.
#
# You may run cmake -i which will then ask for the parameters interactively.
#
# By convention I put all the various build directories under multovl/build.
#
cmake_minimum_required(VERSION 2.8)

project(multovl C CXX)

# Directory layout:-
#    multovl            <== ${CMAKE_SOURCE_DIR}: top-level
#    |-cmake            <== extra CMake includes (finders, OS-specific hacks...)
#    |-doc              <== ${DOCDIR}: Documentation
#    | |-html           <== HTML docs under version control, e.g. user guide
#    | |-doxygen        <== Doxygen input
#    |
#    |-src              <== ${SRCDIR}: generic multovl components
#      |-file           <== file-based: "classic" multovl
#      | |-test         <== unit tests for file-based multovl components (mainly I/O)
#      | |-prob         <== overlap probabilities: [par]multovlprob
#      |   |-test       <== unit tests for multovlprob components
#      |-db             <== database-enabled: pgmultovl
#      | |-test         <== unit tests for database-enabled pgmultovl components
#      |
#      |-test           <== unit tests for generic multovl components
#      |-exercise       <== timing tests
#      |-scripts        <== ${SCRIPTS}: helper scripts

# Brief system/compiler description
message(STATUS "System = ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler = ${CMAKE_CXX_COMPILER_ID}")

# Version string for the project
set(MULTOVL_VERSION_MAJOR 1)
set(MULTOVL_VERSION_MINOR 2)

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

# Git revision string
include("${CMAKE_SOURCE_DIR}/cmake/gitrevision.cmake")
message(STATUS "Git revision = ${GITREVISION}")

# make sure my DEBUG macro is defined if not release (don't rely on NDEBUG)
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG")

# so that we see what goes wrong:-
set(CMAKE_VERBOSE_MAKEFILE ON)

# -- LIBRARIES WE DEPEND ON --

# The cached variables below should be set e.g. from ccmake or cmake -i or
# via -D switches

# if you want to link all extra libraries dynamically
set(STATICON "ON")
if (MULTOVL_USE_DYNAMIC_LIBS)
    set(STATICON "OFF")
endif()

# Boost -- we cannot do anything without you...
set(BOOST_ROOT "/usr/local/boost" CACHE PATH "Top-level Boost directory")
set(Boost_USE_STATIC_LIBS ${STATICON} CACHE PATH "Shall the Boost libraries be linked statically?")
set(Boost_USE_MULTITHREADED ON)

# The minimum Boost version
set(BOOST_MINIMUM_VERSION "1.44.0")

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
# Turns out that from V1.50.0 on Boost.Thread depends on Boost.Chrono
if (Boost_VERSION VERSION_GREATER "1.49")
	list(APPEND MULTOVL_BOOST_LIBS ${Boost_CHRONO_LIBRARY})
endif()

# we always need the Boost headers
# and the binary dir for config.hh
include_directories(
    ${SRCDIR}
    ${PROJECT_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
)

# optional BAM tools for the file-based I/O
set(BAMTOOLS_ROOT "/usr/local/bamtools" CACHE PATH "Top-level BamTools directory")
set(BAMTOOLS_USE_STATIC_LIBS ${STATICON} CACHE PATH "Shall the BAM tools be linked statically?")
find_package(BamTools QUIET)
if (NOT BAMTOOLS_FOUND)
    message(STATUS "BAM file format will not be supported.")
endif()

# optional Postgres database access
set(POSTGRES_ROOT "/usr/local/pgsql" CACHE PATH "Top-level PostgreSQL directory")
set(POSTGRES_USE_STATIC_LIBS ${STATICON} CACHE PATH "Shall the Postgres libraries be linked statically?")
find_package(Postgres QUIET)
set(PQXX_FOUND 0)
if (POSTGRES_FOUND)
    set(PQXX_ROOT "/usr/local/pqxx" CACHE PATH "Top-level PQXX library directory")
    set(PQXX_USE_STATIC_LIBS ${STATICON} CACHE PATH "Shall the PQXX library be linked statically?")
    find_package(PQXX QUIET)
    if (NOT PQXX_FOUND)
        message(STATUS "Database access will not be supported because the PQXX library was not found.")
    endif(NOT PQXX_FOUND)
else(POSTGRES_FOUND)
    message(STATUS "Database access will not be supported because Postgres was not found.")
endif(POSTGRES_FOUND)

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

# we add a config header
# this stores, among other things, the version and the revision
# and the versions of the supporting libraries
# and the compiler data
configure_file (
  "${SRCDIR}/config.hh.in"
  "${PROJECT_BINARY_DIR}/multovl_config.hh"
)

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
    install(FILES "${SCRIPTDIR}/anctrack.awk" 
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

    # optional Postgres example config file
    if (PQXX_FOUND)
        install(FILES "${PROJECT_SOURCE_DIR}/config/regan.cfg"
            DESTINATION ${MULTOVL_DESTDIR}/config)
    endif()
    
    # packaging: common settings
    set(CPACK_PACKAGE_VERSION_MAJOR "${MULTOVL_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${MULTOVL_VERSION_MINOR}")
    if(NOT ${GITREVISION} STREQUAL "X")
        set(CPACK_PACKAGE_VERSION_PATCH "${GITREVCNT}")
    endif()
    set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 1)
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Multiple Overlaps of Genomic Regions")
    set(CPACK_PACKAGE_VENDOR "Andras Aszodi")
    set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.txt")
    set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
    
    # executable package (no source)
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_IGNORE_FILES "cmake/;src/;build/;debug/;release/;\\\\.project;\\\\.git;
        "/\\\\.hh$";"/\\\\.a$";\\\\.DS_Store")
    
    # source package
    set(CPACK_SOURCE_GENERATOR "TGZ")
    set(CPACK_SOURCE_IGNORE_FILES "build/;debug/;release/;\\\\.DS_Store;\\\\.project;\\\\.git")
    include(CPack)
endif()
