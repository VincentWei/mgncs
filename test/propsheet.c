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

#if defined _MGNCSCTRL_PROPSHEET && defined _MGNCSCTRL_DIALOGBOX


#define PAGE_VERSION	1
#define PAGE_CPU		2
#define PAGE_MEMINFO	3
#define PAGE_PARTITION  4
#define PAGE_MINIGUI	5
#define IDC_PROPSHEET	100

#define IDC_SYSINFO		100


static size_t read_sysinfo (const char* file, char* buff, size_t buf_len)
{
	 size_t size;
	 FILE* fp = fopen (file, "r");

	 if (fp == NULL) return 0;

	 size = fread (buff, 1, buf_len, fp);

	 fclose (fp);
	 return size;
}

#define BUF_LEN 10240
static void get_systeminfo (mPage* page)
{
	int type;
	HWND hwnd;
	char buff [BUF_LEN + 1];
	size_t size = 0;

	type = (int)GetWindowAdditionalData (page->hwnd);
	hwnd = GetDlgItem(_c(page)->getPanel(page), IDC_SYSINFO);
	buff [BUF_LEN] = 0;

	switch (type) {
	case PAGE_VERSION:
		size = read_sysinfo ("/proc/version", buff, BUF_LEN);
		buff [size] = 0;
		break;

	case PAGE_CPU:
		size = read_sysinfo ("/proc/cpuinfo", buff, BUF_LEN);
			buff [size] = 0;
		break;

	case PAGE_MEMINFO:
		size = read_sysinfo ("/proc/meminfo", buff, BUF_LEN);
			buff [size] = 0;
		break;

	case PAGE_PARTITION:
		size = read_sysinfo ("/proc/partitions", buff, BUF_LEN);
			buff [size] = 0;
		break;

	case PAGE_MINIGUI:
		size = snprintf (buff, BUF_LEN,
				"MiniGUI version %d.%d.%d.\n"
				"Copyright (C) 1998-2018 FMSoft.\n\n"
				"MiniGUI is a mature, rich-featured, embedded "
				"GUI support system.\n\n"
				"For more information, please visit\n\n"
				"http://www.minigui.com\n",
				MINIGUI_MAJOR_VERSION, MINIGUI_MINOR_VERSION, MINIGUI_MICRO_VERSION);
		break;
	}

	if (size) {
		SetWindowText (hwnd, buff);
	}
	GetWindowText(hwnd, buff, BUF_LEN+1);
}

static void mypage_onInitPage(mWidget* self, DWORD add_data)
{
	get_systeminfo ((mPage*)self);
}

static int mypage_onShowPage(mWidget* self, HWND hwnd, int show_cmd)
{
	return 1;
}

static int mypage_onSheetCmd(mWidget* self, DWORD wParam, DWORD lParam)
{
	if (wParam == IDOK)
		get_systeminfo ((mPage*)self);
	return 0;
}

static NCS_EVENT_HANDLER mypage_handlers[] = {
	{MSG_INITPAGE, mypage_onInitPage },
	{MSG_SHOWPAGE, mypage_onShowPage },
	{MSG_SHEETCMD, mypage_onSheetCmd },
	{0 , NULL }
};

static NCS_RDR_INFO btn_rdr_info[] =
{
	//{"classic","classic", NULL}
	{"skin", "skin", NULL}
	//{"fashion","fashion", NULL}
};

//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_BUTTON,
		IDOK,
		10, 220, 70, 25,
		WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
		WS_EX_NONE,
		"Refresh",
		NULL, //props,
		btn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_BUTTON,
		IDCANCEL,
		330, 220, 70, 25,
		WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
		WS_EX_NONE,
		"Close",
		NULL, //props,
		btn_rdr_info, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
};

static DLGTEMPLATE PageSysInfo = {
	WS_BORDER | WS_CAPTION,
	WS_EX_NONE,
	0, 0, 0, 0,
	"",
	0, 0,
	1, NULL,
	0
};

