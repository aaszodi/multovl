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
# GCOV_COMPILER_FLAGS: flags needed for coverage analysis

set(GCOV_CAPABLE_COMPILER FALSE)
set(GCOV_COMPILER_FLAGS "")
find_program(CTEST_COVERAGE_COMMAND NAMES gcov)
if (NOT CTEST_COVERAGE_COMMAND)
    message(STATUS "No gcov found")
    return()
endif()

# Clang can do it
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(GCOV_COMPILER_FLAGS "--coverage")
	set(GCOV_CAPABLE_COMPILER TRUE)
	return()
endif()

# GCC can do it, but not on OSX with XCode 4 (??)
if(CMAKE_COMPILER_IS_GNUCXX)

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
			message(STATUS "NOTE: The default llvm-g++ in XCode 4 cannot do coverage analysis.")
			message(STATUS "NOTE: Use clang++ instead")
		endif()
	endif()

	# Generic GCC or Apple's GCC with XCode younger than V4.0
	# Detect GCC version
	# http://stackoverflow.com/questions/4058565/check-gcc-minor-in-cmake
	execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
					OUTPUT_VARIABLE GCC_VERSION)
	
	# Looks like GCC below V4.2 needs "-fprofile-arcs -ftest-coverage"
	# and "--coverage" later
	set(GCC_MINIMAL_COVERAGE_VERSION "4.2")
	if (GCC_VERSION VERSION_LESS GCC_MINIMAL_COVERAGE_VERSION)
		set(GCOV_COMPILER_FLAGS "-fprofile-arcs -ftest-coverage")
	else()
		set(GCOV_COMPILER_FLAGS "--coverage")
	endif()
    set(GCOV_CAPABLE_COMPILER TRUE)
    return()
endif()



