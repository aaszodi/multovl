# == src/config/CMakeLists.txt ==
# CMakeLists.txt for the configuration library.

# Figure out the Git settings. These are put in these "global" variables:
set(GITREV_SHA1 "(unknown)" CACHE INTERNAL "Git SHA1" FORCE)
set(GITREV_SHA1SHORT "(unknown)" CACHE INTERNAL "Git short SHA1" FORCE)
set(GITREV_DATE "(unknown)" CACHE INTERNAL "Git date" FORCE)

find_program(GIT_SCM git DOC "Git version control")
mark_as_advanced(GIT_SCM)
find_file(GITDIR NAMES .git PATHS ${CMAKE_SOURCE_DIR} NO_DEFAULT_PATH)
if (${GIT_SCM} AND ${GITDIR})
    # Note that execute_process() runs only during config time
    # therefore the Git version info will NOT be updated after commit or pull
    # unless CMake is re-run
    
    # full SHA1 revision checksum
    execute_process(
        COMMAND ${GIT_SCM} log -1 "--pretty=format:%H" 
        OUTPUT_VARIABLE _sha1
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(GITREV_SHA1 ${_sha1} CACHE INTERNAL "Git SHA1" FORCE)
    
    # short checksum (first 7 chars)
    string(SUBSTRING ${_sha1} 0 7 _sha1s)
    set(GITREV_SHA1SHORT ${_sha1s} CACHE INTERNAL "Git short SHA1" FORCE)
    
    # Git revision date
    execute_process(
        COMMAND ${GIT_SCM} log -1 "--pretty=format:%ai" 
        OUTPUT_VARIABLE _date
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(GITREV_DATE ${_date} CACHE INTERNAL "Git date" FORCE)
endif()

set(cfgsrc
    build.c thirdparty.c version.c
)
foreach(src ${cfgsrc})
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/${src}.in
        ${CMAKE_CURRENT_SOURCE_DIR}/${src}
        @ONLY
    )
endforeach(src)

add_library(cfg STATIC ${cfgsrc}) # always static
flag_fix(cfg)

# -- Config executable --

# Small program to print configuration information
add_executable(multovl_config multovl_config.c)
target_link_libraries(multovl_config cfg)
flag_fix(multovl_config)

# add to global app target list
set(app_targets 
    ${app_targets}
    multovl_config
    CACHE INTERNAL "app targets"
)

# install won't find non-toplevel targets
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # apps
    install(TARGETS multovl_config RUNTIME DESTINATION ${MULTOVL_DESTDIR}/bin)
endif()

