# How to Dockerise MULTOVL

## Build and install

This setup is for MacOS with the Apple Clang compilers. 

```
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ \
-DBOOST_ROOT=/usr/local/boost/current/clang64/ -DCMAKE_INSTALL_PREFIX=~/DEV/install ../../..
```

Then build and install:

```
make -j4 all
make install
```

The installed static executables end up in `${HOME}/DEV/install/multovl-1.3/bin`.

## Docker build context

