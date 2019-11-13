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

#ifdef _MGNCS_RDR_SKIN

static void skin_drawFocusFrame(mWidget *self, HDC hdc, const RECT* rc)
{
	ncsCommRDRDrawFocusFrame(self->hwnd, hdc, rc);
}

static BOOL skin_drawBkground(mWidget *self, HDC hdc, const RECT* inv_rc)
{
	return ncsCommRDRDrawBkgnd(self->hwnd, hdc, inv_rc);
}

static void skin_draw3dbox(mWidget *self, HDC hdc, const RECT *rc, DWORD color, DWORD flag)
{
	ncsCommRDRDraw3dbox(hdc, rc, color, flag);
}

static void skin_drawCheckbox(mWidget *self, HDC hdc, const RECT *rc, DWORD flag)
{
	DWORD fgcolor = ncsGetElement(self, NCS_FGC_WINDOW);
	DWORD bgcolor = ncsGetElement(self, NCS_BGC_WINDOW);
	DWORD discolor = ncsGetElement(self, NCS_FGC_DISABLED_ITEM);

	ncsCommRDRDrawCheckbox(hdc, rc, fgcolor, bgcolor, discolor, flag);
}

static void skin_drawRadio(mWidget *self, HDC hdc, const RECT *rc, DWORD flag)
{
	DWORD fgcolor = ncsGetElement(self, NCS_FGC_WINDOW);
	DWORD bgcolor = ncsGetElement(self, NCS_BGC_WINDOW);
	DWORD discolor = ncsGetElement(self, NCS_FGC_DISABLED_ITEM);

	ncsCommRDRDrawRadio(hdc, rc, fgcolor, bgcolor, discolor, flag);
}

static void skin_drawArrow(mWidget *self, HDC hdc, 
        const RECT *rc, int arrow, DWORD basiccolor, DWORD flag)
{
    DRAWINFO di;
    RECT rc_arrow;
    int _idx_d, _idx_s;
    const BITMAP *bmp_arrow;

    bmp_arrow = GetBitmapFromRes 
    (ncsGetElement(self, NCS_IMAGE_ARROWS));

    if (bmp_arrow == NULL){
        fprintf(stderr, "NCS->skin RDR->widget: get res error!\n");
        return;
    }
        
    rc_arrow.left = rc->left;
    rc_arrow.top  = rc->top;

    if (RECTWP(rc) > bmp_arrow->bmWidth)
        rc_arrow.left += (RECTWP(rc) - bmp_arrow->bmWidth)>>1;

    if (RECTHP(rc) > (bmp_arrow->bmHeight>>4))
        rc_arrow.top += (RECTHP(rc) - (bmp_arrow->bmHeight>>4))>>1;

    rc_arrow.right  = rc_arrow.left + bmp_arrow->bmWidth;
    rc_arrow.bottom = rc_arrow.top + (bmp_arrow->bmHeight>>4);

    switch(arrow)
    {
        case NCSR_ARROW_UP :   _idx_d = 0; break;
        case NCSR_ARROW_DOWN:  _idx_d = 1; break;
        case NCSR_ARROW_LEFT:  _idx_d = 2; break;
        case NCSR_ARROW_RIGHT: _idx_d = 3; break;
        default :              _idx_d = 0; break;
    }

    switch (NCSR_STATUS(flag)) 
    {
        case NCSRS_NORMAL:      _idx_s = 0; break;
        case NCSRS_HIGHLIGHT:   _idx_s = 1; break;
        case NCSRS_SIGNIFICANT: _idx_s = 2; break;
        case NCSRS_DISABLE:     _idx_s = 3; break;
        default:                _idx_s = 0; break;
    }

    di.bmp      = bmp_arrow;
    di.nr_line  = 16;
    di.nr_col   = 1;
    di.idx_col  = 0;
    di.idx_line = _idx_d*4 + _idx_s;
    di.margin1  = 0;
    di.margin2  = 0;
    di.direct   = FALSE;
    di.flip     = FALSE;
    di.style    = DI_FILL_STRETCH;

    ncsSkinDraw (hdc, &rc_arrow, &di);
    
    return;
}

static void skin_drawItem(mWidget *self, HDC hdc, const RECT *rc, DWORD flag)
{
	int id;
	DWORD color;
	
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

static void skin_class_init(mWidgetRenderer * rdr)
{
    rdr->rdr_name       = "skin";
	rdr->drawFocusFrame = skin_drawFocusFrame;
	rdr->drawBkground   = skin_drawBkground;
	rdr->draw3dbox      = skin_draw3dbox;
	rdr->drawCheckbox   = skin_drawCheckbox;
	rdr->drawRadio      = skin_drawRadio;
	rdr->drawArrow      = skin_drawArrow;
	rdr->drawItem       = skin_drawItem;
}

mWidgetRenderer skin_widget_renderer = {
    "",
	skin_class_init,
	NULL,
};

#endif

