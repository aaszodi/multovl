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
    until [ -z "$1" ]; do
        which $1 &> /dev/null
        if [  $? -ne 0 ]; then
            echo "No $1 in your PATH"
            let "missing += 1"
        fi
        shift
    done
    if [ $missing -gt 0 ]; then
        echo "Please install and then try again."
        exit 991
    fi
}

print_help() {
	cat <<HELP
MULTOVL quick build/install script. Works on Linux platforms only.
Builds a Release version with static linkage.
Usage: quickbuild.sh <INSTDIR>
where <INSTDIR> is the installation directory path.
MULTOVL will be installed in <INSTDIR>/multovl-1.3.
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

# Top-level MULTOVL directory (where this script is located)
MULTOVLDIR=$( cd $(dirname $0) ; pwd -P )

# Build in this subdirectory
BUILDDIR=release

# Build and install
mkdir -p $BUILDDIR
cd $BUILDDIR
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${INSTDIR} ..
make -j2 install

# Test the installation
cd ${INSTDIR}/multovl-1.3/bin
./multovltest.sh ./multovl
./multovl --version

# Go back
cd $MULTOVLDIR
