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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mgncs/mgncs.h>

#if defined _MGNCSCTRL_LISTVIEW && _MGNCSCTRL_DIALOGBOX

#define IDC_LISTVIEW	100
#define IDC_BTN1		101
#define IDC_SLEDIT		102

#define COL_NR			TABLESIZE(caption)
#define SCORE_NUM		TABLESIZE(scores)
#define SUB_NUM			3

typedef struct _SCORE {
	char* name;
	int scr[SUB_NUM];
} SCORE;

static char* caption[] = {
	"Name", "Chinese", "Math", "English"
};

static char *classes[] = {
	"Grade 1", "Grade 3", "Grade 2"
};

static SCORE scores[] = {
	{ "Tom", { 81, 96, 75 } },
	{ "Jack", { 98, 62, 84 } },
	{ "Merry", { 79, 88, 89 } },
	{ "Bob", { 79, 88, 89 } },
};

NCS_RDR_INFO rdr_info = {
	//"classic","classic",NULL
	//"fashion","fashion",NULL
	//"skin","skin",NULL
	"flat","flat",NULL
};

static void btn_notify(mWidget *button, int id, int nc, DWORD add_data)
{
	mListView *listView =
		(mListView *)ncsGetChildObj(GetParent(button->hwnd), IDC_LISTVIEW);
	mSlEdit *edit = (mSlEdit *)ncsGetChildObj(GetParent(button->hwnd), IDC_SLEDIT);
	HITEM firstNode, hItem;
	int i, j, score;
	float average = 0;
	char buff[20];

	if (!listView)
		return;

	firstNode = _c(listView)->getChildItem(listView, 0, 0);

	for (i = 0; i < SCORE_NUM; i++) {
		hItem = _c(listView)->getChildItem(listView, firstNode, i);
		int row = _c(listView)->indexOf(listView, hItem);

		for (j = 0; j < SUB_NUM; j++) {
			sscanf(_c(listView)->getItemText(listView, row, j+1), "%d", &score);
#if 0
			fprintf (stderr, "get line[%d] and column[%d]'s score:%d \n",
					_c(listView)->indexOf(listView, hItem), j, score);
#endif
			average += score;
		}
	}
	average = average / (SCORE_NUM * SUB_NUM);

	sprintf (buff, "%4.1f", average);

	_c(edit)->setContent(edit, buff, 0, strlen(buff));
}

static NCS_EVENT_HANDLER btn_handlers[] = {
	NCS_MAP_NOTIFY(NCSN_WIDGET_CLICKED, btn_notify),
	{ 0, NULL }
};


static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_LISTVIEW,
		IDC_LISTVIEW,
		10, 10, 320, 220,
		WS_BORDER | WS_VISIBLE | NCSS_LISTV_SORT
			| NCSS_LISTV_LOOP,
		WS_EX_NONE,
		"score table",
		NULL, //props,
		&rdr_info,
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_BUTTON,
		IDC_BTN1,
		110, 255, 100, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
		"everage score",
		NULL, //props,
		NULL, //rdr_info
		btn_handlers, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_SLEDIT,
		IDC_SLEDIT,
		10, 260, 50, 20,
		WS_BORDER | WS_VISIBLE,
		WS_EX_NONE,
		"",
		NULL,
		NULL,
		NULL,
		NULL,
		0,
		0
	},
};

static NCS_EVENT_HANDLER mymain_handlers[] = {
	{ 0, NULL }
};

#define everage_score					"Everage score"
#define getting_the_average_score		"Getting the average score"

static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_DIALOGBOX,
	7,
	0, 0, 350, 340,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
	getting_the_average_score,
	NULL,
	&rdr_info,
	mymain_handlers,
	_ctrl_templ,
	sizeof(_ctrl_templ) / sizeof(NCS_WND_TEMPLATE),
	0,
	0,
	0,
};

