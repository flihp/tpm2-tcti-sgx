# Copyright 2016 - 2018, Intel Corporation
# SPDX-License-Identifier: BSD-2-Clause
VPATH = $(srcdir) $(builddir)
ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = $(EXTRA_CFLAGS) -I$(SGX_INCLUDE_DIR) -I$(srcdir)/src
AM_CXXFLAGS = $(AM_CFLAGS) $(EXTRA_CXXFLAGS)
ENCLAVE_CFLAGS += -I$(SGX_INCLUDE_DIR)/tlibc
CLEANFILES = \
    example/application \
    example/enclave-signed.so \
    example/enclave.so \
    example/enclave_t.h \
    example/enclave_t.c \
    example/enclave_u.h \
    example/enclave_u.c \
    example/rsa3k.pem

BUILT_SOURCES = example/enclave_t.h \
    example/enclave_t.c \
    example/enclave_u.h \
    example/enclave_u.c

# things we're building
lib_LTLIBRARIES = src/libtcti-sgx-mgr.la
lib_LIBRARIES = src/libtcti-sgx-mgr.a src/libtss2-tcti-sgx.a
noinst_LIBRARIES = test/libtest.a
EXTRA_PROGRAMS = example/application
dist_man3_MANS = man/man3/Tss2_Tcti_Sgx_Init.3
dist_man7_MANS = man/man7/tss2-tcti-sgx.7

# connect unit tests to the test harness
if UNIT
check_PROGRAMS  = \
    test/tcti-sgx-init-param-tests \
    test/tcti-sgx-mgr-init-callback \
    test/tcti-sgx-mgr-init-null-callback \
    test/tcti-sgx-mgr-init-userdata \
    test/tcti-sgx-mgr-init-tests \
    test/tcti-sgx-mgr-ocall-tests \
    test/tcti-sgx-struct-tests \
    test/tcti-sgx-call-tests \
    test/tcti-util
endif
TESTS = $(check_PROGRAMS)

example: example/application example/enclave-signed.so

# headers and where to install them
libtss2_tcti_sgxdir = $(includedir)/tss2
libtss2_tcti_sgx_HEADERS = $(srcdir)/src/tss2-tcti-sgx.h

libtcti_sgx_mgrdir = $(includedir)
libtcti_sgx_mgr_HEADERS = $(srcdir)/src/tcti-sgx-mgr.h

EXTRA_DIST = \
    example/enclave.c \
    example/enclave.edl \
    example/example.config.xml \
    src/tcti-util.h \
    src/tcti-sgx_priv.h \
    src/tcti-sgx-mgr_priv.h \
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
CODE_COVERAGE_DIRECTORY = $(top_builddir)/src
CODE_COVERAGE_IGNORE_PATTERN = "/usr/*" \
   "$(abs_top_srcdir)/src/tcti_sgx_t.*" \
   "$(abs_top_srcdir)/src/tcti_sgx_u.*"

# enclave library
src_libtss2_tcti_sgx_a_CFLAGS  = $(AM_CFLAGS) $(ENCLAVE_CFLAGS) $(CODE_COVERAGE_CFLAGS)
src_libtss2_tcti_sgx_a_SOURCES = src/tcti-sgx.c

# application library
src_libtcti_sgx_mgr_a_CXXFLAGS = $(AM_CXXFLAGS) $(CODE_COVERAGE_CFLAGS)
src_libtcti_sgx_mgr_a_SOURCES = src/tcti-util.cpp src/tcti-sgx-mgr.cpp

src_libtcti_sgx_mgr_la_CXXFLAGS  = $(AM_CXXFLAGS) $(MSSIM_CFLAGS) $(CODE_COVERAGE_CFLAGS)
src_libtcti_sgx_mgr_la_LIBADD = $(MSSIM_LIBS)
src_libtcti_sgx_mgr_la_SOURCES = src/tcti-util.cpp src/tcti-sgx-mgr.cpp

# example application & enclave
example/example_application-application.$(OBJEXT): example/enclave_u.h
example_application_CFLAGS = $(AM_CFLAGS) $(SGX_URTS_CFLAGS) \
    -I$(builddir)/example
