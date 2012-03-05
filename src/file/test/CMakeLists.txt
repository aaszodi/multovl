# == src/file/test/CMakeLists.txt for file-based Multovl testing ==

include_directories(${SRCDIR} ${SRCDIR}/file)

# == Unit tests ==

# libraries needed by the unit tests
# not all of them need all this of course
set(utestlibs
    movl_fileio
    movl
    ${Boost_DATE_TIME_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
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

# -- File I/O tests --

# File test programs
set(filetestprogs
    fileformattest fileiotest
    linereadertest linewritertest
)

foreach(filetestprog ${filetestprogs})
    add_executable(${filetestprog} EXCLUDE_FROM_ALL ${filetestprog}.cc)
    target_link_libraries(${filetestprog} ${utestlibs})
	flag_fix(${filetestprog})
	add_test(${filetestprog} ${testdir}/${filetestprog})
endforeach(filetestprog)

# -- Prob tests --

# NOTE: these may go later somewhere else

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
    ${filetestprogs} ${probtestprogs}
    CACHE INTERNAL "unit test targets"
)

# == "Exerciser" ==

# These are for stress testing, timing etc. and they are not part of the unit tests.

set(EXERCISELIBS 
    movl_fileio movl 
    ${Boost_DATE_TIME_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} 
    ${Boost_SYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY}
)
    
# Programs needed for exercising
# currently, it is only "inputfiles"
# that makes bogus input files
set(progexers inputfiles)
foreach(progexer ${progexers})
    add_executable(${progexer} EXCLUDE_FROM_ALL ${progexer}.cc)
    target_link_libraries(${progexer} ${EXERCISELIBS})
    flag_fix(${progexer})
endforeach(progexer)

add_custom_target(exerciser DEPENDS ${progexers})

