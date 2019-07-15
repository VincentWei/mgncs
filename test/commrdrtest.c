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
#include "../include/mrdr.h"

#ifdef _MGNCSCTRL_DIALOGBOX

static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
	//TODO : initialize
	return TRUE;
}

static void draw_3dbox(HDC hdc, RECT* rc, DWORD flag)
{
	int top = rc->top;
	int bottom = rc->bottom;
	DWORD color = MakeRGBA(0,0,0xFF,0);

	ncsCommRDRDraw3dbox(hdc,rc, color, flag);
	rc->top = bottom + 10;
	rc->bottom = rc->top + bottom - top;
	ncsCommRDRDraw3dbox(hdc,rc, color, flag | NCSRF_FILL);
	rc->top = rc->bottom + 10;
	rc->bottom = rc->top + bottom - top;
	ncsCommRDRDraw3dbox(hdc,rc, color, flag | NCSRF_INNERFRAME);
	rc->top = rc->bottom + 10;
	rc->bottom = rc->top + bottom - top;
	ncsCommRDRDraw3dbox(hdc,rc, color, flag | NCSRF_FILL | NCSRF_INNERFRAME);

	rc->top = top;
	rc->bottom = bottom;
}

static void draw_3DRoundBox(HDC hdc, RECT* rc, int rx, int ry, DWORD flag)
{
	int top = rc->top;
	int bottom = rc->bottom;
	DWORD color = MakeRGBA(0,0,0xFF,0);

	ncsCommRDRDraw3DRoundBox(hdc,rc, rx, ry, color, flag);
	rc->top = bottom + 10;
	rc->bottom = rc->top + bottom - top;
	ncsCommRDRDraw3DRoundBox(hdc,rc, rx, ry, color, flag | NCSRF_FILL);
	rc->top = rc->bottom + 10;
	rc->bottom = rc->top + bottom - top;
	ncsCommRDRDraw3DRoundBox(hdc,rc, rx, ry, color, flag | NCSRF_INNERFRAME);
	rc->top = rc->bottom + 10;
	rc->bottom = rc->top + bottom - top;
	ncsCommRDRDraw3DRoundBox(hdc,rc, rx, ry, color, flag | NCSRF_FILL | NCSRF_INNERFRAME);

	rc->top = top;
	rc->bottom = bottom;
}

static void draw_3dellipse(HDC hdc, RECT *rc)
{
	DWORD color2 = MakeRGBA(0x00,0x80,0x80,0xFF);
	DWORD color1 = MakeRGBA(0x00,0xCC,0xCC,0xFF);
	int width = RECTWP(rc);

	ncsCommRDRDraw3DEllipse(hdc, rc, color1, color2, 0);
	rc->left = rc->right + 10;
	rc->right = rc->left + width;
	ncsCommRDRDraw3DEllipse(hdc, rc, color2,color1, NCSRS_SIGNIFICANT);
}

static void draw_radiobox(HDC hdc, RECT* rc, DWORD flag)
{
	int top = rc->top;
	int bottom = rc->bottom;
	DWORD fgcolor = 0;
	DWORD bgcolor = MakeRGBA(255,255,255,255);
	DWORD discolor = MakeRGBA(128,128,128,255);

	ncsCommRDRDrawRadio(hdc,rc,fgcolor,bgcolor, discolor,
			flag | NCSRF_FILL | NCSRF_FRAME);
	rc->top = bottom + 10;
	rc->bottom = rc->top + bottom - top;
	ncsCommRDRDrawRadio(hdc,rc,fgcolor,bgcolor, discolor,
			flag | NCSRF_FILL | NCSRF_FRAME | NCSRS_DISABLE);
	rc->top = rc->bottom + 10;
	rc->bottom = rc->top + bottom - top;
	ncsCommRDRDrawRadio(hdc,rc,fgcolor,bgcolor, discolor, flag | NCSRF_FRAME);
	rc->top = rc->bottom + 10;
	rc->bottom = rc->top + bottom - top;
	ncsCommRDRDrawRadio(hdc,rc,fgcolor,bgcolor, discolor, flag | NCSRF_FILL);

	rc->top = top;
	rc->bottom = bottom;
}



static void draw_checkbox(HDC hdc, RECT* rc, DWORD flag)
{
	int top = rc->top;
	int bottom = rc->bottom;
	DWORD fgcolor = 0;
	DWORD bgcolor = MakeRGBA(255,255,255,255);
	DWORD discolor = MakeRGBA(128,128,128,255);

	ncsCommRDRDrawCheckbox(hdc, rc,fgcolor,bgcolor, discolor,
			flag | NCSRF_FILL | NCSRF_FRAME);
	rc->top = bottom + 10;
	rc->bottom = rc->top + bottom - top;
	ncsCommRDRDrawCheckbox(hdc,rc,fgcolor,bgcolor, discolor,
			flag | NCSRF_FILL | NCSRF_FRAME | NCSRS_DISABLE);
	rc->top = rc->bottom + 10;
	rc->bottom = rc->top + bottom - top;
	ncsCommRDRDrawCheckbox(hdc,rc,fgcolor,bgcolor, discolor, flag | NCSRF_FRAME);
	rc->top = rc->bottom + 10;
	rc->bottom = rc->top + bottom - top;
	ncsCommRDRDrawCheckbox(hdc,rc,fgcolor,bgcolor, discolor, flag | NCSRF_FILL);

	rc->top = top;
	rc->bottom = bottom;
}

