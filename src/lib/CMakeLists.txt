# == src/lib/CMakeLists.txt ==

# -- Multovl library --

# This library provides the basic Multovl modules
# plus the I/O and "classic" pipeline modules.

set(movlsrc
    baseregion.cc region.cc ancregion.cc errors.cc reglimit.cc
    polite.cc multioverlap.cc multovlopts.cc multiregion.cc timer.cc
    basepipeline.cc classicpipeline.cc
)
if (MULTOVL_USE_STATIC_LIBS)
	add_library(movl STATIC ${movlsrc})
else()
	add_library(movl SHARED ${movlsrc})
endif()
flag_fix(movl)

# Additional I/O components
include("io/components.cmake")

# Probability library
add_subdirectory(prob)
