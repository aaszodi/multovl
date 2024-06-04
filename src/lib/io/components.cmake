# == I/O library components ==

# Tutorial on how to use this:
# https://crascit.com/2016/01/31/enhanced-source-file-handling-with-target_sources/
target_sources(movl
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/bamio.cc
    ${CMAKE_CURRENT_LIST_DIR}/fileformat.cc
    ${CMAKE_CURRENT_LIST_DIR}/fileio.cc
    ${CMAKE_CURRENT_LIST_DIR}/linereader.cc
    ${CMAKE_CURRENT_LIST_DIR}/linewriter.cc
    ${CMAKE_CURRENT_LIST_DIR}/textio.cc
)
