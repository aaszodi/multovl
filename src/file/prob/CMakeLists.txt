# == src/file/prob/CMakeLists.txt ==
# CMakeLists.txt for Multovl probability calculations, src/file/prob directory
# This tool will always be built.

include_directories(${SRCDIR} ${SRCDIR}/file ${SRCDIR}/file/prob)

# overlap probability convenience library
set(movl_probsrc
    empirdistr.cc freeregions.cc randomplacer.cc shuffleovl.cc stat.cc
     probpipeline.cc probopts.cc
)
add_library(movl_prob STATIC ${movl_probsrc})
flag_fix(movl_prob)

# Multiple overlap probabilities (serial and parallel)
set(MULTOVPROBLIBS movl_prob ${FILEMULTOVLIBS})
add_executable(multovlprob multovlprob.cc)
add_executable(parmultovlprob parmultovlprob.cc parprobpipeline.cc parprobopts.cc)

set(PROBPROGS multovlprob parmultovlprob)
foreach(PROBPROG ${PROBPROGS})
    if (PROBPROG STREQUAL "parmultovlprob" AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_link_libraries(${PROBPROG} pthread ${MULTOVPROBLIBS})
    else()
        target_link_libraries(${PROBPROG} ${MULTOVPROBLIBS})
    endif()
    flag_fix(${PROBPROG})
    
    # add to global app target list
    set(app_targets 
        ${app_targets}
        ${PROBPROG}
        CACHE INTERNAL "app targets"
    )
endforeach()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(test)
endif()

# install won't find non-toplevel targets
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # apps
    install(TARGETS ${PROBPROGS} RUNTIME DESTINATION ${MULTOVL_DESTDIR}/bin)
endif()
