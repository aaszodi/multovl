# -- Platform- and compiler-specific fixes & hacks --

# Depending on the architecture and the compiler,
# certain compilation and link flags need to be added.
# We define the 'flag_fix' macro that takes a 'targ' argument
# (the current compile/link target) and fixes the corresponding
# COMPILE_FLAGS and LINK_FLAGS settings.
# Any additional platform-specific stuff can go here as well.

if(CMAKE_CXX_COMPILER_ID STREQUAL "SunPro" AND ${CMAKE_SYSTEM_NAME} STREQUAL "SunOS")
    # [Open]Solaris, SunPro compiler (V>=5.10)
    # use STLport (also when compiling Boost!)
    macro(flag_fix targ)
        set(SUNFLAGS "-library=stlport4")
        set_target_properties(${targ} PROPERTIES COMPILE_FLAGS ${SUNFLAGS} LINK_FLAGS ${SUNFLAGS})
    endmacro(flag_fix)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MIPSpro" AND ${CMAKE_SYSTEM_NAME} MATCHES "IRIX")
    # SGI/Irix
    # use Nekoware Boost 1.35, 32-bit MIPS4
    # don't even try anything less than MIPSpro 7.4!
    message("Using Boost compatibility headers for IRIX/MIPSpro")
    include_directories(${Boost_INCLUDE_DIRS}/boost/compatibility/cpp_c_headers)
    
    macro(flag_fix targ)
        set(IRIXFLAGS "-n32 -mips4")
	    set_target_properties(${targ} PROPERTIES COMPILE_FLAGS ${IRIXFLAGS} LINK_FLAGS ${IRIXFLAGS})
    endmacro(flag_fix)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel" AND ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # Intel compiler on Apple Mac OS X
    # EXC_BAD_ACCESS bug with XCode >=3.2.2 and Intel V11.1 is fixed with -use_asm
    # the -static-intel link flag links in all Intel libs, exec can run w/o installing the compiler
    macro(flag_fix targ)
        set_target_properties(
            ${targ} PROPERTIES COMPILE_FLAGS "-use_asm" 
            LINK_FLAGS "-use_asm -static-intel"
        )
    endmacro(flag_fix)
else()
    # empty macro
    macro(flag_fix targ)
    endmacro(flag_fix)
endif()