example_application_LDADD = src/libtcti-sgx-mgr.la $(SGX_URTS_LIBS) -lpthread
example_application_SOURCES = example/application.c example/enclave_ocalls.c
nodist_example_application_SOURCES = example/enclave_u.c
example/enclave.$(OBJECT): example/enclave_t.c

example/.deps:
	mkdir -p $@
example/enclave_%.$(OBJEXT): example/enclave_%.c | example/.deps

example/enclave.so: CFLAGS+=$(ENCLAVE_CFLAGS) -I$(builddir)/example
example/enclave.so: LDFLAGS+=$(SGX_LIBS_ONLY_L) $(ENCLAVE_LDFLAGS)
example/enclave.so: LDLIBS+=-Wl,--whole-archive -l$(SGX_TRTS_LIB) \
    -Wl,--no-whole-archive -Wl,--start-group -lsgx_tstdc -lsgx_tcrypto \
    -l$(SGX_TSERVICE_LIB) -Wl,--end-group
example/enclave.so: example/enclave_t.$(OBJEXT) example/enclave.$(OBJEXT) src/libtss2-tcti-sgx.a
	$(CC) $(LDFLAGS) $^ -o $@ -Bstatic $(LDLIBS)

example/rsa3k.pem:
	openssl genrsa -3 -out $@ 3072

example/enclave-signed.so: example/enclave.so example/rsa3k.pem
	$(SGX_SIGN_BIN) sign -key example/rsa3k.pem -enclave $< -out $@ -config $(srcdir)/example/example.config.xml

# unit tests
test_libtest_a_CFLAGS = $(CMOCKA_CFLAGS) $(AM_CFLAGS) $(CODE_COVERAGE_CFLAGS)
test_libtest_a_SOURCES = test/tcti-sgx_null-wraps.c test/tcti-sgx-common.c

test_tcti_sgx_init_param_tests_CFLAGS  = $(CMOCKA_CFLAGS) $(AM_CFLAGS) \
    $(CODE_COVERAGE_CFLAGS)
test_tcti_sgx_init_param_tests_LDADD = src/libtss2-tcti-sgx.a \
    test/libtest.a $(CMOCKA_LIBS) $(CODE_COVERAGE_LIBS) $(MSSIM_LIBS)
test_tcti_sgx_init_param_tests_LDFLAGS = $(CMOCKA_WRAPS)
test_tcti_sgx_init_param_tests_SOURCES = test/tcti-sgx-init-param-tests.c

test_tcti_sgx_mgr_init_callback_CXXFLAGS = $(AM_CXXFLAGS) \
    $(CMOCKA_CFLAGS) $(CODE_COVERAGE_CFLAGS)
test_tcti_sgx_mgr_init_callback_LDADD = src/libtcti-sgx-mgr.a \
    $(CMOCKA_LIBS) $(CODE_COVERAGE_LIBS) $(MSSIM_LIBS)
test_tcti_sgx_mgr_init_callback_SOURCES = \
    test/tcti-sgx-mgr-init-callback.cpp

test_tcti_sgx_mgr_init_null_callback_CXXFLAGS = $(AM_CXXFLAGS) \
    $(CMOCKA_CFLAGS) $(CODE_COVERAGE_CFLAGS)
test_tcti_sgx_mgr_init_null_callback_LDADD = src/libtcti-sgx-mgr.a \
    $(CMOCKA_LIBS) $(CODE_COVERAGE_LIBS) $(MSSIM_LIBS)
test_tcti_sgx_mgr_init_null_callback_SOURCES = \
    test/tcti-sgx-mgr-init-null-callback.cpp

test_tcti_sgx_mgr_init_userdata_CXXFLAGS = $(AM_CXXFLAGS) \
    $(CMOCKA_CFLAGS) $(CODE_COVERAGE_CFLAGS)
test_tcti_sgx_mgr_init_userdata_LDADD = src/libtcti-sgx-mgr.a \
    $(CMOCKA_LIBS) $(CODE_COVERAGE_LIBS) $(MSSIM_LIBS)
test_tcti_sgx_mgr_init_userdata_SOURCES = test/tcti-sgx-mgr-init-userdata.cpp

