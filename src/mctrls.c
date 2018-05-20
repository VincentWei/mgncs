/* 
    This file is part of mGNCS, a component for MiniGUI.

    Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"
#include "mem-slab.h"
#include "piece.h"

extern BOOL ncsInitClassicRenderers(void);
#ifdef _MGNCS_RDR_SKIN
extern BOOL ncsInitSkinRenderers(void);
#endif
#ifdef _MGNCS_RDR_FASHION
extern BOOL ncsInitFashionRenderers(void);
extern void ncsUninitFashionRenderers(void);
#endif
#ifdef _MGNCS_RDR_FLAT
extern BOOL ncsInitFlatRenderers(void);
#endif

#ifdef _MGNCSDB_DATASOURCE
extern BOOL ncsInitDefaultDataSource(void);

extern BOOL ncsInitDataBinding(void);
#endif

extern void free_comp_class(void);

#ifdef _MGNCSENGINE_IME
extern BOOL ncsInitIMEClasses(void);
#endif

extern void deinit_widget_info(void);
extern void init_widget_info(void);
extern void init_piece_event_info(void);
extern void deinit_piece_event_info(void);

//////////////////////////////////////////////////////////
static void _init_classes(void)
{
#ifdef _MGNCSDB_DATASOURCE
	ncsInitDataBinding();
#endif
	MGNCS_INIT_CLASS(mObject);
	MGNCS_INIT_CLASS(mComponent);
	//Register mWidget
    MGNCS_INIT_CLASS(mWidget);
    MGNCS_INIT_CLASS(mScrollWidget);
    MGNCS_INIT_CLASS(mItem);
    MGNCS_INIT_CLASS(mItemManager);
    MGNCS_INIT_CLASS(mListColumn);
    MGNCS_INIT_CLASS(mListItem);
    MGNCS_INIT_CLASS(mItemView);

    MGNCS_INIT_CLASS(mPopMenuMgr);

    ncsInitToolItems();

	MGNCS_INIT_CLASS(mInvsbComp);
	MGNCS_INIT_CLASS(mAnimateFrames);
	MGNCS_INIT_CLASS(mGIFAnimateFrames);
	MGNCS_INIT_CLASS(mBmpArrayAnimateFrames);
	//MGNCS_INIT_CLASS(mMemAnimateFrames);

    //for list control series
	MGNCS_INIT_CLASS(mNode);
	MGNCS_INIT_CLASS(mGroupNode);
	MGNCS_INIT_CLASS(mCheckNode);
	MGNCS_INIT_CLASS(mRadioNode);
	MGNCS_INIT_CLASS(mAbstractList);

#ifdef _MGNCSCTRL_LIST
	MGNCS_INIT_CLASS(mListLayout);
	MGNCS_INIT_CLASS(mLVIconLayout);
	MGNCS_INIT_CLASS(mLHIconLayout);
	MGNCS_INIT_CLASS(mLHCenterBoxLayout);
	MGNCS_INIT_CLASS(mLGroupLayout);
#endif

#ifdef _MGNCSDB_DATASOURCE
	ncsInitDefaultDataSource();
#endif

#ifdef _MGNCSENGINE_IME
	ncsInitIMEClasses();
#endif

}

#ifdef _MGRM_THREADS
static void init_pre_proc(void);
#endif

#ifdef _MGNCSCTRL_TEXTEDITOR
///////////////////////////////////////////
static BOOL init_textedit(void)
{
	MGNCS_INIT_CLASS(mObject);
	MGNCS_INIT_CLASS(mCommBTreeNode);
	MGNCS_INIT_CLASS(mCommBTree);
	MGNCS_INIT_CLASS(mCommBTreeLeafIterator);
	MGNCS_INIT_CLASS(mTextBuffer);
	MGNCS_INIT_CLASS(mTextIterator);
	MGNCS_INIT_CLASS(mTextLayout);
	MGNCS_INIT_CLASS(mTextLayoutNode);
	MGNCS_INIT_CLASS(mTextRender);

	MGNCS_REGISTER_COMPONENT_EX(mTextEditor, NCSS_TE_AUTOHSCROLL|NCSS_TE_AUTOVSCROLL |WS_HSCROLL | WS_VSCROLL,
            WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);
    return TRUE;
}
static void uninit_textedit(void)
{
	MGNCS_UNREG_COMPONENT(mTextEditor);
}
#else
#define init_textedit()   do{ }while(0)
#define uninit_textedit() do{ }while(0)

#endif

GHANDLE g_ncsEtcHandle;
static BOOL _prev_defined_ncs_etc = FALSE;

BOOL ncsInitialize(void)
{
    static const char* renderer[] =
    {
        "classic",
        "fashion",
        "skin",
		"flat",
    };

#ifdef _MGRM_THREADS
	init_pre_proc();
#endif

    //can set outside
    if(g_ncsEtcHandle)
    {
        ncsLoadRdrEtcFile(NULL, renderer, sizeof(renderer)/sizeof(char*));
        _prev_defined_ncs_etc = TRUE;
    }
    else
    {
        g_ncsEtcHandle =
            ncsLoadRdrEtcFile(MGNCS_ETCFILENAME, renderer, sizeof(renderer)/sizeof(char*));
        _prev_defined_ncs_etc = FALSE;
        if (!g_ncsEtcHandle) {
            fprintf (stderr, "Load renderer configure:[%s] failure.\n", MGNCS_ETCFILENAME);
            return FALSE;
        }
    }

	//init heap
	slab_init_default_heap();

	//init pieces
	init_pieces_classes();
    init_widget_info();
    init_piece_event_info();

    _init_classes();

	//Register mStatic
	MGNCS_REGISTER_COMPONENT(mStatic);

#ifdef _MGNCSCTRL_LEDLABEL
	MGNCS_REGISTER_COMPONENT(mLEDLabel);
#endif

	//Register mImage
	MGNCS_REGISTER_COMPONENT(mImage);

#ifdef _MGNCSCTRL_RECTANGLE
	MGNCS_REGISTER_COMPONENT(mRectangle);
#endif

	MGNCS_REGISTER_COMPONENT_EX(mGroupBox, WS_NONE,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);

	MGNCS_REGISTER_COMPONENT_EX(mButtonGroup, WS_NONE,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);

	MGNCS_REGISTER_COMPONENT_EX(mButton, WS_NONE,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);

	MGNCS_REGISTER_COMPONENT_EX(mCheckButton, WS_NONE,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);

	MGNCS_REGISTER_COMPONENT_EX(mRadioButton, 0/*NCSS_BUTTON_AUTOCHECK*/, WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);

