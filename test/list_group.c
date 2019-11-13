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

#include <mgncs/mgncs.h>
#include "../include/mrdr.h"

#if defined _MGNCSCTRL_LIST && defined _MGNCSCTRL_DIALOGBOX

#define	IDL_DIR			100
#define	IDL_FILE		110
#define	INFO_NUM		3

static BITMAP demoBmp;

static NCS_MENU_DATA child[] = {
	{0, NULL, 0, "home:  100", NULL, -1, 0, NULL},
	{0, NULL, 0, "office:101", NULL, -1, 0, NULL},
	{0, NULL, 0, "phone: 102", NULL, -1, 0, NULL},
};

NCS_GROUP_INFO groupInfo[] = {
	{ 65, "A..."},
	{ 90, "Z..."},
	{ 74, "J..."},
	{ 72, "H..."},
	{ 80, "P..."},
	{ 84, "T..."},
};


int inWhichGroup(mNode *node)
{
	if (node) {
		return node->addData;
	}

	return NCSID_UNNAMED_GROUP;
}

static NCS_MENU_DATA data[] = {
	{3, child, NCSS_NODE_LTEXTBS, "Zeus", &demoBmp, -1, 90, NULL},
	{3, child, NCSS_NODE_LTEXTBS, "Pan", &demoBmp, -1, 80, NULL},
	{3, child, NCSS_NODE_LTEXTBS, "Apollo", &demoBmp, -1, 65, NULL},
	{3, child, NCSS_NODE_LTEXTBS, "Heracles", &demoBmp, -1, 72, NULL},
	{3, child, NCSS_NODE_LTEXTBS, "Achilles", &demoBmp, -1, 65, NULL},
	{3, child, NCSS_NODE_LTEXTBS, "Jason", &demoBmp, -1, 74, NULL},
	{3, child, NCSS_NODE_LTEXTBS, "Theseus", &demoBmp, -1, 84, NULL},
};

static mLGroupLayout *groupLayout;

static void initListData (mDialogBox *dialog, PBITMAP bmp)
{
	mList *dirObj;

	dirObj = (mList*)ncsGetChildObj(dialog->hwnd, IDL_DIR);

	_c(dirObj)->freeze(dirObj, TRUE);

	ncsSetMenuData((mObject*)dirObj, data, sizeof(data)/sizeof(NCS_MENU_DATA));

	_c(dirObj)->setProperty(dirObj, NCSP_LIST_LAYOUT, NCS_LIST_LGROUP);
	groupLayout = (mLGroupLayout*)_c(dirObj)->getLayout(dirObj);
	_c(groupLayout)->addRef(groupLayout);
	_c(groupLayout)->setGroupInfo(groupLayout, inWhichGroup, groupInfo, TABLESIZE(groupInfo));

	_c(dirObj)->setCurSel(dirObj, 0);
	_c(dirObj)->freeze(dirObj, FALSE);
}

static NCS_RDR_INFO lb_rdr_info[] = {
	{ "classic", "classic", NULL },
};

static NCS_PROP_ENTRY static_props[] = {
	{ NCSP_STATIC_ALIGN, NCS_ALIGN_LEFT },
	{ 0, 0 }
};

static void list_notify(mWidget *self, int id, int nc, DWORD add_data)
{
	mStatic *staticObj = (mStatic*)ncsGetChildObj(GetParent(self->hwnd), IDC_STATIC);
	mList *lstObj = (mList *)self;

	if (nc == NCSN_LIST_ENTERSUBLIST) {
		mNode *selNode = (mNode*)add_data;
		_c(lstObj)->setProperty(lstObj, NCSP_LIST_LAYOUT, NCS_LIST_LLIST);

		if (selNode && staticObj) {
			fprintf (stderr, "enter sub menu=> selected node text:[%s]\n",
					_c(selNode)->getText(selNode));
			SetWindowText(staticObj->hwnd, _c(selNode)->getText(selNode));
		}
	} else if (nc == NCSN_LIST_BACKUPLIST) {
		_c(lstObj)->setLayout(lstObj, (mListLayout*)groupLayout, 0, 0);
		SetWindowText(staticObj->hwnd, "address");
	}
}

static NCS_EVENT_HANDLER list_handlers[] = {
	NCS_MAP_NOTIFY(NCSN_LIST_ENTERSUBLIST, list_notify),
	NCS_MAP_NOTIFY(NCSN_LIST_BACKUPLIST, list_notify),
	{ 0, NULL }
};

static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_STATIC,
		IDC_STATIC,
		0, 0, 160, 15,
		WS_VISIBLE,
		WS_EX_NONE,
		"address",
		static_props,
		lb_rdr_info,
		NULL,
		NULL,
		0,
		0
	},
	{
		NCSCTRL_LIST,
		IDL_DIR,
		0, 20, 166, 188,
		WS_BORDER | WS_TABSTOP | WS_VISIBLE
			| NCSS_NOTIFY | NCSS_LIST_LOOP | NCSS_ASTLST_AUTOSORT,
		WS_EX_NONE,
		"",
		NULL,
		lb_rdr_info,
		list_handlers,
		NULL,
		0,
		0
	},
};

static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_DIALOGBOX,
	7,
	100, 100, 176, 250,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
	"List Demo",
	NULL,
	NULL,
	NULL,
	_ctrl_templ,
	sizeof(_ctrl_templ) / sizeof(NCS_WND_TEMPLATE),
	0,
	0,
	0,
};

int MiniGUIMain(int argc, const char* argv[])
{
	if (argc > 1) {
		lb_rdr_info[0].glb_rdr = argv[1];
		lb_rdr_info[0].ctl_rdr = argv[1];
	}

	ncsInitialize();

	mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect(
			&mymain_templ, HWND_DESKTOP);

	LoadBitmap(HDC_SCREEN, &demoBmp, "./res/listfolder.png");
	initListData (mydlg, &demoBmp);

	_c(mydlg)->doModal(mydlg, TRUE);

	UnloadBitmap(&demoBmp);
	ncsUninitialize();

	return 0;
}
#else //_MGNCSCTRL_LIST _MGNCSCTRL_DIALOGBOX

int main (void)
{
	printf("\n==========================================================\n");
	printf("======== You haven't enable the list, dialogbox contorl =====\n");
	printf("==========================================================\n");
	printf("============== ./configure --enable-list --enable-dialogbox==========\n");
	printf("==========================================================\n\n");
	return 0;
}
#endif	//_MGNCSCTRL_LIST _MGNCSCTRL_DIALOGBOX

