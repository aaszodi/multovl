#!/usr/bin/env bash

# Quick build script
# Release build, static linkage.
# Works on Linux platforms.
# 2022-05-11 Andras Aszodi

# -- Functions --

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

# Returns the number of CPUs.
# In doubt it returns 1 :-)
cpu_count() {
    local ncpu=1
    if [[ -x "$(command -v lscpu)" ]]; then
        ncpu=$(lscpu -p=cpu | egrep -vc '^#')
    else
        # if all else fails, one CPU must still be there, right?
        ncpu=1
    fi
    echo $ncpu
}

print_help() {
	cat <<HELP
MULTOVL quick build/install script. Works on Linux platforms only.
Builds a Release version with static linkage.
Usage: quickbuild.sh INSTDIR [TOOLCHAIN]
where INSTDIR is the installation directory path.
MULTOVL will be installed in <INSTDIR>/multovl/X.Y where "X.Y" is the major.minor version string.
By default the GNU C++ compiler is used. Optionally you can specify the toolchain
as the second command-line argument. Currently 'clang','gnu','intel','sunpro','xlc','nvc' are supported.
This depends on the platform and what's installed there.
HELP
}

# == MAIN ==

# Check pre-requisites
exitif_notinpath make cmake

# Installation directory
if [ $# -lt 1 ]; then
	print_help
	exit 1
fi
INSTDIR=$(realpath $1)
if [ ! -d $INSTDIR ]; then
	echo "Installation directory $INSTDIR does not exist"
	exit 992
fi
if [ ! -w $INSTDIR ]; then
	echo "Installation directory $INSTDIR is not writable by you"
	exit 993
fi

# Toolchain
# cmake will complain if compiler is not available
TOOLCHAIN=${2:-"gnu"}

# Top-level MULTOVL directory (where this script is located)
MULTOVLDIR=$( cd $(dirname $0) ; pwd -P )

# Build in this subdirectory
BUILDDIR="release/${TOOLCHAIN}"

# Configure
mkdir -p $BUILDDIR
cd $BUILDDIR
cmake -DCMAKE_BUILD_TYPE=Release -DTOOLCHAIN=${TOOLCHAIN} -DCMAKE_INSTALL_PREFIX=${INSTDIR} ../..

# Build and install
ncpu=$(cpu_count)
echo "Building on $ncpu CPU(s)"
make -j$ncpu install

# Test the installation
majorver=$(sed -nr 's/^VERSION_MAJOR ([0-9]+)$/\1/p' $MULTOVLDIR/VERSION)
minorver=$(sed -nr 's/^VERSION_MINOR ([0-9]+)$/\1/p' $MULTOVLDIR/VERSION)
cd ${INSTDIR}/multovl/${majorver}.${minorver}/bin
./multovltest.sh ./multovl
./multovl --version

# Go back
cd $MULTOVLDIR
