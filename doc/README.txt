MULTIPLE OVERLAP OF GENOMIC REGIONS
===================================

by Andras Aszodi
Version 1.1, February 2011

See doc/html/guide.html for installation instructions
and a detailed documentation.

This program is under the BSD Open Source license.
See doc/license.txt for details.

Installation for the Impatient
------------------------------

mkdir build 
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT=/path/to/boost \
    -DCMAKE_INSTALL_PREFIX=/path/to/installdir ..

make all_tools
make install

The tools should now be installed in ${CMAKE_INSTALL_PREFIX}/multovl-1.1/bin.
