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

#if defined _MGNCSCTRL_COLORBUTTON && defined _MGNCSCTRL_DIALOGBOX

#define ID_BTN  100


static BOOL mymain_oncolorchanged(mMainWnd* self, mColorButton *sender,
		int id, DWORD param)
{
	SetWindowBkColor(self->hwnd, DWORD2PIXEL(HDC_SCREEN, param));
	InvalidateRect(self->hwnd, NULL, TRUE);

	return FALSE;
}

static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
	//TODO : initialize
	mColorButton *btn = (mColorButton*)_c(self)->getChild(self, ID_BTN);
	if (NULL != btn) {
		_c(btn)->setProperty(btn, NCSP_CLRBTN_CURCOLOR,
				PIXEL2DWORD(HDC_SCREEN, GetWindowBkColor(self->hwnd)));
		ncsAddEventListener((mObject*)btn, (mObject*)self,
				(NCS_CB_ONPIECEEVENT)mymain_oncolorchanged,
				NCSN_CLRBTN_COLORCHANGED);
	}

	return TRUE;
}

static void mymain_onClose(mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
}

static NCS_RDR_INFO btn_rdr_info[] = {
	{ "classic", "classic", NULL }
	// { "skin", "skin", NULL },
	// { "fashion", "fashion", btn_rdr_elements }
};

//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_COLORBUTTON,
		ID_BTN,
		40, 40, 80, 30,
		WS_VISIBLE|NCSS_NOTIFY,
		WS_EX_NONE,
		"button",
		NULL, //props,
		btn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	}
};


static NCS_EVENT_HANDLER mymain_handlers[] = {
	{ MSG_CREATE, mymain_onCreate },
	{ MSG_CLOSE, mymain_onClose },
	{ 0, NULL }
};

//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_MAINWND,
	1,
	0, 0, 180, 140,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
	"ColorButton Test ....",
	NULL,
	NULL,
	mymain_handlers,
	_ctrl_templ,
	sizeof(_ctrl_templ) / sizeof(NCS_WND_TEMPLATE),
	0,
	0,
	0,
};

int MiniGUIMain(int argc, const char* argv[])
{
	if (argc > 1) {
		btn_rdr_info[0].glb_rdr = argv[1];
		btn_rdr_info[0].ctl_rdr = argv[1];
	}

	ncsInitialize();

	mDialogBox* mydlg = (mDialogBox*)ncsCreateMainWindowIndirect(
			&mymain_templ, HWND_DESKTOP);

	_c(mydlg)->doModal(mydlg, TRUE);

	ncsUninitialize();

	return 0;
}

#else // _MGNCSCTRL_COLORBUTTON _MGNCSCTRL_DIALOGBOX

int main (void)
{
	printf("\n==========================================================\n");
	printf("======== You haven't enable the colorbutton, dialogbox contorl =====\n");
	printf("==========================================================\n");
	printf("============== ./configure --enable-colorbutton --enable-dialogbox ==========\n");
	printf("==========================================================\n\n");
	return 0;
}
#endif	// _MGNCSCTRL_COLORBUTTON _MGNCSCTRL_DIALOGBOX


