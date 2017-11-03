AM_CPPFLAGS=-D__MGNCS_LIB__ -I$(abs_top_srcdir)/include -I. -I$(abs_top_srcdir)

noinst_LTLIBRARIES=libskin.la

COMMON_SRCS=skin.c \
			skin_widget.c \
			skin_boxpieces.c \
			skin_combobox.c \
			skin_groupbox.c \
			skin_propsheet.c skin_listview.c 

libskin_la_LIBADD=
libskin_la_SOURCES = $(COMMON_SRCS)
libskin_la_LDFLAGS=

EXTRA_DIST=makefile.ng

CLEANFILES=*.a

#for makefile.ng
SRC_FILES=$(libskin_la_SOURCES)
LIB_NAME=libskin
