# Top-level CMake file for the Multovl tools
# 2010-08-10 Andras Aszodi
#
# NOTE: you should invoke cmake in your build directory like this:-
# cmake -DCMAKE_BUILD_TYPE=(Debug|Release) \
#   -DCMAKE_CXX_COMPILER=your_favourite_compiler \
#   -DBOOST_ROOT=/path/to/boost \
#   -DBAMTOOLS_ROOT=/path/to/bamtools \
#   -DPQXX_ROOT=/path/to/pqxx \
#   -DCMAKE_INSTALL_PREFIX=/path/to/install \
#   ..
#
# Optional parameters:-
#   -DCMAKE_CXX_COMPILER=<compiler> for using a C++ compiler other than the system default
#   -DCMAKE_INSTALL_PREFIX=/inst/path for defining the installation root,
#       makes sense only when CMAKE_BUILD_TYPE=Release
#   -D{Boost,BAMTOOLS,POSTGRES,PQXX}_USE_STATIC_LIBS: if you want static linking (dynamic by default)
#
cmake_minimum_required(VERSION 2.8)
project(MULTOVL CXX)

# Directory layout:-
#    |-cmake
#    |-doc
#    | |-html
#    |
#    |-src
#      |-file
#        |-test
#      |
#      |-db
#        |-test
#      |
#      |-test
#      |-exercise
#      |-scripts

# Version string for the project
set(MULTOVL_VERSION_MAJOR 1)
set(MULTOVL_VERSION_MINOR 2)

# the source directory
# The top-level dir will be CMAKE_SOURCE_DIR because this CMakeLists.txt is there
# the "real sources" are in SRCDIR
set(SRCDIR "${CMAKE_SOURCE_DIR}/src")
set(SCRIPTDIR "${SRCDIR}/scripts")

# extra detection scripts
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

# the installation directory 
set(MULTOVL_DESTDIR "multovl-${MULTOVL_VERSION_MAJOR}.${MULTOVL_VERSION_MINOR}")

# Git revision string
include("${CMAKE_SOURCE_DIR}/cmake/gitrevision.cmake")
message(STATUS "Git revision = ${GITREVISION}")

# make sure my DEBUG macro is defined if not release (don't rely on NDEBUG)
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")

# so that we see what goes wrong:-
set(CMAKE_VERBOSE_MAKEFILE ON)

# -- LIBRARIES WE DEPEND ON --

# The cached variables below should be set e.g. from ccmake or
# via -D switches when invoking cmake directly

# Boost -- we cannot do anything without you...
set(BOOST_ROOT "/usr/local/boost" CACHE PATH "Top-level Boost directory")

# set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package( Boost COMPONENTS regex date_time filesystem system 
	unit_test_framework program_options REQUIRED)
	
# list of Boost libraries needed by apps
set(MULTOVL_BOOST_LIBS ${Boost_DATE_TIME_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} 
	${Boost_SYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY})

# we always need the Boost headers
# and the binary dir for config.hh
include_directories(
    ${SRCDIR}
    ${PROJECT_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
)

# optional BAM tools for the file-based I/O
# set(BAMTOOLS_USE_STATIC_LIBS ON)
find_package(BamTools QUIET)
if (NOT BAMTOOLS_FOUND)
    message(STATUS "BAM file format will not be supported.")
endif()

# optional Postgres database access
# set( POSTGRES_USE_STATIC_LIBS ON)
find_package(Postgres QUIET)
set(PQXX_FOUND 0)
if (POSTGRES_FOUND)
    # set( PQXX_USE_STATIC_LIBS ON)
    find_package(PQXX QUIET)
else()
    message(STATUS "Postgres database access will not be supported.")
endif()

# -- END OF LIBRARIES --

# platform-specific settings
include("${CMAKE_SOURCE_DIR}/cmake/platformspec.cmake")

# -- SOURCE TREE --

# we collect the app targets in this "truly global" variable
# which can be updated from subdirectories
# thanks to Robert J Maynard,
# http://stackoverflow.com/questions/4372512/global-variables-in-cmake-for-dependency-tracking
set(app_targets CACHE INTERNAL "app targets" FORCE)
add_subdirectory(${SRCDIR} bin)

# custom target to make all apps at once
add_custom_target(apps DEPENDS ${app_targets})

# we add a config header
# this stores, among other things, the version and the revision
# and the versions of the supporting libraries
# and the compiler data
configure_file (
  "${SRCDIR}/config.hh.in"
  "${PROJECT_BINARY_DIR}/multovl_config.hh"
)

# -- UNIT TESTS --

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    enable_testing()
    
    # another "truly global" variable
    set(unit_test_targets CACHE INTERNAL "unit test targets" FORCE)
    add_subdirectory(${SRCDIR}/test test)
    add_subdirectory(${SRCDIR}/file/test test/file)
    if (PQXX_FOUND)
        ## add_subdirectory(${SRCDIR}/db/test test/db)
    endif()
    add_custom_target(unit_tests DEPENDS ${unit_test_targets})
endif()

# -- INSTALLATION --

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # apps are installed from their respective subdirs, see there
    # (the only script so far. Note: install(PROGRAMS... wouldn't add the exec permissions)
    install(FILES "${SCRIPTDIR}/anctrack.awk" 
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE 
            GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        DESTINATION ${MULTOVL_DESTDIR}/bin)
    set(DOCDIR "${PROJECT_SOURCE_DIR}/doc")
    install(FILES "${DOCDIR}/README.txt" "${DOCDIR}/license.txt" 
        DESTINATION ${MULTOVL_DESTDIR}/doc)
    install(DIRECTORY "${DOCDIR}/html" DESTINATION ${MULTOVL_DESTDIR}/doc
        PATTERN ".svn" EXCLUDE)

    # optional Postgres example config file
    if (PQXX_FOUND)
        install(FILES "${PROJECT_SOURCE_DIR}/config/regan.cfg"
            DESTINATION ${MULTOVL_DESTDIR}/config)
    endif()
    
    # packaging: common settings
    set(CPACK_PACKAGE_VERSION_MAJOR "${MULTOVL_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${MULTOVL_VERSION_MINOR}")
    if(NOT ${SUBVERSION_REVISION} STREQUAL "0")
        set(CPACK_PACKAGE_VERSION_PATCH "${GITREVISION}")
    endif()
    set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 1)
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Multiple Overlaps of Genomic Regions")
    set(CPACK_PACKAGE_VENDOR "Andras Aszodi")
    set(CPACK_PACKAGE_DESCRIPTION_FILE "${DOCDIR}/README.txt")
    set(CPACK_RESOURCE_FILE_LICENSE "${DOCDIR}/license.txt")
    
    # executable package (no source)
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_IGNORE_FILES "cmake/;src/;debug/;release/;
        "/\\\\.hh$";"/\\\\.a$";\\\\.DS_Store;/\\\\.svn/")
    
    # source package
    set(CPACK_SOURCE_GENERATOR "TGZ")
    set(CPACK_SOURCE_IGNORE_FILES "debug/;release/;\\\\.DS_Store;/\\\\.svn/")
    include(CPack)
endif()