#ifdef _MGNCSCTRL_MENUBUTTON
	MGNCS_REGISTER_COMPONENT_EX(mMenuButton, WS_NONE,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);
#endif

#ifdef _MGNCSCTRL_IMAGEBUTTON
	MGNCS_REGISTER_COMPONENT_EX(mImageButton, WS_NONE,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);
#endif

	MGNCS_REGISTER_COMPONENT(mPanel);

#ifdef _MGNCSCTRL_PROGRESSBAR
    MGNCS_REGISTER_COMPONENT_EX(mProgressBar, WS_NONE,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);
#endif

#ifdef _MGNCSCTRL_COMBOBOX
    MGNCS_REGISTER_COMPONENT_EX(mCombobox, WS_NONE,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);
#endif

#ifdef _MGNCSCTRL_SPINNER
    MGNCS_REGISTER_COMPONENT_EX(mSpinner, WS_NONE,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);
#endif

#ifdef _MGNCSCTRL_SPINBOX
    MGNCS_REGISTER_COMPONENT_EX(mSpinBox, WS_NONE,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);
#endif

#ifdef _MGNCSCTRL_SLIDER
    MGNCS_REGISTER_COMPONENT_EX(mSlider, WS_NONE,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);
#endif

#ifdef _MGNCSCTRL_TRACKBAR
    MGNCS_REGISTER_COMPONENT_EX(mTrackBar, WS_NONE,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);
#endif

#ifdef _MGNCSCTRL_CONTAINER
	MGNCS_REGISTER_COMPONENT_EX(mContainer, WS_HSCROLL | WS_VSCROLL,
            WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);
#endif

#ifdef _MGNCSCTRL_PAGE
    MGNCS_REGISTER_COMPONENT_EX(mPage, WS_HSCROLL | WS_VSCROLL,
            WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);
#endif

#ifdef _MGNCSCTRL_PROPSHEET
    MGNCS_REGISTER_COMPONENT_EX(mPropSheet, WS_NONE,
            WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);
#endif

    MGNCS_REGISTER_COMPONENT_EX(mScrollView, WS_HSCROLL | WS_VSCROLL,
            WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);

#ifdef _MGNCSCTRL_ICONVIEW
	MGNCS_REGISTER_COMPONENT_EX(mIconView, WS_VSCROLL,
            WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);
#endif

#ifdef _MGNCSCTRL_LISTVIEW
MGNCS_REGISTER_COMPONENT_EX(mListView, WS_HSCROLL | WS_VSCROLL,
            WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);
#endif

#ifdef _MGNCSCTRL_LISTBOX
	MGNCS_REGISTER_COMPONENT_EX(mListBox, WS_HSCROLL | WS_VSCROLL,
            WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);
