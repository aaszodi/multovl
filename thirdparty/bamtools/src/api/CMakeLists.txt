# ==========================
# BamTools CMakeLists.txt
# (c) 2010 Derek Barnett
#
# src/api/
# ==========================

# list include paths
include_directories( ${BamTools_SOURCE_DIR}/src ${ZLIB_INCLUDE_DIRS})

# add compiler definitions 
add_definitions( -DBAMTOOLS_API_LIBRARY ) # (for proper exporting of library symbols)
### Note that this added only some MSVC complications with __declspec(dllexport|dllimport)
### which turns out to be unnecessary with Visual Studio 2015. 

# fetch all internal source files
add_subdirectory( internal )

# make list of all API source files
set( BamToolsAPISources
        BamAlignment.cpp
        BamMultiReader.cpp
        BamReader.cpp
        BamWriter.cpp
        SamHeader.cpp
        SamProgram.cpp
        SamProgramChain.cpp
        SamReadGroup.cpp
        SamReadGroupDictionary.cpp
        SamSequence.cpp
        SamSequenceDictionary.cpp
        ${InternalSources}
)

# create main BamTools API library
if (BAMTOOLS_USE_STATIC_LIBS)
    # create main BamTools API static library
    add_library( BamTools-static STATIC ${BamToolsAPISources} )
    set_target_properties( BamTools-static PROPERTIES 
                           OUTPUT_NAME "bamtools" 
                           PREFIX "lib" )
    target_link_libraries( BamTools-static ${ZLIB_LIBRARIES} )
else()
    add_library( BamTools SHARED ${BamToolsAPISources} )
    set_target_properties( BamTools PROPERTIES
                           SOVERSION "2.3.1"
                           OUTPUT_NAME "bamtools" )
    target_link_libraries( BamTools ${ZLIB_LIBRARIES} )
endif()
