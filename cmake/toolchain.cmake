# C and C++ toolchains

# Currently supported toolchains are:
# "clang" = The Clang/LLVM compilers
# "gnu" = The Gnu Compiler Collection
#       with the special cases "gcc6" and "gcc7"
#       on (Linux) installations with several versions
# "intel" = The Intel compilers
# "sunpro" = Solaris Studio compilers
# "nvc" = Nvidia HPC SDK

if(TOOLCHAIN STREQUAL "clang")
    set(CMAKE_C_COMPILER "clang")
    set(CMAKE_CXX_COMPILER "clang++")
elseif (TOOLCHAIN STREQUAL "gnu")
    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")
elseif (TOOLCHAIN STREQUAL "intel")
    set(CMAKE_C_COMPILER "icc")
    set(CMAKE_CXX_COMPILER "icpc")
elseif (TOOLCHAIN STREQUAL "sunpro")
    set(CMAKE_C_COMPILER "cc")
    set(CMAKE_CXX_COMPILER "CC")
elseif (TOOLCHAIN STREQUAL "nvc")
    set(CMAKE_C_COMPILER "nvc")
    set(CMAKE_CXX_COMPILER "nvc++")
else()
    message(FATAL "Cannot configure toolchain ${TOOLCHAIN}. Please set CMAKE_C[XX]_COMPILER manually.")
endif()
