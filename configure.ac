m4_define([major_version], [0])
m4_define([minor_version], [1])
m4_define([version_string],
          [major_version.minor_version])

AC_INIT([tss2-tcti-sgx], [version_string])
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CC
LT_INIT
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_FILES([Makefile])
PKG_CHECK_MODULES([CMOCKA],[cmocka])
PKG_CHECK_MODULES([GLIB],[glib-2.0])
PKG_CHECK_MODULES([SAPI],[sapi])
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

AC_OUTPUT
