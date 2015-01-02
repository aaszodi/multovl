# This script sets CMake variables containing the 
# if Git is available and the working directory is under source control.
# The header will contain a few useful macros
# and will be re-made every time a commit or pull operation changes the Git HEAD.
# Can be used in C and C++ projects. Make sure you add a dependency like
# add_dependencies($some_toplevel_target create_gitrevision)
# otherwise this will not be invoked. 
# Here in Multovl this dependency is added for the "movl" and "movl_fileio" libraries.
# ${SRCDIR} must be defined and shall point to the source tree of the project.
# Add this file to the .gitignore file of the project.
# 
# 2014-03-19 Andras Aszodi

find_program(GIT_SCM git DOC "Git version control")
mark_as_advanced(GIT_SCM)
find_file(GITDIR NAMES .git PATHS ${CMAKE_SOURCE_DIR} NO_DEFAULT_PATH)
if (GIT_SCM AND GITDIR)
    # Git is available, add a meaningful custom command to produce the gitrevision header
    add_custom_command(OUTPUT ${SRCDIR}/gitrevision.h
        COMMAND ${CMAKE_COMMAND} -E echo "#ifndef GITREVISION_HEADER" > ${SRCDIR}/gitrevision.h
        COMMAND ${CMAKE_COMMAND} -E echo "#define GITREVISION_HEADER" >> ${SRCDIR}/gitrevision.h
        COMMAND ${CMAKE_COMMAND} -E echo_append "#define GITREV_SHA1 " >> ${SRCDIR}/gitrevision.h
        COMMAND ${GIT_SCM} log -1 "--pretty=format:%H%n" >> ${SRCDIR}/gitrevision.h
        COMMAND ${CMAKE_COMMAND} -E echo_append "#define GITREV_SHA1SHORT " >> ${SRCDIR}/gitrevision.h
        COMMAND ${GIT_SCM} log -1 "--pretty=format:%h%n" >> ${SRCDIR}/gitrevision.h
        COMMAND ${CMAKE_COMMAND} -E echo_append "#define GITREV_DATE " >> ${SRCDIR}/gitrevision.h
        COMMAND ${GIT_SCM} log -1 "--pretty=format:%ai%n" >> ${SRCDIR}/gitrevision.h
        COMMAND ${CMAKE_COMMAND} -E echo "#endif /* GITREVISION_HEADER */" >> ${SRCDIR}/gitrevision.h
        DEPENDS ${GITDIR}/logs/HEAD
        COMMENT "Making Git revision header in ${SRCDIR}/gitrevision.h"
        VERBATIM
    )
else()
    # No version control
    add_custom_command(OUTPUT ${SRCDIR}/gitrevision.h
        COMMAND ${CMAKE_COMMAND} -E echo "#ifndef GITREVISION_HEADER" > ${SRCDIR}/gitrevision.h
        COMMAND ${CMAKE_COMMAND} -E echo "#define GITREVISION_HEADER" >> ${SRCDIR}/gitrevision.h
        COMMAND ${CMAKE_COMMAND} -E echo_append "#define GITREV_SHA1 unknown" >> ${SRCDIR}/gitrevision.h
        COMMAND ${CMAKE_COMMAND} -E echo_append "#define GITREV_SHA1SHORT unknown" >> ${SRCDIR}/gitrevision.h
        COMMAND ${CMAKE_COMMAND} -E echo_append "#define GITREV_DATE unknown" >> ${SRCDIR}/gitrevision.h
        COMMAND ${CMAKE_COMMAND} -E echo "#endif /* GITREVISION_HEADER */" >> ${SRCDIR}/gitrevision.h
        COMMENT "No Git found, making empty ${SRCDIR}/gitrevision.h"
        VERBATIM
    )
endif()
# this "drives" the custom command
# note that despite what the manual says, the ALL is not sufficient to 
# make this target always out-of-date and therefore it is not invoked
# automatically before every build
add_custom_target(create_gitrevision ALL DEPENDS ${SRCDIR}/gitrevision.h)