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
    target_link_libraries( BamTools ${ZLIB_LIBRARIES)
endif()

# export API headers
include(../ExportHeader.cmake)
set(ApiIncludeDir "api")
ExportHeader(APIHeaders api_global.h             ${ApiIncludeDir})
ExportHeader(APIHeaders BamAlgorithms.h          ${ApiIncludeDir})
ExportHeader(APIHeaders BamAlignment.h           ${ApiIncludeDir})
ExportHeader(APIHeaders BamAux.h                 ${ApiIncludeDir})
ExportHeader(APIHeaders BamConstants.h           ${ApiIncludeDir})
ExportHeader(APIHeaders BamIndex.h               ${ApiIncludeDir})
ExportHeader(APIHeaders BamMultiReader.h         ${ApiIncludeDir})
ExportHeader(APIHeaders BamReader.h              ${ApiIncludeDir})
ExportHeader(APIHeaders BamWriter.h              ${ApiIncludeDir})
ExportHeader(APIHeaders IBamIODevice.h           ${ApiIncludeDir})
ExportHeader(APIHeaders SamConstants.h           ${ApiIncludeDir})
ExportHeader(APIHeaders SamHeader.h              ${ApiIncludeDir})
ExportHeader(APIHeaders SamProgram.h             ${ApiIncludeDir})
ExportHeader(APIHeaders SamProgramChain.h        ${ApiIncludeDir})
ExportHeader(APIHeaders SamReadGroup.h           ${ApiIncludeDir})
ExportHeader(APIHeaders SamReadGroupDictionary.h ${ApiIncludeDir})
ExportHeader(APIHeaders SamSequence.h            ${ApiIncludeDir})
ExportHeader(APIHeaders SamSequenceDictionary.h  ${ApiIncludeDir})

set( AlgorithmsIncludeDir "api/algorithms" )
ExportHeader( AlgorithmsHeaders algorithms/Sort.h ${AlgorithmsIncludeDir} )