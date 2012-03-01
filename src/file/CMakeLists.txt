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
add_library(movl_fileio STATIC ${movl_fileiosrc})
flag_fix(movl_fileio)

# overlap probability convenience library
set(movl_probsrc
    empirdistr.cc freeregions.cc randomplacer.cc shuffleovl.cc stat.cc
)
add_library(movl_prob STATIC ${movl_probsrc})
flag_fix(movl_prob)

# basic set of libraries for all variants
set(FILEMULTOVLIBS movl_fileio ${MULTOVLIBS})   # library order does matter in Linux/g++
if (BAMTOOLS_FOUND)
    list(APPEND FILEMULTOVLIBS ${BAMTOOLS_LIBRARIES})
endif()

# "classic" Multovl, single-core variant
add_executable(multovl multovl.cc classicpipeline.cc classicopts.cc)
flag_fix(multovl)
target_link_libraries(multovl ${FILEMULTOVLIBS})

# Multiple overlap probabilities
set(MULTOVPROBLIBS movl_prob ${FILEMULTOVLIBS})
add_executable(multovlprob multovlprob.cc probpipeline.cc probopts.cc)
flag_fix(multovlprob)
target_link_libraries(multovlprob ${MULTOVPROBLIBS})

# add to global app target list
set(app_targets 
    ${app_targets}
    multovl multovlprob
    CACHE INTERNAL "app targets"
)

# install won't find non-toplevel targets
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # apps
    install(TARGETS multovl multovlprob RUNTIME DESTINATION ${MULTOVL_DESTDIR}/bin)
endif()
