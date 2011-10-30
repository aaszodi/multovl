# == src/wbench/test/CMakeLists.txt ==

# Testing the Multovl "workbench"

include_directories(${SRCDIR} ${SRCDIR}/wbench)

# -- Unit tests --

# libraries needed by the unit tests
# not all of them need all this of course
set(utestlibs
    movl_wb
    movl
    ${Boost_DATE_TIME_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SERIALIZATION_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)
if (BAMTOOLS_FOUND)
    list(APPEND utestlibs ${BAMTOOLS_LIBRARIES})
endif()

# this is where the test executables are, needed for add_test() 
set(testdir ${CMAKE_CURRENT_BINARY_DIR})

# the test framework needs a #define when linked dynamically
if (NOT Boost_USE_STATIC_LIBS)
    add_definitions(-DBOOST_TEST_DYN_LINK)
endif()

# Test programs
set(testprogs
    serialtest
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
