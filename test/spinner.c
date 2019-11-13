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

#if defined _MGNCSCTRL_SPINBOX && defined _MGNCSCTRL_DIALOGBOX


#define ID_SPINNER1	  101
#define ID_SPINNER2	  102

#define MINVALUE		0
#define MAXVALUE		10
#define CURVALUE		5

static int offset = 10, cell = MAXVALUE - MINVALUE + 1, step = 20;
static int cur_x = CURVALUE, cur_y = CURVALUE;


static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
	ncsSetProperty(GetDlgItem(self->hwnd, ID_SPINNER1),
			NCSP_SPNR_TARGET, (DWORD)self->hwnd);
	ncsSetProperty(GetDlgItem(self->hwnd, ID_SPINNER2),
			NCSP_SPNR_TARGET, (DWORD)self->hwnd);

	return TRUE;
}

static void mymain_onClose (mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
}

static void getRectByCellIdx(int posX, int posY, RECT *rc)
{
	if (rc) {
		rc->left = offset + posX * step;
		rc->top = offset + posY * step;
		rc->right = rc->left + step;
		rc->bottom = rc->top + step;
	}
}

static BOOL mymain_onKeyDown (mWidget* self, int message, int wParam, DWORD lParam)
{
	int cur = -1, step, newPos = -1;
	BOOL refresh = FALSE;
	RECT oldrc, newrc;
	int sign = 1;

	if (lParam & KS_SPINPOST) {
		int old_x = cur_x, old_y = cur_y;
		cur_y = (int)ncsGetProperty(GetDlgItem(self->hwnd, ID_SPINNER1),
				NCSP_SPNR_CURPOS);
		cur_x = (int)ncsGetProperty(GetDlgItem(self->hwnd, ID_SPINNER2),
				NCSP_SPNR_CURPOS);

		if (old_x == cur_x && old_y == cur_y)
			return TRUE;

		getRectByCellIdx(old_x, old_y, &oldrc);
		getRectByCellIdx(cur_x, cur_y, &newrc);

		InvalidateRect(self->hwnd, &oldrc, TRUE);
		InvalidateRect(self->hwnd, &newrc, TRUE);
	} else {
		switch (wParam) {
			case SCANCODE_CURSORBLOCKUP:
				sign = -1;
			case SCANCODE_CURSORBLOCKDOWN: {
				cur = (int)ncsGetProperty(GetDlgItem(self->hwnd, ID_SPINNER1),
						NCSP_SPNR_CURPOS);
				step = (int)ncsGetProperty(GetDlgItem(self->hwnd, ID_SPINNER1),
						NCSP_SPNR_LINESTEP);

				newPos = cur + sign * step;

				if (newPos < MINVALUE || newPos > MAXVALUE)
					return TRUE;

				cur_y = newPos;
				refresh = TRUE;
				ncsSetProperty(GetDlgItem(self->hwnd, ID_SPINNER1),
						NCSP_SPNR_CURPOS, newPos);

				//old rect
				getRectByCellIdx(cur_x, cur, &oldrc);
				getRectByCellIdx(cur_x, newPos, &newrc);
				break;
			}

			case SCANCODE_CURSORBLOCKLEFT:
				sign = -1;
			case SCANCODE_CURSORBLOCKRIGHT: {
				cur = (int)ncsGetProperty(GetDlgItem(self->hwnd, ID_SPINNER2),
						NCSP_SPNR_CURPOS);
				step = (int)ncsGetProperty(GetDlgItem(self->hwnd, ID_SPINNER2),
						NCSP_SPNR_LINESTEP);
				newPos = cur + sign * step;

				if (newPos < MINVALUE || newPos > MAXVALUE)
					return TRUE;

				cur_x = newPos;
				refresh = TRUE;
				ncsSetProperty(GetDlgItem(self->hwnd, ID_SPINNER2),
						NCSP_SPNR_CURPOS, newPos);
				//old rect
				getRectByCellIdx(cur, cur_y, &oldrc);
				getRectByCellIdx(newPos, cur_y, &newrc);
				break;
			}
		}

		if (refresh) {
			InvalidateRect(self->hwnd, &oldrc, TRUE);
			InvalidateRect(self->hwnd, &newrc, TRUE);
		}
	}

	return TRUE;
}

