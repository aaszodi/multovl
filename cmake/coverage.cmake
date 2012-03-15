# This script sets up coverage tests for the "Debug" build type.
# It looks for a coverage tool,
# and if it is found, then it sets the compiler flags
# necessary for producing coverage-instrumented executables.
# The current logic is heavily biased towards the GNU compiler and 'gcov'.
# 2012-03-14 Andras Aszodi

# -- GCOV --

# Variables that are set:
# GCOV_CAPABLE_COMPILER: TRUE if the compiler supports gcov
# CTEST_COVERAGE_COMMAND: /usr/bin/gcov or equivalent if gcov is installed, otherwise NOTFOUND
# GCOV_COMPILER_FLAGS: flags needed for coverage analysis, "--coverage" for GNU C++

set(GCOV_CAPABLE_COMPILER FALSE)
set(GCOV_COMPILER_FLAGS "")
find_program(CTEST_COVERAGE_COMMAND NAMES gcov)
if (NOT CTEST_COVERAGE_COMMAND)
    message(STATUS "No gcov found")
    return()
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # Starting with Apple's XCode 4 (?), the stock llvm-g++ does NOT support coverage
    # only "clang++" does
    # we need to figure out the XCode version
    execute_process(
        COMMAND xcodebuild -version
        COMMAND awk "BEGIN { FS=\"[ .]\" } /Xcode/ { print $2 }"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE XCODEVER
        ERROR_VARIABLE XCODEVER_error
        RESULT_VARIABLE XCODEVER_result
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if (NOT(XCODEVER_result EQUAL 0))
        message(STATUS "Could not figure out XCode version, 4 assumed")
        message(STATUS "[The problem was ${XCODEVER_error}]")
        set(XCODEVER 4)
    endif()
    
    if (XCODEVER EQUAL 4)
        # Only clang++ can be used
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            set(GCOV_CAPABLE_COMPILER TRUE)
        else()
            message(STATUS "NOTE: The default llvm-g++ in XCode 4 cannot do coverage analysis.")
            message(STATUS "NOTE: Use clang++ instead and/or complain to Apple!")
        endif()
    else()
        # GNU and clang can be used
        if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            set(GCOV_CAPABLE_COMPILER TRUE)
        endif()
    endif()
elseif(CMAKE_COMPILER_IS_GNUCXX)
    # assumed to work on other platforms
    set(GCOV_CAPABLE_COMPILER TRUE)
endif()

if (GCOV_CAPABLE_COMPILER)
    set(GCOV_COMPILER_FLAGS "--coverage")
endif()