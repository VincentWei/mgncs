#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.60)
AC_INIT(libmgncs, 1.0.9)
AC_CONFIG_SRCDIR(src/mctrls.c)

mgncs_MAJOR_VERSION=1
mgncs_MINOR_VERSION=0
mgncs_MICRO_VERSION=9
mgncs_INTERFACE_AGE=0
mgncs_BINARY_AGE=0
mgncs_VERSION=$mgncs_MAJOR_VERSION.$mgncs_MINOR_VERSION.$mgncs_MICRO_VERSION

AC_SUBST(mgncs_MAJOR_VERSION)
AC_SUBST(mgncs_MINOR_VERSION)
AC_SUBST(mgncs_MICRO_VERSION)
AC_SUBST(mgncs_INTERFACE_AGE)
AC_SUBST(mgncs_BINARY_AGE)
AC_SUBST(mgncs_VERSION)

AC_DEFINE_UNQUOTED(MGNCS_MAJOR_VERSION, $mgncs_MAJOR_VERSION,
            [Major version of mgncs])
AC_DEFINE_UNQUOTED(MGNCS_MINOR_VERSION, $mgncs_MINOR_VERSION,
            [Minor version of mgncs])
AC_DEFINE_UNQUOTED(MGNCS_MICRO_VERSION, $mgncs_MICRO_VERSION,
            [Micro version of mgncs])
AC_DEFINE_UNQUOTED(MGNCS_INTERFACE_AGE, $mgncs_INTERFACE_AGE,
            [Interface age of mgncs])
AC_DEFINE_UNQUOTED(MGNCS_BINARY_AGE, $mgncs_BINARY_AGE,
            [Binary age of mgncs])

# libtool versioning
LT_RELEASE=$mgncs_MAJOR_VERSION.$mgncs_MINOR_VERSION
LT_CURRENT=`expr $mgncs_MICRO_VERSION - $mgncs_INTERFACE_AGE`
LT_REVISION=$mgncs_INTERFACE_AGE
LT_AGE=`expr $mgncs_BINARY_AGE - $mgncs_INTERFACE_AGE`

AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

dnl ========================================================================
dnl Need for cross-compiling
AC_CANONICAL_SYSTEM

dnl ========================================================================
dnl Init automake
AM_INIT_AUTOMAKE

dnl ========================================================================
# Checks for programs.
AC_PROG_CC
AC_PROG_LD
AC_PROG_RANLIB
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl ========================================================================
dnl Put the name and version of the package here

# Checks for libraries.

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/ioctl.h sys/time.h termio.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_PID_T
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_FUNC_ALLOCA
AC_CHECK_FUNCS([bzero dup2 getcwd memmove memset putenv strchr strdup strerror strstr strndup ftruncate mmap])

AM_CONFIG_HEADER(mgncsconfig.h)

dnl ===============================================
dnl System wide options
have_ctrlsuffix="yes"
enable_rdr_skin="yes"
enable_rdr_fashion="yes"
enable_rdr_flat="yes"
enable_dbedit="yes"
enable_iconv="no"

guibuilder_support="no"
build_ctrl_oldmledit="no"
build_ctrl_texteditor="yes"
build_ctrl_textedittitle="no"

build_datasource="yes"
build_datasource_sqlite="no"
build_datasource_xml="no"

build_datasource_static="yes"
build_datasource_text="yes"
build_datasource_ini="yes"

build_ctrl_animate="yes"
build_ctrl_colorbutton="yes"
build_ctrl_container="yes"
build_ctrl_combobox="yes"
build_ctrl_dialogbox="yes"
build_ctrl_iconview="yes"
build_ctrl_imagebutton="yes"
build_ctrl_imwordsel="yes"
build_ctrl_ledlabel="yes"
build_ctrl_list="yes"
build_ctrl_listbox="yes"
build_ctrl_listview="yes"
build_ctrl_monthcalendar="yes"
build_ctrl_menubutton="yes"
build_ctrl_page="yes"
build_ctrl_progressbar="yes"
build_ctrl_propsheet="yes"
build_ctrl_rectangle="yes"
build_ctrl_spinbox="yes"
build_ctrl_spinner="yes"
build_ctrl_scrollbar="yes"
build_ctrl_slider="yes"
build_ctrl_toolbar="yes"
build_ctrl_trackbar="yes"
build_engine_ime="yes"
build_engine_digit="yes"
build_engine_pti="yes"
build_engine_pinyin="yes"
build_engine_digitpti="yes"
build_engine_digitpy="yes"


