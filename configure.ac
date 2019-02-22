# Copyright 2016 - 2018, Intel Corporation
# SPDX-License-Identifier: BSD-2-Clause
AC_INIT([tpm2-tcti-sgx],
        [m4_esyscmd_s([cat ./VERSION])],
        [https://example.org/tpm2-tcti-sgx/bugs],
        [],
        [https://example.org/tpm2-tcti-sgx])
AC_PROG_CC
AC_PROG_CXX
LT_INIT
AM_INIT_AUTOMAKE([foreign subdir-objects] [-Wno-portability])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_MACRO_DIRS([m4])

# ensure SGX SDK environment has been setup
AS_IF([test -z "$SGX_SDK"],[AC_MSG_ERROR([SGX SDK environment not setup.])])

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
        [cmocka >= 1.0],
        [],
        [AC_MSG_ERROR([cmocka version must be > 1.1])])
    ])
AM_CONDITIONAL([UNIT],[test "x$enable_unit" == "xyes"])

# allow caller to specify mode (sim or hw)
AC_ARG_ENABLE(
    [sim],
    [AS_HELP_STRING([--enable-sim],
                    [Build example enclave for SGX simulator (default is yes)])],
    [enable_sim=$enableval],
    [sim=yes])
# Export names of trts & tservice SGX libraries. These are dependent on
# whether or not the component is being linked to run in simulator mode.
AS_IF(
    [test "x$sim" = "xyes"],
    [
    SGX_SIM_SUFFIX="_sim"
    AC_SUBST([SGX_TRTS_LIB],[sgx_trts_sim])
    AC_SUBST([SGX_TSERVICE_LIB],[sgx_tservice_sim])
    ],
    [test "x$sim" = "xno"],
    [
    AC_SUBST([SGX_TRTS_LIB],[sgx_trts])
    AC_SUBST([SGX_TSERVICE_LIB],[sgx_tservice])
    ],
    [AC_MSG_ERROR([bad value for --enable-sim: $sim])])

PKG_CHECK_MODULES([MSSIM],[tss2-tcti-mssim >= 2.0])
PKG_CHECK_MODULES([SGX_URTS],[libsgx_urts$SGX_SIM_SUFFIX >= 2.0])
AX_CODE_COVERAGE

# check for required SGX tools
AC_ARG_VAR([SGX_SIGN_BIN],[SGX signing tool (sgx_sign).])
AC_PATH_PROG([SGX_SIGN_BIN],[sgx_sign])
AS_IF([test -z "$SGX_SIGN_BIN"],
      [AC_MSG_ERROR([sgx_sign tool not found, source SGX SDK 'environment' script])])
AC_ARG_VAR([SGX_EDGER8R_BIN],[SGX signing tool (sgx_sign).])
AC_PATH_PROG([SGX_EDGER8R_BIN],[sgx_edger8r])
AS_IF([test -z "$SGX_EDGER8R_BIN"],
      [AC_MSG_ERROR([sgx_edger8r tool not found, source SGX SDK 'environment' script])])

# check for required SGX headers, export INCLUDE path
AC_ARG_VAR([SGX_INCLUDE_DIR],[SGX SDK include directory])
AS_IF([test -z $SGX_INCLUDE_DIR],
      [SGX_INCLUDE_DIR="$SGX_SDK/include"])
AC_ARG_VAR([SGX_TLIBC_INCLUDE_DIR],[SGX SDK TLIBC include directory])
AS_IF([test -z "$SGX_TLIBC_INCLUDE_DIR"],
      [SGX_TLIBC_INCLUDE_DIR="$SGX_INCLUDE_DIR/tlibc"])
# adding SGX_INCLUDE_DIR to CFLAGS is awkward but necessary
CFLAGS_TMP="$CFLAGS"
CFLAGS="-I$SGX_INCLUDE_DIR -I$SGX_TLIBC_INCLUDE_DIR"
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

