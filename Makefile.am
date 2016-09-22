include sgx_sdk.mk

ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = $(SAPI_CFLAGS) -I$(SGX_INCLUDE_PATH) -I$(srcdir)/src -I$(srcdir)/include -I$(srcdir)/src/include -fPIC
CLEANFILES = \
    src/tss2_tcti_sgx_t.h \
    src/tss2_tcti_sgx_t.c \
    src/tss2_tcti_sgx_u.h \
    src/tss2_tcti_sgx_u.c
BUILT_SOURCES = $(CLEANFILES)

# things we're building
lib_LTLIBRARIES = \
    src/libtss2-tcti-sgx.la \
    src/libtss2-tcti-sgx-mgr.la

# connect unit tests to the test harness
check_PROGRAMS  = \
    test/tss2-tcti-sgx-init-param-tests \
    test/tss2-tcti-sgx-struct-tests \
    test/tss2-tcti-sgx-call-tests
TESTS = $(check_PROGRAMS)

# headers and where to install them
libtss2_tcti_sgxdir      = $(includedir)
libtss2_tcti_sgx_HEADERS = $(srcdir)/include/tss2-tcti-sgx.h

CMOCKA_WRAPS = -Wl,--wrap=tss2_tcti_sgx_init_ocall \
    -Wl,--wrap=tss2_tcti_sgx_transmit_ocall \
    -Wl,--wrap=tss2_tcti_sgx_receive_ocall \
    -Wl,--wrap=tss2_tcti_sgx_finalize_ocall \
    -Wl,--wrap=tss2_tcti_sgx_cancel_ocall \
    -Wl,--wrap=tss2_tcti_sgx_set_locality_ocall

# code covear
@CODE_COVERAGE_RULES@
CODE_COVERAGE_IGNORE_PATTERN = "/usr/*" "$(abs_top_srcdir)/test/*" \
   "$(abs_top_srcdir)/src/tss2_tcti_sgx_t.*" \
   "$(abs_top_srcdir)/src/tss2_tcti_sgx_u.*"

# enclave library
src_libtss2_tcti_sgx_la_CFLAGS  = $(AM_CFLAGS) $(ENCLAVE_CFLAGS)
src_libtss2_tcti_sgx_la_LDFLAGS = -static $(ENCLAVE_LDFLAGS) -Wl,--no-undefined \
    -Wl,--version-script=src/tss2-tcti-sgx.map
src_libtss2_tcti_sgx_la_LDLIBS  = $(ENCLAVE_LDLIBS)
src_libtss2_tcti_sgx_la_SOURCES = src/tss2-tcti-sgx.c src/tss2-tcti-sgx.map \
    src/tss2_tcti_sgx_t.c src/tss2_tcti_sgx_t.h

# application library
src_libtss2_tcti_sgx_mgr_la_CFLAGS  = $(AM_CFLAGS) $(GLIB_CFLAGS)
src_libtss2_tcti_sgx_mgr_la_LDFLAGS = -Wl,--version-script=src/tss2-tcti-sgx-mgr.map
src_libtss2_tcti_sgx_mgr_la_LDLIBS  = $(GLIB_LIBS)
src_libtss2_tcti_sgx_mgr_la_SOURCES = src/tss2-tcti-sgx-mgr.c \
    src/tss2-tcti-sgx-mgr.map src/tss2_tcti_sgx_u.c

# unit tests
test_tss2_tcti_sgx_init_param_tests_CFLAGS  = $(CMOCKA_CFLAGS) $(AM_CFLAGS) \
    $(CODE_COVERAGE_CFLAGS) -I/opt/intel/sgxsdk/include
test_tss2_tcti_sgx_init_param_tests_LDADD   = $(CMOCKA_LIBS) \
    $(CODE_COVERAGE_LIBS)
test_tss2_tcti_sgx_init_param_tests_LDFLAGS = $(CMOCKA_WRAPS)
test_tss2_tcti_sgx_init_param_tests_SOURCES = \
    test/tss2-tcti-sgx-init-param-tests.c \
    test/tss2-tcti-sgx_null-wraps.c \
    src/tss2-tcti-sgx.c
test_tss2_tcti_sgx_struct_tests_CFLAGS  = $(CMOCKA_CFLAGS) $(AM_CFLAGS) \
    $(CODE_COVERAGE_CFLAGS)
test_tss2_tcti_sgx_struct_tests_LDADD   = $(CMOCKA_LIBS) $(CODE_COVERAGE_LIBS)
test_tss2_tcti_sgx_struct_tests_LDFLAGS = $(CMOCKA_WRAPS)
test_tss2_tcti_sgx_struct_tests_SOURCES = test/tss2-tcti-sgx-struct-tests.c \
    test/tss2-tcti-sgx_null-wraps.c test/tss2-tcti-sgx-common.c \
    src/tss2-tcti-sgx.c
test_tss2_tcti_sgx_call_tests_CFLAGS  = $(CMOCKA_CFLAGS) $(AM_CFLAGS) \
    $(CODE_COVERAGE_CFLAGS) 
test_tss2_tcti_sgx_call_tests_LDADD   = $(CMOCKA_LIBS) $(CODE_COVERAGE_LIBS)
test_tss2_tcti_sgx_call_tests_LDFLAGS = $(CMOCKA_WRAPS)
test_tss2_tcti_sgx_call_tests_SOURCES = test/tss2-tcti-sgx-call-tests.c \
    test/tss2-tcti-sgx_null-wraps.c test/tss2-tcti-sgx-common.c \
    src/tss2-tcti-sgx.c
