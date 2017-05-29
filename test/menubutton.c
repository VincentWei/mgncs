#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "../include/mgncs.h"
#include "../include/mrdr.h"

#define ID_BTN  101
#define ID_BTN1 102
#define ID_BTN5 106
#define ID_BTN6 107
#define ID_BTN7 108
#define ID_BTN8 109

static void menubutton_onitemchanged(mMenuButton *self, int id, int nc, DWORD add_data)
{
	char szText[100];
	sprintf(szText,"id=%d\n",id);
	MessageBox(self->hwnd, "test",szText,0);
}

static NCS_EVENT_HANDLER menubutton_handlers[] = {
	NCS_MAP_NOTIFY(NCSN_MNUBTN_ITEMCHANGED, menubutton_onitemchanged),
	{0, NULL}
};

static BITMAP bmp;
static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
	//TODO : initialize
	mPopMenuMgr * popmenu = NEW(mPopMenuMgr);

	_c(popmenu)->addItem(popmenu,0, "item1", NULL, 200, 0, NULL, 0);
	_c(popmenu)->addItem(popmenu,0, "item2", NULL, 201, 0, NULL, 0);
	_c(popmenu)->addItem(popmenu,0, "item3", NULL, 202, 0, NULL, 0);
	_c(popmenu)->addItem(popmenu,0, "item4", NULL, 203, 0, NULL, 0);


	
	//set image
	if(LoadBitmapFromFile(HDC_SCREEN, &bmp, "icon.png")!=0)
	{
		printf("cannot load image file \"icon.png\"\n");
	}

	mButton *mb1 = (mButton*)ncsGetChildObj(self->hwnd, ID_BTN);
	if(mb1)
	{
		_c(mb1)->setProperty(mb1, NCSP_MNUBTN_POPMENU, (DWORD)popmenu);
	}

	
	mb1 = (mButton*)ncsGetChildObj(self->hwnd, ID_BTN1);

	if(mb1)
	{
		_c(mb1)->setProperty(mb1, NCSP_BUTTON_IMAGE, (DWORD)&bmp);
		_c(mb1)->setProperty(mb1, NCSP_MNUBTN_POPMENU, (DWORD)popmenu);
	}

	mb1 = (mButton*)ncsGetChildObj(self->hwnd, ID_BTN5);
	if(mb1)
	{
		_c(mb1)->setProperty(mb1, NCSP_BUTTON_IMAGE, (DWORD)&bmp);
		_c(mb1)->setProperty(mb1, NCSP_MNUBTN_POPMENU, (DWORD)popmenu);
	}

	mb1 = (mButton*)ncsGetChildObj(self->hwnd, ID_BTN6);
	if(mb1)
	{
		_c(mb1)->setProperty(mb1, NCSP_BUTTON_IMAGE, (DWORD)&bmp);
		_c(mb1)->setProperty(mb1, NCSP_BUTTON_ALIGN, (DWORD)NCS_ALIGN_RIGHT);
		_c(mb1)->setProperty(mb1, NCSP_MNUBTN_POPMENU, (DWORD)popmenu);
	}
	
	mb1 = (mButton*)ncsGetChildObj(self->hwnd, ID_BTN7);
	if(mb1)
	{
		_c(mb1)->setProperty(mb1, NCSP_BUTTON_IMAGE, (DWORD)&bmp);
		_c(mb1)->setProperty(mb1, NCSP_MNUBTN_POPMENU, (DWORD)popmenu);
	}

	mb1 = (mButton*)ncsGetChildObj(self->hwnd, ID_BTN8);
	if(mb1)
	{
		_c(mb1)->setProperty(mb1, NCSP_BUTTON_IMAGE, (DWORD)&bmp);
		_c(mb1)->setProperty(mb1, NCSP_BUTTON_VALIGN, (DWORD)NCS_VALIGN_BOTTOM);
		_c(mb1)->setProperty(mb1, NCSP_MNUBTN_POPMENU, (DWORD)popmenu);
	}
	return TRUE;
}

static void mymain_onClose(mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
}

NCS_RDR_ELEMENT btn_rdr_elements[] = 
{
	{ NCS_MODE_USEFLAT, 1},
	{ -1, 0 }
};
static NCS_RDR_INFO btn_rdr_info[] =
{
    {"skin", "skin", NULL},
//	{"fashion","fashion", btn_rdr_elements}
};

//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_MENUBUTTON, 
		ID_BTN,
		40, 130, 80, 25,
		WS_BORDER | WS_VISIBLE,
		WS_EX_NONE,
		"button",
		NULL, //props,
		btn_rdr_info, //rdr_info
		menubutton_handlers, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_MENUBUTTON, 
		ID_BTN1,
		40, 180, 50, 50,
		WS_VISIBLE | NCSS_BUTTON_IMAGE,
		WS_EX_NONE,
		"",
		NULL, //props,
		btn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},


	{
		NCSCTRL_MENUBUTTON, 
		ID_BTN5,
		40, 250, 200, 30,
		WS_VISIBLE | NCSS_BUTTON_IMAGELABEL,
		WS_EX_NONE,
		"Image Label button",
		NULL, //props,
		btn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},

	{
		NCSCTRL_MENUBUTTON, 
		ID_BTN6,
		300, 250, 200, 30,
		WS_VISIBLE | NCSS_BUTTON_IMAGELABEL ,
		WS_EX_NONE,
		"Image Label Right",
		NULL, //props,
		btn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_MENUBUTTON, 
		ID_BTN7,
		40, 300, 200, 60,
		WS_VISIBLE | NCSS_BUTTON_IMAGELABEL | NCSS_BUTTON_VERTIMAGELABEL,
		WS_EX_NONE,
		"Vert Image Label",
		NULL, //props,
		btn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_MENUBUTTON, 
		ID_BTN8,
		300, 300, 200, 60,
		WS_VISIBLE | NCSS_BUTTON_IMAGELABEL | NCSS_BUTTON_VERTIMAGELABEL ,
		WS_EX_NONE,
		"Vert Bottom Image Label",
		NULL, //props,
		btn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},



   /* {
		NCSCTRL_RADIOBUTTON, 
		ID_BTN,
		10, 40, 100, 25,
		WS_VISIBLE,
		WS_EX_NONE,
		"radio button",
		NULL, //props,
		chkbtn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
    {
		NCSCTRL_CHECKBUTTON, 
		ID_BTN,
		10, 80, 100, 25,
		WS_VISIBLE,
		WS_EX_NONE,
		"check button",
		NULL, //props,
		chkbtn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},*/
};


static NCS_EVENT_HANDLER mymain_handlers[] = {
	{MSG_CREATE, mymain_onCreate},
	{MSG_CLOSE, mymain_onClose},
	{0, NULL}
};

//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_DIALOGBOX, 
	1,
	0, 0, 800, 600,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
    "Button Test ....",
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
	if(argc > 1)
	{
		btn_rdr_info[0].glb_rdr = argv[1];
		btn_rdr_info[0].ctl_rdr = argv[1];
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

