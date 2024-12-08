# How to Dockerise MULTOVL

## Build and install

First build a Release version with static linking and install it.
Define the installation location via `-DCMAKE_INSTALL_PREFIX` to CMake.

The installation is under `${CMAKE_INSTALL_PREFIX}/multovl/${MULTOVL_VERSION}`
where `${MULTOVL_VERSION}` is currently `1.5`.

## Build the Docker image

The installation directory above serves as the Docker build context:

`cd ${CMAKE_INSTALL_PREFIX}/multovl/${MULTOVL_VERSION}`

and then

`docker build -t aaszodi/multovl:${MULTOVL_VERSION} .` (use the current version tag)

`docker image ls` should then show an image like this:

```
REPOSITORY        TAG                IMAGE ID       CREATED          SIZE
aaszodi/multovl   1.5                a7a16b85445d   16 minutes ago   79.7MB
```


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

Then push the image, e.g. for Version 1.5:

`docker push aaszodi/multovl:1.5`
