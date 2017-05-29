
INCLUDES=-I$(abs_top_srcdir)/include -I. -I.. -I$(abs_top_srcdir)

noinst_LTLIBRARIES=libime.la

COMMON_SRCS= digit_ime.c \
	mime.c           \
	mpinyin-ime.c    \
	mpti-ime.c 

HEAD_FILES=

libime_la_SOURCES=$(COMMON_SRCS) $(HEAD_FILES)
libime_la_LIBADD=
libime_la_LDFAGS=

EXTRA_DIST=pinyin-idx.c charset-convert.c mgpti_dict.c pinyin-dict.c makefile.ng

#for makefile.ng
SRC_FILES=$(COMMON_SRCS)
LIB_NAME=libime
