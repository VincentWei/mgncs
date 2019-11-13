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

#ifdef _MGNCSCTRL_PROPSHEET

#define ICON_OFFSET 2

extern mWidgetRenderer classic_widget_renderer;

static void classic_resetHeadArea (mPropSheet *self,
        RECT* rcClient, DWORD style)
{
    int defHeight = 16 + 4 + 2* ICON_OFFSET;

    self->headRect.left = 0;
    self->headRect.right = RECTWP(rcClient);

    if ((style&NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM) {
        self->headRect.bottom = RECTHP(rcClient);
    }
    else {
        self->headRect.bottom = defHeight;
    }
    self->headRect.top = self->headRect.bottom - defHeight;

    self->maxTabWidth = RECTW(self->headRect);
    //16 for button size
    self->scrollTabWidth = RECTW(self->headRect) - 2*16;
}

static void classic_getRect(mPropSheet *self,
        RECT* rcClient, RECT* rcResult, int which)
{
    int     btnSize = 16;
    DWORD   style = GetWindowStyle (self->hwnd);

    switch (which) {
        case NCSF_PRPSHT_BORDER:
        case NCSF_PRPSHT_PAGE:
        {
            rcResult->left = rcClient->left;
            rcResult->right = rcClient->right;

            if ((style & NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM) {
                rcResult->top = rcClient->top + 1;
                rcResult->bottom =
                    rcResult->top + RECTHP(rcClient) - RECTH(self->headRect);
            }
            else {
                rcResult->top = self->headRect.bottom - 1;
                rcResult->bottom = rcClient->bottom;
            }
            if (NCSF_PRPSHT_BORDER == which)
                break;

            rcResult->left += 1;
            rcResult->right -= 1;
            rcResult->top += 1;
            rcResult->bottom -= 1;
            break;
        }
        case NCSF_PRPSHT_TAB:
        {
            if ((style & NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM) {
                rcResult->top = self->headRect.top + 1;
                rcResult->bottom = self->headRect.bottom;
            }
            else {
                rcResult->top = self->headRect.top;
                rcResult->bottom = self->headRect.bottom - 1;
            }

            if ((style&NCSS_PRPSHT_BTNMASK) == NCSS_PRPSHT_SCROLLABLE
                    &&self->btnShow) {
                rcResult->left = self->headRect.left + btnSize;
                rcResult->right = self->headRect.right - btnSize;
            }
            else {
                rcResult->left = self->headRect.left;
                rcResult->right = self->headRect.right;
            }
            break;
        }
        case NCSF_PRPSHT_LEFT:
            if ((style&NCSS_PRPSHT_BTNMASK) == NCSS_PRPSHT_SCROLLABLE
                    &&self->btnShow) {
                rcResult->left = self->headRect.left;
                rcResult->right = rcResult->left + btnSize;
                rcResult->top = self->headRect.top;
                rcResult->bottom = self->headRect.bottom;
            }
            else {
                SetRectEmpty(rcResult);
            }
            return;

        case NCSF_PRPSHT_RIGHT:
            if ((style&NCSS_PRPSHT_BTNMASK) == NCSS_PRPSHT_SCROLLABLE
                    &&self->btnShow) {
                rcResult->right = self->headRect.right;
                rcResult->left = rcResult->right - btnSize;
                rcResult->top = self->headRect.top;
                rcResult->bottom = self->headRect.bottom;
            }
            else {
                SetRectEmpty(rcResult);
            }
            break;

        default:
            break;
    }
}

static void classic_drawBorder(mPropSheet *self,
        HDC hdc, RECT* rcBorder)
{
    DWORD light_c, bgc;
    gal_pixel pen_color;

    bgc = ncsGetElement ((mWidget*)self, NCS_BGC_3DBODY);
	light_c  = ncsCommRDRCalc3dboxColor(bgc, NCSR_COLOR_DARKER);
    pen_color = SetPenColor(hdc, ncsColor2Pixel(hdc, light_c));

    Rectangle(hdc, rcBorder->left, rcBorder->top,
            rcBorder->right - 1, rcBorder->bottom - 1);

    SetPenColor(hdc, pen_color);
}

static void classic_drawScrollBtn(mPropSheet *self, HDC hdc,
        RECT* rcBtn, int which)
{
    DWORD color;

    color = ncsGetElement ((mWidget*)self, NCS_FGC_WINDOW);

	self->renderer->drawArrow(self, hdc, rcBtn, which, color, NCSRF_FILL);
}

static void classic_drawTab(mPropSheet *self, HDC hdc,
        RECT* rcTab, const char* title, HICON hIcon, BOOL active)
{
    DWORD light_c, bgcolor;
	RECT rc;
	int x, ty, by;
    DWORD   style = GetWindowStyle (self->hwnd);
    int     offset = 0;
    gal_pixel old = 0, oldPenColor;

    x = rcTab->left;
    ty = rcTab->top;
    by = rcTab->bottom;

    GetClientRect(self->hwnd, &rc);
    if (rcTab->right >= rc.right)
        rcTab->right = rc.right - 1;

    bgcolor = ncsGetElement ((mWidget*)self, NCS_BGC_3DBODY);
    if (!active) {
        old = SetBrushColor(hdc, ncsColor2Pixel(hdc, bgcolor));
        offset = 2;
    }
    else {
        if ((style&NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM) {
            FillBox (hdc, rcTab->left, ty - 1, RECTWP(rcTab), 1);
        }
        else {
            FillBox(hdc, rcTab->left, rcTab->bottom - 1, RECTWP(rcTab), 2);
        }
    }

	light_c  = ncsCommRDRCalc3dboxColor(bgcolor, NCSR_COLOR_DARKER);
    oldPenColor = SetPenColor(hdc, ncsColor2Pixel(hdc, light_c));

    if ((style&NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM) {
        FillBox (hdc, rcTab->left, ty, RECTWP(rcTab), RECTHP(rcTab) - 2 - offset);
        FillBox (hdc, rcTab->left + 1, by - offset - 2, RECTWP(rcTab) - 2, 1);
        FillBox (hdc, rcTab->left + 2, by - offset - 1, RECTWP(rcTab) - 4, 1);

        MoveTo (hdc, rcTab->left, ty - 1);
        LineTo (hdc, rcTab->left, by - 3 -offset);

        MoveTo (hdc, rcTab->left + 1, by - 2 - offset);
        LineTo (hdc, rcTab->left + 1, by - 2 - offset);

        MoveTo (hdc, rcTab->left + 2, by - 1 - offset);
        LineTo (hdc, rcTab->right - 2, by - 1 - offset);

        MoveTo (hdc, rcTab->right - 1, by - 2 - offset);
        LineTo (hdc, rcTab->right - 1, by - 2 - offset);

        MoveTo (hdc, rcTab->right, ty - 1);
        LineTo (hdc, rcTab->right, by - 3 - offset);
    }
    else {
        FillBox (hdc, rcTab->left, ty + offset + 2, RECTWP(rcTab), RECTHP(rcTab) - 2 - offset);
        FillBox (hdc, rcTab->left + 1, ty + offset + 1, RECTWP(rcTab) - 2, 1);
        FillBox (hdc, rcTab->left + 2, ty + offset, RECTWP(rcTab) - 4, 1);

        MoveTo (hdc, rcTab->left, by);
        LineTo (hdc, rcTab->left, ty + offset + 2);

        MoveTo (hdc, rcTab->left + 1, ty + 1 + offset);
        LineTo (hdc, rcTab->left + 1, ty + 1 + offset);

        MoveTo (hdc, rcTab->left + 2, ty + offset);
        LineTo (hdc, rcTab->right - 2, ty + offset);

        MoveTo (hdc, rcTab->right - 1, ty + 1 + offset);
        LineTo (hdc, rcTab->right - 1, ty + 1 + offset);

        MoveTo (hdc, rcTab->right, by - 1);
        LineTo (hdc, rcTab->right, ty + 2 + offset);
    }
    if (!active) {
        SetBrushColor(hdc, old);
    }
    SetPenColor(hdc, oldPenColor);

    if ((style&NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM) {
        by -= 2;
    }
    else {
        ty += 2;
    }

    /* draw the ICON */
    if (hIcon) {
        int icon_x, icon_y;
        icon_x = RECTHP(rcTab) - 8;
        icon_y = icon_x;

        x += 2;
        DrawIcon (hdc, x, ty + 4, icon_x, icon_y, hIcon);
        x += icon_x;
        x += 2;
    }

    if (title) {
        /* draw the TEXT */
        SetBkMode (hdc, BM_TRANSPARENT);
        SetRect(&rc, x, ty, rcTab->right, by);
        DrawText(hdc, title, -1, &rc, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
    }
}

static void classic_propsheet_class_init (mPropSheetRenderer* rdr)
{
	classic_widget_renderer.class_init((mWidgetRenderer*)rdr);
	rdr->resetHeadArea = classic_resetHeadArea;
	rdr->getRect = classic_getRect;
	rdr->drawScrollBtn = classic_drawScrollBtn;
	rdr->drawBorder = classic_drawBorder;
	rdr->drawTab = classic_drawTab;
}

mPropSheetRenderer classic_propsheet_renderer = {
    "",
	classic_propsheet_class_init,
	&classic_widget_renderer
};

#endif //_MGNCSCTRL_PROPSHEET

