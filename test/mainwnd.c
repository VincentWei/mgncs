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


static const char* strbkname = NULL;
static int mode = 0;


static void mymain_onNCCreate(mMainWnd* self)
{
	printf("------ NCCREATE\n");
	if (strbkname) {
//		ncsSetRDRElementAttr(self->hwnd, RDR_BKIMAGE, (DWORD)strbkname);
//		ncsSetRDRElementAttr(self->hwnd, RDR_BKIMAGE_MODE, (DWORD)mode);
		_c(self)->setProperty(self, NCSP_WIDGET_BKIMAGE_FILE, (DWORD)strbkname);
		_c(self)->setProperty(self, NCSP_WIDGET_BKIMAGE_MODE, (DWORD)mode);
	}
}

static BOOL mymain_onCreate(mMainWnd* self, DWORD dwAddData )
{
	printf("%s:%s\n",__FUNCTION__, (char*)dwAddData);

	return TRUE;
}

static void mymain_onActive(mWidget*self, int message)
{
	printf("Active At:%x\n", message);
}

#if 0
static void mymain_onSizeChanging(mWidget* self, const RECT *rcExcept, RECT* rcReal)
{
	*rcReal = *rcExcept;

	printf("SizeChanging: Excpet RECT={%d,%d,%d,%d}\n", rcExcept->left, rcExcept->top, rcExcept->right, rcExcept->bottom);
}
#endif

static BOOL mymain_onSizeChanged(mWidget* self, RECT *rt)
{
	printf("SizeChanged: rt={%d,%d,%d,%d}\n", rt->left, rt->top, rt->right, rt->bottom);
	return TRUE;
}

static void mymain_onCSizeChanged(mWidget* self, int cx, int cy)
{
	printf("Client SizeChanged:%d,%d\n", cx, cy);
}

static BOOL mymain_onFontChanging(mWidget* self, PLOGFONT pfont)
{
	printf("FontChanging: %p\n", pfont);
	return TRUE;
}

static BOOL mymain_onEraseBkgnd(mWidget* self, HDC hdc, const RECT *inv_rect)
{
	printf(" Erase BKgnd: hdc=%p, inv_rect=%p\n", (void*)hdc, inv_rect);
	return FALSE;
}

static void mymain_onPaint(mWidget *self, HDC hdc, const CLIPRGN* inv)
{
	TextOut(hdc, 0, 0, "My Main Window Test");
}

static BOOL mymain_onClose(mWidget* self, int message)
{
	printf("Close\n");
	DestroyMainWindow(self->hwnd);
	return TRUE;
}

static BOOL mymain_onKeys(mWidget *self, int message, int scancode, DWORD key_flags)
{
	switch (message) {
	case MSG_KEYDOWN:
		printf("KeyDown : "); break;
	case MSG_KEYUP:
		printf("KeyUp : "); break;
	case MSG_CHAR:
		printf("Char : ");  break;
	}

	printf(" cancode = %c (%d), key flags=%x\n", scancode, scancode, (int)key_flags);
	return TRUE;
}

static BOOL mymain_onMouses(mWidget * self, int message, int x, int y, DWORD key_flags)
{
	switch (message) {
	case MSG_LBUTTONDOWN:
		printf("Left Button Down : ");break;
	case MSG_LBUTTONUP:
		printf("Left Button Up : "); break;
	case MSG_LBUTTONDBLCLK:
		printf("Left Button Dobule Click : "); break;
	case MSG_MOUSEMOVE:
		printf("Mouse Move : "); break;
	case MSG_RBUTTONDOWN:
		printf("Right Button Down : "); break;
	case MSG_RBUTTONUP:
		printf("Right Button Up : "); break;
	case MSG_RBUTTONDBLCLK:
		printf("Right Button Dobule Click : "); break;
	}

	printf("%d,%d, flags=%x\n", x, y, (int)key_flags);
	return TRUE;
}

