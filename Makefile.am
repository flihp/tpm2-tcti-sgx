# Copyright 2016 - 2018, Intel Corporation
# SPDX-License-Identifier: BSD-2-Clause
include sgx_sdk.mk

ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = $(TSS2_SYS_CFLAGS) -I$(SGX_INCLUDE_PATH) -I$(srcdir)/src -I$(srcdir)/src/include -fPIC
CLEANFILES = \
    example/enclave_t.h \
    example/enclave_t.c \
    example/enclave_u.h \
    example/enclave_u.c

BUILT_SOURCES = $(CLEANFILES)

# things we're building
lib_LIBRARIES = src/libtcti-sgx-mgr.a src/libtss2-tcti-sgx.a
noinst_PROGRAMS = example/application

all-local: \
    example/enclave-signed.so

# connect unit tests to the test harness
if UNIT
check_PROGRAMS  = \
    test/tcti-sgx-init-param-tests \
    test/tcti-sgx-mgr-init-tests \
    test/tcti-sgx-mgr-ocall-tests \
    test/tcti-sgx-struct-tests \
    test/tcti-sgx-call-tests
endif
TESTS = $(check_PROGRAMS)

# headers and where to install them
libtss2_tcti_sgxdir = $(includedir)/tss2
libtss2_tcti_sgx_HEADERS = $(srcdir)/src/include/tss2-tcti-sgx.h

libtcti_sgx_mgrdir = $(includedir)
libtcti_sgx_mgr_HEADERS = $(srcdir)/src/include/tcti-sgx-mgr.h

EXTRA_DIST = \
    src/include/tcti-sgx_priv.h \
    src/tss2_tcti_sgx.edl \
    test/tcti-sgx-common.h \
    AUTHORS \
    VERSION

CMOCKA_WRAPS = \
    -Wl,--wrap=tcti_sgx_init_ocall \
    -Wl,--wrap=tcti_sgx_transmit_ocall \
    -Wl,--wrap=tcti_sgx_receive_ocall \
    -Wl,--wrap=tcti_sgx_finalize_ocall \
    -Wl,--wrap=tcti_sgx_cancel_ocall \
    -Wl,--wrap=tcti_sgx_set_locality_ocall

# code covear
@CODE_COVERAGE_RULES@
CODE_COVERAGE_IGNORE_PATTERN = "/usr/*" "$(abs_top_srcdir)/test/*" \
   "$(abs_top_srcdir)/src/tcti_sgx_t.*" \
   "$(abs_top_srcdir)/src/tcti_sgx_u.*"

# enclave library
src_libtss2_tcti_sgx_a_CFLAGS  = $(AM_CFLAGS) $(ENCLAVE_CFLAGS)
src_libtss2_tcti_sgx_a_LDLIBS  = $(ENCLAVE_LDLIBS)
src_libtss2_tcti_sgx_a_SOURCES = src/tcti-sgx.c

# application library
src_libtcti_sgx_mgr_a_CFLAGS  = $(AM_CFLAGS) $(GLIB_CFLAGS)
src_libtcti_sgx_mgr_a_LDLIBS  = $(GLIB_LIBS)
src_libtcti_sgx_mgr_a_SOURCES = src/tcti-sgx-mgr.c

# example application & enclave
example_application_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(TABRMD_CFLAGS) \
     -I$(SGX_SDK)/include
example_application_LDADD = $(GLIB_LIBS) $(TABRMD_LIBS) \
    -L$(SGX_LIBRARY_PATH) -l$(URTS_LIBRARY_NAME) src/libtcti-sgx-mgr.a
example_application_SOURCES = example/application.c example/enclave_ocalls.c
nodist_example_application_SOURCES = example/enclave_u.c
example/application.$(OBJEXT): example/enclave_u.c

example/.deps:
	mkdir -p $@
example/enclave_t.$(OBJEXT): example/enclave_t.c | example/.deps

example/enclave.so: CFLAGS+=$(ENCLAVE_CFLAGS)
example/enclave.so: LDFLAGS+=$(ENCLAVE_LDFLAGS)
example/enclave.so: LDLIBS+=$(ENCLAVE_LDLIBS)
example/enclave.so: example/enclave_t.$(OBJEXT) example/enclave.$(OBJEXT) src/libtss2-tcti-sgx.a
	$(CC) $(LDFLAGS) $^ -o $@ -Bstatic $(LDLIBS)

