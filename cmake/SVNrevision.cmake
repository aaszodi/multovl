# Simplified Subversion revision finder
# Invokes 'svnversion' and puts the "real" overall revision number in ${SUBVERSION_REVISION}
# or "0" on error
# 2010-11-13 Andras Aszodi

set(SUBVERSION_REVISION "X")    # 'unknown' by default (SVN rev-s start at 1)

find_program(SVNVERSION svnversion
  DOC "SVN version utility program")
mark_as_advanced(SVNVERSION)

if(SVNVERSION)

    execute_process(COMMAND ${SVNVERSION} ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE SVNVERSION_output
        ERROR_VARIABLE SVNVERSION_error
        RESULT_VARIABLE SVNVERSION_result
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(${SVNVERSION_result} EQUAL 0)
        string(REGEX REPLACE "^[0-9]*:" "" 
            SUBVERSION_REVISION ${SVNVERSION_output})
    else()
        message(WARNING "Command \"${SVNVERSION} ${CMAKE_SOURCE_DIR}\" returned error \"${SVNVERSION_error}\"")
    endif()
    
endif()
