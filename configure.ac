AC_PREREQ(2.63)
AC_INIT([proccer], [1.0.0], [dan@d3fy.net],, [https://proccer.defiance.technology/])

AC_CONFIG_SRCDIR([src/proccer.h])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([build])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

AC_PROG_CC
AC_PROG_INSTALL

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

AC_C_INLINE

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
