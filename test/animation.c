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
//#include "../include/mrdr.h"

#if defined _MGNCSCTRL_ANIMATE && defined _MGNCSCTRL_DIALOGBOX

#define IDC_ANI0		100
#define IDC_BTN1		101
#define IDC_BTN2		102
#define IDC_BTN3		103
#define IDC_ANIM		104


static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
	//TODO : initialize
	return TRUE;
}

static void mymain_onClose(mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
}

//Propties for
static NCS_PROP_ENTRY animate_props [] = {
	{ NCSP_ANMT_GIFFILE, (DWORD)"./res/cube.gif" },
	{ NCSP_ANMT_INTERVAL, 6 },
	{0, 0}
};

static NCS_PROP_ENTRY animate_props_ex [] = {
	{ NCSP_ANMT_DIR, (DWORD)"./res/" },
	{ NCSP_ANMT_INTERVAL, 100 },
	{0, 0}
};

NCS_RDR_ELEMENT btn_rdr_elements[] =
{
	{ NCS_MODE_USEFLAT, 1},
	{ -1, 0 }
};

static NCS_RDR_INFO btn_rdr_info[] =
{
	{"flat", "flat", NULL}
};

NCS_RDR_ELEMENT animate_rdr_elements[] = {
	{ NCS_MODE_USEFLAT, 1 },
	{ -1, 0 }
};

static NCS_RDR_INFO animate_rdr_info[] =
{
	{ "flat", "flat", NULL }
};

static void btn_notify(mWidget* button, int id, int nc, DWORD add_data)
{
	mAnimate* anim = (mAnimate*)ncsGetChildObj(GetParent(button->hwnd), IDC_ANI0);

	switch (id) {
		case IDC_BTN1:
			ncsAnimateStart(anim);
			break;

		case IDC_BTN2:
			ncsAnimatePauseResume(anim);
			break;

		case IDC_BTN3:
			ncsAnimateStop(anim);
			break;

		default:
			break;
	}
}

static NCS_EVENT_HANDLER btn_handlers [] = {
	NCS_MAP_NOTIFY(NCSN_BUTTON_PUSHED, btn_notify),
	{ 0, NULL }
};

//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_ANIMATE,
		IDC_ANI0,
		50, 50, 300, 300,
		WS_BORDER | WS_VISIBLE |NCSS_ANMT_AUTOFIT | NCSS_ANMT_AUTOLOOP /*| NCSS_ANMT_SCALE*/ | NCSS_ANMT_AUTOPLAY,
		WS_EX_NONE,
		"test",
		animate_props, //props,
		animate_rdr_info,
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_ANIMATE,
		IDC_ANIM,
		0, 230, 300, 300,
		WS_BORDER | WS_VISIBLE | NCSS_ANMT_AUTOLOOP | NCSS_ANMT_AUTOFIT | NCSS_ANMT_AUTOPLAY,// | NCSS_ANMT_SCALE,
		WS_EX_NONE,
		"test2",
		animate_props_ex, //props,
		animate_rdr_info,
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_BUTTON,
		IDC_BTN1,
		450, 100, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
		"Start",
		NULL,
		btn_rdr_info,
		btn_handlers,
		NULL,
		0,
		0
	},
	{
		NCSCTRL_BUTTON,
		IDC_BTN2,
		450, 200, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
		"Pause",
		NULL,
		btn_rdr_info,
		btn_handlers,
		NULL,
		0,
		0
	},
	{
		NCSCTRL_BUTTON,
		IDC_BTN3,
		450, 300, 70, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
		"Stop",
		NULL,
		btn_rdr_info,
		btn_handlers,
		NULL,
		0,
		0
	},
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
	0, 0, 600, 600,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
	"animate Test ....",
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

	mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect(
			&mymain_templ, HWND_DESKTOP);

	_c(mydlg)->doModal(mydlg, TRUE);

	ncsUninitialize();

	return 0;
}

#else // _MGNCSCTRL_ANIMATE _MGNCSCTRL_DIALOGBOX

int main (void)
{
	printf("\n==========================================================\n");
	printf("======== You haven't enable the animate, dialogbox  contorl =====\n");
	printf("==========================================================\n");
	printf("============== ./configure --enable-animate --enable-dialogbox ==========\n");
	printf("==========================================================\n\n");
	return 0;
}
#endif	// _MGNCSCTRL_ANIMATE _MGNCSCTRL_DIALOGBOX
