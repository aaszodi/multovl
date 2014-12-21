# == src/file/CMakeLists.txt ==
# CMakeLists.txt for file-based Multovl, src/file directory
# This tool will always be built.

# File I/O convenience library
set(movl_fileiosrc 
    fileformat.cc fileio.cc textio.cc linereader.cc linewriter.cc
)
if (BAMTOOLS_FOUND)
    include_directories(
        ${BAMTOOLS_INCLUDE_DIRS}
    )
    list(APPEND movl_fileiosrc bamio.cc)
endif()
if (MULTOVL_USE_STATIC_LIBS)
	add_library(movl_fileio STATIC ${movl_fileiosrc})
else()
	add_library(movl_fileio SHARED ${movl_fileiosrc})
endif()
flag_fix(movl_fileio)

# basic set of libraries for all variants
set(FILEMULTOVLIBS movl_fileio ${MULTOVLIBS})   # library order does matter in Linux/g++
if (BAMTOOLS_FOUND)
    list(APPEND FILEMULTOVLIBS ${BAMTOOLS_LIBRARIES})
endif()

# "classic" Multovl, single-core variant
add_executable(multovl multovl.cc classicpipeline.cc classicopts.cc)
target_link_libraries(multovl ${FILEMULTOVLIBS})
add_dependencies(multovl always_rebuild)
flag_fix(multovl)

# add to global app target list
set(app_targets 
    ${app_targets}
    multovl
    CACHE INTERNAL "app targets"
)

# install won't find non-toplevel targets
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # apps
    install(TARGETS multovl RUNTIME DESTINATION ${MULTOVL_DESTDIR}/bin)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(test)
endif()

# overlap probabilities
add_subdirectory(prob)
