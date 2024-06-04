# == src/CMakeLists.txt ==

# Top-level source directory for the Multovl apps

include_directories(${SRCDIR}/incl)

# Configuration library
add_subdirectory(config)

# Multovl library (libmovl.{a,so})
add_subdirectory(lib)

# Exercising/timing
add_subdirectory(${SRCDIR}/exercise exercise)

# basic set of libraries
set(MULTOVLIBS movl cfg ${MULTOVL_BOOST_LIBS})   # library order does matter in Linux/g++

# Unit tests
if (MULTICONFIG OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
    
    # Code coverage
    # Currently only GNU compiler + gcov 
    include("${CMAKE_SOURCE_DIR}/cmake/coverage.cmake")
    if(GCOV_CAPABLE_COMPILER)
        set(CMAKE_CXX_FLAGS "-g -O0 -Wall ${GCOV_COMPILER_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCOV_COMPILER_FLAGS}")
        message(STATUS "Coverage tests were set up.")
    endif()
endif()

# -- Multovl applications --

# "classic" Multovl, single-core variant
add_executable(multovl multovl.cc)
target_link_libraries(multovl ${MULTOVLIBS})
flag_fix(multovl)

# Multiple overlap probabilities (serial)
set(MULTOVPROBLIBS movl_prob ${FILEMULTOVLIBS})
add_executable(multovlprob multovlprob.cc)
target_link_libraries(multovlprob ${MULTOVLPROBLIBS})
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
    multovl_config multovl 
    multovlprob parmultovlprob)


