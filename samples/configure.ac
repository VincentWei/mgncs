#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([mgncs-samples],[1.0.9])
AC_CONFIG_SRCDIR([src/helloworld.c])

dnl ========================================================================
dnl needed for cross-compiling
AC_CANONICAL_SYSTEM

dnl ========================================================================
# Checks for programs.
AC_PROG_MAKE_SET
AC_PROG_CC

AM_INIT_AUTOMAKE

dnl ========================================================================
dnl Put the name and version of the package here

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC

# Checks for system functions.
AC_FUNC_MALLOC
AC_FUNC_VPRINTF

# Checks for libraries.

PKG_CHECK_MODULES([MINIGUI], [minigui >= 3.0.10])
PKG_CHECK_MODULES([MGNCS], [mgncs >= 1.0.6])

dnl ========================================================================
dnl check for run-time mode of MiniGUI
dnl ========================================================================
threads_version="no"
AC_CHECK_DECLS(_MGRM_THREADS, threads_version="yes", foo=bar, [#include <minigui/common.h>])
procs_version="no"
AC_CHECK_DECLS(_MGRM_PROCESSES, procs_version="yes", foo=bar, [#include <minigui/common.h>])
standalone_version="no"
AC_CHECK_DECLS(_MGRM_STANDALONE, standalone_version="yes", foo=bar, [#include <minigui/common.h>])

CFLAGS="$CFLAGS -D_REENTRANT"
if test "$ac_cv_prog_gcc" = "yes"; then
    CFLAGS="$CFLAGS -Wall -Wstrict-prototypes -pipe"
fi

APP_LIBS="$APP_LIBS $MGNCS_LIBS $MINIGUI_LIBS"

AC_SUBST(APP_LIBS)

AC_CONFIG_FILES([
    Makefile
    src/Makefile
    src/res/Makefile
])

AC_OUTPUT

