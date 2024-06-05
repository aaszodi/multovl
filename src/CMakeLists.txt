# == src/CMakeLists.txt ==

# Top-level source directory for the Multovl apps

include_directories(${SRCDIR}/incl)

# Multovl library (libmovl.{a,so})
add_subdirectory(lib)

# basic set of libraries
set(MULTOVLIBS movl ${BAMTOOLS_LIBRARIES} ${MULTOVL_BOOST_LIBS})

# Unit tests
if (MULTICONFIG OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(tests)
endif()

# Exercising/timing
add_subdirectory(${SRCDIR}/exercise exercise)

# -- Multovl applications --

# "classic" Multovl, single-core variant
add_executable(multovl multovl.cc)
target_link_libraries(multovl ${MULTOVLIBS})
flag_fix(multovl)

# Multiple overlap probabilities (serial)
set(MULTOVPROBLIBS movl_prob ${MULTOVLIBS})
add_executable(multovlprob multovlprob.cc)
target_link_libraries(multovlprob ${MULTOVPROBLIBS})
flag_fix(multovlprob)

# Multiple overlap probabilities (parallel)
add_executable(parmultovlprob parmultovlprob.cc)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(parmultovlprob pthread ${MULTOVPROBLIBS})
else()
    target_link_libraries(parmultovlprob ${MULTOVPROBLIBS})
endif()
flag_fix(parmultovlprob)

# target to build all apps
add_custom_target(apps DEPENDS 
    multovl 
    multovlprob parmultovlprob)


