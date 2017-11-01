/**
 * \file mgncs.h
 * \author
 * \date 2009/02
 *
 * This file includes the common structure and API of ncs.
 *
 \verbatim

    Copyright (C) 2009 Feynman Software.

    All rights reserved by Feynman Software.

    This file is part of mgncs, which is new control
    set of MiniGUI.

  	\endverbatim
 */


/**
 * $Id:$
 *
 *      Copyright (C) 2009 Feynman Software.
 */

#ifndef _MGNCS_H
#define _MGNCS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

#ifndef MGNCS_MAJOR_VERSION
# ifdef __MGNCS_LIB__
# if defined(__CMAKE_PROJECT__) || defined(WIN32)
#    include "mgncsconfig.h"
# else
#    include "../mgncsconfig.h"
# endif
#else
#    include "mgncsconfig.h"
#    undef PACKAGE
#    undef VERSION
#    undef PACKAGE_BUGREPORT
#    undef PACKAGE_NAME
#    undef PACKAGE_STRING
#    undef PACKAGE_TARNAME
#    undef PACKAGE_VERSION
#endif
#endif

#include "mdblist.h"
#include "mcommon.h"
#include "mtype.h"
#include "mem-slab.h"
#include "hashtable.h"
#include "mobject.h"

#include "mcomponent.h"
#include "mwidget.h"
#include "mpopmenumgr.h"
#include "mtoolitem.h"

#include "piece.h"
#include "mtoolbaritems.h"

#include "mscroll_widget.h"

#include "mpanel.h"

#ifdef _MGNCSCTRL_CONTAINER
#include "mcontainer.h"
#endif

#include "mstatic.h"
#include "mimage.h"

#ifdef _MGNCSCTRL_RECTANGLE
#include "mrect.h"
#endif

#include "mgroupbox.h"
#include "mbuttongroup.h"
#include "mbutton.h"
#include "mcheckbutton.h"
#include "mradiobutton.h"

#ifdef _MGNCSCTRL_MENUBUTTON
#include "mmenubutton.h"
#endif

#include "mimagebutton.h"

#ifdef _MGNCSCTRL_PROGRESSBAR
#include "mprogressbar.h"
#endif

#ifdef _MGNCSCTRL_SPINNER
#include "mspinner.h"
#endif

#ifdef _MGNCSCTRL_SPINBOX
#include "mspinbox.h"
#endif

#ifdef _MGNCSCTRL_SLIDER
#include "mslider.h"
#endif

#ifdef _MGNCSCTRL_TRACKBAR
#include "mtrackbar.h"
#endif

#ifdef _MGNCSCTRL_PAGE
#include "mpage.h"
#endif

#ifdef _MGNCSCTRL_PROPSHEET
#include "mpropsheet.h"
#endif

#include "mmainwnd.h"

#ifdef _MGNCSCTRL_DIALOGBOX
#include "mdialog.h"
#endif

#include "mdblist.h"
#include "mitem.h"
#include "mitem_manager.h"
#include "mitemview.h"

#ifdef _MGNCSCTRL_ICONVIEW
#include "miconview.h"
#endif

#include "mscrollview.h"

#include "mlistcolumn.h"
#include "mlistitem.h"

#ifdef _MGNCSCTRL_LISTVIEW
#include "mlistview.h"
#endif

#ifdef _MGNCSCTRL_LISTBOX
#include "mlistbox.h"
#endif

#ifdef _MGNCSCTRL_SCROLLBAR
#include "mscrollbar.h"
#endif

#ifdef _MGNCSCTRL_MONTHCALENDAR
#include "mmonthcalendar.h"
#endif

#ifdef _MGNCSCTRL_TOOLBAR
#include "mtoolbar.h"
#endif

#include "minvisible-component.h"
#include "mtimer.h"

#include "medit.h"
#include "msledit.h"

#ifdef _MGNCSCTRL_ANIMATE
#include "manimate.h"
#endif

#include "mmledit.h"

#ifdef _MGNCSCTRL_COMBOBOX
#include "mcombobox.h"
#endif

#ifdef _MGNCSCTRL_LEDLABEL
#include "mledstatic.h"
#endif

#ifdef _MGNCSCTRL_COLORBUTTON
#include "mcolorbutton.h"
#endif

#include "mseparator.h"

#include "mnode.h"
#include "mchecknode.h"
#include "mradionode.h"
#include "mabstractlist.h"
#include "mlist_layout.h"

#ifdef _MGNCSCTRL_LIST
#include "mlist.h"
#endif

#include "mrdr.h"

#ifdef _MGNCSDB_DATASOURCE
#include "mdatabinding.h"
#include "mdatasource.h"
#endif

#ifdef _MGNCSCTRL_TEXTEDITOR
#include "mbtree.h"
#include "mtexteditbase.h"
#include "mtextedit.h"
#endif

#include "mresmgr.h"

#ifdef _MGNCSENGINE_IME
#include "mime.h"
#endif

#ifdef _MGNCSCTRL_IMWORDSEL
#include "mime-wordsel.h"
#endif

typedef  mMainWnd* (*PNCS_CREATE_MAIN)(HPACKAGE, HWND hParent, HICON h_icon, HMENU h_menu, DWORD);

#ifdef _MGRM_THREADS
MGNCS_EXPORT mMainWnd* ncsSyncCallCreate(HWND host, PNCS_CREATE_MAIN pCreateMain, \
		HPACKAGE hPack, \
		HICON h_icon, \
		HMENU h_menu, \
		DWORD user);
#else


#define ncsSyncCallCreate(host, pCreate, hPack, h_icon, h_menu, user) \
	((PNCS_CREATE_MAIN)(pCreate))(hPack, host, h_icon, h_menu, user)

#endif



/**
 * \fn BOOL ncsInitialize(void)
 * \brief Register all controls and other classes in the MGNCS.
 */
MGNCS_EXPORT BOOL ncsInitialize(void);

/**
 * \fn BOOL ncsInitialize(void)
 * \brief Unregister all controls and other classes in the MGNCS.
 */
MGNCS_EXPORT void ncsUninitialize(void);

#ifdef __cplusplus
}
#endif

#endif

