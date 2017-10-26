AM_CPPFLAGS=-I$(abs_top_srcdir)/include -I. -I$(abs_top_srcdir)

noinst_LTLIBRARIES=libfashion.la

ENABLE_FASHION=@enable_fashion_rdr@

COMMON_SRCS= fashion_common.c \
	fashion.c \
	fashion_widget.c \
	fashion_groupbox.c \
	fashion_combobox.c \
	fashion_propsheet.c fashion_listview.c \
	fashion_boxpieces.c	

COMMON_LIBADD=
COMMON_LDFLAGS=
	
libfashion_la_LIBADD= $(COMMON_LIBADD)
libfashion_la_SOURCES = $(COMMON_SRCS)
libfashion_la_LDFLAGS= $(COMMON_LDFLAGS)

EXTRA_DIST=fashion_common.h makefile.ng

CLEANFILES=*.a

#for makefile.ng
SRC_FILES=$(libfashion_la_SOURCES)
LIB_NAME=libfashion
