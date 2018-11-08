ENCLAVE_LDFLAGS = -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles \
    $(SGX_LIBS_ONLY_L) -Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
    -Wl,-pie,-eenclave_entry -Wl,--export-dynamic -Wl,--defsym,__ImageBase=0 \
    -Wl,-fuse-ld=gold -Wl,--rosegment
ENCLAVE_LDLIBS = -Wl,--whole-archive -l$(SGX_TRTS_LIB) \
    -Wl,--no-whole-archive -Wl,--start-group -lsgx_tstdc -lsgx_tcrypto \
    -l$(SGX_TSERVICE_LIB) -Wl,--end-group

ENCLAVE_SEARCH_PATH = --search-path $(srcdir)/src \
    --search-path $(srcdir)/src/include --search-path $(SGX_INCLUDE_DIR)

%_u.h %_u.c : %.edl
	$(SGX_EDGER8R_BIN) --untrusted $(ENCLAVE_SEARCH_PATH) --untrusted-dir $(dir $^) $^

%_t.h %_t.c : %.edl
	$(SGX_EDGER8R_BIN) --trusted $(ENCLAVE_SEARCH_PATH) --trusted-dir $(dir $^) $^

