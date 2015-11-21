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

# Test scripts
if(WIN32)
    set(EXT "bat")
else()
    set(EXT "sh")
endif()

# make the 'runner' test script
# use this to run the parallel multovls "forever" to catch multithreaded bugs
# Native conversion of the current bindir is needed for Windows and cannot hurt on Unix
file(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}" EXERCISE_PATH)
configure_file( runner.${EXT}.in ${CMAKE_CURRENT_BINARY_DIR}/runner.${EXT} @ONLY)

# make the 'timer' test script
# use this to collect timing data
### configure_file( timer.${EXT}.in timer.${EXT} @ONLY)

add_custom_target(exerciser DEPENDS inputfiles)
