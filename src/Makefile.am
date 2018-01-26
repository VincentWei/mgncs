SUBDIRS=renderer pieces datasource ime

lib_LTLIBRARIES=libmgncs@MGNCS_LIBSUFFIX@.la

AM_CPPFLAGS=-D__MGNCS_LIB__ -I$(abs_top_srcdir)/include -I. -I$(abs_top_srcdir)

COMMON_SRCS= mobject.c mcomponent.c mwidget.c mresmanager.c \
			mstatic.c mimage.c mrect.c mgroupbox.c mbuttongroup.c \
			mbutton.c mcheckbutton.c mradiobutton.c mimagebutton.c \
			mctrls.c mscroll_widget.c mpanel.c mprogressbar.c \
			mitem.c mitem_manager.c mitemview.c mlistbox.c \
			mcombobox.c mspinbox.c mspinner.c mslider.c \
			mtrackbar.c mcontainer.c mpage.c mpropsheet.c \
			mcommon.c mmainwnd.c mdialog.c minvisible-component.c \
			mem-slab.c mtimer.c medit.c msledit.c mscrollview.c \
			miconview.c mlistcolumn.c mlistitem.c mlistview.c \
			mmledit.c  mscrollbar.c  \
			mmonthcalendar.c \
			mpopmenumgr.c \
			mtoolitem.c \
			mtoolbar.c \
			mmenubutton.c \
			mledstatic.c \
			mcolorbutton.c \
			manimate.c manimateframes.c mgifanimateframes.c mbmparrayanimateframes.c \
			mseparator.c \
			mdatabinding.c \
			mdatasource.c \
			mtype.c \
			mime-wordsel.c \
			mnode.c mradionode.c mchecknode.c mabstractlist.c mlist.c mlist_layout.c

RENDER_SRCS=mrdr.c hashtable.c

TEXTEITOR_SRCS=mbtree.c mtextedit.c

libmgncs@MGNCS_LIBSUFFIX@_la_LIBADD = @DEP_LIBS@ \
                        renderer/classic/libclassic.la \
						renderer/skin/libskin.la \
						renderer/fashion/libfashion.la \
						renderer/flat/libflat.la \
						pieces/libpieces.la \
						datasource/libdatasource.la \
						ime/libime.la
libmgncs@MGNCS_LIBSUFFIX@_la_SOURCES = $(COMMON_SRCS) $(RENDER_SRCS) $(TEXTEITOR_SRCS)
libmgncs@MGNCS_LIBSUFFIX@_la_LDFLAGS = -release $(LT_RELEASE) -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

EXTRA_DIST= uthash.h iconview_content_data.c makefile.ng

CLEANFILES=*.a

#for makefile.ng
SRC_FILES= $(COMMON_SRCS) $(RENDER_SRCS) $(TEXTEITOR_SRCS)
LIB_NAME=libsrc