#endif

	MGNCS_REGISTER_COMPONENT_EX(mMainWnd, 0, 0, IDC_ARROW, NCS_BGC_3DBODY);

#ifdef _MGNCSCTRL_DIALOGBOX
	MGNCS_REGISTER_COMPONENT_EX(mDialogBox, 0, 0, IDC_ARROW, NCS_BGC_3DBODY);
#endif

	MGNCS_REGISTER_COMPONENT(mTimer);

	MGNCS_REGISTER_COMPONENT(mEdit);

	MGNCS_REGISTER_COMPONENT_EX(mSlEdit, WS_NONE, WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);

#ifdef _MGNCSCTRL_OLD_MLEDIT
	MGNCS_REGISTER_COMPONENT_EX(mMlEdit, WS_HSCROLL | WS_VSCROLL,
            WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);
#endif

#ifdef _MGNCSCTRL_SCROLLBAR
	MGNCS_REGISTER_COMPONENT_EX(mScrollBar, WS_NONE,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);
#endif

#ifdef _MGNCSCTRL_ANIMATE
	MGNCS_REGISTER_COMPONENT(mAnimate);
#endif

#ifdef _MGNCSCTRL_MONTHCALENDAR
	MGNCS_REGISTER_COMPONENT_EX(mMonthCalendar, WS_NONE,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);
#endif

#ifdef _MGNCSCTRL_TOOLBAR
	MGNCS_REGISTER_COMPONENT_EX(mToolBar, WS_NONE,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);
#endif

#ifdef _MGNCSCTRL_COLORBUTTON
	MGNCS_REGISTER_COMPONENT_EX(mColorButton, WS_NONE,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);
#endif

	MGNCS_REGISTER_COMPONENT_EX(mSeparator, WS_NONE,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);

#ifdef _MGNCSCTRL_IMWORDSEL
	MGNCS_REGISTER_COMPONENT_EX(mIMWordSel, 0,
		WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);
#endif

    //for list control
#ifdef _MGNCSCTRL_LIST
	MGNCS_REGISTER_COMPONENT_EX(mList, WS_VSCROLL,
			WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);
#endif

	init_textedit();

	ncsInitClassicRenderers();

#ifdef _MGNCS_RDR_SKIN
	ncsInitSkinRenderers();
#endif

#ifdef _MGNCS_RDR_FASHION
	ncsInitFashionRenderers();
#endif

#ifdef _MGNCS_RDR_FLAT
	ncsInitFlatRenderers();
#endif

	return TRUE;
}

