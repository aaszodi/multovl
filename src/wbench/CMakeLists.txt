# == src/wbench/CMakeLists.txt ==

# CMakeLists.txt for Multovl workbench with serialization, src/wbench directory
# This tool will always be built.

# Boost-serializable movl lib
set(movl_wbsrc 
    wbregion.cc
)
if (BAMTOOLS_FOUND)
    include_directories(
        ${BAMTOOLS_INCLUDE_DIRS}
    )
    list(APPEND movl_wbsrc ${SRCDIR}/file/bamio.cc)
endif()
add_library(movl_wb STATIC ${movl_wbsrc})
flag_fix(movl_wb)

### we're not here yet!!!
if(0)

# basic set of libraries for all variants
set(WBMULTOVLIBS movl_wb)   # library order does matter in Linux/g++
if (BAMTOOLS_FOUND)
    list(APPEND WBMULTOVLIBS ${BAMTOOLS_LIBRARIES})
endif()

add_executable(wbmultovl wbmultovl.cc wbpipeline.cc wbopts.cc)
flag_fix(multovl)
target_link_libraries(wbmultovl ${WBMULTOVLIBS})

# add to global app target list
set(app_targets 
    ${app_targets}
    wbmultovl
    CACHE INTERNAL "app targets"
)

# install won't find non-toplevel targets
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # apps
    install(TARGETS wbmultovl RUNTIME DESTINATION ${MULTOVL_DESTDIR}/bin)
endif()

endif() ### Not here yet!!!