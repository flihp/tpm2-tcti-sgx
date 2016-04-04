ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -I$(srcdir)/include -I$(srcdir)/src/include

# things we're building
lib_LTLIBRARIES = src/libtss2tcti_skeleton.la

# connect unit tests to the test harness
check_PROGRAMS  = test/tss2tcti-skeleton-init-param-tests \
                  test/tss2tcti-skeleton-struct-tests
TESTS = $(check_PROGRAMS)

# headers and where to install them
libtss2tcti_skeletondir      = $(includedir)
libtss2tcti_skeleton_HEADERS = $(srcdir)/include/tss2tcti-skeleton.h

# library
src_libtss2tcti_skeleton_la_SOURCES = src/tss2tcti-skeleton.c

# unit tests
test_tss2tcti_skeleton_init_param_tests_CFLAGS  = $(CMOCKA_CFLAGS) $(AM_CFLAGS)
test_tss2tcti_skeleton_init_param_tests_LDADD   = $(CMOCKA_LIBS) $(lib_LTLIBRARIES)
test_tss2tcti_skeleton_init_param_tests_SOURCES = test/tss2tcti-skeleton-init-param-tests.c
test_tss2tcti_skeleton_struct_tests_CFLAGS = $(CMOCKA_CFLAGS) $(AM_CFLAGS)
test_tss2tcti_skeleton_struct_tests_LDADD  = $(CMOCKA_LIBS) $(lib_LTLIBRARIES)
test_tss2tcti_skeleton_struct_tests_SOURCES = test/tss2tcti-skeleton-struct-tests.c
