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
#include <mgplus/mgplus.h>

#include "mgncs.h"
#include "fashion_common.h"

#ifdef _MGNCS_RDR_FASHION

#ifdef _MGNCSCTRL_COMBOBOX
extern mComboboxRenderer fashion_combobox_renderer;
#endif

extern mWidgetRenderer fashion_widget_renderer;
extern mGroupBoxRenderer fashion_groupbox_renderer;

#ifdef _MGNCSCTRL_PROPSHEET
extern mPropSheetRenderer fashion_propsheet_renderer;
#endif

#ifdef _MGNCSCTRL_LISTVIEW
extern mListView fashion_listview_renderer;
#endif

#define RDR_ENTRY(CLASSNAME, classname) \
	{ NCSCTRL_##CLASSNAME, (mWidgetRenderer*)(void*)(&(fashion_##classname##_renderer))}


extern void fashion_init_boxpiece_renderer(void);

BOOL ncsInitFashionRenderers(void)
{
	int i;
	NCS_RDR_ENTRY entries[] = {
		RDR_ENTRY(WIDGET, widget),
		RDR_ENTRY(GROUPBOX, groupbox),
#ifdef _MGNCSCTRL_COMBOBOX
		RDR_ENTRY(COMBOBOX, combobox),
#endif
#ifdef _MGNCSCTRL_PROPSHEET
		RDR_ENTRY(PROPSHEET, propsheet),
#endif
#ifdef _MGNCSCTRL_LISTVIEW
		RDR_ENTRY(LISTVIEW, listview)
#endif
	};

	MGPlusRegisterFashionLFRDR();

	for(i = 0; i< sizeof(entries)/sizeof(NCS_RDR_ENTRY); i++)
	{
		entries[i].renderer->class_init(entries[i].renderer);
		if(entries[i].renderer->init_self)
			entries[i].renderer->init_self(entries[i].renderer);
	}

	init_fashion_common();

	fashion_init_boxpiece_renderer();

	return ncsRegisterCtrlRDRs("fashion",
		entries,
		sizeof(entries)/sizeof(NCS_RDR_ENTRY));
}

void ncsUninitFashionRenderers(void)
{
    uninit_fashion_common();
    MGPlusUnregisterFashionLFRDR();
}

#endif


