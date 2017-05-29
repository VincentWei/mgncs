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

#ifdef _MGNCS_DATASOURCE
extern BOOL ncsInitDefaultDataSource(void);

extern BOOL ncsInitDataBinding(void);
#endif

extern void free_comp_class(void);

extern BOOL ncsInitIMEClasses(void);

extern void deinit_widget_info(void);
extern void init_widget_info(void);
extern void init_piece_event_info(void);
extern void deinit_piece_event_info(void);

//////////////////////////////////////////////////////////
static void _init_classes(void)
{
#ifdef _MGNCS_DATASOURCE
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
	MGNCS_INIT_CLASS(mListLayout);
	MGNCS_INIT_CLASS(mLVIconLayout);
	MGNCS_INIT_CLASS(mLHIconLayout);
	MGNCS_INIT_CLASS(mLHCenterBoxLayout);
	MGNCS_INIT_CLASS(mLGroupLayout);
#ifdef _MGNCS_DATASOURCE
	ncsInitDefaultDataSource();
#endif

	ncsInitIMEClasses();

}

#ifdef _MGRM_THREADS
static void init_pre_proc(void);
#endif

#ifdef _MGNCS_TEXTEDITOR
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
	MGNCS_REGISTER_COMPONENT(mLEDLabel);

	//Register mImage
	MGNCS_REGISTER_COMPONENT(mImage);

	MGNCS_REGISTER_COMPONENT(mRectangle);

	MGNCS_REGISTER_COMPONENT(mGroupBox);

	MGNCS_REGISTER_COMPONENT(mButtonGroup);

	MGNCS_REGISTER_COMPONENT(mButton);

	MGNCS_REGISTER_COMPONENT(mCheckButton);

	MGNCS_REGISTER_COMPONENT_EX(mRadioButton, 0/*NCSS_BUTTON_AUTOCHECK*/, WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);

	MGNCS_REGISTER_COMPONENT(mMenuButton);

	MGNCS_REGISTER_COMPONENT(mPanel);

    MGNCS_REGISTER_COMPONENT(mProgressBar);

    MGNCS_REGISTER_COMPONENT(mCombobox);

    MGNCS_REGISTER_COMPONENT(mSpinner);

    MGNCS_REGISTER_COMPONENT(mSpinBox);

    MGNCS_REGISTER_COMPONENT(mSlider);

    MGNCS_REGISTER_COMPONENT(mTrackBar);

	MGNCS_REGISTER_COMPONENT_EX(mContainer, WS_HSCROLL | WS_VSCROLL,
            WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);

    MGNCS_REGISTER_COMPONENT_EX(mPage, WS_HSCROLL | WS_VSCROLL,
            WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);

    MGNCS_REGISTER_COMPONENT_EX(mPropSheet, WS_NONE, 
            WS_EX_NONE, IDC_ARROW, NCS_BGC_3DBODY);

	MGNCS_REGISTER_COMPONENT_EX(mScrollView, WS_HSCROLL | WS_VSCROLL,
            WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);

	MGNCS_REGISTER_COMPONENT_EX(mIconView, WS_VSCROLL,
            WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);

	MGNCS_REGISTER_COMPONENT_EX(mListView, WS_HSCROLL | WS_VSCROLL,
            WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);

	MGNCS_REGISTER_COMPONENT_EX(mListBox, WS_HSCROLL | WS_VSCROLL,
            WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);

	MGNCS_REGISTER_COMPONENT_EX(mMainWnd, 0, 0, IDC_ARROW, NCS_BGC_3DBODY);
	MGNCS_REGISTER_COMPONENT_EX(mDialogBox, 0, 0, IDC_ARROW, NCS_BGC_3DBODY);

	MGNCS_REGISTER_COMPONENT(mTimer);

	MGNCS_REGISTER_COMPONENT(mEdit);

	MGNCS_REGISTER_COMPONENT_EX(mSlEdit, WS_NONE, WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);

