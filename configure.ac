m4_define([major_version], [0])
m4_define([minor_version], [1])
m4_define([version_string],
          [major_version.minor_version])

AC_INIT([tss2-tcti-sgx], [version_string])
AC_PROG_CC
LT_INIT
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_FILES([Makefile])
PKG_CHECK_MODULES([CMOCKA],[cmocka >= 1.1])
PKG_CHECK_MODULES([GLIB],[glib-2.0])
PKG_CHECK_MODULES([TSS2_SYS],[tss2-sys >= 2.0])
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

AC_OUTPUT
