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
/**
 ** classic_combobox.c
 **
 ** wangjian<wangjian@minigui.org>
 **
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

extern mWidgetRenderer classic_widget_renderer;

static void classic_drawGroupbox (mGroupBox *self, HDC hdc, const RECT *rect)
{
	DWORD color;
   
    color = ncsGetElement(self, NCS_BGC_3DBODY);

	self->renderer->draw3dbox(self, hdc, rect, color, LFRDR_BTN_STATUS_NORMAL);	
}

static void classic_groupbox_class_init (mGroupBoxRenderer* rdr)
{
	classic_widget_renderer.class_init((mWidgetRenderer*)rdr);
    rdr->drawGroupbox = classic_drawGroupbox;
}

mGroupBoxRenderer classic_groupbox_renderer =
{
    "",
	classic_groupbox_class_init,
	(mStaticRenderer*)(void *)&classic_widget_renderer
};

