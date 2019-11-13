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

#include <mgplus/mgplus.h>

#include "mgncs.h"
#include "fashion_common.h"

#ifdef _MGNCS_RDR_FASHION

static void fashion_widget_drawFocusFrame(mWidget *self, HDC hdc, const RECT *rc)
{
	ncsCommRDRDrawFocusFrame(self->hwnd, hdc, rc);
}

static BOOL fashion_widget_drawBkground(mWidget *self, HDC hdc, const RECT* inv_rc)
{
	return ncsCommRDRDrawBkgnd(self->hwnd, hdc, inv_rc);
}

static void fashion_widget_draw3dbox(mWidget* self, HDC hdc, const RECT *rc, DWORD color, DWORD flag)
{
	int round_x = ncsGetElement((mWidget*)self, NCS_METRICS_3DBODY_ROUNDX);
	int round_y = ncsGetElement((mWidget*)self, NCS_METRICS_3DBODY_ROUNDY);
	fashion_fillMiddleGradientRountRect(hdc,
		color,
		ncsCommRDRCalc3dboxColor(color, 200),
		ncsCommRDRCalc3dboxColor(color, 150),
		ncsGetElement((mWidget*)self, NCS_MODE_BGC),
		rc,
		round_x, round_y,
		NCS_ROUND_CORNER_ALL);

	color = ncsCommRDRCalc3dboxColor(color, -80);

	if(round_x > 0 && round_y > 0)
		ncsCommRDRDrawHalfRoundRect(hdc, rc, round_x, round_y, color,NCS_ROUND_CORNER_ALL|NCS_BORDER_ALL);
	else
	{
		gal_pixel old = SetPenColor(hdc, ncsColor2Pixel(hdc, color));
		Rectangle(hdc, rc->left, rc->top, rc->right, rc->bottom);
		SetPenColor(hdc, old);
	}

}

static void fashion_widget_drawCheckbox(mWidget *self, HDC hdc, const RECT *rc, DWORD flags)
{
	DWORD fgcolor = ncsGetElement((mWidget*)self, NCS_FGC_WINDOW);
	DWORD bgcolor = ncsGetElement((mWidget*)self, NCS_BGC_WINDOW);
	DWORD discolor = ncsGetElement((mWidget*)self, NCS_FGC_DISABLED_ITEM);

	ncsCommRDRDrawCheckbox(hdc, rc, fgcolor, bgcolor, discolor, flags);
}

static void fashion_widget_drawRadio(mWidget *self, HDC hdc, const RECT *rc, DWORD flag)
{
	DWORD fgcolor = ncsGetElement((mWidget*)self, NCS_FGC_WINDOW);
	DWORD bgcolor = ncsGetElement((mWidget*)self, NCS_BGC_WINDOW);
	DWORD discolor = ncsGetElement((mWidget*)self, NCS_FGC_DISABLED_ITEM);

	ncsCommRDRDrawRadio(hdc, rc, fgcolor, bgcolor, discolor, flag);
}

static void fashion_widget_drawArrow(mWidget *self, HDC hdc, const RECT* rc, int arrow, DWORD basiccolor, DWORD flag)
{
	ncsCommRDRDrawArrow(hdc, rc, arrow, basiccolor, flag&NCSRF_FILL);
}

static void fashion_widget_drawItem(mWidget *self, HDC hdc, const RECT *rc, DWORD flag)
{
    DWORD color;
	BOOL  bgradiant = FALSE;

	switch(NCSR_STATUS(flag))
	{
	case NCSRS_HIGHLIGHT:
        color = ncsGetElement((mWidget*)self, NCS_BGC_HILIGHT_ITEM);
		bgradiant = TRUE;
		break;
	case NCSRS_SELECTED:
        color = ncsGetElement((mWidget*)self, NCS_BGC_SELECTED_ITEM);
		bgradiant = TRUE;
		break;
	case NCSRS_DISABLE:
        color = ncsGetElement((mWidget*)self, NCS_BGC_DISABLED_ITEM);
		break;
	case NCSRS_SIGNIFICANT:
	default:
        color = ncsGetElement((mWidget*)self, NCS_BGC_WINDOW);
		break;

	}
	if(bgradiant)
	{
		fashion_fillMiddleGradientRountRect(hdc,
			color,
			ncsCommRDRCalc3dboxColor(color, -60),
			ncsCommRDRCalc3dboxColor(color, -100),
			ncsGetElement((mWidget*)self, NCS_MODE_ITEM),
			rc,
			0,0,
			NCS_ROUND_CORNER_ALL);
	}
	else
	{
		ncsCommRDRDrawItem(hdc, rc, color);
	}


			
}

static void fashion_class_init(mWidgetRenderer * rdr)
{
    rdr->rdr_name = "fashion";
	rdr->drawFocusFrame = fashion_widget_drawFocusFrame;
	rdr->drawBkground = fashion_widget_drawBkground;
	rdr->draw3dbox = fashion_widget_draw3dbox;
	rdr->drawCheckbox = fashion_widget_drawCheckbox;
	rdr->drawRadio = fashion_widget_drawRadio;
	rdr->drawArrow = fashion_widget_drawArrow;
	rdr->drawItem = fashion_widget_drawItem;
}



mWidgetRenderer fashion_widget_renderer = {
	"",
	fashion_class_init,
	NULL,
};

#endif

