# CMakeLists.txt for exercising the MULTOVL programs
# unlike the unit tests, these will be built also for Release

# Programs needed for exercising
# currently, it is only "inputfiles"
# that makes bogus input files
if(WIN32)
    add_executable(inputfiles inputfiles.cc wgetopt.c)
else()
    add_executable(inputfiles inputfiles.cc)
endif()
flag_fix(inputfiles)
if (MULTOVL_USE_STATIC_LIBS)
	target_link_libraries(inputfiles ${MULTOVLIBS})
else()
	# Even though `inputfiles` doesn't need BamTools,
	# `movl` wants that when linking dynamically
	target_link_libraries(inputfiles ${MULTOVLIBS} ${BAMTOOLS_LIBRARIES})
endif()

# Test scripts
if(WIN32)
    set(EXT "bat")
else()
    set(EXT "sh")
endif()

# The 'runner' test script
# use this for timing and performance profiling if available

# Native conversion of the current bindir is needed for Windows and cannot hurt on Unix
file(TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}" EXERCISE_PATH)

# Profiling
if (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        find_program(PROFILER NAMES perf)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        find_program(PROFILER NAMES xctrace)
        message(STATUS "Profiler = ${PROFILER}")
    else()
        message(STATUS "Profiling: Currently only perf on Linux and xctrace on MacOS are supported")
        set(PROFILER profiler-NOTFOUND) # the "-NOTFOUND" suffix is checked by the runner script
    endif()
else()
    message(STATUS "Profiling: Not a RelWithDebInfo build")
    set(PROFILER profiler-NOTFOUND)
endif()
if (PROFILER)
    message(STATUS "Profiler tool ${PROFILER} found, profiling enabled")
else()
    message(STATUS "Profiling will not be configured because prerequisites were not met")
endif()

configure_file( runner.${EXT}.in ${CMAKE_CURRENT_BINARY_DIR}/runner.${EXT} @ONLY)

add_custom_target(exerciser DEPENDS inputfiles)
