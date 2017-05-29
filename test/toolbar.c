/*
 * toolbar.c
 *
 *  Created on: 2009-6-20
 *      Author: dongjunjie
 */

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
#define ID_BTN1  102
#define ID_PROG 200
#define ID_COMBO1 201

static NCS_PROP_ENTRY combo_props [] =
{
	{NCSP_COMB_DROPDOWNHEIGHT, 60},
	{ 0, 0 }
};

static NCS_WND_TEMPLATE combobox_templ={
	NCSCTRL_COMBOBOX,
	ID_COMBO1,
	0, 0, 100, 20,
	WS_VISIBLE | NCSS_CMBOX_DROPDOWNLIST,
	WS_EX_NONE,
	"input edit 1...",
	combo_props,
	NULL,
	NULL,
	NULL,
	0,
	0
};

static char * item [] =
{
    "List item ---0",
    "List item ---1",
    "List item ---2",
    "List item ---3"
};

static void init_combobox(mCombobox *com)
{
    int i;
    for (i = 0; i < 4; i++){
        _c(com)->addItem (com, item[i], 0);
    }
}

static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
	//TODO : initialize

	mToolBar *toolbar = (mToolBar*)ncsGetChildObj(self->hwnd, ID_BTN);
	mToolImage * toolimg1 = ncsNewToolImageFromFile("./toolbar1.png",4,TRUE, FALSE);
	mToolImage * toolimg2 = ncsNewToolImageFromFile("./toolbar2.png",4,TRUE, FALSE);
	mToolImage * toolimg3 = ncsNewToolImageFromFile("./toolbar3.png",4,TRUE, FALSE);
	mToolImage * toolimg4 = ncsNewToolImageFromFile("./toolbar4.png",4,TRUE, FALSE);
	if(toolbar)
	{
		mPopMenuMgr * popmenu = NEW(mPopMenuMgr);

		_c(popmenu)->addItem(popmenu,0, "item1", NULL, 200, 0, NULL, 0);
		_c(popmenu)->addItem(popmenu,0, "item2", NULL, 201, 0, NULL, 0);
		_c(popmenu)->addItem(popmenu,0, "item3", NULL, 202, 0, NULL, 0);
		_c(popmenu)->addItem(popmenu,0, "item4", NULL, 203, 0, NULL, 0);

		_c(toolbar)->addItem(toolbar,ncsCreatePushToolItem(100,toolimg1,NULL, 0), 0, 0, 0);
		_c(toolbar)->addItem(toolbar,ncsCreatePushToolItem(101,toolimg2,"test2", 0),0, 0, 0);
		_c(toolbar)->addItem(toolbar,ncsCreateSeparatorItem(), 0, 0, 0);
		_c(toolbar)->addItem(toolbar,ncsCreateMenuToolItem(103,toolimg3,NULL, 0, popmenu), 0, 0, 0);
		_c(toolbar)->addItem(toolbar,ncsCreateCheckToolItem(104,toolimg4,NULL, 0,NCS_TOOLITEM_CHECKED),0, 0, 0);
		_c(toolbar)->addItem(toolbar,ncsCreatePushToolItem(105,NULL,"test3", 0),0,0,0);

	}

	toolbar = (mToolBar*)ncsGetChildObj(self->hwnd, ID_BTN1);
	if(toolbar)
	{
		mWidget *widget;
		_c(toolbar)->addItem(toolbar,ncsCreateRadioToolItem(100,toolimg1,NULL, 0), 0, 0, 0);
		_c(toolbar)->addItem(toolbar,ncsCreateRadioToolItem(101,toolimg2, NULL, 0),0, 0, 0);
		_c(toolbar)->addItem(toolbar,ncsCreateSeparatorItem(), 0, 0, 0);
		widget = ncsCreateWindowIndirect(&combobox_templ, toolbar->hwnd);
		init_combobox((mCombobox*)widget);
		_c(toolbar)->addItem(toolbar,ncsCreateWidgetToolItem(widget),0, 50, 0);
	}

	return TRUE;
}

static void mymain_onClose(mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
}




//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_TOOLBAR,
		ID_BTN,
		0, 0, 200, 25,
		WS_BORDER | WS_VISIBLE,
		WS_EX_NONE,
		"toolbar",
		NULL, //props,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_TOOLBAR,
		ID_BTN1,
		0, 40, 200, 25,
		WS_BORDER | WS_VISIBLE,
		WS_EX_NONE,
		"toolbar",
		NULL, //props,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	}
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

