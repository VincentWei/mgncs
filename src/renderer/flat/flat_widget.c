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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCS_RDR_FLAT
static const char* flat_name = "flat";

extern gal_pixel flat_set_crosss_brush(HDC hdc, gal_pixel color);

static void flat_uninit(mWidgetRenderer *rdr)
{
	if(rdr->prv_data)
		free(rdr->prv_data);
}

static void flat_drawFocusFrame(mWidget *self, HDC hdc, const RECT* rc)
{
	//ncsCommRDRDrawFocusFrame(self->hwnd, hdc, rc);
}

static BOOL flat_drawBkground(mWidget *self, HDC hdc, const RECT* inv_rc)
{
	return ncsCommRDRDrawBkgnd(self->hwnd, hdc, inv_rc);
}


static BOOL flat_init(mWidgetRenderer *rdr)
{
    return TRUE;
}

#define NO_ROUND 0x80000000
extern void flat_draw_3dbox(HDC hdc, const RECT *rc, int flag ,int state, int check_state, mWidget *owner);
static void flat_draw3dbox(mWidget *self, HDC hdc, const RECT *rc, DWORD color, DWORD flag)
{
//	ncsCommRDRDraw3dbox(hdc, rc, color, flag);
	int state = 0;
	int check_state = 0;

	switch((flag & 0xFFFF))
	{
	case NCSRS_HIGHLIGHT:    state = NCS_ABP_HILIGHT; break;
	case NCSRS_SIGNIFICANT:  state = NCS_ABP_PUSHED; break;
	case NCSRS_DISABLE:      state = NCS_ABP_DISABLED; break;
	case NCSRS_SELECTED:     check_state = NCS_ABP_CHECKED; break;
	case NCSRS_HALFSELECTED: check_state = NCS_ABP_HALFCHECKED; break;
	}
	
	flat_draw_3dbox(hdc, rc, flag&(NCSRF_FILL|NO_ROUND), state, check_state, self);
}

static void flat_drawCheckbox(mWidget *self, HDC hdc, const RECT *rc, DWORD flag)
{
	DWORD fgcolor = ncsGetElement(self, NCS_FGC_WINDOW);
	DWORD bgcolor = ncsGetElement(self, NCS_BGC_WINDOW);
	DWORD discolor = ncsGetElement(self, NCS_FGC_DISABLED_ITEM);

	ncsCommRDRDrawCheckbox(hdc, rc, fgcolor, bgcolor, discolor, flag);
}

static void flat_drawRadio(mWidget *self, HDC hdc, const RECT *rc, DWORD flag)
{
	DWORD fgcolor = ncsGetElement(self, NCS_FGC_WINDOW);
	DWORD bgcolor = ncsGetElement(self, NCS_BGC_WINDOW);
	DWORD discolor = ncsGetElement(self, NCS_FGC_DISABLED_ITEM);

	ncsCommRDRDrawRadio(hdc, rc, fgcolor, bgcolor, discolor, flag);
}

static void flat_drawArrow(mWidget *self, HDC hdc, 
        const RECT *rc, int arrow,DWORD basiccolor, DWORD flag)
{
	ncsCommRDRDrawArrow(hdc, rc, arrow, basiccolor, flag&NCSRF_FILL);
}

static void flat_drawItem(mWidget *self, HDC hdc, const RECT *rc, DWORD flag)
{
	DWORD color;
	int id;
	switch(NCSR_STATUS(flag))
	{
	case NCSRS_HIGHLIGHT:
		id = NCS_BGC_HILIGHT_ITEM;
		break;
	case NCSRS_SELECTED:
		id = NCS_BGC_SELECTED_ITEM;
		break;
	case NCSRS_DISABLE:
		id = NCS_BGC_DISABLED_ITEM;
		break;
	case NCSRS_SIGNIFICANT:
	default:
		id = NCS_BGC_WINDOW;
	}

	color = ncsGetElement(self, id);

	ncsCommRDRDrawItem(hdc, rc, color);
}

static void flat_class_init(mWidgetRenderer * rdr)
{
    rdr->rdr_name = flat_name;
	rdr->drawFocusFrame = flat_drawFocusFrame;
	rdr->drawBkground = flat_drawBkground;
	rdr->draw3dbox = flat_draw3dbox;
	rdr->drawCheckbox = flat_drawCheckbox;
	rdr->drawRadio = flat_drawRadio;
	rdr->drawArrow = flat_drawArrow;
	rdr->drawItem = flat_drawItem;
}

mWidgetRenderer flat_widget_renderer = {
	"",
	flat_class_init,
	NULL,
	flat_init,
	flat_uninit,
};

#endif

