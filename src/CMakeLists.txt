# CMakeLists.txt for Multovl and variants, top-level source directory

# Sometimes the Intel compiler won't play nicely with the GNU-compiled boost::date_time
# needs further investigation, here's a band-aid to fix the Timer module
if(NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
    set_source_files_properties(timer.cc PROPERTIES 
        COMPILE_DEFINITIONS "CAN_USE_BOOST_DATETIME_MICROSEC_CLOCK")
endif()

# Convenience library for 'multovl' and variants
set(movlsrc
    region.cc ancregion.cc errors.cc reglimit.cc multireglimit.cc
    polite.cc multioverlap.cc multovlopts.cc multiregion.cc timer.cc
    basepipeline.cc pipeline.cc
)
add_library(movl STATIC ${movlsrc})
flag_fix(movl)

# basic set of libraries for all variants
set(MULTOVLIBS movl ${MULTOVL_BOOST_LIBS})   # library order does matter in Linux/g++

# basic module unit tests
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(test)
endif()

# multovl (the file-based variant) is always built
add_subdirectory(file)

# PQXX_FOUND implies that Postgres has also been found
if (PQXX_FOUND)
    # build pgmultovl as well
    add_subdirectory(db)
endif()

