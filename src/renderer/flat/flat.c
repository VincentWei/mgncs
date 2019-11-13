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

#include "mgncs.h"

#ifdef _MGNCS_RDR_FLAT

extern mWidgetRenderer flat_widget_renderer;

#ifdef _MGNCSCTRL_COMBOBOX
extern mComboboxRenderer flat_combobox_renderer;
#endif

extern mGroupBoxRenderer flat_groupbox_renderer;

#ifdef _MGNCSCTRL_PROPSHEET
extern mPropSheetRenderer flat_propsheet_renderer;
#endif

#ifdef _MGNCSCTRL_LISTVIEW
extern mListViewRenderer flat_listview_renderer;
#endif //_MGNCSCTRL_LISTVIEW

#define RDR_ENTRY(CLASSNAME, classname) \
	{ NCSCTRL_##CLASSNAME, (mWidgetRenderer*)(void*)(&(flat_##classname##_renderer))}

extern void flat_init_boxpiece_renderer(void);

BOOL ncsInitFlatRenderers(void)
{
	int i;
	NCS_RDR_ENTRY entries [] = {
		RDR_ENTRY(WIDGET,widget),
		RDR_ENTRY(STATIC,widget),
		RDR_ENTRY(IMAGE,widget),
		RDR_ENTRY(GROUPBOX,groupbox),
#ifdef _MGNCSCTRL_COMBOBOX
		RDR_ENTRY(COMBOBOX, combobox),
#endif
#ifdef _MGNCSCTRL_PROPSHEET
		RDR_ENTRY(PROPSHEET, propsheet),
#endif
#ifdef _MGNCSCTRL_LISTVIEW
		RDR_ENTRY(LISTVIEW, listview),
#endif //_MGNCSCTRL_LISTVIEW
		//TODO other render
	};

	for(i=0; i< sizeof(entries)/sizeof(NCS_RDR_ENTRY); i++)
	{
		entries[i].renderer->class_init(entries[i].renderer);
		if(entries[i].renderer->init_self)
			entries[i].renderer->init_self(entries[i].renderer);
	}

	flat_init_boxpiece_renderer();

	return ncsRegisterCtrlRDRs("flat",
		entries,
		sizeof(entries)/sizeof(NCS_RDR_ENTRY)
		);
}

#endif

