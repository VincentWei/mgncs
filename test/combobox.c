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
 *   <http://www.minigui.com/en/about/licensing-policy/>.
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

#if defined _MGNCSCTRL_COMBOBOX && defined _MGNCSCTRL_DIALOGBOX


#define IDC_COMBO1  100
#define IDC_COMBO2  101
#define IDC_COMBO3  102

static char* item[] = {
	"List item ---0",
	"List item ---1",
	"List item ---2",
	"List item ---3",
	"List item ---4"
};

static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
	int i;
	mCombobox *com;

	for (i = 0; i < sizeof(item) / sizeof(char*); i++) {
		com = (mCombobox *)ncsGetChildObj(self->hwnd, IDC_COMBO1);
		_c(com)->addItem (com, item[i], 0);
		com = (mCombobox *)ncsGetChildObj(self->hwnd, IDC_COMBO2);
		_c(com)->addItem (com, item[i], 0);
		com = (mCombobox *)ncsGetChildObj(self->hwnd, IDC_COMBO3);
		_c(com)->addItem (com, item[i], 0);
	}

	return TRUE;
}

static void mymain_onClose(mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
}

static NCS_EVENT_HANDLER combo_handlers[] = {
	{ 0, NULL }
};

static NCS_PROP_ENTRY combo_props[] = {
	{ NCSP_COMB_DROPDOWNHEIGHT, 60 },
	{ 0, 0 }
};

static NCS_RDR_INFO combo_rdr_info[] = {
	{ "classic", "classic", NULL },
	// { "flat", "flat", NULL },
	// { "skin", "skin", NULL },
};

static NCS_WND_TEMPLATE combobox_templ[] = {
	{
		NCSCTRL_COMBOBOX,
		IDC_COMBO1,
		50, 10, 190, 20,
		WS_BORDER | WS_VISIBLE | NCSS_CMBOX_DROPDOWNLIST,
		WS_EX_TRANSPARENT,
		"input edit 1...",
		combo_props,
		combo_rdr_info,
		combo_handlers,
		NULL,
		0,
		0
	},
	{
		NCSCTRL_COMBOBOX,
		IDC_COMBO2,
		10, 100, 190, 100,
		WS_BORDER | WS_VISIBLE | NCSS_CMBOX_SIMPLE,
		WS_EX_NONE,
		"input edit 2...",
		combo_props,
		combo_rdr_info,
		combo_handlers,
		NULL,
		0,
		0
	},
	{
		NCSCTRL_COMBOBOX,
		IDC_COMBO3,
		10, 550, 190, 20,
		WS_BORDER | WS_VISIBLE | NCSS_CMBOX_DROPDOWNLIST,
		WS_EX_NONE,
		"input edit 3...",
		combo_props,
		combo_rdr_info,
		combo_handlers,
		NULL,
		0,
		0
	}
};

static NCS_EVENT_HANDLER mymain_handlers[] = {
	{ MSG_CREATE, mymain_onCreate },
	{ MSG_CLOSE, mymain_onClose },
	{ 0, NULL }
};

static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_DIALOGBOX,
	1,
	0, 0, 800, 600,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
	"ComboBox tesing ....",
	NULL,
	NULL,
	mymain_handlers,
	combobox_templ,
	sizeof(combobox_templ) / sizeof(NCS_WND_TEMPLATE),
	0,
	0,
	0,
};

int MiniGUIMain(int argc, const char* argv[])
{
	if (argc > 1) {
		combo_rdr_info[0].glb_rdr = argv[1];
		combo_rdr_info[0].ctl_rdr = argv[1];
	}

	ncsInitialize();

	mDialogBox* mydlg = (mDialogBox*)ncsCreateMainWindowIndirect(
			&mymain_templ, HWND_DESKTOP);

	_c(mydlg)->doModal(mydlg, TRUE);

	ncsUninitialize();

	return 0;
}


#else //_MGNCSCTRL_COMBOBOX _MGNCSCTRL_DIALOGBOX

int main (void)
{
	printf("\n==========================================================\n");
	printf("======== You haven't enable the combobox, dialogbox contorl =====\n");
	printf("==========================================================\n");
	printf("============== ./configure --enable-combobox --enable-dialogbox==========\n");
	printf("==========================================================\n\n");
	return 0;
}
#endif	//_MGNCSCTRL_COMBOBOX _MGNCSCTRL_DIALOGBOX
