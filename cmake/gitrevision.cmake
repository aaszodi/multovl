# Simplified Git revision finder
# If the root is under Git control, then count the number of commits
# and concoct a revision string out of it
# if not found, then use the previous settings
# 2011-10-29 Andras Aszodi

set(GIT_REVISION "X")    # 'unknown' by default

find_program(GIT_SCM git
  DOC "Git version control")
mark_as_advanced(GIT_SCM)

if(GIT_SCM)

    # if there's a .git directory, we're under version control,
    # can work out the revision
    find_file(GITDIR NAMES .git PATHS ${CMAKE_SOURCE_DIR} NO_DEFAULT_PATH)
    if(GITDIR)
        execute_process(
            COMMAND ${GIT_SCM} rev-list --all
            COMMAND wc -l
            COMMAND sed -E -e "s/^[[:blank:]]+//"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GITREVCNT_output
            ERROR_VARIABLE GITREVCNT_error
            RESULT_VARIABLE GITREVCNT_result
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        execute_process(
            COMMAND ${GIT_SCM} log -1 "--pretty=format:%h %ai"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GITLOG_output
            ERROR_VARIABLE GITLOG_error
            RESULT_VARIABLE GITLOG_result
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if (${GITREVCNT_result} EQUAL 0 AND ${GITLOG_result} EQUAL 0)
            set(GITREVISION "${GITREVCNT_output} [${GITLOG_output}]")
        else()
            message(WARNING "Git rev error: ${GITREVCNT_error} ${GITLOG_error}")
        endif()
    endif()
    
endif()
