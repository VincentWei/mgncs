#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "../include/mgncs.h"
#include "../include/mdatabinding.h"

#define ID_TRB1     101
#define ID_TRB2     102
#define IDC_SLEDIT  103

//static char buff [64];
//static HWND h_dlg;
//static gal_pixel block;


static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
	//TODO : initialize
	mTrackBar * tb = (mTrackBar*)_c(self)->getChild(self, ID_TRB1);
	mSlEdit   * se = (mSlEdit*) _c(self)->getChild(self, IDC_SLEDIT);

	ncsConnectBindProps(NCS_CMPT_PROP(tb, NCSN_TRKBAR_CHANGED, NCSP_TRKBAR_CURPOS, NCS_BT_INT, NCS_PROP_FLAG_READ|NCS_PROP_FLAG_WRITE),
		NCS_CMPT_PROP(se, NCSN_EDIT_CHANGE, NCSP_WIDGET_TEXT, NCS_BT_STR, NCS_PROP_FLAG_READ|NCS_PROP_FLAG_WRITE),
		NCS_BPT_DBL);
	
	ncsAutoReflectObjectBindProps((mObject *)se);
	
    return TRUE;
}

static void mymain_onClose(mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
}
//Propties for
static NCS_PROP_ENTRY trk_props [] = {
    {NCSP_TRKBAR_MINPOS, 0},
    {NCSP_TRKBAR_MAXPOS, 20},
    {NCSP_TRKBAR_CURPOS, 10},
    {NCSP_TRKBAR_LINESTEP, 2},
    {NCSP_TRKBAR_PAGESTEP, 5},
	{0, 0}
};

static NCS_RDR_INFO track_rdr_info[] =
{
    {"flat", "flat", NULL},
    //{"skin", "skin", NULL},
    //{"classic", "classic", NULL},
	//{"fashion","fashion",NULL}
};


static NCS_EVENT_HANDLER trk1_handlers[] = {
	{0, NULL}
};
/*
static NCS_EVENT_HANDLER trk2_handlers[] = {
	{0, NULL}
};
*/

//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_TRACKBAR, 
		ID_TRB1,
		10, 260, 240, 40,
		WS_BORDER | WS_VISIBLE | NCSS_TRKBAR_NOTICK | NCSS_NOTIFY ,
		WS_EX_TRANSPARENT,
		"",
		trk_props, //props,
		track_rdr_info, //rdr_info
		trk1_handlers, //handlers,
		NULL, //controls
		0,
		0, //add data
		MakeRGBA(255,0,0,255)
	},
	{
		NCSCTRL_SLEDIT, 
		IDC_SLEDIT,
		10, 10, 150, 30,
		WS_BORDER | WS_VISIBLE | NCSS_EDIT_CENTER | NCSS_EDIT_UPPERCASE | NCSS_NOTIFY,
		WS_EX_NONE,
        "edit",
		NULL,
		NULL,
		NULL,
		NULL,
		0,
		0
	},
};


static NCS_EVENT_HANDLER mymain_handlers[] = {
	{MSG_CREATE, mymain_onCreate },
	{MSG_CLOSE, mymain_onClose },
	{0, NULL }
};

//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_DIALOGBOX, 
	1,
	0, 0, 320, 330,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
    "Trackbar Test ....",
	NULL,
	NULL,
	mymain_handlers,
	_ctrl_templ,
	sizeof(_ctrl_templ)/sizeof(NCS_WND_TEMPLATE),
	0,
	MakeRGBA(255,255,255,255)
};

int MiniGUIMain(int argc, const char* argv[])
{
	if(argc>1)
	{
		track_rdr_info[0].glb_rdr = argv[1];
		track_rdr_info[0].ctl_rdr = argv[1];
	}


	ncsInitialize();
	mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect 
                                (&mymain_templ, HWND_DESKTOP);

	_c(mydlg)->doModal(mydlg, TRUE);

	MainWindowThreadCleanup(mydlg->hwnd);
	return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

