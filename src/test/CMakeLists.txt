# == src/test/CMakeLists.txt for generic Multovl component testing ==

include_directories(${SRCDIR} ${SRCDIR}/test)

# -- Unit tests --

# test helper library
# currently serialization tools only
add_library(testhelper STATIC tempfile.cc)
flag_fix(testhelper)

# libraries that may be needed by the unit tests
set (genericlibs
    testhelper
    movl
)
if (BAMTOOLS_FOUND)
    list(APPEND genericlibs ${BAMTOOLS_LIBRARIES})
endif()
set(utestlibs
    ${utestlibs}
    ${genericlibs}
    ${Boost_DATE_TIME_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SERIALIZATION_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    CACHE INTERNAL "unit test libs"
)

# the test framework needs a #define when linked dynamically
if (NOT Boost_USE_STATIC_LIBS)
    add_definitions(-DBOOST_TEST_DYN_LINK)
endif()

# this is where the test executables are, needed for add_test() 
set(testdir ${CMAKE_CURRENT_BINARY_DIR})

# Test programs
set(testprogs
    regiontest ancregiontest reglimittest multireglimittest
    multiregiontest errortest 
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
