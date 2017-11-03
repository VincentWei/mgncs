AM_CPPFLAGS=-D__MGNCS_LIB__ -I$(abs_top_srcdir)/include -I. -I$(abs_top_srcdir)

noinst_LTLIBRARIES=libclassic.la

COMMON_SRCS=classic.c \
	classic_widget.c \
	classic_combobox.c \
	classic_groupbox.c \
	classic_propsheet.c \
	classic_listview.c \
	classic_boxpieces.c

libclassic_la_LIBADD= 
libclassic_la_SOURCES = $(COMMON_SRCS)
libclassic_la_LDFLAGS=

EXTRA_DIST=makefile.ng
CLEANFILES=*.a

#for makefile.ng
SRC_FILES=$(libclassic_la_SOURCES)
LIB_NAME=libclassic