#ifdef _MGNCS_OLD_MLEDIT
	MGNCS_REGISTER_COMPONENT_EX(mMlEdit, WS_HSCROLL | WS_VSCROLL, 
            WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);
#endif

	MGNCS_REGISTER_COMPONENT(mScrollBar);
	MGNCS_REGISTER_COMPONENT(mAnimate);

	MGNCS_REGISTER_COMPONENT(mMonthCalendar);

	MGNCS_REGISTER_COMPONENT(mToolBar);

	MGNCS_REGISTER_COMPONENT(mColorButton);

	MGNCS_REGISTER_COMPONENT(mSeparator);

	MGNCS_REGISTER_COMPONENT_EX(mIMWordSel, 0, 
            WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);

    //for list control
	MGNCS_REGISTER_COMPONENT_EX(mList, WS_VSCROLL, 
            WS_EX_NONE, IDC_ARROW, NCS_BGC_WINDOW);

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
	MGNCS_UNREG_COMPONENT(mLEDLabel);
	MGNCS_UNREG_COMPONENT(mImage);
	MGNCS_UNREG_COMPONENT(mRectangle);
	MGNCS_UNREG_COMPONENT(mGroupBox);
	MGNCS_UNREG_COMPONENT(mButtonGroup);
	MGNCS_UNREG_COMPONENT(mButton);
	MGNCS_UNREG_COMPONENT(mCheckButton);
	MGNCS_UNREG_COMPONENT(mRadioButton);
	MGNCS_UNREG_COMPONENT(mMenuButton);
	MGNCS_UNREG_COMPONENT(mPanel);
	MGNCS_UNREG_COMPONENT(mProgressBar);
	MGNCS_UNREG_COMPONENT(mCombobox);
	MGNCS_UNREG_COMPONENT(mSpinner);
	MGNCS_UNREG_COMPONENT(mSpinBox);
	MGNCS_UNREG_COMPONENT(mSlider);
	MGNCS_UNREG_COMPONENT(mTrackBar);
	MGNCS_UNREG_COMPONENT(mContainer);
	MGNCS_UNREG_COMPONENT(mPage);
	MGNCS_UNREG_COMPONENT(mPropSheet);
	MGNCS_UNREG_COMPONENT(mScrollView);
	MGNCS_UNREG_COMPONENT(mIconView);
	MGNCS_UNREG_COMPONENT(mListView);
	MGNCS_UNREG_COMPONENT(mListBox);
	MGNCS_UNREG_COMPONENT(mMainWnd);
	MGNCS_UNREG_COMPONENT(mDialogBox);
    MGNCS_UNREG_COMPONENT(mTimer);
	MGNCS_UNREG_COMPONENT(mEdit);
	MGNCS_UNREG_COMPONENT(mSlEdit);
#ifdef _MGNCS_OLD_MLEDIT
	MGNCS_UNREG_COMPONENT(mMlEdit);
#endif
	MGNCS_UNREG_COMPONENT(mScrollBar);
	MGNCS_UNREG_COMPONENT(mAnimate);
	MGNCS_UNREG_COMPONENT(mMonthCalendar);
	MGNCS_UNREG_COMPONENT(mToolBar);
	MGNCS_UNREG_COMPONENT(mColorButton);
	MGNCS_UNREG_COMPONENT(mIMWordSel);
    MGNCS_UNREG_COMPONENT(mSeparator);

	uninit_textedit();

    //for list
    MGNCS_UNREG_COMPONENT(mList);
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

static int _my_default_proc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
	if(message == NCSM_SYNCREQ)
	{
		if(SYNCREQ_MAINCREATE == wParam)
		{
			syncreq_callcreate * pcc = (syncreq_callcreate*)lParam;
			if(pcc && pcc->createMain){
				return (int)(pcc->createMain)(pcc->h_pack, hwnd, 
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