///////////////////////////////////
void ncsUninitialize(void)
{
	MGNCS_UNREG_COMPONENT(mStatic);

#ifdef _MGNCSCTRL_LEDLABEL
	MGNCS_UNREG_COMPONENT(mLEDLabel);
#endif

	MGNCS_UNREG_COMPONENT(mImage);

#ifdef _MGNCSCTRL_RECTANGLE
	MGNCS_UNREG_COMPONENT(mRectangle);
#endif

	MGNCS_UNREG_COMPONENT(mGroupBox);
	MGNCS_UNREG_COMPONENT(mButtonGroup);
	MGNCS_UNREG_COMPONENT(mButton);
	MGNCS_UNREG_COMPONENT(mCheckButton);
	MGNCS_UNREG_COMPONENT(mRadioButton);

#ifdef _MGNCSCTRL_MENUBUTTON
	MGNCS_UNREG_COMPONENT(mMenuButton);
#endif

#ifdef _MGNCSCTRL_IMAGEBUTTON
	MGNCS_UNREG_COMPONENT(mImageButton);
#endif

	MGNCS_UNREG_COMPONENT(mPanel);

#ifdef _MGNCSCTRL_PROGRESSBAR
	MGNCS_UNREG_COMPONENT(mProgressBar);
#endif

#ifdef _MGNCSCTRL_COMBOBOX
	MGNCS_UNREG_COMPONENT(mCombobox);
#endif

#ifdef _MGNCSCTRL_SPINNER
	MGNCS_UNREG_COMPONENT(mSpinner);
#endif

#ifdef _MGNCSCTRL_SPINBOX
	MGNCS_UNREG_COMPONENT(mSpinBox);
#endif

#ifdef _MGNCSCTRL_SLIDER
	MGNCS_UNREG_COMPONENT(mSlider);
#endif

#ifdef _MGNCSCTRL_TRACKBAR
	MGNCS_UNREG_COMPONENT(mTrackBar);
#endif

#ifdef _MGNCSCTRL_CONTAINER
	MGNCS_UNREG_COMPONENT(mContainer);
#endif

#ifdef _MGNCSCTRL_PAGE
	MGNCS_UNREG_COMPONENT(mPage);
#endif

#ifdef _MGNCSCTRL_PROPSHEET
	MGNCS_UNREG_COMPONENT(mPropSheet);
#endif

	MGNCS_UNREG_COMPONENT(mScrollView);

#ifdef _MGNCSCTRL_ICONVIEW
	MGNCS_UNREG_COMPONENT(mIconView);
#endif

#ifdef _MGNCSCTRL_LISTVIEW
	MGNCS_UNREG_COMPONENT(mListView);
#endif

#ifdef _MGNCSCTRL_LISTBOX
	MGNCS_UNREG_COMPONENT(mListBox);
#endif

	MGNCS_UNREG_COMPONENT(mMainWnd);

#ifdef _MGNCSCTRL_DIALOGBOX
	MGNCS_UNREG_COMPONENT(mDialogBox);
#endif

    MGNCS_UNREG_COMPONENT(mTimer);
	MGNCS_UNREG_COMPONENT(mEdit);
	MGNCS_UNREG_COMPONENT(mSlEdit);
#ifdef _MGNCSCTRL_OLD_MLEDIT
	MGNCS_UNREG_COMPONENT(mMlEdit);
#endif

#ifdef _MGNCSCTRL_SCROLLBAR
	MGNCS_UNREG_COMPONENT(mScrollBar);
#endif

#ifdef _MGNCSCTRL_ANIMATE
	MGNCS_UNREG_COMPONENT(mAnimate);
#endif

#ifdef _MGNCSCTRL_MONTHCALENDAR
	MGNCS_UNREG_COMPONENT(mMonthCalendar);
#endif

#ifdef _MGNCSCTRL_TOOLBAR
	MGNCS_UNREG_COMPONENT(mToolBar);
#endif

#ifdef _MGNCSCTRL_COLORBUTTON
	MGNCS_UNREG_COMPONENT(mColorButton);
#endif

#ifdef _MGNCSCTRL_IMWORDSEL
	MGNCS_UNREG_COMPONENT(mIMWordSel);
#endif

	MGNCS_UNREG_COMPONENT(mSeparator);

	uninit_textedit();

    //for list
#ifdef _MGNCSCTRL_LIST
    MGNCS_UNREG_COMPONENT(mList);
#endif

    free_comp_class();
	//unregister ctrl rdr
	ncsUnregisterCtrlRDRs(NULL,NULL);

#ifdef _MGNCS_RDR_FASHION
	ncsUninitFashionRenderers();
#endif

	//unload etc info
	ncsUnloadRdrEtcFile(_prev_defined_ncs_etc? (GHANDLE)0 : g_ncsEtcHandle);
    g_ncsEtcHandle = 0;

	//uninit default deap
	slab_unit_default_heap();
    deinit_piece_event_info();
    deinit_widget_info();
}

/////////////////////////////////////////////////////////
#ifdef _MGRM_THREADS

#define NCSM_SYNCREQ  99999
#define SYNCREQ_MAINCREATE  1

typedef struct _syncreq_callcreate{
	PNCS_CREATE_MAIN  createMain;
	HPACKAGE          h_pack;
	HICON             h_icon;
	HMENU             h_menu;
	DWORD             user;
}syncreq_callcreate;

MGNCS_EXPORT mMainWnd* ncsSyncCallCreate(HWND host, PNCS_CREATE_MAIN pCreateMain,
		HPACKAGE hPack,
		HICON h_icon,
		HMENU h_menu,
		DWORD user)
{
	if(host == HWND_DESKTOP || host == HWND_NULL)
	{
		return pCreateMain(hPack, host, h_icon, h_menu, user);
	}
	else
	{
		syncreq_callcreate cc = {
			pCreateMain,
			hPack,
			h_icon,
			h_menu,
			user
		};

		return (mMainWnd*)SendMessage(host, NCSM_SYNCREQ, SYNCREQ_MAINCREATE, (LPARAM)&cc);
	}
}


static WNDPROC _default_proc = NULL;

static LRESULT _my_default_proc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == NCSM_SYNCREQ)
	{
		if(SYNCREQ_MAINCREATE == wParam)
		{
			syncreq_callcreate * pcc = (syncreq_callcreate*)lParam;
			if(pcc && pcc->createMain){
				return (LRESULT)(pcc->createMain)(pcc->h_pack, hwnd,
						pcc->h_icon, pcc->h_menu, pcc->user);
            }
		}
		return 0;
	}

    return _default_proc (hwnd, message, wParam, lParam);
}

static void init_pre_proc(void)
{
	_default_proc = DefaultMainWinProc;
	DefaultMainWinProc = _my_default_proc;
}

#endif



