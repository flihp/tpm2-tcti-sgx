### SGX environment ###
SGX_SDK ?= /opt/intel/sgxsdk
SGX_MODE ?= SIM

if SGX_32BITS
    SGX_COMMON_CFLAGS = -m32
    SGX_LIBRARY_PATH = $(SGX_SDK)/lib
endif

if SGX_64BITS
    SGX_COMMON_CFLAGS = -m64
    SGX_LIBRARY_PATH = $(SGX_SDK)/lib64
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

### we don't use -nostdinc here so we can get TPM2 types ###
### should probably have a set of flags for compiling libraries for use by SGX
### enclaves and a set for the enclave itself
ENCLAVE_CFLAGS = $(SGX_COMMON_CFLAGS) -O0 -fvisibility=hidden \
    -fpie -fstack-protector -I$(SGX_SDK)/include  -I$(SGX_SDK)/include/tlibc
ENCLAVE_LDFLAGS = -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles \
    -L$(SGX_LIBRARY_PATH) -Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
    -Wl,-pie,-eenclave_entry -Wl,--export-dynamic -Wl,--defsym,__ImageBase=0 \
    -Wl,-fuse-ld=gold -Wl,--rosegment
ENCLAVE_LDLIBS = -Wl,--whole-archive -l$(TRTS_LIBRARY_NAME) \
    -Wl,--no-whole-archive -Wl,--start-group -lsgx_tstdc -lsgx_tcrypto \
    -l$(SERVICE_LIBRARY_NAME) -Wl,--end-group

ENCLAVE_SEARCH_PATH = --search-path $(srcdir)/src \
    --search-path $(srcdir)/src/include --search-path $(SGX_INCLUDE_PATH)

%_u.h %_u.c : %.edl
	$(SGX_EDGER8R_BIN) --untrusted $(ENCLAVE_SEARCH_PATH) --untrusted-dir $(dir $^) $^

%_t.h %_t.c : %.edl
	$(SGX_EDGER8R_BIN) --trusted $(ENCLAVE_SEARCH_PATH) --trusted-dir $(dir $^) $^