static void draw_arrow(HDC hdc, RECT* rc, int arrow)
{
	int top = rc->top;
	int bottom = rc->bottom;
	DWORD color = MakeRGBA(0,0,255,255);

	ncsCommRDRDrawArrow(hdc, rc,arrow, color, FALSE);
	rc->top = rc->bottom + 10;
	rc->bottom = rc->top + bottom - top;
	ncsCommRDRDrawArrow(hdc, rc,arrow, color, TRUE);

	rc->top = top;
	rc->bottom = bottom;
}

static void mymain_onPaint(mWidget* self, HDC hdc, const CLIPRGN* inv)
{
	int width = 50;
	int height = 50;
	RECT rc = {0, 10, width, height + 10};

	draw_3dbox(hdc, &rc, NCSRS_NORMAL);
	rc.left = rc.right + 10;
	rc.right = rc.left + width;
	draw_3dbox(hdc, &rc, NCSRS_HIGHLIGHT);
	rc.left = rc.right + 10;
	rc.right = rc.left + width;
	draw_3dbox(hdc, &rc, NCSRS_SIGNIFICANT);
	rc.left = rc.right + 10;
	rc.right = rc.left + width;
	draw_3dbox(hdc, &rc, NCSRS_DISABLE);

	rc.left = rc.right + 10;
	int rx = 5;
	int ry = 5;

	draw_3DRoundBox(hdc, &rc,rx, ry,  NCSRS_NORMAL);
	rc.left = rc.right + 10;
	rc.right = rc.left + width;
	draw_3DRoundBox(hdc, &rc,rx, ry, NCSRS_HIGHLIGHT);
	rc.left = rc.right + 10;
	rc.right = rc.left + width;
	draw_3DRoundBox(hdc, &rc,rx, ry, NCSRS_SIGNIFICANT);
	rc.left = rc.right + 10;
	rc.right = rc.left + width;
	draw_3DRoundBox(hdc, &rc, rx, ry,NCSRS_DISABLE);

	rc.left = 0;
	rc.top = rc.bottom + height*4 + 20;
	rc.bottom = rc.top + height;
	rc.right = width;
	draw_3dellipse(hdc, &rc);

	rc.top = rc.bottom + height*4 + 20;
	rc.bottom = rc.top + height;
	rc.left = 0;
	rc.right = width;
	draw_checkbox(hdc, &rc, NCSRS_NORMAL);
	rc.left = rc.right + 10;
	rc.right = rc.left + width;
	draw_checkbox(hdc, &rc, NCSRS_HIGHLIGHT);
	rc.left = rc.right + 10;
	rc.right = rc.left + width;
	draw_checkbox(hdc, &rc, NCSRS_SIGNIFICANT);

	rc.left = rc.right + 10;
	rc.right = rc.left + width;
	draw_radiobox(hdc, &rc, NCSRS_NORMAL);
	rc.left = rc.right + 10;
	rc.right = rc.left + width;
	draw_radiobox(hdc, &rc, NCSRS_HIGHLIGHT);
	rc.left = rc.right + 10;
	rc.right = rc.left + width;
	draw_radiobox(hdc, &rc, NCSRS_SIGNIFICANT);

	rc.left = 0;
	rc.right = rc.left + width;
	rc.top = rc.bottom + height*3 + 20;
	rc.bottom = rc.top + height;
	draw_arrow(hdc, &rc, NCSR_ARROW_LEFT);
	rc.left = rc.right + 10;
	rc.right = rc.left + width;
	draw_arrow(hdc, &rc, NCSR_ARROW_RIGHT);
	rc.left = rc.right + 10;
	rc.right = rc.left + width;
	draw_arrow(hdc, &rc, NCSR_ARROW_UP);
	rc.left = rc.right + 10;
	rc.right = rc.left + width;
	draw_arrow(hdc, &rc, NCSR_ARROW_DOWN);
}

static void mymain_onClose(mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
}

#if 0
//Propties for
static NCS_PROP_ENTRY _props[] = {
	{ 0, 0 }
};
#endif

/*
//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_ ,
		ID_,
		10, 10, 0, 0,
		WS_BORDER | WS_VISIBLE,
		WS_EX_NONE,
		"",
		NULL, //props,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
};
*/

static NCS_EVENT_HANDLER mymain_handlers[] = {
	{ MSG_CREATE, mymain_onCreate },
	{ MSG_CLOSE, mymain_onClose },
	{ MSG_PAINT, mymain_onPaint },
	{ 0,  NULL }
};

//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_DIALOGBOX,
	1,
	0, 0, 800, 600,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
	" Test ....",
	NULL,
	NULL,
	mymain_handlers,
	NULL,//_ctrl_templ,
	0,//sizeof(_ctrl_templ) / sizeof(NCS_WND_TEMPLATE),
	0,
	0, 0,
};

int MiniGUIMain(int argc, const char* argv[])
{
	ncsInitialize();

	mymain_templ.w = g_rcScr.right;
	mymain_templ.h = g_rcScr.bottom;

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