example/rsa3k.pem:
	openssl genrsa -3 -out $@ 3072

example/enclave-signed.so: example/enclave.so example/rsa3k.pem
	$(SGX_ENCLAVE_SIGNER) sign -key example/rsa3k.pem -enclave $< -out $@ -config example/example.config.xml

# unit tests
test_tcti_sgx_init_param_tests_CFLAGS  = $(CMOCKA_CFLAGS) $(AM_CFLAGS) \
    $(CODE_COVERAGE_CFLAGS) -I/opt/intel/sgxsdk/include
test_tcti_sgx_init_param_tests_LDADD   = $(CMOCKA_LIBS) \
    $(CODE_COVERAGE_LIBS)
test_tcti_sgx_init_param_tests_LDFLAGS = $(CMOCKA_WRAPS)
test_tcti_sgx_init_param_tests_SOURCES = \
    test/tcti-sgx-init-param-tests.c \
    test/tcti-sgx_null-wraps.c \
    src/tcti-sgx.c

test_tcti_sgx_mgr_init_tests_CFLAGS = $(CMOCKA_CFLAGS) $(AM_CFLAGS) \
    $(CODE_COVERAGE_CFLAGS) $(GLIB_CFLAGS)
test_tcti_sgx_mgr_init_tests_LDADD = $(CMOCKA_LIBS) $(CODE_COVERAGE_LIBS) $(GLIB_LIBS)
test_tcti_sgx_mgr_init_tests_LDFLAGS = -Wl,--wrap=calloc,--wrap=open \
    -Wl,--wrap=read,--wrap=g_hash_table_insert
test_tcti_sgx_mgr_init_tests_SOURCES = \
    test/tcti-sgx-mgr-init-tests.c \
    src/tcti-sgx-mgr.c

test_tcti_sgx_mgr_ocall_tests_CFLAGS = $(CMOCKA_CFLAGS) $(AM_CFLAGS) \
    $(CODE_COVERAGE_CFLAGS) $(GLIB_CFLAGS)
test_tcti_sgx_mgr_ocall_tests_LDADD = $(CMOCKA_LIBS) $(CODE_COVERAGE_LIBS) $(GLIB_LIBS)
test_tcti_sgx_mgr_ocall_tests_LDFLAGS = -Wl,--wrap=g_hash_table_lookup
test_tcti_sgx_mgr_ocall_tests_SOURCES = \
    test/tcti-sgx-mgr-ocall-tests.c \
    src/tcti-sgx-mgr.c

test_tcti_sgx_struct_tests_CFLAGS  = $(CMOCKA_CFLAGS) $(AM_CFLAGS) \
    $(CODE_COVERAGE_CFLAGS)
test_tcti_sgx_struct_tests_LDADD   = $(CMOCKA_LIBS) $(CODE_COVERAGE_LIBS)
test_tcti_sgx_struct_tests_LDFLAGS = $(CMOCKA_WRAPS)
test_tcti_sgx_struct_tests_SOURCES = \
    test/tcti-sgx-struct-tests.c \
    test/tcti-sgx_null-wraps.c \
    test/tcti-sgx-common.c \
    src/tcti-sgx.c

test_tcti_sgx_call_tests_CFLAGS  = $(CMOCKA_CFLAGS) $(AM_CFLAGS) \
    $(CODE_COVERAGE_CFLAGS) 
test_tcti_sgx_call_tests_LDADD   = $(CMOCKA_LIBS) $(CODE_COVERAGE_LIBS)
test_tcti_sgx_call_tests_LDFLAGS = $(CMOCKA_WRAPS)
test_tcti_sgx_call_tests_SOURCES = \
    test/tcti-sgx-call-tests.c \
    test/tcti-sgx_null-wraps.c \
    test/tcti-sgx-common.c \
    src/tcti-sgx.c

AUTHORS :
	git log --format='%aN <%aE>' | grep -v 'users.noreply.github.com' | sort | \
	    uniq -c | sort -nr | sed 's/^\s*//' | cut -d" " -f2- > $@
