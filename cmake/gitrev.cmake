# == gitrev.cmake ==

# Figure out the Git settings. The following "global" variables will be set:
# GITREV_SHA1 = the full Git digest
# GITREV_SHA1SHORT = the first 7 characters of ${GITREV_SHA1}
# GITREV_DATE = the date of the most recent commit
# Note that these variables will not be updated after a `git commit` or `git pull`
# unless CMake is re-run.

set(GITREV_SHA1 "(unknown)" CACHE INTERNAL "Git SHA1" FORCE)
set(GITREV_SHA1SHORT "(unknown)" CACHE INTERNAL "Git short SHA1" FORCE)
set(GITREV_DATE "(unknown)" CACHE INTERNAL "Git date" FORCE)
set(GITREV_COUNT "(unknown)" CACHE INTERNAL "Git revision count" FORCE)

find_program(GIT_SCM git DOC "Git version control")
mark_as_advanced(GIT_SCM)
find_file(GITDIR NAMES .git PATHS ${CMAKE_SOURCE_DIR} NO_DEFAULT_PATH)
set(GIT_AVAILABLE ${GIT_SCM} AND ${GITDIR})
if (GIT_AVAILABLE)
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

    # Git revision count
    execute_process(
        COMMAND ${GIT_SCM} rev-list --all --count
        OUTPUT_VARIABLE _count
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(GITREV_COUNT ${_count} CACHE INTERNAL "Git revision count" FORCE)

endif()

