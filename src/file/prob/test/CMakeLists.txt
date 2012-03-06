# == src/file/prob/test/CMakeLists.txt for Multovl probability testing ==

include_directories(${SRCDIR} ${SRCDIR}/file ${SRCDIR}/file/prob)

# == Unit tests ==

# add the probability modules to the libraries needed for testing
set(utestlibs
    movl_prob
    ${utestlibs}
    CACHE INTERNAL "unit test libs"
)

# the test framework needs a #define when linked dynamically
if (NOT Boost_USE_STATIC_LIBS)
    add_definitions(-DBOOST_TEST_DYN_LINK)
endif()

# this is where the test executables are, needed for add_test() 
set(testdir ${CMAKE_CURRENT_BINARY_DIR})

# -- Prob tests --

set(probtestprogs
    empirdistrtest freeregionstest randomplacertest stattest shuffleovltest
)

foreach(probtestprog ${probtestprogs})
    add_executable(${probtestprog} EXCLUDE_FROM_ALL ${probtestprog}.cc)
    target_link_libraries(${probtestprog} movl_prob ${utestlibs})
	flag_fix(${probtestprog})
	add_test(${probtestprog} ${testdir}/${probtestprog})
endforeach(probtestprog)

# ---

# update global unit test target list
set(unit_test_targets 
    ${unit_test_targets}
    ${probtestprogs}
    CACHE INTERNAL "unit test targets"
)

