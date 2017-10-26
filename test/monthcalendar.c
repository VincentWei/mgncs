#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mgncs/mgncs.h>
#include "../include/mrdr.h"

#if defined _MGNCSCTRL_MONTHCALENDAR && defined _MGNCSCTRL_DIALOGBOX

#define ID_BTN  101
#define ID_PROG 200


static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
	//TODO : initialize
	return TRUE;
}

static void mymain_onClose(mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
}

static BOOL mymain_oncsizechanged(mWidget *self, int cx, int cy)
{
	HWND hwnd = GetDlgItem(self->hwnd, ID_BTN);
	MoveWindow(hwnd, 0, 0, cx, cy,TRUE);

	return FALSE;
}

static NCS_RDR_INFO month_rdr_info[] = {
	{"flat", "flat", NULL},
	//{"skin", "skin", NULL},
//	{"fashion","fashion", btn_rdr_elements}
};

//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_MONTHCALENDAR,
		ID_BTN,
		20, 20, 200, 150,
		WS_BORDER | WS_VISIBLE,
		WS_EX_NONE,
		"",
		NULL, //props,
		month_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
};

static NCS_EVENT_HANDLER mymain_handlers[] = {
	{MSG_CREATE, mymain_onCreate},
	{MSG_CLOSE, mymain_onClose},
	{MSG_CSIZECHANGED, mymain_oncsizechanged},
	{0, NULL}
};

//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_DIALOGBOX,
	1,
	0, 0, 320, 320,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
	"MonthCalendar Test ....",
	NULL,
	NULL,
	mymain_handlers,
	_ctrl_templ,
	sizeof(_ctrl_templ)/sizeof(NCS_WND_TEMPLATE),
	0,
	0, 0,
};

int MiniGUIMain(int argc, const char* argv[])
{
	if (argc > 1) {
		month_rdr_info[0].glb_rdr = argv[1];
		month_rdr_info[0].ctl_rdr = argv[1];
	}

	ncsInitialize();

	mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect(
			&mymain_templ, HWND_DESKTOP);

	_c(mydlg)->doModal(mydlg, TRUE);

	ncsUninitialize();

	return 0;
}

#else //_MGNCSCTRL_MONTHCALENDAR _MGNCSCTRL_DIALOGBOX

int main (void)
{
	printf("\n==========================================================\n");
	printf("======== You haven't enable the monthcalendar dialogbox contorl =====\n");
	printf("==========================================================\n");
	printf("============== ./configure --enable-monthcalendar --enable-dialogbox ==========\n");
	printf("==========================================================\n\n");
	return 0;
}
#endif	//_MGNCSCTRL_MONTHCALENDAR _MGNCSCTRL_DIALOGBOX