AC_ARG_ENABLE(skinrdr, 
[ --enable-skinrdr          include skin renderer <default=yes>],
enable_rdr_skin=$enableval)

AC_ARG_ENABLE(fashionrdr,
[ --enable-fashionrdr       include fashion renderer <default=yes>],
enable_rdr_fashion=$enableval)

AC_ARG_ENABLE(flatrdr,
[ --enable-flatrdr          include flat renderer <default=yes>],
enable_rdr_flat=$enableval)

if test "x$enable_rdr_skin" = "xyes"; then
	AC_DEFINE(_MGNCS_RDR_SKIN, 1,
			[Define if include skin renderer])
fi

if test "x$enable_rdr_fashion" = "xyes"; then
	AC_DEFINE(_MGNCS_RDR_FASHION, 1,
		[Define if include fashion renderer])
fi

if test "x$enable_rdr_flat" = "xyes"; then
	AC_DEFINE(_MGNCS_RDR_FLAT, 1,
		[Define if include flat renderer])
fi

AC_ARG_ENABLE(ctrlsuffix, 
[ --enable-ctrlsuffix       have or not the prefix of ncs name<default=yes>],
have_ctrlsuffix=$enableval)

if test "x$have_ctrlsuffix" = "xyes"; 
then
	AC_DEFINE(_MGNCS_CLASS_SUFFIX, "_",
		[Define the control surfix name])
else
	AC_DEFINE(_MGNCS_CLASS_SUFFIX, "",
		[Define the control surfix name])
fi

AC_ARG_ENABLE(dbedit,
[  --enable-dbedit           use edit with double buffer support <default=yes>],
enable_dbedit=$enableval)

if test "x$enable_dbedit" = "xyes"; then
  AC_DEFINE(_MGNCS_USE_DBEDIT, 1,
            [Define if build NCS for using edit with double buffer ])
fi

AC_ARG_ENABLE(guibuilder-support,
[ --enable-guibuilder-support   new control set used to support guibuilder <default=no>],
guibuilder_support=$enableval)

if test "x$guibuilder_support" = "xyes"; then
	AC_DEFINE(_MGNCS_GUIBUILDER_SUPPORT, 1,
		[Define if support guibuilder])
fi

AC_ARG_ENABLE(ctrlold-mledit, 
[ --enable-ctrlold-mledit support the old multiline editor <default=no>],
build_ctrl_oldmledit=$enableval)

if test "x$build_ctrl_oldmledit" = "xyes"; then
	AC_DEFINE(_MGNCSCTRL_OLD_MLEDIT, 1,
			 [Define if support old multiline editor])
fi

AC_ARG_ENABLE(ctrltexteditor,
[ --enable-ctrltexteditor support the new texteditor <default=yes>],
build_ctrl_texteditor=$enableval)

if test "x$build_ctrl_texteditor" = "xyes"; then
	AC_DEFINE(_MGNCSCTRL_TEXTEDITOR, 1,
			  [Define if support new text editor])
fi

AC_ARG_ENABLE(ctrltextedittitle,
[ --enable-ctrltextedittitle support the title of new texteditor  <default=no>],
build_ctrl_textedittitle=$enableval)

if test "x$build_ctrl_textedittitle" = "xyes"; then
	AC_DEFINE(_MGNCSCTRL_TEXTEDITTITLE, 1,
			  [Define if support title of new text editor])
fi



AC_ARG_WITH(libsuffix,
  [  --with-libsuffix=ARG    configure the suffix of the library name.])

case "$with_libsuffix" in
    "")
        cfgfile="mgncs.cfg"
        MGNCS_LIBSUFFIX=""
        have_libsuffix="no"
    ;;
    *)
        cfgfile="mgncs.cfg"
        MGNCS_LIBSUFFIX="_$withval"
        have_libsuffix="yes"
    ;;
esac

AC_ARG_ENABLE(useiconv,
    [ --enable-useiconv enable the iconv as charset convertor <default=no>],
enable_iconv=$enableval)

if test "x$enable_iconv" = "xyes"; then
    AC_DEFINE(_MGNCS_CHARSET_ICONV, 1,
              [Define if use iconv as charset convertor])
fi

AC_ARG_ENABLE(dbdatasource,
[ --enable-dbdatasource    enable datasource support <default=yes>],
build_datasource=$enableval)

