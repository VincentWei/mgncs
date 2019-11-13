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

#ifdef _MGNCS_RDR_SKIN
#define ICON_OFFSET 2

extern mWidgetRenderer skin_widget_renderer;

static void skin_resetHeadArea (mPropSheet *self, RECT* rcClient, DWORD style)
{
    int height, minHeight, maxHeight = 40;
    DWORD key;
    PBITMAP pBmp;

    minHeight = 16 + 4 + 2* ICON_OFFSET;
    key = ncsGetElement((mWidget*)self, NCS_IMAGE_PRPSHT_TAB);
    pBmp = GetBitmapFromRes (key);
    if (pBmp) {
        height = (pBmp->bmHeight>>2) + 2*(ICON_OFFSET+1);
    }
    else {
        height = minHeight;
    }
    if (height < minHeight)
        height = minHeight;

    if (height > maxHeight)
        height = maxHeight;

    self->headRect.left = 0;
    self->headRect.right = RECTWP(rcClient);

    if ((style&NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM) {
        self->headRect.bottom = RECTHP(rcClient);
    }
    else {
        self->headRect.bottom = height;
    }

    self->headRect.top = self->headRect.bottom - height;
    self->maxTabWidth = RECTW(self->headRect);
    self->scrollTabWidth = RECTW(self->headRect) - 2*16;
}

static void skin_getRect(mPropSheet *self,
        RECT* rcClient, RECT* rcResult, int which)
{
    int     btnSize = 16;
    int     tabBorder = 2;
    DWORD   style = GetWindowStyle (self->hwnd);

    switch (which)
    {
        case NCSF_PRPSHT_BORDER:
        {
            RECT rcWin;
            GetWindowRect (self->hwnd, &rcWin);

            rcResult->left = 0;
            rcResult->right = self->headRect.right;

            if ((style & NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM) {
                rcResult->top = 0;
                rcResult->bottom = RECTH(rcWin) - RECTH(self->headRect) + tabBorder;
            }
            else {
                rcResult->top = self->headRect.bottom - tabBorder;
                rcResult->bottom = RECTH(rcWin) ;
            }
            return;
        }
        case NCSF_PRPSHT_PAGE:
        {
            rcResult->left = tabBorder;
            rcResult->right = rcClient->right - tabBorder;

            if ((style & NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM) {
                rcResult->top = tabBorder;
            }
            else {
                rcResult->top = self->headRect.bottom;
            }
            rcResult->bottom = rcResult->top + RECTHP(rcClient)
                - RECTH(self->headRect) - 2 * tabBorder;
            return;
        }
        case NCSF_PRPSHT_TAB:
        {
            rcResult->top = self->headRect.top;
            rcResult->bottom = self->headRect.bottom;

            if ((style&NCSS_PRPSHT_BTNMASK) == NCSS_PRPSHT_SCROLLABLE
                    &&self->btnShow) {
                rcResult->left = self->headRect.left + btnSize;
                rcResult->right = self->headRect.right - btnSize;
            }
            else {
                rcResult->left = self->headRect.left;
                rcResult->right = self->headRect.right;
            }

            return;
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
            return;

        default:
            break;
    }
    return;
}

static void skin_drawBorder(mPropSheet *self, HDC hdc, RECT* rcBorder)
{
    DWORD color, light_c, darker_c, darkest_c;

    color = ncsGetElement((mWidget*)self, NCS_BGC_3DBODY);
	light_c   = ncsCommRDRCalc3dboxColor(color, NCSR_COLOR_LIGHTEST);
	darker_c  = ncsCommRDRCalc3dboxColor(color, NCSR_COLOR_DARKER);
	darkest_c = ncsCommRDRCalc3dboxColor(color, NCSR_COLOR_DARKEST);

    rcBorder->right--;
    rcBorder->bottom--;

    SetPenColor(hdc, DWORD2Pixel (hdc, light_c));
    MoveTo (hdc, rcBorder->left, rcBorder->bottom - 1);
    LineTo (hdc, rcBorder->left, rcBorder->top + 1);
    LineTo (hdc, rcBorder->right - 1, rcBorder->top + 1);

    SetPenColor (hdc, DWORD2Pixel (hdc, darkest_c));
    MoveTo (hdc, rcBorder->left, rcBorder->bottom);
    LineTo (hdc, rcBorder->right, rcBorder->bottom);
    LineTo (hdc, rcBorder->right, rcBorder->top + 1);

    SetPenColor (hdc, DWORD2Pixel (hdc, darker_c));
    MoveTo (hdc, rcBorder->left + 1, rcBorder->bottom - 1);
    LineTo (hdc, rcBorder->right - 1, rcBorder->bottom - 1);
    LineTo (hdc, rcBorder->right - 1, rcBorder->top + 2);

    return;
}

static void skin_drawScrollBtn(mPropSheet *self, HDC hdc,
        RECT* rcBtn, int which)
{
	self->renderer->drawArrow(self, hdc, rcBtn, which, 0L, TRUE);
}

static void skin_drawTab(mPropSheet *self, HDC hdc,
        RECT* rcTab, const char* title, HICON hIcon, BOOL active)
{
    int x, ty, by;
    DWORD key;
    DRAWINFO di;
    BOOL bottom;
	RECT rc;

    key = ncsGetElement((mWidget*)self, NCS_IMAGE_PRPSHT_TAB);
    if (!(di.bmp = GetBitmapFromRes (key)))
        return;

    bottom = (GetWindowStyle(self->hwnd) & NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM;

    di.nr_line  = 4;
    di.nr_col   = 1;
    di.idx_col  = 0;
    di.margin1  = 3;
    di.margin2  = 4;
    di.direct   = FALSE;
    di.flip     = bottom ? TRUE : FALSE;
    di.idx_line = 0;
    di.style    = DI_FILL_TILE;

    x = rcTab->left + 2;
    ty = rcTab->top;

    if (!active) {
        if (bottom) {
            ty -= 2;
            by = rcTab->bottom - 2;
        } else {
            ty += 2;
            by = rcTab->bottom;
        }

     } else {
        di.idx_line = 2;
        by = rcTab->bottom;
     }

    ncsSkinDraw (hdc, rcTab, &di);

    /* draw the ICON */
    ty += 2 + 2;
    if (hIcon) {
        int icon_x, icon_y;
        icon_x = RECTHP(rcTab) - 8;
        icon_y = icon_x;

        DrawIcon (hdc, x, ty, icon_x, icon_y, hIcon);
        x += icon_x;
        x += 2;
    }

    /* draw the TEXT */
    SetBkMode (hdc, BM_TRANSPARENT);
    SetRect(&rc, x, ty, rcTab->right, by);
    DrawText(hdc, title, -1, &rc, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}


static void skin_propsheet_class_init (mPropSheetRenderer* rdr)
{
	skin_widget_renderer.class_init((mWidgetRenderer*)rdr);
	rdr->resetHeadArea = skin_resetHeadArea;
	rdr->getRect       = skin_getRect;
	rdr->drawScrollBtn = skin_drawScrollBtn;
	rdr->drawBorder    = skin_drawBorder;
	rdr->drawTab       = skin_drawTab;
}

mPropSheetRenderer skin_propsheet_renderer = {
    "",
	skin_propsheet_class_init,
	&skin_widget_renderer,
};

#endif
#endif // _MGNCSCTRL_PROPSHEET
