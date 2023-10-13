# - Run Doxygen
#
#  Copyright (c) 2009, 2010 Tobias Rautenkranz <tobias@rautenkranz.ch>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
# 
# Modified and adapted by Andras Aszodi 
# 2010-04-09

# Adds a doxygen target that runs doxygen to generate the html documentation.
# We do not generate LaTeX docs.
# The doxygen target is added to the doc target as dependency.
# i.e.: the API documentation is built with:
#  make doc
#
# Variables you may define are:
#  DOXYFILE_OUTPUT_DIR - Path where the Doxygen output is stored. Defaults to "doc".
#
#  DOXYFILE_LATEX_DIR - Directory where the Doxygen LaTeX output is stored. Defaults to "latex".
#
#  DOXYFILE_HTML_DIR - Directory where the Doxygen html output is stored. Defaults to "html".
#

macro(usedoxygen_set_default name value)
	if(NOT DEFINED "${name}")
		set("${name}" "${value}")
	endif()
endmacro()

message(STATUS "doxygen current source dir = ${CMAKE_CURRENT_SOURCE_DIR}")
find_file(DOXYFILE_IN "Doxyfile.in"
        PATHS "${CMAKE_CURRENT_SOURCE_DIR}"
        NO_DEFAULT_PATH)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DOXYFILE_IN DEFAULT_MSG "DOXYFILE_IN")

if(DOXYFILE_IN_FOUND)
	set(DOXYFILE_LATEX "NO")
	set(DOXYFILE_PDFLATEX "NO")
	set(DOXYFILE_DOT "NO")

	usedoxygen_set_default(DOXYFILE_OUTPUT_DIR "${PROJECT_BINARY_DIR}/doc/doxygen")
	usedoxygen_set_default(DOXYFILE_HTML_DIR "html")
    configure_file(${DOXYFILE_IN} ${PROJECT_BINARY_DIR}/doxygen/Doxyfile ESCAPE_QUOTES IMMEDIATE @ONLY)
    add_custom_target(doxygen ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/doxygen/Doxyfile)
	set_property(DIRECTORY APPEND PROPERTY
			ADDITIONAL_MAKE_CLEAN_FILES "${DOXYFILE_OUTPUT_DIR}/${DOXYFILE_HTML_DIR}")

	add_custom_target(doc ALL)
	add_dependencies(doc doxygen)
endif()