static void mymain_onPaint (mWidget *self, HDC hdc, const CLIPRGN* rgn)
{
	int i;

	//draw cell box
	for (i = 0; i <= cell; i++) {
		//row
		MoveTo (hdc, offset,			offset + i*step);
		LineTo (hdc, offset + step*cell,offset + i*step);
		//column
		MoveTo (hdc, offset + i*step,   offset);
		LineTo (hdc, offset + i*step,   offset + step*cell);
	}

	SetBrushColor (hdc, PIXEL_black);
	FillBox (hdc, offset + cur_x * step, offset + cur_y * step, step, step);
}

//Propties for
static NCS_PROP_ENTRY spinbox_props [] = {
	{NCSP_SPNR_MINPOS, MINVALUE},
	{NCSP_SPNR_MAXPOS, MAXVALUE},
	{NCSP_SPNR_CURPOS, CURVALUE},
	{NCSP_SPNR_LINESTEP, 1},
	{0, 0}
};

static NCS_RDR_INFO spin_rdr_info[] = {
	{"classic", "classic"}
	//{"skin", "skin", NULL},
	//{"fashion", "fashion", NULL},
	//{"flat", "flat", NULL},
};

//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_SPINNER ,
		ID_SPINNER1,
		290, 150, 20, 20,
		WS_VISIBLE | NCSS_SPNBOX_VERTICAL | NCSS_NOTIFY,
		WS_EX_NONE,
		"spinbox",
		spinbox_props, //props,
		spin_rdr_info, //NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_SPINNER ,
		ID_SPINNER2,
		290, 100, 20, 20,
		WS_VISIBLE | NCSS_SPNBOX_HORIZONTAL | NCSS_NOTIFY,
		WS_EX_NONE,
		"spinbox",
		spinbox_props, //props,
		spin_rdr_info, //NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	}
};

static NCS_EVENT_HANDLER mymain_handlers[] = {
	{MSG_CREATE,mymain_onCreate},
	{MSG_CLOSE,mymain_onClose},
	{MSG_PAINT,mymain_onPaint},
	{MSG_KEYDOWN,mymain_onKeyDown},
	{0, NULL}
};

static NCS_RDR_INFO mymain_rdr_info[] =
{
	{"skin", "skin", NULL},
};


//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_DIALOGBOX,
	1,
	0, 0, 360, 300,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
	"Spinner Test ....",
	NULL,
	mymain_rdr_info,
	mymain_handlers,
	_ctrl_templ,
	sizeof(_ctrl_templ)/sizeof(NCS_WND_TEMPLATE),
	0,
	0, 0,
};

int MiniGUIMain(int argc, const char* argv[])
{
	if (argc > 1) {
		spin_rdr_info[0].glb_rdr = argv[1];
		spin_rdr_info[0].ctl_rdr = argv[1];
	}

	ncsInitialize();

	mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect(
			&mymain_templ, HWND_DESKTOP);

	_c(mydlg)->doModal(mydlg, TRUE);

	ncsUninitialize();

	return 0;
}

#else //_MGNCSCTRL_SPINBOX _MGNCSCTRL_DIALOGBOX

int main (void)
{
	printf("\n==========================================================\n");
	printf("======== You haven't enable the spinbox, dialogbox contorl =====\n");
	printf("==========================================================\n");
	printf("============== ./configure --enable-spinbox --enable-dialogbox ==========\n");
	printf("==========================================================\n\n");
	return 0;
}
#endif	//_MGNCSCTRL_SPINNER _MGNCSCTRL_DIALOGBOX