if test "x$build_datasource" = "xyes"; then
	AC_DEFINE(_MGNCSDB_DATASOURCE, 1,
		[Define if support datasource])
fi

AC_ARG_ENABLE(dbsqlite,
[ --enable-dbsqlite     enable sqlite datasource support <default=no>],
build_datasource_sqlite=$enableval)

if test "x$build_datasource_sqlite" = "xyes"; then
    AC_DEFINE(_MGNCSDB_DATASOURCE, 1,
        [Define if support datasource])
    AC_DEFINE(_MGNCSDB_SQLITE, 1,
		[Define if support sqlite])
fi

AC_ARG_ENABLE(dbxml,
[ --enable-dbxml    enable libxml2 datasource support <default=no>],
build_datasource_xml=$enableval)

if test "x$build_datasource_xml" = "xyes"; then
    AC_DEFINE(_MGNCSDB_DATASOURCE, 1,
        [Define if support datasource])
    AC_DEFINE(_MGNCSDB_XML, 1,
		[Define if support xml])
fi

AC_ARG_ENABLE(dbstatic, 
[ --enable-dbstatic         enable the staticdatasource support or not <default=yes>],
build_datasource_static=$enableval)

if test "x$build_datasource_static" = "xyes"; then
    AC_DEFINE(_MGNCSDB_DATASOURCE, 1,
        [Define if support datasource])
    AC_DEFINE(_MGNCSDB_STATIC, 1, 
        [Define if support staticdatasource])
fi

AC_ARG_ENABLE(dbtext, 
[ --enable-dbtext         enable the textdatasource support or not <default=yes>],
build_datasource_text=$enableval)

if test "x$build_datasource_text" = "xyes"; then
    AC_DEFINE(_MGNCSDB_DATASOURCE, 1,
        [Define if support datasource])
    AC_DEFINE(_MGNCSDB_TEXT, 1, 
        [Define if support textdatasource])
fi

AC_ARG_ENABLE(dbini, 
[ --enable-dbini         enable the inidatasource support or not <default=yes>],
build_datasource_ini=$enableval)

if test "x$build_datasource_ini" = "xyes"; then
    AC_DEFINE(_MGNCSDB_DATASOURCE, 1,
        [Define if support datasource])
    AC_DEFINE(_MGNCSDB_INI, 1, 
        [Define if support inidatasource])
fi

AC_ARG_ENABLE(ctrlanimate, 
[ --enable-ctrlanimate         enable the animate support or not <default=yes>],
build_ctrl_animate=$enableval)

if test "x$build_ctrl_animate" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_ANIMATE, 1, 
        [Define if support animate])
fi

AC_ARG_ENABLE(ctrlcolorbutton, 
[ --enable-ctrlcolorbutton         enable the colorbutton support or not <default=yes>],
build_ctrl_colorbutton=$enableval)

if test "x$build_ctrl_colorbutton" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_COLORBUTTON, 1, 
        [Define if support colorbutton])
fi

AC_ARG_ENABLE(ctrlcontainer, 
[ --enable-ctrlcontainer         enable the container support or not <default=yes>],
build_ctrl_container=$enableval)

if test "x$build_ctrl_container" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_CONTAINER, 1, 
        [Define if support container])
fi

AC_ARG_ENABLE(ctrlpage, 
[ --enable-ctrlpage         enable the page support or not <default=yes>],
build_ctrl_page=$enableval)

if test "x$build_ctrl_page" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_CONTAINER, 1, 
        [Define if support container])
    AC_DEFINE(_MGNCSCTRL_PAGE, 1, 
        [Define if support page])
fi

AC_ARG_ENABLE(ctrlpropsheet, 
[ --enable-ctrlpropsheet         enable the propsheet support or not <default=yes>],
build_ctrl_propsheet=$enableval)

if test "x$build_ctrl_propsheet" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_CONTAINER, 1, 
        [Define if support container])
    AC_DEFINE(_MGNCSCTRL_PAGE, 1, 
        [Define if support page])
    AC_DEFINE(_MGNCSCTRL_PROPSHEET, 1, 
        [Define if support propsheet])
fi

AC_ARG_ENABLE(ctrllistbox, 
[ --enable-ctrllistbox         enable the listbox support or not <default=yes>],
build_ctrl_listbox=$enableval)

if test "x$build_ctrl_listbox" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_LISTBOX, 1, 
        [Define if support listbox])
