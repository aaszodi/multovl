# == src/file/prob/CMakeLists.txt ==
# CMakeLists.txt for Multovl probability calculations, src/file/prob directory
# This tool will always be built.

include_directories(${SRCDIR} ${SRCDIR}/file ${SRCDIR}/file/prob)

# overlap probability convenience library
set(movl_probsrc
    empirdistr.cc freeregions.cc randomplacer.cc shuffleovl.cc stat.cc
)
add_library(movl_prob STATIC ${movl_probsrc})
flag_fix(movl_prob)

# Multiple overlap probabilities
set(MULTOVPROBLIBS movl_prob ${FILEMULTOVLIBS})
add_executable(multovlprob multovlprob.cc probpipeline.cc probopts.cc)
flag_fix(multovlprob)
target_link_libraries(multovlprob ${MULTOVPROBLIBS})

# add to global app target list
set(app_targets 
    ${app_targets}
    multovlprob
    CACHE INTERNAL "app targets"
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(test)
endif()

# install won't find non-toplevel targets
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # apps
    install(TARGETS multovlprob RUNTIME DESTINATION ${MULTOVL_DESTDIR}/bin)
endif()
