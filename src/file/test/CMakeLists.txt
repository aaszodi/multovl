# == src/file/test/CMakeLists.txt for file-based Multovl testing ==

include_directories(${SRCDIR} ${SRCDIR}/file)

# == Unit tests ==

# add the File I/O modules to the libraries needed for testing
set(utestlibs
    movl_fileio
    ${utestlibs}
    CACHE INTERNAL "unit test libs"
)

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

# ---

# update global unit test target list
set(unit_test_targets 
    ${unit_test_targets}
    ${filetestprogs}
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

