# Simplified Git revision finder
# If the root is under Git control, then count the number of commits
# and concoct a revision string out of it
# if not found, then use the previous settings
# 2011-10-29 Andras Aszodi

set(GITREVISION "X")    # 'unknown' by default

find_program(GIT_SCM git
  DOC "Git version control")
mark_as_advanced(GIT_SCM)

if(GIT_SCM)

    # if there's a .git directory, we're under version control,
    # can work out the revision
    # note: all this hackery is Unix-specific (uses wc, sed, cat, echo...)
    find_file(GITDIR NAMES .git PATHS ${CMAKE_SOURCE_DIR} NO_DEFAULT_PATH)
    if(GITDIR)
        execute_process(
            COMMAND ${GIT_SCM} rev-list --all
            COMMAND wc -l
            COMMAND sed -E -e "s/^[[:blank:]]+//"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GITREVCNT
            ERROR_VARIABLE GITREVCNT_error
            RESULT_VARIABLE GITREVCNT_result
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        execute_process(
            COMMAND ${GIT_SCM} log -1 "--pretty=format:%h %ai"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GITLOG
            ERROR_VARIABLE GITLOG_error
            RESULT_VARIABLE GITLOG_result
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if (${GITREVCNT_result} EQUAL 0 AND ${GITLOG_result} EQUAL 0)
            # all went well
            set(GITREVISION "${GITREVCNT} [${GITLOG}]")
            
            # save the current revision into this helper file 
            # which itself is NOT under version control
            file(WRITE ${CMAKE_SOURCE_DIR}/gitrevision.txt ${GITREVISION})
        else()
            message(WARNING "Cannot get Git revision: ${GITREVCNT_error} ${GITLOG_error}")
        endif()
    else(GITDIR)
        # not under version control, most probably installing from tarball
        # read from the gitrevision.txt file which might have been packaged
        file(READ gitrevision.txt GITREVISION)
        string(REGEX REPLACE " \\[.*$" "" ${GITREVISION} GITREVCNT)
    endif(GITDIR)
    
endif()
