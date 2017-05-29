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
#include "mcontainer.h"
#include "mstatic.h"
#include "mimage.h"
#include "mrect.h"
#include "mgroupbox.h"
#include "mbuttongroup.h"
#include "mbutton.h"
#include "mcheckbutton.h"
#include "mradiobutton.h"
#include "mmenubutton.h"
#include "mprogressbar.h"
#include "mspinner.h"
#include "mspinbox.h"
#include "mslider.h"
#include "mtrackbar.h"
#include "mpage.h"
#include "mpropsheet.h"
#include "mmainwnd.h"
#include "mdialog.h"
#include "mdblist.h"
#include "mitem.h"
#include "mitem_manager.h"
#include "mitemview.h"
#include "miconview.h"
#include "mscrollview.h"
#include "mlistcolumn.h"
#include "mlistitem.h"
#include "mlistview.h"
#include "mlistbox.h"
#include "mscrollbar.h"
#include "mmonthcalendar.h"
#include "mtoolbar.h"

#include "minvisible-component.h"
#include "mtimer.h"

#include "medit.h"
#include "msledit.h"
#include "manimate.h"
#include "mmledit.h"
#include "mcombobox.h"
#include "mledstatic.h"
#include "mcolorbutton.h"

#include "mseparator.h"

#include "mnode.h"
#include "mchecknode.h"
#include "mradionode.h"
#include "mabstractlist.h"
#include "mlist_layout.h"
#include "mlist.h"

#include "mrdr.h"

#ifdef _MGNCS_DATASOURCE
#include "mdatabinding.h"
#include "mdatasource.h"
#endif

#ifdef _MGNCS_TEXTEDITOR
#include "mbtree.h"
#include "mtexteditbase.h"
#include "mtextedit.h"
#endif

#include "mresmgr.h"

#include "mime.h"
#include "mime-wordsel.h"

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

