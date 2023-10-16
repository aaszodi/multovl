# -- src/CMakeLists.txt --
# for Multovl and variants, top-level source directory

# Configuration library
add_subdirectory(config)

# Sometimes the Intel compiler won't play nicely with the GNU-compiled boost::date_time
# needs further investigation, here's a band-aid to fix the Timer module
if(NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
    set_source_files_properties(timer.cc PROPERTIES 
        COMPILE_DEFINITIONS "CAN_USE_BOOST_DATETIME_MICROSEC_CLOCK")
endif()

# Convenience library for 'multovl' and variants
set(movlsrc
    baseregion.cc region.cc ancregion.cc errors.cc reglimit.cc
    polite.cc multioverlap.cc multovlopts.cc multiregion.cc timer.cc
    basepipeline.cc pipeline.cc
)
if (MULTOVL_USE_STATIC_LIBS)
	add_library(movl STATIC ${movlsrc})
else()
	add_library(movl SHARED ${movlsrc})
endif()
flag_fix(movl)

# basic set of libraries for all variants
set(MULTOVLIBS movl cfg ${MULTOVL_BOOST_LIBS})   # library order does matter in Linux/g++

# basic module unit tests
if (MULTICONFIG OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(test)
endif()

# classic multovl (the file-based variant) is always built
add_subdirectory(file)

# Exercising/timing, always built
add_subdirectory(${SRCDIR}/exercise exercise)
