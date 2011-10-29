# Module to find the PostgreSQL include dirs and libraries
# Andras Aszodi
# 2010-06-17

# This module supplies the following:-
# POSTGRES_FOUND => true if POSTGRES is found
# POSTGRES_INCLUDE_DIRS => location of the POSTGRES headers
# POSTGRES_LIBRARY_DIRS => location of the POSTGRES libraries
# POSTGRES_LIBRARIES => the POSTGRES lib to be linked
# 
# This script uses the 'pg_config' executable which is searched for
# in various locations first.
# You can help if you define the POSTGRES_ROOT variable
# e.g. if pg_config is at /foo/bar/pgsql/bin/pg_config
# then POSTGRES_ROOT=/foo/bar/pgsql can be very useful

# You may set POSTGRES_USE_STATIC_LIBS if you want static linking

set(POSTGRES_FOUND FALSE)

# Maybe pg_config is in $PATH? 
# Try it, if it returns the bindir then the toplevel will be the directory containing the bindir
execute_process(
    COMMAND pg_config "--bindir"
    COMMAND sed -e s@/bin$@@
    OUTPUT_VARIABLE POSTGRES_TOPLEVEL
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (NOT POSTGRES_TOPLEVEL)
    # try the usual paths
    find_path(POSTGRES_TOPLEVEL /bin/pg_config 
        PATHS
        ${POSTGRES_ROOT}
        /usr/local
        /usr/local/pgsql
        /usr/local/postgres
    #    /opt/local
    )
    set(PG_CONFIG "${POSTGRES_TOPLEVEL}/bin/pg_config")
else()
    set(PG_CONFIG pg_config)    # is in PATH
endif()

if( POSTGRES_TOPLEVEL )
    execute_process(
        COMMAND ${PG_CONFIG} "--includedir"
        OUTPUT_VARIABLE POSTGRES_INCLUDE_DIRS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${PG_CONFIG} "--libdir"
        OUTPUT_VARIABLE POSTGRES_LIBRARY_DIRS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${PG_CONFIG} "--version"
        OUTPUT_VARIABLE POSTGRES_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(LIBPQ_NAME pq)
    if( POSTGRES_USE_STATIC_LIBS )
        set(LIBPQ_NAME "libpq.a")
    endif()

    find_library(POSTGRES_LIBRARIES NAMES ${LIBPQ_NAME} PATHS ${POSTGRES_LIBRARY_DIRS})

    if (POSTGRES_LIBRARIES)
        set(POSTGRES_FOUND TRUE)
        if (POSTGRES_USE_STATIC_LIBS)
            # find all the other extra libs Postgres may use
            # e.g. "-lpgport -lz -lreadline -lcrypt -ldl -lm"
            # and makes a list from them, like "pgport;z;readline;crypt;dl;m"
            execute_process(
                COMMAND ${PG_CONFIG} "--libs" 
                COMMAND sed -e s/-l//g
                COMMAND sed -e "s/ /;/g"
                COMMAND sed -e "s/;$//"
                OUTPUT_VARIABLE POSTGRES_XLIBS
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            
            # Try to find the static versions first
            # NOTE:Ubuntu 8.04LTS needs sudo apt-get install libgss0
            message(STATUS "Looking for extra Postgres libraries...")
            foreach(xlib ${POSTGRES_XLIBS})
                find_library(xlibloc NAMES "lib${xlib}.a" PATHS ${POSTGRES_LIBRARY_DIRS})
                if (xlibloc)
                    list(APPEND POSTGRES_LIBRARIES ${xlibloc})
                    message(STATUS "lib${xlib}.a found in ${xlibloc}")
                    unset(xlibloc CACHE)
                else()
                    list(APPEND POSTGRES_LIBRARIES ${xlib})
                    message(STATUS "lib${xlib} added as dynamic lib")
                endif()
            endforeach(xlib)
            
        endif()
        message(STATUS "${POSTGRES_VERSION} found at ${POSTGRES_TOPLEVEL}")
        message(STATUS "Include dirs: ${POSTGRES_INCLUDE_DIRS}")
        message(STATUS "Library dirs: ${POSTGRES_LIBRARY_DIRS}")
    endif()
endif()
MARK_AS_ADVANCED( POSTGRES_FOUND )
