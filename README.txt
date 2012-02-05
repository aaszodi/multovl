MULTIPLE OVERLAP OF GENOMIC REGIONS
===================================

by Andras Aszodi
Version 1.2

See doc/html/guide.html for installation instructions
and a detailed documentation.

This program is under the BSD Open Source license.
See doc/license.txt for details.

Installation for the Impatient
------------------------------

You will need cmake 2.8 or above, a C++98-compliant compiler, and the Boost libraries (1.40 or above) to build Multovl. The basic steps:

mkdir build 
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBOOST_ROOT=/path/to/boost \
	-DBAMTOOLS_ROOT=/path/to/bamtools \
	-DPQXX_ROOT=/path/to/pqxx \
    -DCMAKE_INSTALL_PREFIX=/path/to/installdir ..

make apps
make install (this you may need to run as root, depending on installdir permissions)

The tools should now be installed in ${CMAKE_INSTALL_PREFIX}/multovl-1.2/bin.

Additional Cmake settings are listed in the top-level CMakeLists.txt, or you may run
cmake -i for interactive options, or ccmake.
