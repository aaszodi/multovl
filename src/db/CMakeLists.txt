# CMakeLists.txt for database-accessing Multovl, src/db directory
# This will be built only if Postgres and PQXX are available

include_directories(
    ${POSTGRES_INCLUDE_DIRS}
    ${PQXX_INCLUDE_DIRECTORIES}
)
set(PGMULTOVL_SRC
    pgmultovl.cc
    pgpipeline.cc
    pgmultovlopts.cc
)
add_executable(pgmultovl
    ${PGMULTOVL_SRC}
)
target_link_libraries(pgmultovl 
    ${PQXX_LIBRARIES} ${POSTGRES_LIBRARIES}
    ${MULTOVLIBS}
)
flag_fix(pgmultovl)

# add to global app target list
set(app_targets 
    ${app_targets}
    pgmultovl
    CACHE INTERNAL "app targets"
)

# install won't find non-toplevel targets
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # apps
    install(TARGETS pgmultovl RUNTIME DESTINATION ${MULTOVL_DESTDIR}/bin)
endif()
