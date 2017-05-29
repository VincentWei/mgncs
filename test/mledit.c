
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "mgncs.h"

#ifdef _MGNCS_OLD_MLEDIT

#define IDC_SLEDIT  100
#define IDC_BTN1    101
#define IDC_BTN2    102
#define IDC_BTN3    103
#define IDC_BTN4    104
#define IDC_BTN5    105
#define IDC_BTN6    106
#define IDC_BTN7    107
#define IDC_BTN8    108
#define IDC_BTN9    109
#define IDC_BTN10   110
#define IDC_BTN11   111
#define IDC_BTN12   112
#define IDC_BTN13   113
#define IDC_BTN14   114
#define IDC_BTN15   115
#define IDC_BTN16   116
#define IDC_BTN17   117
#define IDC_BTN18   118
#define IDC_BTN19   119
#define IDC_BTN20   120
#define IDC_BTN21   121

static char buff[1024];

static BOOL btn_notify(mMlEdit * edit, mWidget *button, int nc, DWORD add_data)
{
    int s, e;
    MessageBox(edit->hwnd, "","",0);

    switch (_c(button)->getId(button))
    {
        case IDC_BTN1 :
            _c(edit)->setContent (edit, "settext succeed!", 4, 9);
            break;
        case IDC_BTN2 :
            _c(edit)->replaceText (edit, "replace!", 0, -1, 0, -1);
            break;
        case IDC_BTN3 :
            _c(edit)->insert (edit, "insert!", 0, -1, 0);
            break;
        case IDC_BTN4 :
            _c(edit)->append (edit, "append!", 0, -1);
            break;
        case IDC_BTN5 :
            _c(edit)->getContent (edit, buff, 2, 2, 5);
            InvalidateRect(GetParent(button->hwnd), NULL, TRUE);
            break;
        case IDC_BTN6 :
            _c(edit)->setSel (edit, 4, 10);
            break;
        case IDC_BTN7 :
            _c(edit)->getSel (edit,  &s, &e);
            printf("selected start = %d. end = %d\n", s, e);
            break;
        case IDC_BTN8 :
            _c(edit)->resetContent (edit);
            break;
        case IDC_BTN9 :
            break;
        case IDC_BTN10 :
            _c(edit)->copy (edit);
            break;
        case IDC_BTN11 :
            _c(edit)->paste (edit);
            break;
        case IDC_BTN12 :
            _c(edit)->cut (edit);
            break;
        case IDC_BTN13 :
            _c(edit)->setTitle (edit, "Title: \n", 5);
            break;
        case IDC_BTN14 :
            _c(edit)->getTitle (edit, buff, 10);
            printf("getTitle : %s\n", buff);
            break;
        case IDC_BTN15 :
            printf("line number : %d\n", (int)_c(edit)->getProperty(edit, NCSP_MLEDIT_LINECOUNT));
            break;
        case IDC_BTN16 :
            break;
        case IDC_BTN17 :
            break;
    }

	return TRUE;

}



static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
    sprintf(buff, "init");
	mWidget *edit = ncsGetChildObj(self->hwnd, IDC_SLEDIT);
	int i;
	for(i=IDC_BTN1; i<= IDC_BTN17; i++)
	{
		ncsAddEventListener((mObject*)ncsGetChildObj(self->hwnd,i), (mObject*)edit, (NCS_CB_ONPIECEEVENT)btn_notify, NCSN_BUTTON_PUSHED);
	}
	return TRUE;
}

static int mymain_onPaint(mWidget* self, HDC hdc)
{
    TextOut(hdc, 10, 270, buff);
	return TRUE;
}

static void mymain_onClose(mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
}

static NCS_EVENT_HANDLER edit_handlers [] = {

	{0, NULL}	
};

static NCS_PROP_ENTRY edit_props [] = {
	{ 0, 0 }
};

static NCS_RDR_INFO edit_rdr_info[] =
{
    //{"skin", "skin", NULL},
    //{"fashion", "fashion", NULL},
	{"classic","classic",NULL}
};

/*
static NCS_EVENT_HANDLER btn_handlers [] = {
    NCS_MAP_NOTIFY(NCSN_BUTTON_PUSHED, btn_notify),
	{0, NULL}	
};*/

