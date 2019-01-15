This file contains instructions to build and install libtss2-tcti-sgx.

# Dependencies
To build and install the libtss2-tcti-sgx library the following software
packages are required. In many cases the name of the package associated
with a dependency is distro specific. Please consult the search feature
in your distros package manager:

* GNU Autoconf
* GNU Automake
* GNU Libtool
* tpm2-tss header files
* C compiler & libc implementation
* C++ compiler & STL
* cmocka unit test framework
* Intel SGX SDK
* OpenSSL `genrsa` utility

# Building from Source
## Bootstrapping the build (git only)
To configure the tpm2-tcti-sgx source code first run the bootstrap script.
This is required to generate scripts for your platform required in later
build steps:
```
$ ./bootstrap
```

## Configure the Build
Before configuring the build directory the environment must be set up. Do
this by sourcing the `environment` script packaged with the SGX SDK. The
location of this script will depend on where you've installed the SGX SDK.
The default is: `/opt/intel/sgxsdk/environment`

Then configure the build directory by running the configure script:
```
$ ./configure
```

## Compilation
Compiling the code requires running `make`:
```
$ make
```
