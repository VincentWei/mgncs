#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([mgncs-samples],[1.0.8])
AC_CONFIG_SRCDIR([src/helloworld.c])

dnl ========================================================================
dnl needed for cross-compiling
AC_CANONICAL_SYSTEM

dnl ========================================================================
# Checks for programs.
AC_PROG_MAKE_SET
AC_PROG_CC
#AC_PROG_LD

AM_INIT_AUTOMAKE

dnl ========================================================================
dnl Put the name and version of the package here

# Checks for libraries.

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_VPRINTF

dnl ========================================================================
dnl check for run-time mode of MiniGUI
dnl ========================================================================

have_libminigui="no"
AC_CHECK_HEADERS(minigui/common.h, have_libminigui="yes", foo=bar)

have_libmgncs="no"
AC_CHECK_HEADERS(mgncs/mgncs.h, have_libmgncs="yes", foo=bar, 
[[#ifdef HAVE_MINIGUI_COMMON_H
 # include <minigui/common.h>
 # include <minigui/minigui.h>
 # include <minigui/gdi.h>
 # include <minigui/window.h>
 #endif
 ]])

threads_version="no"
AC_CHECK_DECLS(_MGRM_THREADS, threads_version="yes", foo=bar, [#include <minigui/common.h>])

procs_version="no"
AC_CHECK_DECLS(_MGRM_PROCESSES, procs_version="yes", foo=bar, [#include <minigui/common.h>])

standalone_version="no"
AC_CHECK_DECLS(_MGRM_STANDALONE, standalone_version="yes", foo=bar, [#include <minigui/common.h>])

if test "x$threads_version" = "xyes"; then
   CFLAGS="$CFLAGS -D_REENTRANT"
   APP_LIBS="$APP_LIBS -lminigui_ths -lpthread"
else
    if test "x$standalone_version" = "xyes"; then
        APP_LIBS="$APP_LIBS -lminigui_sa"
    else
        APP_LIBS="$APP_LIBS -lminigui_procs"
    fi  
fi

AC_CHECK_DECLS(_HAVE_MATH_LIB, APP_LIBS="$APP_LIBS -lm", foo=bar, [#include <minigui/common.h>])
AC_CHECK_DECLS(_MGIMAGE_PNG, APP_LIBS="$APP_LIBS -lpng", foo=bar, [#include <minigui/common.h>])
AC_CHECK_DECLS(_MGIMAGE_JPG, APP_LIBS="$APP_LIBS -ljpeg", foo=bar, [#include <minigui/common.h>])
AC_CHECK_DECLS(_MGFONT_TTF, APP_LIBS="$APP_LIBS -lttf", foo=bar, [#include <minigui/common.h>])
AC_CHECK_DECLS(_MGFONT_FT2, APP_LIBS="$APP_LIBS -lfreetype", foo=bar, [#include <minigui/common.h>])
AC_CHECK_DECLS(_MGIAL_DLCUSTOM, APP_LIBS="$APP_LIBS -ldl", foo=bar, [#include <minigui/common.h>])

if test "$ac_cv_prog_gcc" = "yes"; then
    CFLAGS="$CFLAGS -Wall -Wstrict-prototypes -pipe"
fi

AC_SUBST(APP_LIBS)

AC_CONFIG_FILES([
Makefile
src/Makefile
src/res/Makefile
])

AC_OUTPUT

if test "x$have_libminigui" != "xyes"; then
    AC_MSG_WARN([
        MiniGUI is not properly installed on the system. You need 
        MiniGUI v3.0.3 or later for building this package. 
        Please install MiniGUI v3.0.3 first.
    ])
fi

if test "x$have_libmgncs" != "xyes"; then
    AC_MSG_WARN([
        mGNCS is not properly installed on the system. You need 
        mGNCS v1.0 or later for building this package. 
        Please install mGNCS v1.0 first.
    ])
fi

