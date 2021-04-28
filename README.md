[![Build Status](https://travis-ci.org/flihp/tpm2-tcti-sgx.svg?branch=master)](https://travis-ci.org/flihp/tpm2-tcti-sgx)
[![Coverage Status](https://coveralls.io/repos/github/flihp/tpm2-tcti-sgx/badge.svg)](https://coveralls.io/github/flihp/tpm2-tcti-sgx)

# SGX TCTI
NOTE: This project is no longer actively maintained.

This is an implementation of a TPM command transmission interface (TCTI).
It conforms to the TCG TSS2 TCTI specification version 1.0. It is
designed for use within an Intel&reg; software guard extensions (SGX)
enclave. When used in conjunction with the TSS2 system API (libtss2-sys)
this TCTI allows use of the full set of TPM2 commands from within an SGX
enclave.

## Assumptions & Background
It is assumed that those reading this document and attempting to use the
SGX TCTI are familiar with:
1) C / C++ application development, compilation and linking concepts
2) GNU autotools & libtool
3) Intel&reg; SGX application development

Please consult the relevant documentation before proceeding.

## Build & Install
Instructions to build and install this software are available in the
INSTALL.md file.

## Contributing / Getting in touch
If you're looking to discuss the source code in this project or get
questions answered you should join the TPM2 mailing list:
https://lists.01.org/mailman/listinfo/tpm2. We also have an IRC
channel set up on FreeNode called #tpm2.0-tss.

In the event that you want to contribute bug reports or patches to the
project, please consult the Contribution Guidelines in CONTRIBUTING.md.

## Libraries
A typical TCTI implementation requires only a single library. The
additional interface at the enclave boundary requires that the TCTI
library (the "trusted" library in SGX speak) be paired with an
"unstrusted" companion library.

We represent these two libraries and the boundaries they interface with
in the following diagram:
![tcti sgx lib boundary](https://github.com/flihp/tpm2-tcti-sgx/wiki/images/tcti-sgx-lib-boundary.png)

### libtss2-tcti-sgx.a
Enclaves using the TSS2 System API (libtss2-sys) to communicate with the
TPM2 must use TCTI contexts as provided by the SGX TCTI library
(libtss2-tcti-sgx.a). SGX enclaves must be linked statically and so we
build only a static library. SGX prevents this library from supporting the
dynamic TCTI loading interface.

Internally this library maps the TCTI API calls to the `ocall` mechanism
provided by SGX. The ocalls used by the TCTI library (and exposed by the
companion library) are described in the EDL file: src/tss2_tcti_sgx.edl.

NOTE: No enclave developer should need to interact with the ocalls
directly. Instead use the TCTI API.

### libtcti-sgx-mgr.so
This library provides implementations for the ocalls invoked by the SGX
TCTI library. There is a 1:1 correspondence of TCTI functions to `ocall`s
provided by this companion / "untrusted" library. The interface exposed to
the hosting application is documented in the header: src/tcti-sgx-mgr.h.

## Example Usage
An example enclave and application implementation is provided in
example/application.c. and example/enclave.c respectively. The enclave
shared object and application executable are built by the `all` / default
make target.

The enclave is compiled into a shared object and then signed with a
generated key. The application is run by passing the path to the signed
enclave `so` file. Currently the application assumes it is connecting to
an instance of the `tpm2-abrmd` on the session bus.

## Source Tree Layout
├── src : all source code built into the SGX TCTI library and the  
│         companion management library, edl, headers etc  
├── example : application and enclave demonstrating simple use of  
│             libtss2-tcti-sgx  
└── test : unit tests  
