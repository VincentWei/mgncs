///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/*
 *   This file is part of mGNCS, a component for MiniGUI.
 * 
 *   Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mgncs/mgncs.h>

#ifdef _MGNCSCTRL_DIALOGBOX

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


static char buff[1024];


static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
    sprintf(buff, "init");
    SetFocus(GetDlgItem(self->hwnd, IDC_SLEDIT));
	return TRUE;
}

static int mymain_onPaint(mWidget* self, HDC hdc)
{
    TextOut(hdc, 10, 200, buff);
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
	{NCSP_SLEDIT_TIPTEXT, (DWORD)"Tip Text Testing"},
	{ 0, 0 }
};

static NCS_RDR_INFO edit_rdr_info[] = {
    //{"skin", "skin", NULL},
    //{"fashion", "fashion", NULL},
	{"classic","classic",NULL}
};

static void btn_notify(mWidget *button, int id, int nc, DWORD add_data)
{
    int s, e;
    mSlEdit *edit = (mSlEdit *)ncsGetChildObj(GetParent(button->hwnd), IDC_SLEDIT);

    switch (id) {
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
			_c(edit)->getContent (edit, buff, 1024, 0, -1);
			RECT rc = {10, 200, 200, 230};
			InvalidateRect(GetParent(button->hwnd), &rc, TRUE);
            break;
        case IDC_BTN6 :
            _c(edit)->setSel (edit, 0, -1);
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
    }
}

static NCS_EVENT_HANDLER btn_handlers [] = {
    NCS_MAP_NOTIFY(NCSN_BUTTON_PUSHED, btn_notify),
	{0, NULL}
};

static NCS_WND_TEMPLATE sledit_templ[] = {
	{
		NCSCTRL_SLEDIT,
		IDC_SLEDIT,
		10, 10, 150, 30,
		WS_BORDER | WS_VISIBLE | NCSS_SLEDIT_TIP,
		WS_EX_NONE,
        "",
		edit_props,
		edit_rdr_info,
		edit_handlers,
		NULL,
		0,
		0
	},
	{
		NCSCTRL_SLEDIT,
		IDC_SLEDIT,
		10, 50, 150, 30,
		WS_BORDER | WS_VISIBLE | NCSS_EDIT_CENTER | NCSS_SLEDIT_PASSWORD,
		0,
        "edit",
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
		btn_handlers,
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
		btn_handlers,
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
		btn_handlers,
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
		btn_handlers,
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
		btn_handlers,
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
		btn_handlers,
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
		btn_handlers,
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
		btn_handlers,
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
		btn_handlers,
		NULL,
		0,
		0
	},
    {
		NCSCTRL_BUTTON,
		IDC_BTN10,
		250, 370, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "copy",
		NULL,
		NULL,
		btn_handlers,
		NULL,
		0,
		0
	},
    {
		NCSCTRL_BUTTON,
		IDC_BTN11,
		250, 410, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
        "paste",
		NULL,
		NULL,
		btn_handlers,
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
	0, 0, 400, 480,
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
	if (argc > 1) {
		edit_rdr_info[0].glb_rdr = argv[1];
		edit_rdr_info[0].ctl_rdr = argv[1];
	}

#ifdef _MGRM_PROCESSES
    JoinLayer (NAME_DEF_LAYER, NULL, 0, 0);
#endif

	ncsInitialize();

	mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect
                                (&mymain_templ, HWND_DESKTOP);
	_c(mydlg)->doModal(mydlg, TRUE);

	ncsUninitialize();

    return 0;
}


#else //_MGNCSCTRL_DIALOGBOX

int main (void)
{
	printf("\n==========================================================\n");
	printf("======== You haven't enable the dialogbox contorl =====\n");
	printf("==========================================================\n");
	printf("============== ./configure --enable-dialogbox ==========\n");
	printf("==========================================================\n\n");
	return 0;

}
#endif	//_MGNCSCTRL_DIALOGBOX

