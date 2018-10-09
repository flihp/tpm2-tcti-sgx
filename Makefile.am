# Copyright 2016 - 2018, Intel Corporation
# SPDX-License-Identifier: BSD-2-Clause
include sgx_sdk.mk

ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = $(TSS2_SYS_CFLAGS) -I$(SGX_INCLUDE_PATH) -I$(srcdir)/src -I$(srcdir)/src/include -fPIC
CLEANFILES = \
    src/tcti_sgx_t.h \
    src/tcti_sgx_t.c \
    src/tcti_sgx_u.h \
    src/tcti_sgx_u.c
BUILT_SOURCES = $(CLEANFILES)

# things we're building
lib_LTLIBRARIES = src/libtcti-sgx-mgr.la
lib_LIBRARIES = src/libtss2-tcti-sgx.a

# connect unit tests to the test harness
check_PROGRAMS  = \
    test/tcti-sgx-init-param-tests \
    test/tcti-sgx-mgr-init-tests \
    test/tcti-sgx-mgr-ocall-tests \
    test/tcti-sgx-struct-tests \
    test/tcti-sgx-call-tests
TESTS = $(check_PROGRAMS)

# headers and where to install them
libtss2_tcti_sgxdir = $(includedir)/tss2
libtss2_tcti_sgx_HEADERS = $(srcdir)/src/include/tss2-tcti-sgx.h

libtcti_sgx_mgrdir = $(includedir)
libtcti_sgx_mgr_HEADERS = $(srcdir)/src/include/tcti-sgx-mgr.h

EXTRA_DIST = \
    src/include/tcti-sgx_priv.h \
    src/tcti_sgx.edl \
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
src_libtss2_tcti_sgx_a_SOURCES = src/tcti-sgx.c src/tcti-sgx.map
nodist_src_libtss2_tcti_sgx_a_SOURCES = src/tcti_sgx_t.c
src/src_libtss2_tcti_sgx_a-tcti-sgx.$(OBJEXT): src/tcti_sgx_t.c

# application library
src_libtcti_sgx_mgr_la_CFLAGS  = $(AM_CFLAGS) $(GLIB_CFLAGS)
src_libtcti_sgx_mgr_la_LDFLAGS = -Wl,--version-script=src/tcti-sgx-mgr.map
src_libtcti_sgx_mgr_la_LDLIBS  = $(GLIB_LIBS)
src_libtcti_sgx_mgr_la_SOURCES = \
    src/tcti-sgx-mgr.c \
    src/tcti-sgx-mgr.map
nodist_src_libtcti_sgx_mgr_la_SOURCES = src/tcti_sgx_u.c
./src/src_libtcti_sgx_mgr_la-tcti-sgx-mgr.lo: src/tcti_sgx_u.c

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
