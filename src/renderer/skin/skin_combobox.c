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
 ** skin_combobox.c
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

#ifdef _MGNCSCTRL_COMBOBOX

#ifdef _MGNCS_RDR_SKIN

extern mWidgetRenderer skin_widget_renderer;


static void skin_drawDropDownBtn (mCombobox *self, HDC hdc, const RECT *rect)
{
	DWORD key;
    DRAWINFO _di;
    const BITMAP *bmp;

    key = ncsGetElement(self, NCS_IMAGE_ARROWSHELL);
    if (!(bmp = GetBitmapFromRes(key)))
        return;

    _di.bmp      = bmp;
    _di.nr_line  = 4;
    _di.nr_col   = 1;
    _di.idx_col  = 0;
    _di.idx_line = (IS_SET (self, NCSF_CMBOX_HILIGHT)) ? 1 : 0;
    _di.margin1  = 0;
    _di.margin2  = 0;
    _di.direct   = FALSE;
    _di.flip     = FALSE;
    _di.style    = DI_FILL_STRETCH;

	//draw shell
    ncsSkinDraw (hdc, rect, &_di);

	//draw arrow
	self->renderer->drawArrow(self, hdc,
            rect, NCSR_ARROW_DOWN, 0L, NCSRF_FILL);
}

static void skin_combobox_class_init (mComboboxRenderer* rdr)
{
	skin_widget_renderer.class_init((mWidgetRenderer*)rdr);
	rdr->drawDropDownBtn = skin_drawDropDownBtn;
}

mComboboxRenderer skin_combobox_renderer =
{
    "skin",
	skin_combobox_class_init,
	(void*)&skin_widget_renderer,
};
#endif
#endif //_MGNCSCTRL_COMBOBOX

