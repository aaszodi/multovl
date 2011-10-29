# == src/test/CMakeLists.txt for Multovl testing ==

include_directories(${SRCDIR})

# -- Unit tests --

# libraries needed by the unit tests
# not all of them need all this of course
set(utestlibs
    movl
    ${Boost_DATE_TIME_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

# the test framework needs a #define when linked dynamically
if (NOT Boost_USE_STATIC_LIBS)
    add_definitions(-DBOOST_TEST_DYN_LINK)
endif()

# this is where the test executables are, needed for add_test() 
set(testdir ${CMAKE_CURRENT_BINARY_DIR})

# Test programs
set(testprogs
    regiontest ancregiontest multiregiontest errortest 
    politetest multovloptstest
    multioverlaptest timertest
)

foreach(testprog ${testprogs})
    add_executable(${testprog} EXCLUDE_FROM_ALL ${testprog}.cc)
    target_link_libraries(${testprog} ${utestlibs})
	flag_fix(${testprog})
	add_test(${testprog} ${testdir}/${testprog})
endforeach(testprog)

# add to global unit test target list
set(unit_test_targets 
    ${unit_test_targets}
    ${testprogs}
    CACHE INTERNAL "unit test targets"
)
