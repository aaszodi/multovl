# CMakeLists.txt for exercising the MULTOVL programs
# unlike the unit tests, these will be built also for Release

include_directories(${SRCDIR} ${SRCDIR}/file)

# Programs needed for exercising
# currently, it is only "inputfiles"
# that makes bogus input files
if(WIN32)
    add_executable(inputfiles inputfiles.cc wgetopt.c)
else()
    add_executable(inputfiles inputfiles.cc)
endif()
flag_fix(inputfiles)
target_link_libraries(inputfiles movl_fileio ${MULTOVLIBS})

# make the 'runner' test script
# use this to run the parallel multovls "forever" to catch multithreaded bugs
configure_file( runner.sh.in runner.sh @ONLY)

# make the 'timer' test script
# use this to collect timing data
### configure_file( timer.sh.in timer.sh @ONLY)

add_custom_target(exerciser DEPENDS ${progexers})
