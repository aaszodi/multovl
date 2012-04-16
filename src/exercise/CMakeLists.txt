# CMakeLists.txt for exercising the MULTOVL programs
# unlike the unit tests, these will be built also for Release

include_directories(${SRCDIR} ${SRCDIR}/file)

# Programs needed for exercising
# currently, it is only "inputfiles"
# that makes bogus input files
set(progexers inputfiles)
foreach(progexer ${progexers})
    add_executable(${progexer} ${progexer}.cc)
    flag_fix(${progexer})
    target_link_libraries(${progexer} movl_fileio ${MULTOVLIBS})
endforeach(progexer)

# make the 'runner' test script
# use this to run the parallel multovls "forever" to catch multithreaded bugs
configure_file( runner.sh.in runner.sh @ONLY)

# make the 'timer' test script
# use this to collect timing data
### configure_file( timer.sh.in timer.sh @ONLY)

add_custom_target(exerciser DEPENDS ${progexers})
