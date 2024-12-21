# How to Dockerise MULTOVL

**Short version**: Run the `docker_image` target in a Release build.

**Detailed explanations** are below.

## How the image is built

The Docker image provided by the CMake target `docker_image` simply
encapsulates the Multovl executables already built by the `apps` target.
In other words, the Multovl tools are not built together with the Docker image,
they are just copied into it. This saves image build time. The price
you pay for this convenience:

- The image can be built on _bona fide_ Linux hosts only. That is, you cannot build
on a Windows or MacOS host using Docker Desktop, because the Multovl architecture
will not be Linux and therefore cannot be run in the container.
- The Multovl tools must be built with static linking to avoid
copying the support libraries. This is the default anyway.

## How to build the Docker image

First build a Release version with static linking on a Linux host
by running `make -j N apps` where `N` is the number of build threads.

Then simply run `make docker_image` which will be very quick. The
longest phase is the download of the parent OS image (currently Ubuntu 24.04 LTS).
The resulting image will be tagged as `aaszodi/multovl:X.Y-ARCH` where
`X.Y` is the Multovl version (currently 1.5) and `ARCH` is the host 
machine architecture as returned by `uname -m`. For instance, `x86_64`
for Intel CPUs, `aarch64` for ARM, `ppc64le` for PowerPC.

## Uploading the image to the Docker Hub

Log in to the Docker hub:

`docker login -u aaszodi`

This will ask for the password and then complain a bit:

```
WARNING! Your password will be stored unencrypted in /home/aszodi/.docker/config.json.
Configure a credential helper to remove this warning. See
https://docs.docker.com/engine/reference/commandline/login/#credentials-store

Login Succeeded
```

Then push the image, e.g. the ARM version:

`docker push aaszodi/multovl:1.5-aarch64`