test_tcti_sgx_mgr_init_tests_CXXFLAGS = $(AM_CXXFLAGS) \
    $(CMOCKA_CFLAGS) $(CODE_COVERAGE_CFLAGS) $(MSSIM_LIBS)
test_tcti_sgx_mgr_init_tests_LDADD = src/libtcti-sgx-mgr.a $(CMOCKA_LIBS) \
    $(CODE_COVERAGE_LIBS) $(MSSIM_LIBS) -lstdc++
test_tcti_sgx_mgr_init_tests_LDFLAGS = -Wl,--wrap=calloc,--wrap=open \
    -Wl,--wrap=read,--wrap=free
test_tcti_sgx_mgr_init_tests_SOURCES = test/tcti-sgx-mgr-init-tests.cpp

test_tcti_sgx_mgr_ocall_tests_CXXFLAGS = $(AM_CXXFLAGS) \
    $(CMOCKA_CFLAGS) $(CODE_COVERAGE_CFLAGS) $(MSSIM_LIBS)
test_tcti_sgx_mgr_ocall_tests_LDADD = src/libtcti-sgx-mgr.a $(CMOCKA_LIBS) \
    $(CODE_COVERAGE_LIBS) $(MSSIM_LIBS) -lstdc++
test_tcti_sgx_mgr_ocall_tests_SOURCES = test/tcti-sgx-mgr-ocall-tests.cpp

test_tcti_sgx_struct_tests_CFLAGS  = $(CMOCKA_CFLAGS) $(AM_CFLAGS) \
    $(CODE_COVERAGE_CFLAGS)
test_tcti_sgx_struct_tests_LDADD = src/libtss2-tcti-sgx.a \
    test/libtest.a $(CMOCKA_LIBS) $(CODE_COVERAGE_LIBS) $(MSSIM_LIBS)
test_tcti_sgx_struct_tests_LDFLAGS = $(CMOCKA_WRAPS)
test_tcti_sgx_struct_tests_SOURCES = test/tcti-sgx-struct-tests.c

test_tcti_sgx_call_tests_CFLAGS  = $(CMOCKA_CFLAGS) $(AM_CFLAGS) \
    $(CODE_COVERAGE_CFLAGS) 
test_tcti_sgx_call_tests_LDADD = src/libtss2-tcti-sgx.a test/libtest.a \
    $(CMOCKA_LIBS) $(CODE_COVERAGE_LIBS) $(MSSIM_LIBS)
test_tcti_sgx_call_tests_LDFLAGS = $(CMOCKA_WRAPS)
test_tcti_sgx_call_tests_SOURCES = test/tcti-sgx-call-tests.c

test_tcti_util_CFLAGS  = $(CMOCKA_CFLAGS) $(AM_CFLAGS) \
    $(CODE_COVERAGE_CFLAGS)
test_tcti_util_LDADD = src/libtcti-sgx-mgr.a \
    $(CMOCKA_LIBS) $(CODE_COVERAGE_LIBS) $(MSSIM_LIBS)
test_tcti_util_LDFLAGS = -Wl,--wrap=Tss2_Tcti_Mssim_Init,--wrap=calloc,--wrap=free
test_tcti_util_SOURCES = test/tcti-util.c

AUTHORS :
	git log --format='%aN <%aE>' | grep -v 'users.noreply.github.com' | sort | \
	    uniq -c | sort -nr | sed 's/^\s*//' | cut -d" " -f2- > $@

ENCLAVE_SEARCH_PATH = --search-path $(srcdir)/src \
    --search-path $(SGX_INCLUDE_DIR)

%_u.h %_u.c : %.edl
	$(SGX_EDGER8R_BIN) --untrusted $(ENCLAVE_SEARCH_PATH) \
	    --untrusted-dir ${subst $(srcdir),$(builddir),$(dir $^)} $^
%_t.h %_t.c : %.edl
	$(SGX_EDGER8R_BIN) --trusted $(ENCLAVE_SEARCH_PATH) \
	    --trusted-dir ${subst $(srcdir),$(builddir),$(dir $^)} $^