fi

AC_ARG_ENABLE(ctrlcombobox, 
[ --enable-ctrlcombobox         enable the combobox support or not <default=yes>],
build_ctrl_combobox=$enableval)

if test "x$build_ctrl_combobox" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_LISTBOX, 1, 
        [Define if support listbox])
    AC_DEFINE(_MGNCSCTRL_COMBOBOX, 1, 
        [Define if support combobox])
fi

AC_ARG_ENABLE(ctrldialogbox, 
[ --enable-ctrldialogbox         enable the dialogbox support or not <default=yes>],
build_ctrl_dialogbox=$enableval)

if test "x$build_ctrl_dialogbox" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_DIALOGBOX, 1, 
        [Define if support dialogbox])
fi

AC_ARG_ENABLE(ctrliconview, 
[ --enable-ctrliconview         enable the iconview support or not <default=yes>],
build_ctrl_iconview=$enableval)

if test "x$build_ctrl_iconview" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_ICONVIEW, 1, 
        [Define if support iconview])
fi

AC_ARG_ENABLE(ctrlimagebutton, 
[ --enable-ctrlimagebutton         enable the imagebutton support or not <default=yes>],
build_ctrl_imagebutton=$enableval)

if test "x$build_ctrl_imagebutton" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_IMAGEBUTTON, 1, 
        [Define if support imagebutton])
fi

AC_ARG_ENABLE(ctrlledlabel, 
[ --enable-ctrlledlabel         enable the ledlabel support or not <default=yes>],
build_ctrl_ledlabel=$enableval)

if test "x$build_ctrl_ledlabel" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_LEDLABEL, 1, 
        [Define if support ledlabel])
fi

AC_ARG_ENABLE(ctrllist, 
[ --enable-ctrllist         enable the list support or not <default=yes>],
build_ctrl_list=$enableval)

if test "x$build_ctrl_list" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_LIST, 1, 
        [Define if support list])
fi

AC_ARG_ENABLE(ctrllistview, 
[ --enable-ctrllistview         enable the listview support or not <default=yes>],
build_ctrl_listview=$enableval)

if test "x$build_ctrl_listview" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_LISTVIEW, 1, 
        [Define if support listview])
fi

AC_ARG_ENABLE(ctrlmenubutton, 
[ --enable-ctrlmenubutton         enable the menubutton support or not <default=yes>],
build_ctrl_menubutton=$enableval)

if test "x$build_ctrl_menubutton" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_MENUBUTTON, 1, 
        [Define if support menubutton])
fi

AC_ARG_ENABLE(ctrlprogressbar, 
[ --enable-ctrlprogressbar         enable the progressbar support or not <default=yes>],
build_ctrl_progressbar=$enableval)

if test "x$build_ctrl_progressbar" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_PROGRESSBAR, 1, 
        [Define if support progressbar])
fi

AC_ARG_ENABLE(ctrlrectangle, 
[ --enable-ctrlrectangle         enable the rectangle support or not <default=yes>],
build_ctrl_rectangle=$enableval)

if test "x$build_ctrl_rectangle" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_RECTANGLE, 1, 
        [Define if support rectangle])
fi

AC_ARG_ENABLE(ctrlspinner, 
[ --enable-ctrlspinner         enable the spinner support or not <default=yes>],
build_ctrl_spinner=$enableval)

if test "x$build_ctrl_spinner" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_SPINNER, 1, 
        [Define if support spinner])
fi

AC_ARG_ENABLE(ctrlspinbox, 
[ --enable-ctrlspinbox         enable the spinbox support or not <default=yes>],
build_ctrl_spinbox=$enableval)

if test "x$build_ctrl_spinbox" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_SPINNER, 1, 
        [Define if support spinner])
    AC_DEFINE(_MGNCSCTRL_SPINBOX, 1, 
        [Define if support spinbox])
fi

AC_ARG_ENABLE(ctrlmonthcalendar, 
[ --enable-ctrlmonthcalendar         enable the monthcalendar support or not <default=yes>],
build_ctrl_monthcalendar=$enableval)

if test "x$build_ctrl_monthcalendar" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_SPINNER, 1, 
        [Define if support spinner])
    AC_DEFINE(_MGNCSCTRL_SPINBOX, 1, 
        [Define if support spinbox])
    AC_DEFINE(_MGNCSCTRL_MONTHCALENDAR, 1, 
        [Define if support monthcalendar])
