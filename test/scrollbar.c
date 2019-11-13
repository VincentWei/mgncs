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

#if defined _MGNCSCTRL_SCROLLBAR && defined _MGNCSCTRL_RECTANGLE && _MGNCSCTRL_DIALOGBOX

#define ID_SBR  100
#define ID_SBG  101
#define ID_SBB  102
#define ID_RECT 200


static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
	return TRUE;
}

static void mymain_onClose(mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
}

//define the progress properites
static NCS_PROP_ENTRY scrollbar_props [] = {
	{NCSP_SCRLBR_MAXPOS, 255},
	{NCSP_SCRLBR_MINPOS, 0 },
	{NCSP_SCRLBR_LINESTEP, 5},
	{NCSP_SCRLBR_CURPOS, 0 },
	{ 0, 0 }
};

static void scrollbar_notify(mScrollBar* self, int id, int code, DWORD add_data)
{
	mRectangle *rect = (mRectangle*)ncsGetChildObj(GetParent(self->hwnd), ID_RECT);

	if (rect) {
		DWORD fill_color = _c(rect)->getProperty(rect, NCSP_RECTANGLE_FILLCOLOR);
		int r = GetRValue(fill_color);
		int g = GetGValue(fill_color);
		int b = GetBValue(fill_color);
		int v = _c(self)->getProperty(self, NCSP_SCRLBR_CURPOS);
		switch(id) {
		case ID_SBR:
			r = v;
			break;
		case ID_SBG:
			g = v;
			break;
		case ID_SBB:
			b = v;
			break;
		}

		fill_color = MakeRGBA(r, g, b,255);

		_c(rect)->setProperty(rect, NCSP_RECTANGLE_FILLCOLOR, fill_color);
		InvalidateRect(rect->hwnd, NULL, TRUE);
	}
}

static NCS_EVENT_HANDLER scrollbar_notifies[] = {
	NCS_MAP_NOTIFY(NCSN_SCRLBR_CHANGED, scrollbar_notify),
	{0, NULL}
};

static NCS_RDR_INFO sb_rdr_info[] = {
	{"flat", "flat", NULL},
	//{"skin", "skin", NULL},
	//{"fashion","fashion", NULL}
};

