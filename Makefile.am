include sgx_sdk.mk

ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -I$(SGX_INCLUDE_PATH) -I$(srcdir)/include -I$(srcdir)/src/include -fPIC

# things we're building
lib_LTLIBRARIES = src/libtss2-tcti-sgx.la

# connect unit tests to the test harness
check_PROGRAMS  = \
    test/tss2-tcti-sgx-init-param-tests \
    test/tss2-tcti-sgx-struct-tests \
    test/tss2-tcti-sgx-call-tests
TESTS = $(check_PROGRAMS)

# headers and where to install them
libtss2_tcti_sgxdir      = $(includedir)
libtss2_tcti_sgx_HEADERS = $(srcdir)/include/tss2-tcti-sgx.h

# library
src_libtss2_tcti_sgx_la_CFLAGS  = $(AM_CFLAGS)
src_libtss2_tcti_sgx_la_LDFLAGS = -Wl,--no-undefined \
    -Wl,--version-script=src/tss2-tcti-sgx.map
src_libtss2_tcti_sgx_la_SOURCES = src/tss2-tcti-sgx.c src/tss2-tcti-sgx.map

# unit tests
test_tss2_tcti_sgx_init_param_tests_CFLAGS  = $(CMOCKA_CFLAGS) $(AM_CFLAGS)
test_tss2_tcti_sgx_init_param_tests_LDADD   = $(CMOCKA_LIBS) $(lib_LTLIBRARIES)
test_tss2_tcti_sgx_init_param_tests_SOURCES = \
    test/tss2-tcti-sgx-init-param-tests.c
test_tss2_tcti_sgx_struct_tests_CFLAGS = $(CMOCKA_CFLAGS) $(AM_CFLAGS)
test_tss2_tcti_sgx_struct_tests_LDADD  = $(CMOCKA_LIBS) $(lib_LTLIBRARIES)
test_tss2_tcti_sgx_struct_tests_SOURCES = test/tss2-tcti-sgx-struct-tests.c \
    test/tss2-tcti-sgx-common.c
test_tss2_tcti_sgx_call_tests_CFLAGS = $(CMOCKA_CFLAGS) $(AM_CFLAGS)
test_tss2_tcti_sgx_call_tests_LDADD  = $(CMOCKA_LIBS) $(lib_LTLIBRARIES)
test_tss2_tcti_sgx_call_tests_SOURCES = test/tss2-tcti-sgx-call-tests.c \
    test/tss2-tcti-sgx-common.c

TSS2_TCTI_SGX_T_OUT = src/tss2-tcti-sgx_t.h src/tss2-tcti-sgx_t.c
TSS2_TCTI_SGX_U_OUT = src/tss2-tcti-sgx_u.h src/tss2-tcti-sgx_u.c
TSS2_TCTI_SGX_T_OBJ = src/tss2-tcti-sgx_t.o
TSS2_TCTI_SGX_U_OBJ = src/tss2-tcti-sgx_u.o
