AM_CPPFLAGS=-D__MGNCS_LIB__ -I$(abs_top_srcdir)/include -I. -I$(abs_top_srcdir)

noinst_LTLIBRARIES=libflat.la

COMMON_SRCS=flat.c \
	flat_boxpieces.c \
	flat_widget.c \
	flat_combobox.c \
	flat_listview.c \
	flat_propsheet.c \
	flat_groupbox.c

libflat_la_SOURCES=$(COMMON_SRCS)
libflat_la_LIBADD=
libflat_la_LDFLAGS=

EXTRA_DIST=makefile.ng

CLEANFILES=*.a

#for makefile.ng
SRC_FILES=$(libflat_la_SOURCES)
LIB_NAME=libflat
