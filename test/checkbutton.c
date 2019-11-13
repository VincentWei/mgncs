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
#include "../include/mrdr.h"

#ifdef _MGNCSCTRL_DIALOGBOX

#define ID_BTN0		100
#define ID_BTN1		101
#define ID_BTN2		102
#define ID_BTN3		103
#define ID_BTN4		104
#define ID_BTN5		105

static BITMAP bmp;


static BOOL mymain_onCreate(mWidget* _this, DWORD add_data)
{
	//TODO : initialize
	//set image
	if (LoadBitmapFromFile(HDC_SCREEN, &bmp, "icon.png") != 0) {
		printf("cannot load image file \"icon.png\"\n");
	}

	mButton *mb1 = (mButton*)ncsGetChildObj(_this->hwnd, ID_BTN1);
	if (NULL != mb1)
		_c(mb1)->setProperty(mb1, NCSP_BUTTON_IMAGE, (DWORD)&bmp);

	mb1 = (mButton*)ncsGetChildObj(_this->hwnd, ID_BTN2);
	if (NULL != mb1)
		_c(mb1)->setProperty(mb1, NCSP_BUTTON_IMAGE, (DWORD)&bmp);

	mb1 = (mButton*)ncsGetChildObj(_this->hwnd, ID_BTN3);
	if (NULL != mb1) {
		_c(mb1)->setProperty(mb1, NCSP_BUTTON_IMAGE, (DWORD)&bmp);
		_c(mb1)->setProperty(mb1, NCSP_BUTTON_ALIGN, (DWORD)NCS_ALIGN_RIGHT);
	}

	mb1 = (mButton*)ncsGetChildObj(_this->hwnd, ID_BTN4);
	if (NULL != mb1)
		_c(mb1)->setProperty(mb1, NCSP_BUTTON_IMAGE, (DWORD)&bmp);

	mb1 = (mButton*)ncsGetChildObj(_this->hwnd, ID_BTN5);
	if (NULL != mb1) {
		_c(mb1)->setProperty(mb1, NCSP_BUTTON_IMAGE, (DWORD)&bmp);
		_c(mb1)->setProperty(mb1, NCSP_BUTTON_VALIGN, (DWORD)NCS_VALIGN_BOTTOM);
	}

	return TRUE;
}

static void mymain_onClose(mWidget* _this, int message)
{
	DestroyMainWindow(_this->hwnd);
	PostQuitMessage(0);
}

NCS_RDR_ELEMENT btn_rdr_elements[] = {
	{ NCS_MODE_USEFLAT, 1 },
	{ -1, 0 }
};

static NCS_RDR_INFO btn_rdr_info[] = {
	// { "flat", "flat", NULL },
	// { "skin", "skin", NULL },
	{ "fashion","fashion", btn_rdr_elements }
};

//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_CHECKBUTTON,
		ID_BTN0,
		40, 130, 80, 25,
		WS_BORDER | WS_VISIBLE,
		WS_EX_NONE,
		"button",
		NULL, //props,
		btn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_CHECKBUTTON,
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
		NCSCTRL_CHECKBUTTON,
		ID_BTN1,
		40, 10, 120, 30,
		WS_VISIBLE  | NCSS_BUTTON_AUTOCHECK | NCSS_BUTTON_CHECKABLE,
		WS_EX_NONE,
		"Auto Check button",
		NULL, //props,
		btn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_CHECKBUTTON,
		ID_BTN1,
		40, 80, 200, 30,
		WS_VISIBLE | NCSS_BUTTON_AUTOCHECK | NCSS_BUTTON_CHECKABLE | NCSS_BUTTON_3DCHECK,
		WS_EX_NONE,
		"3D Auto Check button",
		NULL, //props,
		btn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_CHECKBUTTON,
		ID_BTN2,
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
		NCSCTRL_CHECKBUTTON,
		ID_BTN3,
		300, 250, 200, 30,
		WS_VISIBLE | NCSS_BUTTON_IMAGELABEL | NCSS_BUTTON_CHECKABLE | NCSS_BUTTON_AUTOCHECK,
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
		NCSCTRL_CHECKBUTTON,
		ID_BTN4,
		40, 300, 200, 100,
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
		NCSCTRL_CHECKBUTTON,
		ID_BTN5,
		300, 300, 200, 100,
		WS_VISIBLE | NCSS_BUTTON_IMAGELABEL | NCSS_BUTTON_VERTIMAGELABEL | NCSS_BUTTON_CHECKABLE | NCSS_BUTTON_AUTOCHECK,
		WS_EX_NONE,
		"Vert Bottom Image Label",
		NULL, //props,
		btn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
/*
 	{
		NCSCTRL_RADIOBUTTON,
		ID_BTN0,
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
		ID_BTN0,
		10, 80 , 100, 25,
		WS_VIS IBLE,
		WS_EX_ NONE,
		"check  button",
		NULL, //props,
		chkbtn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
*/
};


static NCS_EVENT_HANDLER mymain_handlers[] = {
	{ MSG_CREATE, mymain_onCreate },
	{ MSG_CLOSE, mymain_onClose },
	{ 0, NULL }
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


