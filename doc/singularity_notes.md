# Singularity setup

Because Singularity needs root rights to build a container,
you'll need a Linux system where you have admin privileges.

## Installing Singularity

2021-04-12

Source: `https://sylabs.io/guides/3.7/admin-guide/installation.html#installation-on-linux`

These installation notes refer to the `dtr` Azure VM
that runs Ubuntu 20.04 LTS. No software development
was done on this system before so first the basic
requirements for building Multovl need to be installed.

### Basic build environment

```bash
sudo apt-get update
sudo apt-get install gcc-10 g++-10
sudo apt-get install libboost-all-dev # V1.71
sudo apt-get install cmake # brings in gcc-9
```

### Singularity prerequisites

#### Ubuntu packages

Some of these (e.g. `git`) were already installed. This comes from 
```bash
sudo apt-get install -y \
    build-essential \
    uuid-dev \
    libgpgme-dev \
    squashfs-tools \
    libseccomp-dev \
    wget \
    pkg-config \
    git \
    cryptsetup-bin
```

#### The Go programming language

Instruction source: `https://golang.org/doc/install`

Download the binary (in `~/Downloads`):

`wget https://golang.org/dl/go1.16.3.linux-amd64.tar.gz`

Extract the tarball into `/usr/local`:

`sudo tar -C /usr/local -xzf go1.16.3.linux-amd64.tar.gz`

Make it available to everyone by adding the following line to `/etc/profile`:

```
export PATH=$PATH:/usr/local/go/bin
```

Check the installation by `go version`

Should print something like:

```
go version go1.16.3 linux/amd64
```

### Singularity itself

Download and extract the latest tarball from GitHub (or, optionally, one could clone the repo).
I did this in the `Downloads` directory:

```bash
wget https://github.com/hpcng/singularity/releases/download/v3.7.3/singularity-3.7.3.tar.gz
tar zxpf singularity-3.7.3.tar.gz
cd singularity
```

Compile, build and install (by default it goes into the `/usr/local` hierarchy):

```bash
./mconfig
make -C ./builddir
sudo make -C ./builddir install
```

Took a little while to compile, but no hiccups. `which singularity` returns `/usr/local/bin/singularity`.

Added the following to `~/.bashrc`:

```
source /usr/local/etc/bash_completion.d/singularity
```

to support the command line completion features.


