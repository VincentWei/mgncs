
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "../include/mgncs.h"

#define IDC_PROG    100
#define IDC_PROG1    100
#define IDC_PROG2    101
#define IDC_PROG3    102
#define IDC_PROG4    103
#define IDC_PROG5    104
#define IDC_PROG6    105
#define IDC_PROG7    106
#define IDC_TIME    200


static int pb_pos = 0;

static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
	ncsSetProperty(GetDlgItem(self->hwnd, IDC_PROG3), NCSP_PROG_LINESTEP, 2);
	ncsSetProperty(GetDlgItem(self->hwnd, IDC_PROG5), NCSP_PROG_LINESTEP, 5);
	SetTimer(self->hwnd, IDC_TIME, 30);
	return TRUE;
}

static BOOL mymain_onTimer(mWidget* self, int id, DWORD count)
{
	int i;
	pb_pos ++;
	printf("--- pb_pos = %d,id=%d,count=%d\n", pb_pos,id, (int)count);
	for(i=IDC_PROG1; i<=IDC_PROG6; i++)
	{
		mProgressBar * pb = (mProgressBar*)ncsGetChildObj(self->hwnd, i);
		if(pb){
			_c(pb)->setProperty(pb, NCSP_PROG_CURPOS, pb_pos);
		}
	}
    return TRUE;
}

static void mymain_onClose(mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
}

static NCS_EVENT_HANDLER progress_handlers [] = {

	{0, NULL}	
};

//define the progress properites
static NCS_PROP_ENTRY progress_props [] = {
	{NCSP_PROG_MAXPOS, 100},
	{NCSP_PROG_MINPOS, 0 },
	{NCSP_PROG_LINESTEP, 10},
	{NCSP_PROG_CURPOS, 0 },
	{ 0, 0 }
};

static NCS_RDR_INFO prog_rdr_info[] =
{
    {"flat", "flat", NULL},
    //{"skin", "skin", NULL},
    //{"fashion", "fashion", NULL},
	//{"classic","classic",NULL}
};

//define the tmplate of progress bar
static NCS_WND_TEMPLATE progress_templ[] = {
	{
		NCSCTRL_PROGRESSBAR, 
		IDC_PROG1,
		10, 10, 290, 30,
		WS_BORDER | WS_VISIBLE,
		WS_EX_NONE,
        "",
		progress_props,
		prog_rdr_info,
		progress_handlers,
		NULL,
		0,
		0
	},
	{
		NCSCTRL_PROGRESSBAR,
		IDC_PROG2,
		10, 50, 290, 30,
		WS_BORDER | WS_VISIBLE | NCSS_PRGBAR_BLOCKS,
		WS_EX_NONE,
        "",
		progress_props,
		prog_rdr_info,
		progress_handlers,
		NULL,
		0,
		0
	},
	{
		NCSCTRL_PROGRESSBAR, 
		IDC_PROG3,
		10, 90, 290, 30,
		WS_BORDER | WS_VISIBLE | NCSS_PRGBAR_SHOWPERCENT,
		WS_EX_NONE,
        "",
		progress_props,
		prog_rdr_info,
		progress_handlers,
		NULL,
		0,
		0
	},
	{
		NCSCTRL_PROGRESSBAR, 
		IDC_PROG4,
		10, 150, 30, 100,
		WS_BORDER | WS_VISIBLE | NCSS_PRGBAR_VERTICAL,
		WS_EX_NONE,
        "",
		progress_props,
		prog_rdr_info,
		progress_handlers,
		NULL,
		0,
		0
	},
	{
		NCSCTRL_PROGRESSBAR, 
		IDC_PROG5,
		100, 150, 30, 100,
		WS_BORDER | WS_VISIBLE | NCSS_PRGBAR_VERTICAL | NCSS_PRGBAR_SHOWPERCENT,
		WS_EX_NONE,
        "",
		progress_props,
		prog_rdr_info,
		progress_handlers,
		NULL,
		0,
		0
	},
	{
		NCSCTRL_PROGRESSBAR, 
		IDC_PROG6,
		200, 150, 30, 100,
		WS_BORDER | WS_VISIBLE | NCSS_PRGBAR_VERTICAL | NCSS_PRGBAR_BLOCKS,
		WS_EX_NONE,
        "",
		progress_props,
		prog_rdr_info,
		progress_handlers,
		NULL,
		0,
		0
	}
};

//define the main window handlers
static NCS_EVENT_HANDLER mymain_handlers[] = {
	{MSG_CREATE, mymain_onCreate}	,
	{MSG_TIMER, mymain_onTimer},
	{MSG_CLOSE, mymain_onClose},
	{0, NULL}
};
//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_DIALOGBOX, 
	1,
	0, 0, 320, 320,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
    "ProgressBar tesing ....",
	NULL,
	NULL,
	mymain_handlers,
	progress_templ,
	sizeof(progress_templ)/sizeof(NCS_WND_TEMPLATE),
	0,
	0, 0,
};

int MiniGUIMain(int argc, const char* argv[])
{

	if(argc > 1)
	{
		prog_rdr_info[0].glb_rdr = argv[1];
		prog_rdr_info[0].ctl_rdr = argv[1];
	}

	//MSG Msg;
	ncsInitialize();
	mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect 
                                (&mymain_templ, HWND_DESKTOP);

	_c(mydlg)->doModal(mydlg, TRUE);
/*	
    while(GetMessage(&Msg, mydlg->hwnd))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
*/	
	MainWindowThreadCleanup(mydlg->hwnd);
    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif

