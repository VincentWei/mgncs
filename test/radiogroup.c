#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "../include/mgncs.h"

#define ID_GROUP 200
#define ID_BTN1  101
#define ID_BTN2  102

static BOOL mymain_onRadioChanged(mMainWnd* self, mButtonGroup* sender, int event_id, DWORD param)
{
	mWidget* sel = (mWidget*)_c(sender)->getProperty(sender, NCSP_BTNGRP_SELOBJ);

	if(sel)
	{
		MessageBox(self->hwnd, (const char*)_c(sel)->getProperty(sel, NCSP_WIDGET_TEXT), "Info", 0);
	}

	return TRUE;
}

static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
	//TODO : initialize
	//connect
	ncsAddEventListener((mObject*)(_c(self)->getChild(self, ID_GROUP)), (mObject*)self, (NCS_CB_ONOBJEVENT)mymain_onRadioChanged, NCSN_BTNGRP_SELCHANGED);
	return TRUE;
}

static void mymain_onClose(mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
}


//Propties for
static NCS_PROP_ENTRY radioGroup_props [] = {
    {NCSP_BUTTON_GROUPID, ID_GROUP},
	{0, 0}
};


static NCS_RDR_INFO btn_rdr_info[] =
{
    {"skin", "skin", NULL},
//	{"fashion","fashion", btn_rdr_elements}
};


//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = {
    {
		NCSCTRL_BUTTONGROUP , 
		ID_GROUP,
		5, 10, 300, 180,
		WS_VISIBLE|NCSS_NOTIFY,
		WS_EX_NONE,
		"buttongroup",
		NULL, //props,
		btn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
    {
		NCSCTRL_RADIOBUTTON, 
		ID_BTN1,
		10, 40, 150, 25,
		WS_VISIBLE | NCSS_BUTTON_AUTOCHECK,
		WS_EX_NONE,
		"radio button 1",
		radioGroup_props, //props,
		btn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
    {
		NCSCTRL_RADIOBUTTON, 
		ID_BTN2,
		10, 80, 150, 25,
		WS_VISIBLE | NCSS_BUTTON_AUTOCHECK,
		WS_EX_NONE,
		"radio button 2",
		radioGroup_props, //props,
		btn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
    {
		NCSCTRL_RADIOBUTTON, 
		ID_BTN2,
		10, 120, 150, 25,
		WS_VISIBLE | NCSS_BUTTON_AUTOCHECK,
		WS_EX_NONE,
		"radio button 3",
		radioGroup_props, //props,
		btn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
};


static NCS_EVENT_HANDLER mymain_handlers[] = {
	{MSG_CREATE,mymain_onCreate},
	{MSG_CLOSE,mymain_onClose},
	{0, NULL}
};

//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_DIALOGBOX, 
	1,
	0, 0, 320, 320,
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
	if(argc>1)
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

