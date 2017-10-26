
AM_CPPFLAGS=-I$(abs_top_srcdir)/include -I. -I.. -I$(abs_top_srcdir)

noinst_LTLIBRARIES=libdatasource.la

COMMON_SRCS=mstaticds.c comm_parser.c mtextds.c minids.c msqlds.c mxmlds.c xquery.c

HEAD_FILES=mstaticds.h comm_parser.h mtextds.h minids.h msqlds.h mxmlds.h xquery.h

libdatasource_la_SOURCES=$(COMMON_SRCS) $(HEAD_FILES)
libdatasource_la_LIBADD=
libdatasource_la_LDFLAGS=

EXTRA_DIST=str_expression.c makefile.ng

#for makefile.ng
SRC_FILES=$(COMMON_SRCS)
LIB_NAME=libdatasource
