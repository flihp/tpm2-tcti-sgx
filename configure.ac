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
AC_OUTPUT