fi

AC_ARG_ENABLE(ctrlslider, 
[ --enable-ctrlslider         enable the slider support or not <default=yes>],
build_ctrl_slider=$enableval)

if test "x$build_ctrl_slider" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_SLIDER, 1, 
        [Define if support slider])
fi

AC_ARG_ENABLE(ctrlscrollbar, 
[ --enable-ctrlscrollbar         enable the scrollbar support or not <default=yes>],
build_ctrl_scrollbar=$enableval)

if test "x$build_ctrl_scrollbar" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_SLIDER, 1, 
        [Define if support slider])
    AC_DEFINE(_MGNCSCTRL_SCROLLBAR, 1, 
        [Define if support scrollbar])
fi

AC_ARG_ENABLE(ctrltrackbar, 
[ --enable-ctrltrackbar         enable the trackbar support or not <default=yes>],
build_ctrl_trackbar=$enableval)

if test "x$build_ctrl_trackbar" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_SLIDER, 1, 
        [Define if support slider])
    AC_DEFINE(_MGNCSCTRL_TRACKBAR, 1, 
        [Define if support trackbar])
fi

AC_ARG_ENABLE(ctrltoolbar, 
[ --enable-ctrltoolbar         enable the toolbar support or not <default=yes>],
build_ctrl_toolbar=$enableval)

if test "x$build_ctrl_toolbar" = "xyes"; then
    AC_DEFINE(_MGNCSCTRL_TOOLBAR, 1, 
        [Define if support toolbar])
fi

AC_ARG_ENABLE(engineime, 
[ --enable-engineime         enable the imeengine support or not <default=yes>],
build_engine_ime=$enableval)

if test "x$build_engine_ime" = "xyes"; then
    AC_DEFINE(_MGNCSENGINE_IME, 1, 
        [Define if support imeengine])
fi

AC_ARG_ENABLE(ctrlimwordsel, 
[ --enable-ctrlimwordsel         enable the imwordsel support or not <default=yes>],
build_ctrl_imwordsel=$enableval)

if test "x$build_ctrl_imwordsel" = "xyes"; then
    AC_DEFINE(_MGNCSENGINE_IME, 1, 
        [Define if support imeengine])
    AC_DEFINE(_MGNCSCTRL_IMWORDSEL, 1, 
        [Define if support imwordsel])
fi

AC_ARG_ENABLE(enginepinyin, 
[ --enable-enginepinyin         enable the pinyinengine support or not <default=yes>],
build_engine_pinyin=$enableval)

if test "x$build_engine_pinyin" = "xyes"; then
    AC_DEFINE(_MGNCSENGINE_IME, 1, 
        [Define if support imeengine])
    AC_DEFINE(_MGNCSENGINE_PINYIN, 1, 
        [Define if support pinyinengine])
fi

AC_ARG_ENABLE(enginedigit, 
[ --enable-enginedigit         enable the digitengine support or not <default=yes>],
build_engine_digit=$enableval)

if test "x$build_engine_digit" = "xyes"; then
    AC_DEFINE(_MGNCSENGINE_IME, 1, 
        [Define if support imeengine])
    AC_DEFINE(_MGNCSENGINE_DIGIT, 1, 
        [Define if support digitengine])
fi

AC_ARG_ENABLE(enginepti, 
[ --enable-enginepti         enable the ptiengine support or not <default=yes>],
build_engine_pti=$enableval)

if test "x$build_engine_pti" = "xyes"; then
    AC_DEFINE(_MGNCSENGINE_IME, 1, 
        [Define if support imeengine])
    AC_DEFINE(_MGNCSENGINE_PTI, 1, 
        [Define if support ptiengine])
fi

AC_ARG_ENABLE(enginedigitpti, 
[ --enable-enginedigitpti         enable the digitptiengine support or not <default=yes>],
build_engine_digitpti=$enableval)

if test "x$build_engine_digitpti" = "xyes"; then
    AC_DEFINE(_MGNCSENGINE_IME, 1, 
        [Define if support imeengine])
    AC_DEFINE(_MGNCSENGINE_DIGIT, 1, 
        [Define if support digitengine])
    AC_DEFINE(_MGNCSENGINE_PTI, 1, 
        [Define if support ptiengine])
    AC_DEFINE(_MGNCSENGINE_DIGITPTI, 1, 
        [Define if support digitptiengine])
fi

