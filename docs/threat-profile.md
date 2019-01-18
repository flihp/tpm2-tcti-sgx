# Threat Profile
When building a threat model for an application it's important to consider
the risks posed by each depenency. Developers building applications /
enclaves using the SGX TCTI are encouraged to so and this document is
intended to provide data to support this task.

## Usage Scenario
The SGX TCTI is provided as two libraries. See README.md for a detailed
description of each. This document assumes that the consuming application
uses these libraries as intended:
* libtss2-tcti-sgx.a - This library provides a TCTI module for use with
the higher-layer TSS2 APIs. It must be linked statically into the SGX
enclave.
* libtcti-sgx-mgr.so - This library provides an implementation of the
ocalls described in src/tss2_tcti_sgx.edl and the management layer
required to move TPM2 command / response buffers between the enclave
and the TPM.
