# == src/tests/CMakeLists.txt for generic Multovl component testing ==

include_directories(${SRCDIR} ${SRCDIR}/config ${SRCDIR}/tests)

# -- Unit tests --

# test helper library
# currently serialization tools only
add_library(testhelper STATIC tempfile.cc)
flag_fix(testhelper)

set(utestlibs
    testhelper
    movl_prob movl
    ${BAMTOOLS_LIBRARIES}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SERIALIZATION_LIBRARY}
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
    baseregiontest regiontest 
    ancregiontest reglimittest
    multiregiontest errortest 
    politetest multovloptstest
    multioverlaptest timertest
    fileformattest fileiotest
    linereadertest linewritertest
    empirdistrtest freeregionstest
    stattest shuffleovltest
)

foreach(testprog ${testprogs})
    add_executable(${testprog} EXCLUDE_FROM_ALL ${testprog}.cc)
    target_link_libraries(${testprog} ${utestlibs})
	flag_fix(${testprog})
	add_test(${testprog} ${testdir}/${testprog})
endforeach(testprog)

add_custom_target(unit_tests DEPENDS ${testprogs})
