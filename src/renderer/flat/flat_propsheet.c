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
#ifdef _MGNCS_RDR_FLAT

extern mWidgetRenderer flat_widget_renderer;

static void flat_resetHeadArea (mPropSheet *self,
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

static void flat_getRect(mPropSheet *self,
        RECT* rcClient, RECT* rcResult, int which)
{
    int     tabBorder = 1;
    int     btnSize = 16;
    DWORD   style = GetWindowStyle (self->hwnd);

    switch (which) {
        case NCSF_PRPSHT_BORDER:
        {
            RECT rcWin;
            GetWindowRect (self->hwnd, &rcWin);

            rcResult->left = 0;
            rcResult->right = self->headRect.right;

            if ((style & NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM) {
                rcResult->top = 0;
                rcResult->bottom = RECTH(rcWin)
                    - RECTH(self->headRect) + tabBorder;
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

static void flat_drawBorder(mPropSheet *self,
        HDC hdc, RECT* rcBorder)
{
    DWORD color;
    gal_pixel old;

    color = ncsGetElement((mWidget*)self, NCS_FGC_3DBODY);
    old = SetPenColor (hdc, DWORD2Pixel(hdc, color));

    Rectangle (hdc, rcBorder->left, rcBorder->top,
            rcBorder->right - 1, rcBorder->bottom - 2);
    SetPenColor(hdc, old);
    return;
}

static void flat_drawScrollBtn(mPropSheet *self, HDC hdc,
        RECT* rcBtn, int which)
{
    DWORD color;

    color = ncsGetElement((mWidget*)self, NCS_FGC_WINDOW);

	self->renderer->drawArrow(self, hdc, rcBtn, which, color, TRUE);
}

static void flat_drawTab(mPropSheet *self, HDC hdc,
        RECT* rcTab, const char* title, HICON hIcon, BOOL active)
{
    DWORD   style = GetWindowStyle (self->hwnd);
    DWORD   fgcolor, bgcolor;

    int x, ty, by;
    gal_pixel old = 0;
    RECT rc;

    fgcolor = ncsGetElement((mWidget*)self, NCS_FGC_3DBODY);

    x = rcTab->left;
    ty = rcTab->top;
    by = rcTab->bottom;

    GetClientRect(self->hwnd, &rc);
    if (rcTab->right >= rc.right)
        rcTab->right = rc.right - 1;

    if ((style&NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM) {
        by -= 2;
    }
    else {
        ty += 2;
    }

    if (active) {
        if ((style&NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM) {
            FillBox (hdc, rcTab->left, ty, RECTWP(rcTab), 1);
        }
        else {
            FillBox (hdc, rcTab->left, by - 1, RECTWP(rcTab), 1);
        }
    }
    else {
        bgcolor = ncsGetElement((mWidget*)self, NCS_BGC_PRPSHT_NMLTAB);
        old = SetBrushColor(hdc, DWORD2Pixel(hdc, bgcolor));
    }

    if ((style&NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM) {
        FillBox (hdc, rcTab->left + 2, ty + 1, RECTWP(rcTab) - 4, RECTHP(rcTab)-3);
        FillBox (hdc, rcTab->left + 1,   ty + 1, 1, RECTHP(rcTab)-4);
        FillBox (hdc, rcTab->right - 2,  ty + 1, 1, RECTHP(rcTab)-4);

        FillBox (hdc, rcTab->left,       ty + 1, 1, RECTHP(rcTab)-5);
        FillBox (hdc, rcTab->right - 1,  ty + 1, 1, RECTHP(rcTab)-5);

        //draw edge
        MoveTo (hdc, rcTab->left, ty);
        LineTo (hdc, rcTab->left, by - 3 );

        MoveTo (hdc, rcTab->left + 1, by - 2);
        LineTo (hdc, rcTab->left + 1, by - 2);

        MoveTo (hdc, rcTab->left + 2, by - 1);
        LineTo (hdc, rcTab->right - 2, by - 1);

        MoveTo (hdc, rcTab->right - 1, by - 2);
        LineTo (hdc, rcTab->right - 1, by - 2);

        MoveTo (hdc, rcTab->right, ty);
        LineTo (hdc, rcTab->right, by - 3);
    } else {
        FillBox (hdc, rcTab->left + 2, ty, RECTWP(rcTab) - 4, RECTHP(rcTab)-3);
        FillBox (hdc, rcTab->left + 1,   ty + 1, 1, RECTHP(rcTab)-4);
        FillBox (hdc, rcTab->right - 2,  ty + 1, 1, RECTHP(rcTab)-4);

        FillBox (hdc, rcTab->left,       ty + 2, 1, RECTHP(rcTab)-5);
        FillBox (hdc, rcTab->right - 1,  ty + 2, 1, RECTHP(rcTab)-5);

        MoveTo (hdc, rcTab->left, by - 1);
        LineTo (hdc, rcTab->left, ty + 2);

        MoveTo (hdc, rcTab->left + 1, ty + 1);
        LineTo (hdc, rcTab->left + 1, ty + 1);

        MoveTo (hdc, rcTab->left + 2, ty);
        LineTo (hdc, rcTab->right - 2, ty);

        MoveTo (hdc, rcTab->right - 1, ty + 1);
        LineTo (hdc, rcTab->right - 1, ty + 1);

        MoveTo (hdc, rcTab->right, by - 1);
        LineTo (hdc, rcTab->right, ty + 2);
    }
    if (!active) {
        SetBrushColor(hdc, old);
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
        SetBkColor (hdc, DWORD2Pixel (hdc, fgcolor));
        SetBkMode (hdc, BM_TRANSPARENT);
        SetRect(&rc, x, ty, rcTab->right, by);
        DrawText(hdc, title, -1, &rc, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
    }
}

static void flat_propsheet_class_init (mPropSheetRenderer* rdr)
{
	flat_widget_renderer.class_init((mWidgetRenderer*)rdr);
	rdr->resetHeadArea = flat_resetHeadArea;
	rdr->getRect = flat_getRect;
	rdr->drawScrollBtn = flat_drawScrollBtn;
	rdr->drawBorder = flat_drawBorder;
	rdr->drawTab = flat_drawTab;
}

mPropSheetRenderer flat_propsheet_renderer = {
    "",
	flat_propsheet_class_init,
	&flat_widget_renderer
};

#endif
#endif // _MGNCSCTRL_PROPSHEET