AC_ARG_ENABLE(enginedigitpy, 
[ --enable-enginedigitpy         enable the digitpyengine support or not <default=yes>],
build_engine_digitpy=$enableval)

if test "x$build_engine_digitpy" = "xyes"; then
    AC_DEFINE(_MGNCSENGINE_IME, 1, 
        [Define if support imeengine])
    AC_DEFINE(_MGNCSENGINE_DIGIT, 1, 
        [Define if support digitengine])
    AC_DEFINE(_MGNCSENGINE_PINYIN, 1, 
        [Define if support pinyinengine])
    AC_DEFINE(_MGNCSENGINE_DIGITPY, 1, 
        [Define if support digitpyengine])
fi


AC_DEFINE_UNQUOTED(MGNCS_ETCFILENAME, "${cfgfile}", [MGNCS configure file name])
AM_CONDITIONAL(MGNCS_HAVE_LIBSUFFIX, test "x$have_libsuffix" = "xyes")
AC_SUBST(MGNCS_LIBSUFFIX)
AC_DEFINE_UNQUOTED(_MGNCS_LIBSUFFIX, "${MGNCS_LIBSUFFIX}", [MGNCS library suffix])


dnl ========================================================================
dnl check for run-time mode of MiniGUI
dnl ========================================================================

have_libminigui="no"
AC_CHECK_HEADERS(minigui/common.h, have_libminigui=yes, foo=bar)

threads_version="no"
AC_CHECK_DECLS(_MGRM_THREADS, threads_version="yes", foo=bar, [#include <minigui/common.h>])

procs_version="no"
AC_CHECK_DECLS(_MGRM_PROCESSES, procs_version="yes", foo=bar, [#include <minigui/common.h>])

standalone_version="no"
AC_CHECK_DECLS(_MGRM_STANDALONE, standalone_version="yes", foo=bar, [#include <minigui/common.h>])


if test "x$threads_version" = "xyes"; then
   	CFLAGS="$CFLAGS -D_REENTRANT"
    if test "x$have_libsuffix" = "xno"; then
        APP_LIBS="$APP_LIBS -lminigui_ths"
    fi
else
	if test "x$standalone_version" = "xyes"; then
        if test "x$have_libsuffix" = "xno"; then
            APP_LIBS="$APP_LIBS -lminigui_sa"
        fi
   	else
        if test "x$have_libsuffix" = "xno"; then
            APP_LIBS="$APP_LIBS -lminigui_procs"
        fi
   	fi  
fi

if test "x$have_libsuffix" = "xyes"; then
    APP_LIBS="$APP_LIBS -lminigui_msd"
fi

if test "x$threads_version" != "xyes"; then
    lib_pthread="yes"
    AC_CHECK_DECLS(_MGGAL_SHADOW, APP_LIBS="$APP_LIBS -lpthread", lib_pthread="no", [#include <minigui/common.h>])
    if test "x$lib_pthread" = "xno"; then
        AC_CHECK_DECLS(_MGGAL_MLSHADOW, APP_LIBS="$APP_LIBS -lpthread", no_pthread="no", [#include <minigui/common.h>])
    fi
else
	APP_LIBS="$APP_LIBS -lpthread"
fi

if test "x$build_datasource_sqlite" = "xyes"; then
	APP_LIBS="$APP_LIBS -lsqlite3"
fi

if test "x$build_datasource_xml" = "xyes"; then
	APP_LIBS="$APP_LIBS -lxml2"
    CFLAGS="$CFLAGS -I/usr/include/libxml2"
fi

CFLAGS="$CFLAGS -D__MGNCS_LIB__"

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

AC_CONFIG_FILES([Makefile mgncs.pc
                 include/Makefile
                 include/pieces/Makefile
                 src/Makefile
                 src/renderer/Makefile
                 src/renderer/classic/Makefile
                 src/renderer/skin/Makefile
                 src/renderer/fashion/Makefile
                 src/renderer/flat/Makefile
                 src/pieces/Makefile
                 src/datasource/Makefile
                 src/ime/Makefile
                 etc/Makefile
                 build/Makefile])

AC_OUTPUT

if test "x$have_libminigui" != "xyes"; then
    AC_MSG_WARN([
        MiniGUI is not properly installed on the system. You need 
        MiniGUI Ver 3.0.10 or later for building this package. 
        Please configure and install MiniGUI Ver 3.0.10 first.
    ])
fi

