# -- thirdparty/CMakeLists.txt --

# BamTools V2.3.0 was integrated into the Multovl source
# and edited so that it builds correctly both under UNIX and MSVC
# The inofficial version number is V2.3.1
set(BAMTOOLS_TOPLEVEL "${THIRDPARTY_DIR}/bamtools")
set(BAMTOOLS_BUILDDIR "${CMAKE_BINARY_DIR}/bamtools")

# We need the BamTools API only
# "our" version's CMake setup understands this variable
set(BAMTOOLS_API_ONLY "YES")
set(BAMTOOLS_USE_STATIC_LIBS ${MULTOVL_USE_STATIC_LIBS} 
    CACHE BOOL "Shall the BamTools libraries be linked statically?")

# BAMTOOLS_INCLUDE_DIRS => location of the BAMtools API headers, used in include_directories(...):
# the MultOvl source files include "api/XXX.h"
set(BAMTOOLS_INCLUDE_DIRS "${BAMTOOLS_TOPLEVEL}/src")
# BAMTOOLS_LIBRARY_DIRS => location of the BAMtools library, used in link_directories(...)
set(BAMTOOLS_LIBRARY_DIRS "${BAMTOOLS_BUILDDIR}")

# BamTools needs Zlib
# On UNIX platforms Zlib is almost always provided by the system
# set ZLIB_ROOT to help CMake to find it if needed
find_package(ZLIB REQUIRED)
if (NOT ZLIB_FOUND)
    # let's build it ourselves, this usually happens under Windows only
    add_subdirectory(${THIRDPARTY_DIR}/zlib ${CMAKE_BINARY_DIR}/zlib)
    if (BAMTOOLS_USE_STATIC_LIBS)
        string(REPLACE "zlib.lib" "zlibstatic.lib" ZLIB_LIBRARIES ${ZLIB_LIBRARIES})
    endif()
endif()

# build BamTools API
set(BamTools_SOURCE_DIR ${BAMTOOLS_TOPLEVEL})
add_subdirectory(${BAMTOOLS_TOPLEVEL}/src/api ${BAMTOOLS_BUILDDIR})
if (BAMTOOLS_USE_STATIC_LIBS)
    set(BAMTOOLS_CORE_LIBRARY "BamTools-static")
else()
    set(BAMTOOLS_CORE_LIBRARY "BamTools")
endif()

# BAMTOOLS_LIBRARIES => BamTools plus Zlib, used in target_link_libraries(...)
set(BAMTOOLS_LIBRARIES ${BAMTOOLS_CORE_LIBRARY} ${ZLIB_LIBRARIES})