static NCS_WND_TEMPLATE sledit_templ[] = {
	{
		NCSCTRL_MLEDIT, 
		IDC_SLEDIT,
		10, 10, 160, 240,
		WS_BORDER | WS_VISIBLE /*| WS_VSCROLL*/ 
            | NCSS_EDIT_CENTER | NCSS_MLEDIT_AUTOWRAP | NCSS_MLEDIT_TITLE | NCSS_EDIT_BASELINE,
		WS_EX_NONE,
        "mledit",
		edit_props,
		edit_rdr_info,
		edit_handlers,
		NULL,
		0,
		0
	},
	{
		NCSCTRL_BUTTON, 
		IDC_BTN1,
		250, 10, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "setText",
		NULL,
		NULL,
		NULL,//btn_handlers
		NULL,
		0,
		0
	},	
    {
		NCSCTRL_BUTTON, 
		IDC_BTN2,
		250, 50, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "replaceText",
		NULL,
		NULL,
		NULL,//btn_handlers
		NULL,
		0,
		0
	},
    {
		NCSCTRL_BUTTON, 
		IDC_BTN3,
		250, 90, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "insert",
		NULL,
		NULL,
		NULL,//btn_handlers
		NULL,
		0,
		0
	},
    {
		NCSCTRL_BUTTON, 
		IDC_BTN4,
		250, 130, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "append",
		NULL,
		NULL,
		NULL,//btn_handlers
		NULL,
		0,
		0
	},
    {
		NCSCTRL_BUTTON, 
		IDC_BTN5,
		250, 170, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "getText",
		NULL,
		NULL,
		NULL,//btn_handlers
		NULL,
		0,
		0
	},
    {
		NCSCTRL_BUTTON, 
		IDC_BTN6,
		250, 210, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "setSel",
		NULL,
		NULL,
		NULL,//btn_handlers
		NULL,
		0,
		0
	},
    {
		NCSCTRL_BUTTON, 
		IDC_BTN7,
		250, 250, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "getsel",
		NULL,
		NULL,
		NULL,//btn_handlers
		NULL,
		0,
		0
	},
    {
		NCSCTRL_BUTTON, 
		IDC_BTN8,
		250, 290, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "reset",
		NULL,
		NULL,
		NULL,//btn_handlers
		NULL,
		0,
		0
	},
    {
		NCSCTRL_BUTTON, 
		IDC_BTN9,
		250, 330, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "setColor",
		NULL,
		NULL,
		NULL,//btn_handlers
		NULL,
		0,
		0
	},
    {
		NCSCTRL_BUTTON, 
		IDC_BTN10,
		340, 10, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "copy",
		NULL,
		NULL,
		NULL,//btn_handlers
		NULL,
		0,
		0
	},
    {
		NCSCTRL_BUTTON, 
		IDC_BTN11,
		340, 50, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "paste",
		NULL,
		NULL,
		NULL,//btn_handlers
		NULL,
		0,
		0
	},
    {
		NCSCTRL_BUTTON, 
		IDC_BTN12,
		340, 90, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "cut",
		NULL,
		NULL,
		NULL,//btn_handlers
		NULL,
		0,
		0
	},
    {
		NCSCTRL_BUTTON, 
		IDC_BTN13,
		340, 130, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "set Title",
		NULL,
		NULL,
		NULL,//btn_handlers
		NULL,
		0,
		0
	},
    {
		NCSCTRL_BUTTON, 
		IDC_BTN14,
		340, 170, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "getTitle",
		NULL,
		NULL,
		NULL,//btn_handlers
		NULL,
		0,
		0
	},
    {
		NCSCTRL_BUTTON, 
		IDC_BTN15,
		340, 210, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "lineCont",
		NULL,
		NULL,
		NULL,//btn_handlers
		NULL,
		0,
		0
	},
    {
		NCSCTRL_BUTTON, 
		IDC_BTN16,
		340, 250, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "",
		NULL,
		NULL,
		NULL,//btn_handlers
		NULL,
		0,
		0
	},
    {
		NCSCTRL_BUTTON, 
		IDC_BTN17,
		340, 290, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "",
		NULL,
		NULL,
		NULL,//btn_handlers
		NULL,
		0,
		0
	},


};

//define the main window handlers
static NCS_EVENT_HANDLER mymain_handlers[] = {
	{MSG_CREATE, mymain_onCreate}	,
	{MSG_PAINT, mymain_onPaint}	,
	{MSG_CLOSE, mymain_onClose},
	{0, NULL}
};
//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_DIALOGBOX, 
	1,
	0, 0, 480, 480,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
    "single edit tesing ....",
	NULL,
	NULL,
	mymain_handlers,
	sledit_templ,
	sizeof(sledit_templ)/sizeof(NCS_WND_TEMPLATE),
	0,
	0, 0,
};

int MiniGUIMain(int argc, const char* argv[])
{
	
	if(argc > 1)
	{
		edit_rdr_info[0].glb_rdr = argv[1];
		edit_rdr_info[0].ctl_rdr = argv[1];
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
#else

int main(int argc, const char* argv[])
{
	fprintf(stderr, "WARNING, your mgncs is not open the mledit support, use --enable-old-mledit to rebuild your mgncs\n");
	return 0;
}

#endif

