#!/bin/bash

# Quick build script
# Release build, static linkage.
# Works on Linux platforms.
# 2022-05-11 Andras Aszodi

# Top-level MULTOVL directory (where this script is located)
MULTOVLDIR=$( cd $(dirname $0) ; pwd -P )

# Build in this subdirectory
BUILDDIR=release
# Install in this directory
INSTALLDIR=${1:-"/usr/local"}

# Build and install
mkdir -p $BUILDDIR
cd $BUILDDIR
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${INSTALLDIR} ..
make -j2 install

# Test the installation
cd ${INSTALLDIR}/multovl-1.3/bin
./multovltest.sh ./multovl
./multovl --version

# Go back
cd $MULTOVLDIR
