# == src/lib/CMakeLists.txt ==

# -- Multovl library --

# This library provides the basic Multovl modules
# plus the I/O and "classic" pipeline modules.

# Configuration setup
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.cc.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.cc
    @ONLY
)

set(movlsrc
    config.cc baseregion.cc region.cc ancregion.cc errors.cc reglimit.cc
    polite.cc multioverlap.cc multovlopts.cc classicopts.cc
    multiregion.cc timer.cc
    basepipeline.cc classicpipeline.cc
)
if (MULTOVL_USE_STATIC_LIBS)
	add_library(movl STATIC ${movlsrc})
else()
	add_library(movl SHARED ${movlsrc})
endif()
target_link_libraries(movl PUBLIC ${BAMTOOLS_LIBRARIES} ${MULTOVL_BOOST_LIBS})
flag_fix(movl)

# Additional I/O components
include("io/components.cmake")

# -- Multovl Probability library --

add_subdirectory(prob)
