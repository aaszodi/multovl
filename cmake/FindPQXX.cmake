# NOTE: this file comes from the VTK library, http://www.vtk.org
# modified by AA, 2011-06-22
# Find PostGreSQL C++ library and header file
# Sets
#   PQXX_FOUND                 to 0 or 1 depending on result
#   PQXX_INCLUDE_DIRECTORIES  to the directory containing pqxx/pqxx
#   PQXX_LIBRARIES            to the libpqxx client library (and any dependents required)
# If PQXX_REQUIRED is defined, then a fatal error message will be generated if libpqxx is not found
# Added by AA:
# You can help if you define the PQXX_ROOT variable
# e.g. if the installation is at /foo/bar/pqxx/{include,lib}
# then specify -DPQXX_ROOT=/foo/bar/pqxx when invoking cmake
# You may set PQXX_USE_STATIC_LIBS if you want static linking

if ( NOT PQXX_INCLUDE_DIRECTORIES OR NOT PQXX_LIBRARIES )

    # note: we find Postgres anyway before PQXX, don't worry
    # I took out all the Postgres references (e.g. for the libraries)
  ### FIND_PACKAGE( POSTGRES REQUIRED )
  ### if ( POSTGRES_FOUND )
    file( TO_CMAKE_PATH "$ENV{PQXX_DIR}" _PQXX_DIR )

    # added by AA
    if (PQXX_USE_STATIC_LIBS)
        set(LIBSEARCHNAME libpqxx.a)
    else()
        set(LIBSEARCHNAME pqxx)
    endif()
    find_library( PQXX_LIBRARY
      NAMES ${LIBSEARCHNAME}
      PATHS
        ${PQXX_ROOT}/lib
        ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/bin
        /usr/local/pgsql/lib
        /usr/local/lib
        /usr/lib
      DOC "Location of libpqxx library"
      NO_DEFAULT_PATH
    )

    find_path( PQXX_HEADER_PATH
      NAMES pqxx/pqxx
      PATHS
        ${PQXX_ROOT}/include
        ${CMAKE_INSTALL_PREFIX}/include
        /usr/local/pgsql/include
        /usr/local/include
        /usr/include
      DOC "Path to pqxx/pqxx header file. Do not include the 'pqxx' directory in this value."
      NO_DEFAULT_PATH
    )
  ### endif ( POSTGRES_FOUND )

  if ( PQXX_HEADER_PATH AND PQXX_LIBRARY )

    set( PQXX_FOUND 1 CACHE INTERNAL "PQXX found" FORCE )
    set( PQXX_INCLUDE_DIRECTORIES "${PQXX_HEADER_PATH}" CACHE STRING "Include directories for PostGreSQL C++ library"  FORCE )
    set( PQXX_LIBRARIES "${PQXX_LIBRARY}" CACHE STRING "Link libraries for PostGreSQL C++ interface" FORCE )

    mark_as_advanced( FORCE PQXX_INCLUDE_DIRECTORIES )
    mark_as_advanced( FORCE PQXX_LIBRARIES )
    message(STATUS "PQXX library found")
  else ( PQXX_HEADER_PATH AND PQXX_LIBRARY )
    message(STATUS "PQXX NOT FOUND" )
  endif ( PQXX_HEADER_PATH AND PQXX_LIBRARY )

endif ( NOT PQXX_INCLUDE_DIRECTORIES OR NOT PQXX_LIBRARIES )

