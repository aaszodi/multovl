# Building Multovl on the VBC cluster

2021-04-12

The VBC cluster ("CBE") is a HPC installation at the Vienna Biocenter
where I am employed at the time of this writing. For more info see `https//clip.science`.

## Environment

The following modules must be loaded to build Multovl:

- **CMake** : `module load cmake/.3.13.3-gcccore-7.3.0`
- **Boost** : `module load boost/1.67.0-foss-2018b`

Note that the CMake modules are "hidden". This setup works with the 
currently default build environment `build-env/2020` that provides
Version 7.3 of the GNU compilers.

## Configuration and debug build

```bash
mkdir -p build/debug
cd build/debug
cmake -DCMAKE_BUILD_TYPE=Debug -DBOOST_ROOT=/software/2020/software/boost/1.67.0-foss-2018b  ../..
make -j4 unit_tests
make test
make -j4 apps
```

