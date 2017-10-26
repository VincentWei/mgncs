
INCLUDES=-I$(abs_top_srcdir)/include -I$(abs_top_srcdir)/include/pieces -I. -I$(abs_top_srcdir) -I..

noinst_LTLIBRARIES=libpieces.la

COMMON_SRCS= mbuttonboxpiece.c \
		mcontainerpiece.c \
		mhotpiece.c \
		piece-event.c \
		mimagepiece.c \
		mlabelpiece.c \
		mrenderablepiece.c \
		mstaticpiece.c \
		mpairpiece.c \
		mabstractbuttonpiece.c \
		mlayoutpiece.c \
		piece.c \
		mpushbuttonpiece.c \
		mimagelabelpiece.c \
		msliderpiece.c \
		mthumbboxpiece.c \
		mtrackbarpiece.c \
		mcheckbuttonpiece.c \
		mcheckboxpiece.c \
		mradioboxpiece.c \
		mscrollbarpiece.c \
		marrowpiece.c \
		mleftarrowpiece.c \
		mrightarrowpiece.c \
		muparrowpiece.c \
		mdownarrowpiece.c \
		mboxlayoutpiece.c \
		mhboxlayoutpiece.c \
		mvboxlayoutpiece.c  \
		marrowbuttonpiece.c	\
		manimatepiece.c \
		mspinboxpiece.c \
		mspinnerpiece.c \
		mhspinboxpiece.c \
		mvspinboxpiece.c \
		mgridboxpiece.c \
		mdaygridpiece.c \
		mmonthpiece.c  \
		mnumspinnedpiece.c \
		mlistspinnedpiece.c \
		mweekheadpiece.c \
		mtoolimgpiece.c \
		mtoolimgitempiece.c \
		mseparatorpiece.c \
		mlinelayoutpiece.c \
		mwidgetwrapperpiece.c \
		mrectpiece.c \
		mprogresspiece.c \
		mscrollthumbboxpiece.c \
		msimplelistpiece.c     \
		mledstaticpiece.c 

libpieces_la_SOURCES= $(COMMON_SRCS)
libpieces_la_LIBADD=
libpieces_la_LDFLAGS=

EXTRA_DIST=makefile.ng

CLEANFILES=*.a

#for makefile.ng
SRC_FILES=$(libpieces_la_SOURCES)
LIB_NAME=libpieces