static CTRLDATA CtrlSysInfo [] = {
	{
		CTRL_STATIC,
		WS_VISIBLE | SS_LEFT,
		10, 10, 370, 160,
		IDC_SYSINFO,
		"test",
		0
	}
};

static NCS_RDR_INFO prop_rdr_info[] = {
	//{"flat", "flat", NULL},
	//{"skin", "skin", NULL},
	{"classic", "classic", NULL},
	//{"fashion", "fashion", NULL},
};

static int init_propsheet (mDialogBox* self)
{
	mPropSheet *propsheet =
		(mPropSheet*) ncsCreateWindow (NCSCTRL_PROPSHEET,
					   "", WS_VISIBLE | NCSS_PRPSHT_SCROLLABLE, WS_EX_NONE,
					   IDC_PROPSHEET,
					   10, 10, 390, 200, self->hwnd,
					   NULL, prop_rdr_info, NULL, 0);

	if (!propsheet) {
		fprintf (stderr, "propsheet create error. \n");
		return 1;
	}

	PageSysInfo.controls = CtrlSysInfo;
	PageSysInfo.caption = "version_info";
	PageSysInfo.dwAddData = PAGE_VERSION;
	_c(propsheet)->addPage(propsheet, &PageSysInfo, mypage_handlers);

	PageSysInfo.caption = "CPU_info";
	PageSysInfo.dwAddData = PAGE_CPU;
	_c(propsheet)->addPage(propsheet, &PageSysInfo, mypage_handlers);

	PageSysInfo.caption = "mem_info";
	PageSysInfo.dwAddData = PAGE_MEMINFO;
	_c(propsheet)->addPage(propsheet, &PageSysInfo, mypage_handlers);

	PageSysInfo.caption = "partition_info";
	PageSysInfo.dwAddData = PAGE_PARTITION;
	_c(propsheet)->addPage(propsheet, &PageSysInfo, mypage_handlers);

	PageSysInfo.caption = "MiniGUI_info";
	PageSysInfo.dwAddData = PAGE_MINIGUI;
	_c(propsheet)->addPage(propsheet, &PageSysInfo, mypage_handlers);

	return 0;
}

static BOOL mymain_onCommand(mWidget* self, HWND hWnd, int id, int code)
{
	switch (id) {
		case IDOK: {
			HWND hPropSheet = GetDlgItem(self->hwnd, IDC_PROPSHEET);
			mPropSheet *obj = (mPropSheet*)GetWindowAdditionalData2(hPropSheet);
			_c(obj)->broadCastMsg(obj, IDOK, 0);
			break;
		}

		case IDCANCEL:
			_c((mDialogBox*)self)->endDialog((mDialogBox*)self, IDCANCEL);
			break;
	}

	return TRUE;
}

static NCS_EVENT_HANDLER mymain_handlers[] = {
	{MSG_COMMAND, mymain_onCommand },
	{0, NULL }
};

//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_DIALOGBOX,
	1,
	0, 0, 410, 285,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
	"PropSheet Test",
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
		prop_rdr_info[0].glb_rdr = argv[1];
		prop_rdr_info[0].ctl_rdr = argv[1];

		btn_rdr_info[0].glb_rdr = argv[1];
		btn_rdr_info[0].ctl_rdr = argv[1];
	}

	ncsInitialize();

	mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect(
			&mymain_templ, HWND_DESKTOP);

	init_propsheet(mydlg);
	_c(mydlg)->doModal(mydlg, TRUE);

	ncsUninitialize();

	return 0;
}
#else //_MGNCSCTRL_PROPSHEET _MGNCSCTRL_DIALOGBOX

int main (void)
{
	printf("\n==========================================================\n");
	printf("======== You haven't enable the propsheet, dialogbox contorl =====\n");
	printf("==========================================================\n");
	printf("============== ./configure --enable-propsheet --enable-dialogbox ==========\n");
	printf("==========================================================\n\n");
	return 0;
}
#endif	//_MGNCSCTRL_PROPSHEET _MGNCSCTRL_DIALOGBOX
