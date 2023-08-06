#!/usr/bin/env bash

# Quick build script
# Release build, static linkage.
# 2022-05-11 Andras Aszodi

# -- Functions and globals --

# Checks whether the arguments (program names) are in the user's $PATH,
# exits if not found. 
exitif_notinpath() {
    local missing=0
    
    # iterate over all arguments
    until [[ -z "$1" ]]; do
        if [[ ! -x "$(command -v $1)" ]]; then
            echo "No $1 in your PATH"
            let "missing += 1"
        fi
        shift
    done
    if [[ $missing -gt 0 ]]; then
        echo "Please install and then try again."
        exit 991
    fi
}

# `realpath` replacement (absolute path with symlinks resolved)
# all UNIX, not just Linux + GNU coreutils
real_path() {
    echo $( cd $(dirname $1) ; pwd -P )
}

# Top-level MULTOVL directory (where this script is located)
MULTOVLDIR=$(real_path $0)

# Prints the version string as "X.Y"
# depends on a file called "VERSION" in the top-level Multovl directory
version_str() {
    local majorver=$(sed -nr 's/^VERSION_MAJOR ([0-9]+)$/\1/p' $MULTOVLDIR/VERSION)
    local minorver=$(sed -nr 's/^VERSION_MINOR ([0-9]+)$/\1/p' $MULTOVLDIR/VERSION)
    echo "${majorver}.${minorver}"
}

# Returns the number of CPUs.
# In doubt it returns 1 :-)
cpu_count() {
    local ncpu=1
    if [[ -x "$(command -v getconf)" ]]; then
        # Linux and MacOS
        ncpu=$(getconf _NPROCESSORS_ONLN)
        if [[ "$ncpu" == "undefined" ]]; then
            ncpu=1
        fi
    elif [[ -x "$(command -v lscpu)" ]]; then
        # Linux
        ncpu=$(lscpu -p=cpu | egrep -vc '^#')
    else
        # if all else fails, one CPU must still be there, right?
        ncpu=1
    fi
    echo $ncpu
}

print_help() {
	cat <<HELP
MULTOVL Version $(version_str) quick build/install script. 
Works on Linux platforms only.
Builds a Release version with static linkage.
Usage: quickbuild.sh INSTDIR [TOOLCHAIN]
    INSTDIR is the installation directory path. MULTOVL will be installed in <INSTDIR>/multovl/$(version_str)
    TOOLCHAIN: one of 'clang','gnu','intel','sunpro','xlc','nvc', default 'gnu'
HELP
}

# == MAIN ==

# Check pre-requisites
exitif_notinpath make cmake

# Installation directory
if [[ $# -lt 1 ]]; then
	print_help
	exit 91
fi
INSTDIR=$(real_path $1/dummy) # adds dummy file to get the directory part
if [ ! -d $INSTDIR ]; then
	echo "Installation directory $INSTDIR does not exist"
	exit 92
fi
if [ ! -w $INSTDIR ]; then
	echo "Installation directory $INSTDIR is not writable by you"
	exit 93
fi

# Toolchain
# cmake will complain if compiler is not available
TOOLCHAIN=${2:-"gnu"}

# Build in this subdirectory
BUILDDIR="quickbuild/${TOOLCHAIN}/release"

# Configure
mkdir -p $BUILDDIR
cd $BUILDDIR
if ! cmake -DCMAKE_BUILD_TYPE=Release -DTOOLCHAIN=${TOOLCHAIN} -DCMAKE_INSTALL_PREFIX=${INSTDIR} ../../.. ; then
    echo "Could not configure build, exiting"
    exit 94
fi

# Build and install
ncpu=$(cpu_count)
echo "Building on $ncpu CPU(s)"
if ! make -j$ncpu install ; then
    echo "Could not build/install, exiting"
    exit 95
fi

# Test the installation
cd ${INSTDIR}/multovl/$(version_str)/bin
./multovltest.sh ./multovl
./multovl --version