//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_SCROLLBAR,
		ID_SBR,
		10, 20, 200, 25,
		WS_BORDER | NCSS_NOTIFY | WS_VISIBLE,
		WS_EX_NONE,
		"",
		scrollbar_props, //props,
		sb_rdr_info, //rdr_info
		scrollbar_notifies,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_SCROLLBAR,
		ID_SBR,
		10, 50, 200, 25,
		WS_BORDER | NCSS_NOTIFY | WS_VISIBLE | NCSS_SCRLBR_ARROWS,
		WS_EX_NONE,
		"",
		scrollbar_props, //props,
		sb_rdr_info, //rdr_info
		scrollbar_notifies,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_SCROLLBAR,
		ID_SBG,
		10, 80, 200, 25,
		WS_BORDER | NCSS_NOTIFY | WS_VISIBLE | NCSS_SCRLBR_ARROWS | NCSS_SCRLBR_RIGHTDBLARROWS,
		WS_EX_NONE,
		"",
		scrollbar_props, //props,
		sb_rdr_info, //rdr_info
		scrollbar_notifies,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_SCROLLBAR,
		ID_SBG,
		10, 110, 200, 25,
		WS_BORDER | NCSS_NOTIFY | WS_VISIBLE | NCSS_SCRLBR_RIGHTDBLARROWS,
		WS_EX_NONE,
		"",
		scrollbar_props, //props,
		sb_rdr_info, //rdr_info
		scrollbar_notifies,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_SCROLLBAR,
		ID_SBB,
		10, 140, 200, 25,
		WS_BORDER | NCSS_NOTIFY | WS_VISIBLE | NCSS_SCRLBR_ARROWS | NCSS_SCRLBR_LEFTDBLARROWS,
		WS_EX_NONE,
		"",
		scrollbar_props, //props,
		sb_rdr_info, //rdr_info
		scrollbar_notifies,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_SCROLLBAR,
		ID_SBB,
		10, 170, 200, 25,
		WS_BORDER | NCSS_NOTIFY | WS_VISIBLE | NCSS_SCRLBR_LEFTDBLARROWS,
		WS_EX_NONE,
		"",
		scrollbar_props, //props,
		sb_rdr_info, //rdr_info
		scrollbar_notifies,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_SCROLLBAR,
		ID_SBR,
		10, 200, 25, 200,
		WS_BORDER | NCSS_NOTIFY | WS_VISIBLE | NCSS_SCRLBR_VERTICAL,
		WS_EX_NONE,
		"",
		scrollbar_props, //props,
		sb_rdr_info, //rdr_info
		scrollbar_notifies,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_SCROLLBAR,
		ID_SBR,
		40, 200, 25, 200,
		WS_BORDER | NCSS_NOTIFY | WS_VISIBLE | NCSS_SCRLBR_ARROWS | NCSS_SCRLBR_VERTICAL,
		WS_EX_NONE,
		"",
		scrollbar_props, //props,
		sb_rdr_info, //rdr_info
		scrollbar_notifies,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_SCROLLBAR,
		ID_SBG,
		70, 200, 25, 200,
		WS_BORDER | NCSS_NOTIFY | WS_VISIBLE | NCSS_SCRLBR_ARROWS | NCSS_SCRLBR_RIGHTDBLARROWS | NCSS_SCRLBR_VERTICAL,
		WS_EX_NONE,
		"",
		scrollbar_props, //props,
		sb_rdr_info, //rdr_info
		scrollbar_notifies,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_SCROLLBAR,
		ID_SBG,
		100, 200, 25, 200,
		WS_BORDER | NCSS_NOTIFY | WS_VISIBLE | NCSS_SCRLBR_RIGHTDBLARROWS | NCSS_SCRLBR_VERTICAL,
		WS_EX_NONE,
		"",
		scrollbar_props, //props,
		sb_rdr_info, //rdr_info
		scrollbar_notifies,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_SCROLLBAR,
		ID_SBB,
		130, 200, 25, 200,
		WS_BORDER | NCSS_NOTIFY | WS_VISIBLE | NCSS_SCRLBR_ARROWS | NCSS_SCRLBR_LEFTDBLARROWS | NCSS_SCRLBR_VERTICAL,
		WS_EX_NONE,
		"",
		scrollbar_props, //props,
		sb_rdr_info, //rdr_info
		scrollbar_notifies,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_SCROLLBAR,
		ID_SBB,
		160, 200, 25,200,
		WS_BORDER | NCSS_NOTIFY | WS_VISIBLE | NCSS_SCRLBR_LEFTDBLARROWS | NCSS_SCRLBR_VERTICAL,
		WS_EX_NONE,
		"",
		scrollbar_props, //props,
		sb_rdr_info, //rdr_info
		scrollbar_notifies,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_RECTANGLE,
		ID_RECT,
		200, 200, 40, 40,
		WS_VISIBLE,
		WS_EX_NONE,
		"",
		NULL, //props,
		NULL,//btn2_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
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
	0, 0, 300, 500,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
	"scrollbar Test ....",
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
		sb_rdr_info[0].glb_rdr = argv[1];
		sb_rdr_info[0].ctl_rdr = argv[1];
	}

	ncsInitialize();

	mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect(
			&mymain_templ, HWND_DESKTOP);

	_c(mydlg)->doModal(mydlg, TRUE);

	ncsUninitialize();

	return 0;
}


#else //_MGNCSCTRL_SCROLLBAR _MGNCSCTRL_RECTANGLE

int main (void)
{
	printf("\n==========================================================\n");
	printf("======== You haven't enable the scrollbar, rectangle, dialogbox contorl =====\n");
	printf("==========================================================\n");
	printf("============== ./configure --enable-scrollbar --enable-rectangle --enable-dialogbox ==========\n");
	printf("==========================================================\n\n");
	return 0;
}
#endif	//_MGNCSCTRL_SCROLLBAR _MGNCSCTRL_RECTANGLE