dnl ADD_COMPILER_FLAG:
dnl   A macro to add a compiler flags to the provided variable. This macro
dnl   will check to be sure the compiler supports the flag. Unsupported
dnl   flags will cause the configure script to fail.
dnl $1: C compiler flag to check and add.
dnl $2: Add flags to this variable.
AC_DEFUN([default_cflag],[m4_default([$1],[EXTRA_CFLAGS])])
AC_DEFUN(
    [ADD_COMPILER_FLAG],
    [AX_CHECK_COMPILE_FLAG(
        [$1],
        [
            default_cflag([$2])="$default_cflag([$2]) $1"
            AC_SUBST(default_cflag([$2]))
        ],
        [AC_MSG_ERROR([Unsupported compile flag: $1])],
        [-Werror]
    )]
)
dnl ADD_CXX_COMPILER_FLAG:
dnl   A macro to add a compiler flags to the provided variable. This macro
dnl   will check to be sure the compiler supports the flag. Unsupported
dnl   flags will cause the configure script to fail.
dnl $1: C++ compiler flag to check and add.
dnl $2: Add flags to this variable.
AC_DEFUN([default_cxxflag],[m4_default([$1],[EXTRA_CXXFLAGS])])
AC_DEFUN(
    [ADD_CXX_COMPILER_FLAG],
    [AC_LANG_PUSH([C++])
     AX_CHECK_COMPILE_FLAG(
        [$1],
        [
            default_cxxflag([$2])="$default_cxxflag([$2]) $1"
            AC_SUBST(default_cxxflag([$2]))
        ],
        [AC_MSG_ERROR([Unsupported compile flag: $1])],
        [-Werror])
     AC_LANG_POP([C++])
    ]
)

ADD_COMPILER_FLAG([-fPIC])
ADD_COMPILER_FLAG([-Wall])
ADD_COMPILER_FLAG([-Werror])
ADD_CXX_COMPILER_FLAG([-std=c++11])
AS_IF([test "$CODE_COVERAGE_ENABLED" = "no"],
      [ADD_COMPILER_FLAG([-fvisibility=hidden])])
# CFLAGS used when building code that runs in the enclave
ADD_COMPILER_FLAG([-fpie],[ENCLAVE_CFLAGS])
ADD_COMPILER_FLAG([-fstack-protector],[ENCLAVE_CFLAGS])

AC_DEFUN([default_ldflag],[m4_default([$1],[EXTRA_LDFLAGS])])
AC_DEFUN(
    [ADD_LINK_FLAG],
    [AX_CHECK_LINK_FLAG(
        [$1],
        [
            default_ldflag([$2])="$default_ldflag([$2]) $1"
            AC_SUBST(default_ldflag([$2]))
        ],
        [AC_MSG_ERROR([Unsupported link flag: $1])],
        [-Werror -Wl,-fuse-ld=gold]
    )]
)
ADD_LINK_FLAG([-Wl,-fuse-ld=gold])
AS_IF([test "$CODE_COVERAGE_ENABLED" = "no"],
      [ADD_LINK_FLAG([-Wl,-z,defs])])
ADD_LINK_FLAG([-Wl,-z,now])
ADD_LINK_FLAG([-Wl,-z,relro])
ADD_LINK_FLAG([-Wl,--no-undefined],[ENCLAVE_LDFLAGS])
ADD_LINK_FLAG([-nostdlib],[ENCLAVE_LDFLAGS])
ADD_LINK_FLAG([-nostartfiles],[ENCLAVE_LDFLAGS])
ADD_LINK_FLAG([-Bstatic],[ENCLAVE_LDFLAGS])
ADD_LINK_FLAG([-Wl,--no-undefined],[ENCLAVE_LDFLAGS])
ADD_LINK_FLAG([-pie],[ENCLAVE_LDFLAGS])
ADD_LINK_FLAG([-Wl,-eenclave_entry],[ENCLAVE_LDFLAGS])
ADD_LINK_FLAG([-Wl,--export-dynamic],[ENCLAVE_LDFLAGS])
ADD_LINK_FLAG([-Wl,--defsym,__ImageBase=0],[ENCLAVE_LDFLAGS])
ADD_LINK_FLAG([-Wl,--rosegment],[ENCLAVE_LDFLAGS])

# we need the path for SGX libraries for static linking
# installer doesn't put these in a standard location :(
AC_ARG_VAR([SGX_LIBS_ONLY_L],[stuff and things])
AS_IF([test -z "$SGX_LIBS_ONLY_L"],
      [SGX_LIBS_ONLY_L=$(pkg-config libsgx_urts_sim --libs-only-L)])

AC_OUTPUT