static HITEM add_class_item(mListView* self, NCS_LISTV_ITEMINFO* info)
{
	NCS_LISTV_ITEMDATA subdata;
	HITEM hItem;

	subdata.row = info->index;
	subdata.col = 0;
	subdata.text= classes[info->index];
	subdata.textColor = 0;
	subdata.flags = 0;
	subdata.image = 0;

	info->dataSize = 1;
	info->data = &subdata;

	hItem = _c(self)->addItem (self, info);

	return hItem;
}

static HITEM add_score_item(mListView* self, NCS_LISTV_ITEMINFO* info)
{
	char buff[3][20];
	NCS_LISTV_ITEMDATA subdata[4];
	HITEM hItem;
	int i = info->index;
	int j;

	for (j = 0; j < 4; j ++) {
		subdata[j].flags = 0;
		subdata[j].image = 0;
		subdata[j].row = info->index;
		subdata[j].col = j;
		if (j == 0) {
			subdata[j].text = scores[i].name;
			subdata[j].textColor = 0;
		}
		else {
			sprintf (buff[j-1], "%d", scores[i].scr[j-1]);
			subdata[j].text = buff[j-1];
			if (scores[i].scr[j-1] > 90)
				subdata[j].textColor = 0xFF0000FF;
			else
				subdata[j].textColor = 0;
		}
	}

	info->dataSize = 4;
	info->data = subdata;

	hItem = _c(self)->addItem (self, info);

	if (!hItem)
		return 0;

	return hItem;
}

static BOOL initListView(mDialogBox* self)
{
	int	 i, j;
	int	 color;
	HITEM   hItem, subItem;
	HWND	hListView = GetDlgItem (self->hwnd, IDC_LISTVIEW);
	mListView	   *listView;
	NCS_LISTV_ITEMINFO	 info;
	NCS_LISTV_CLMINFO   lvcol;

	listView = (mListView*)ncsObjFromHandle(hListView);
	if (!listView)
		return FALSE;

	//add column
	for (i = 0; i < COL_NR; i++) {
		lvcol.index = i;
		lvcol.text = caption[i];
		lvcol.width = 75;
		lvcol.pfnCmp = NULL;
		lvcol.flags = NCSF_LSTCLM_CENTERALIGN | NCSF_LSTHDR_CENTERALIGN
			| NCSF_LSTCLM_VCENTERALIGN | NCSF_LSTHDR_VCENTERALIGN;
		_c(listView)->addColumn(listView, &lvcol);
	}

	_c(listView)->freeze(listView, TRUE);
	hItem = 0;

	info.height = 25;
	info.flags = 0;
	info.foldIcon = 0;
	info.unfoldIcon = 0;
	info.dataSize = 1;

	for (i = 0; i < 3; i++) {
		info.parent = 0;
		info.index = i;
		hItem = add_class_item (listView, &info);

		for (j = 0; j < SCORE_NUM; j++) {
			info.parent = hItem;
			info.index = j;
			subItem = add_score_item (listView, &info);
		}

	}

	//_c(listView)->setHeadVisible(listView, FALSE);
	_c(listView)->freeze(listView, FALSE);

	color = 0xFFFFFF00;
	_c(listView)->setBackground(listView, 1, 1, &color);

	color = 0xFF00FF00;
	_c(listView)->setBackground(listView, 3, 2, &color);

	return TRUE;
}

int MiniGUIMain(int argc, const char* argv[])
{
	if (argc > 1) {
		rdr_info.glb_rdr = argv[1];
		rdr_info.ctl_rdr = argv[1];
	}

	ncsInitialize();

	mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect(
			&mymain_templ, HWND_DESKTOP);

	initListView(mydlg);
	_c(mydlg)->doModal(mydlg, TRUE);

	ncsUninitialize();

	return 0;
}
#else //_MGNCSCTRL_LISTVIEW _MGNCSCTRL_DIALOGBOX

int main (void)
{
	printf("\n==========================================================\n");
	printf("======== You haven't enable the listview, dialogbox contorl =====\n");
	printf("==========================================================\n");
	printf("============== ./configure --enable-listview --enable-dialogbox ==========\n");
	printf("==========================================================\n\n");
	return 0;
}
#endif	//_MGNCSCTRL_LISTVIEW _MGNCSCTRL_DIALOGBOX