static BOOL mymain_onNCMouses(mWidget* self, int message, int x, int y, int hit_code)
{
	switch (message) {
	case MSG_NCLBUTTONDOWN:
		printf("NC Left Button Down : ");break;
	case MSG_NCLBUTTONUP:
		printf("NC Left Button Up : "); break;
	case MSG_NCLBUTTONDBLCLK:
		printf("NC Left Button Dobule Click : "); break;
	case MSG_NCMOUSEMOVE:
		printf("NC Mouse Move : "); break;
	case MSG_NCRBUTTONDOWN:
		printf("NC Right Button Down : "); break;
	case MSG_NCRBUTTONUP:
		printf("NC Right Button Up : "); break;
	case MSG_NCRBUTTONDBLCLK:
		printf("NC Right Button Dobule Click : "); break;
	}

	printf("%d,%d, hit_code=%x\n", x, y, hit_code);
	return FALSE;

}

static  void mymain_onDestroy(mWidget* self, int message)
{
	printf("destroy this window\n");
	PostQuitMessage(0);
}

static NCS_EVENT_HANDLER mymain_handlers [] = {
	{ MSG_NCCREATE, mymain_onNCCreate },
	{ MSG_CREATE, mymain_onCreate },
	{ MSG_ACTIVE, mymain_onActive },
	//{ MSG_SIZECHANGING, mymain_onSizeChanging },
	{ MSG_SIZECHANGED, mymain_onSizeChanged },
	{ MSG_CSIZECHANGED, mymain_onCSizeChanged },
	{ MSG_FONTCHANGING, mymain_onFontChanging },
	{ MSG_ERASEBKGND, mymain_onEraseBkgnd },
	{ MSG_PAINT, mymain_onPaint },
	{ MSG_CLOSE, mymain_onClose },
	{ MSG_KEYDOWN, mymain_onKeys },
	{ MSG_KEYUP, mymain_onKeys },
	{ MSG_CHAR, mymain_onKeys },
	{ MSG_LBUTTONDOWN, mymain_onMouses },
	{ MSG_LBUTTONUP, mymain_onMouses },
	{ MSG_LBUTTONDBLCLK, mymain_onMouses },
	{ MSG_MOUSEMOVE, mymain_onMouses },
	{ MSG_RBUTTONDOWN, mymain_onMouses },
	{ MSG_RBUTTONUP, mymain_onMouses },
	{ MSG_RBUTTONDBLCLK, mymain_onMouses },
	{ MSG_NCLBUTTONDOWN, mymain_onNCMouses },
	{ MSG_NCLBUTTONUP, mymain_onNCMouses },
	{ MSG_NCLBUTTONDBLCLK, mymain_onMouses },
	{ MSG_NCMOUSEMOVE, mymain_onNCMouses },
	{ MSG_NCRBUTTONDOWN, mymain_onNCMouses },
	{ MSG_NCRBUTTONUP, mymain_onNCMouses },
	{ MSG_NCRBUTTONDBLCLK, mymain_onMouses },
	{ MSG_DESTROY, mymain_onDestroy },
	{ 0, NULL }
};

NCS_RDR_INFO rdr_info[] = {
	{ "fashion", "fashion", NULL }
};

int MiniGUIMain(int argc, const char* argv[])
{
	MSG Msg;
	ncsInitialize();

	printf("usage %s [bkground bitmap] [mode]\n",argv[0]);
	printf("\t mode is:\n\t\t0 -- normal\n\t\t1 -- scaled\n\t\t2 -- title\n");

	//get bkground bitmap
	if (argc >= 2) {
		strbkname = argv[1];
		LoadResource(strbkname, RES_TYPE_IMAGE, (DWORD)HDC_SCREEN);
	}

	if (argc >= 3) {
		mode = atoi(argv[2]);
	}

	mWidget* mymain = ncsCreateMainWindow(NCSCTRL_MAINWND, "My Test",
		WS_CAPTION|WS_BORDER|WS_VISIBLE,
		WS_EX_NONE,
		1,
		0, 0, 600,480,
		HWND_DESKTOP,
		0, 0,
		NULL,
		rdr_info,
		mymain_handlers,
		(DWORD)"Hello");

	while (GetMessage(&Msg, mymain->hwnd)) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	//ReleaseRes(Str2Key(strbkname));
	ncsUninitialize();

	return 0;
}

