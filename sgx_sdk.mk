### SGX environment ###
SGX_SDK ?= /opt/intel/sgxsdk
SGX_MODE ?= SIM

if SGX_32BITS
    SGX_COMMON_CFLAGS = -m32
    SGX_LIBRARY_PATH = $(SGX_SDK)/lib
    SGX_ENCLAVE_SIGNER = $(SGX_SDK)/bin/x86/sgx_sign
    SGX_EDGER8R = $(SGX_SDK)/bin/x86/sgx_edger8r
endif

if SGX_64BITS
    SGX_COMMON_CFLAGS = -m64
    SGX_LIBRARY_PATH = $(SGX_SDK)/lib64
    SGX_ENCLAVE_SIGNER = $(SGX_SDK)/bin/x64/sgx_sign
    SGX_EDGER8R = $(SGX_SDK)/bin/x64/sgx_edger8r
endif

if SGX_MODE_SIM
    TRTS_LIBRARY_NAME = sgx_trts_sim
    URTS_LIBRARY_NAME = sgx_urts_sim
    SERVICE_LIBRARY_NAME = sgx_tservice_sim
    UAE_SERVICE_LIBRARY_NAME = sgx_uae_service_sim
else
    TRTS_LIBRARY_NAME = sgx_trts
    URTS_LIBRARY_NAME = sgx_urts
    SERVICE_LIBRARY_NAME = sgx_tservice
    UAE_SERVICE_LIBRARY_NAME = sgx_uae_service
endif

SGX_INCLUDE_PATH = $(SGX_SDK)/include

%_u.h %_u.c : %.edl
	$(SGX_EDGER8R) --untrusted --search-path $(shell pwd) --search-path $(SGX_INCLUDE_PATH) --untrusted-dir $(dir $@) $^

%_t.h %_t.c : %.edl
	$(SGX_EDGER8R) --trusted --search-path $(shell pwd) --search-path $(SGX_INCLUDE_PATH) --trusted-dir $(dir $@) $^

