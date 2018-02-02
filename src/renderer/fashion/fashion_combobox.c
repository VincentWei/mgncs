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

#include "mgncs.h"

#ifdef _MGNCSCTRL_COMBOBOX

#ifdef _MGNCS_RDR_FASHION

extern mWidgetRenderer fashion_widget_renderer;

static void fashion_drawDropDownBtn (mCombobox *self, HDC hdc, const RECT *rect)
{
	DWORD fgc_3d;
    DWORD status;

    fgc_3d = ncsGetElement(self, NCS_BGC_3DBODY);

    if (IS_SET (self, NCSF_CMBOX_HILIGHT)) {
		status = NCSRS_HIGHLIGHT;
    } else {
		status = NCSRS_NORMAL;
    }

	//draw 3dbox
	self->renderer->draw3dbox(self, hdc, rect, fgc_3d, status|NCSRF_FILL);
	//draw arrow
	self->renderer->drawArrow(self, hdc, rect, NCSR_ARROW_DOWN,
		ncsGetElement(self, NCS_FGC_WINDOW), NCSRF_FILL);
}

static void fashion_combobox_class_init (mComboboxRenderer* rdr)
{
	fashion_widget_renderer.class_init((mWidgetRenderer*)rdr);
	rdr->drawDropDownBtn = fashion_drawDropDownBtn;
}

mComboboxRenderer fashion_combobox_renderer =
{
    "fashion",
	fashion_combobox_class_init,
	(void*)&fashion_widget_renderer
};
#endif //_MGNCS_RDR_FASHION
#endif //_MGNCSCTRL_COMBOBOX
