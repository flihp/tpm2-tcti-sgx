# Copyright 2016 - 2018, Intel Corporation
# SPDX-License-Identifier: BSD-2-Clause
AC_INIT([tpm2-tcti-sgx],
        [m4_esyscmd_s([cat ./VERSION])],
        [https://example.org/tpm2-tcti-sgx/bugs],
        [],
        [https://example.org/tpm2-tcti-sgx])
AC_PROG_CC
LT_INIT
AM_INIT_AUTOMAKE([subdir-objects] [-Wno-portability])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_MACRO_DIRS([m4])

# enable / disable unit tests: --[enable|disable]-unit
AC_ARG_ENABLE(
    [unit],
    [AS_HELP_STRING([--enable-unit],
                    [build cmocka unit tests (default is no)])],
    [enable_unit=$enableval],
    [enable_unit=no])
AS_IF(
    [test x"$enable_unit" == "xyes"],
    [
    PKG_CHECK_MODULES(
        [CMOCKA],
        [cmocka >= 1.1],
        [],
        [AC_MSG_ERROR([cmocka version must be > 1.1])])
    ])
AM_CONDITIONAL([UNIT],[test "x$enable_unit" == "xyes"])

PKG_CHECK_MODULES([GLIB],[glib-2.0])
PKG_CHECK_MODULES([TABRMD],[tss2-tcti-tabrmd >= 2.0])
AX_CODE_COVERAGE
# get sizeof void pointer, define variable to let Makefile.am know
AC_CHECK_SIZEOF([void*])
AM_CONDITIONAL([SGX_32BITS],[test x$ac_cv_sizeof_voidp = x4])
AM_CONDITIONAL([SGX_64BITS],[test x$ac_cv_sizeof_voidp = x8])
# allow caller to specify mode (sim or hw)
AC_ARG_ENABLE([sim],
[  --enable-sim    Turn on SGX simulation],
[case "${enableval}" in
  yes) sim=true ;;
  no)  sim=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-sim]) ;;
esac],[sim=true])
AM_CONDITIONAL([SGX_MODE_SIM], [test x$sim = xtrue])
# Location of SGX SDK directory. This requires special handling because it
# disregards many GNU Make / Linux conventions.
AC_ARG_WITH([sgxsdkdir],
            AS_HELP_STRING([--with-sgxsdkdir],
                           [Path to SGX SDK]),
            [with_sgxsdkdir=$withval],
            [with_sgxsdkdir=/opt/intel/sgxsdk])
AC_SUBST([SGX_SDK],[$with_sgxsdkdir])

# check for required SGX tools
AC_ARG_VAR([SGX_SIGN_BIN],[SGX signing tool (sgx_sign).])
AC_PATH_PROG([SGX_SIGN_BIN],[sgx_sign],[],[$PATH:$with_sgxsdkdir/bin/x64])
AS_IF([test -z "$SGX_SIGN_BIN"],
      [AC_MSG_ERROR([sgx_sign tool is required to build example enclave])])
AC_ARG_VAR([SGX_EDGER8R_BIN],[SGX signing tool (sgx_sign).])
AC_PATH_PROG([SGX_EDGER8R_BIN],[sgx_edger8r],[],[$PATH:$with_sgxsdkdir/bin/x64])
AS_IF([test -z "$SGX_EDGER8R_BIN"],
      [AC_MSG_ERROR([sgx_edger8r tool is required to build example enclave])])

# check for required SGX headers, export INCLUDE path
AC_ARG_VAR([SGX_INCLUDE_DIR],[SGX SDK include directory])
AS_IF([test -z $SGX_INCLUDE_DIR],
      [SGX_INCLUDE_DIR="$with_sgxsdkdir/include"])
# adding SGX_INCLUDE_DIR to CFLAGS is awkward but necessary
CFLAGS_TMP="$CFLAGS"
CFLAGS="$CFLAGS -I$SGX_INCLUDE_DIR"
AC_CHECK_HEADER(
    [sgx_error.h],
    [],
    [AC_MSG_ERROR([Couldn't find or include sgx_error.h])],
    [#include <inttypes.h>])
AC_CHECK_HEADER(
    [sgx_urts.h],
    [],
    [AC_MSG_ERROR([Couldn't find or include sgx_urts.h])],
    [#ifdef HAVE_SGX_ERROR_H
     #include <sgx_error.h>
     #endif])
CFLAGS="$CFLAGS_TMP"
AC_OUTPUT
