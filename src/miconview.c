/*
    The implementation of mIconView class.

    This file is part of mGNCS, a component for MiniGUI.

    Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSCTRL_ICONVIEW

#define DEF_ICON_WIDTH  48
#define DEF_ICON_HEIGHT 48

static void mIconView_drawItem (mItemView *self, HITEM hItem, HDC hdc, RECT *rcDraw)
{
    int index;
    const char* label;
    RECT rcTxt;
    int x, y;
    PBITMAP bmp;
    mIconView *iv_self = (mIconView*)self;

    memset (&rcTxt, 0, sizeof(rcTxt));
    SetBkMode (hdc, BM_TRANSPARENT);

    index = _c(iv_self)->indexOf(iv_self, hItem);

    if (index < 0)
        return;

    if (_c(iv_self)->isSelected(iv_self, hItem)) {
        SetTextColor (hdc, ncsColor2Pixel(hdc, ncsGetElement(self, NCS_FGC_SELECTED_ITEM)));
        iv_self->renderer->drawItem(iv_self, hdc, rcDraw, NCSRS_HIGHLIGHT);
    }
    else {
        SetTextColor (hdc, ncsColor2Pixel(hdc, ncsGetElement(self, NCS_FGC_WINDOW)));
    }

    label = _c(iv_self)->getTextByIdx(iv_self, index);
    if (label && strcmp(label, "") != 0) {
        rcTxt = *rcDraw;
        rcTxt.top = rcTxt.bottom - GetWindowFont(iv_self->hwnd)->size - 2;
        DrawText (hdc, label, -1,
                &rcTxt, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    }

    bmp = (PBITMAP)(_c(iv_self)->getImage(iv_self, hItem));

    if (bmp) {
        int bmpw = 0, bmph = 0;

        x = rcDraw->left + (RECTWP(rcDraw) - (int)bmp->bmWidth ) / 2;
        if (x < rcDraw->left) {
            x = rcDraw->left;
            bmpw = RECTWP(rcDraw);
        }
        y = rcDraw->top
            + ( RECTHP(rcDraw) - RECTH(rcTxt) - (int)bmp->bmHeight) / 2;
        if (y < rcDraw->top) {
            y = rcDraw->top;
            bmph = RECTHP(rcDraw) - RECTH(rcTxt);
        }
        FillBoxWithBitmap (hdc, x, y, bmpw, bmph, bmp);
    }
}

static void mIconView_construct (mIconView *self, DWORD addData)
{
    RECT rcWnd;

    g_stmItemViewCls.construct((mItemView*)self, addData);

    GetClientRect (self->hwnd, &rcWnd);

    _c(self)->initMargins(self, 5, 5, 5, 5);
    /*owner*/
    self->defItemWidth = DEF_ICON_WIDTH;
    self->defItemHeight = DEF_ICON_HEIGHT;

    self->nrCol = self->contWidth / self->defItemWidth;

    if (self->nrCol <= 0)
        self->nrCol = 1;

    self->flags = 0;
    self->itemOps.drawItem = mIconView_drawItem;
}

static int
mIconView_onKeyDown(mIconView* self, int scancode, int state)
{
    HITEM hItem = 0, curHilighted;
    int curSel;
    int count = _c(self)->getItemCount(self);

    curHilighted = _c(self)->getHilight(self);
    curSel = _c(self)->indexOf(self, curHilighted);

    switch (scancode) {
        case SCANCODE_CURSORBLOCKDOWN:
        {
            int next = curSel + self->nrCol;
            if (GetWindowStyle(self->hwnd) & NCSS_ICONV_LOOP && next >= count) {
                next -= (count + self->nrCol - count % self->nrCol);
                if (next < 0) next += self->nrCol;
            }
            hItem = (HITEM)_c(self)->getItem(self, next);
        }
            break;
        case SCANCODE_CURSORBLOCKUP:
        {
            int next = curSel - self->nrCol;
            if (GetWindowStyle(self->hwnd) & NCSS_ICONV_LOOP && next < 0) {
                next += (count + self->nrCol - count % self->nrCol);
                if (next >= count) next -= self->nrCol;
            }
            hItem = (HITEM)_c(self)->getItem(self, next);
        }
            break;
        case SCANCODE_CURSORBLOCKLEFT:
        {
            hItem = _c(self)->getPrev(self, curHilighted);
            if (GetWindowStyle(self->hwnd) & NCSS_ICONV_LOOP && !hItem) {
                hItem = _c(self)->getPrev(self, 0);
            }
        }
            break;
        case SCANCODE_CURSORBLOCKRIGHT:
        {
            hItem = _c(self)->getNext(self, curHilighted);
            if (GetWindowStyle(self->hwnd) & NCSS_ICONV_LOOP && !hItem) {
                hItem = _c(self)->getNext(self, 0);
            }
        }
            break;
        case SCANCODE_HOME:
        {
            hItem = _c(self)->getNext(self, 0);
        }
            break;
        case SCANCODE_END:
        {
            hItem = _c(self)->getPrev(self, 0);
        }
            break;

        case SCANCODE_KEYPADENTER:
        case SCANCODE_ENTER:
            ncsNotifyParent ((mWidget*)self, NCSN_ITEMV_ENTER);
            return 0;
    }

    if (hItem) {
        if (hItem != curHilighted) {
            _c(self)->hilight(self, hItem);
        }
        _c(self)->showItem(self, hItem);
    }
    return 1;
}

static int mIconView_inItem (mIconView *self, int mouseX, int mouseY,
                     HITEM *pRet, POINT *pt)
{
    int row, col, index;
    HITEM hItem;

    if (mouseY < 0 || mouseY >= _c(self)->getTotalHeight(self))
        return -1;

    if (mouseX < 0 || mouseX >= self->defItemWidth * self->nrCol)
        return -1;

    row = mouseY / self->defItemHeight;
    col = mouseX / self->defItemWidth;

    index = row * self->nrCol + col;
    hItem = _c(self)->getItem(self, index);

    if (pRet)
        *pRet = hItem;

    if (pt)
        pt->y = col * self->defItemHeight;

    return index;
}

static int mIconView_getRect (mIconView *self,
        HITEM hItem, RECT *rcItem, BOOL bConv)
{
    int index = _c(self)->indexOf(self, hItem);

    if (index < 0)
        return -1;

    rcItem->top = (index / self->nrCol ) * self->defItemHeight;
    rcItem->left = (index % self->nrCol ) * self->defItemWidth;


    if (bConv)
        _c(self)->contentToWindow(self, &rcItem->left, &rcItem->top);

    rcItem->bottom = rcItem->top + self->defItemHeight;
    rcItem->right = rcItem->left + self->defItemWidth;

    return 0;
}

static HITEM mIconView_addItem(mIconView *self,
        NCS_ICONV_ITEMINFO *info, int *pos)
{
    HITEM   hItem;
    int     idx;

    if (!info)
        return 0;

    hItem = _c(self)->createItem(self, 0, 0, info->index,
            self->defItemHeight, info->label, info->addData, &idx, FALSE);

    if (!hItem) {
        return 0;
    }

    if (pos)
        *pos = idx;

    //set image
    if (info && info->bmp)
        _c(self)->setImage(self, hItem, ((DWORD)info->bmp));

    if (self->nrCol == 1 || _c(self)->getItemCount(self)%self->nrCol == 1) {
        _c(self)->adjustItemsHeight(self, self->defItemHeight);
    }
    else
        _c(self)->refreshItem(self, hItem, NULL);

    return hItem;
}

static int mIconView_onSizeChanged(mIconView* self, RECT *rcClient)
{
    int oldH, newH, newCol;

    Class(mScrollWidget).onSizeChanged((mScrollWidget*)self, rcClient);

    oldH = _c(self)->getTotalHeight(self);

    self->contWidth = self->visWidth;
    self->nrCol = self->contWidth / self->defItemWidth;

    if (self->nrCol <= 0)
        self->nrCol = 1;

    newCol = (_c(self)->getItemCount(self) + self->nrCol - 1) /self->nrCol;
    newH = newCol *self->defItemHeight;

    if (newH != oldH) {
        _c(self)->adjustItemsHeight(self, newH - oldH);
    }

    return 0;
}

static void mIconView_setIconSize(mIconView *self, int width, int height)
{
    RECT rc;
    if (width <= 0)
        self->defItemWidth = DEF_ICON_WIDTH;
    else
        self->defItemWidth = width;

    if (height <= 0)
        self->defItemHeight = DEF_ICON_HEIGHT;
    else
        self->defItemHeight = height;

    self->nrCol = self->contWidth/self->defItemWidth;

    if (self->nrCol <= 0)
        self->nrCol = 1;

    GetClientRect(self->hwnd, &rc);
    _c(self)->onSizeChanged(self, &rc);

    InvalidateRect(self->hwnd, NULL, TRUE);
}

static int mIconView_getFirstVisItem(mIconView *self)
{
    int i = 0;
    int top = 0, bottom = 0;
    list_t *me, *first;

    first = _c(self)->getQueue(self);

    for (me = first->next; me != first; me = me->next) {
        top = (i / self->nrCol) * self->defItemHeight;
        bottom = top + self->defItemHeight;
        _c(self)->contentToViewport(self, NULL, &bottom);

        if (bottom < 0)
            i++;
        else
            break;
    }

    return i;
}

static void mIconView_onPaint(mIconView *self, HDC hdc, const PCLIPRGN pinv_clip)
{
    list_t *me, *queue;
    HICON hItem;
    RECT rcAll, rcDraw;
    int i = 0;
    RECT rcVis;

    rcAll.left = 0;
    rcAll.top = 0;
    rcAll.right = self->contWidth;
    rcAll.bottom = self->contHeight;

    _c(self)->contentToWindow(self, &rcAll.left, &rcAll.top);
    _c(self)->contentToWindow(self, &rcAll.right, &rcAll.bottom);
    _c(self)->getVisRect(self, &rcVis);

    ClipRectIntersect (hdc, &rcVis);
    queue = _c(self)->getQueue(self);

    for (me = queue->next; me != queue; me = me->next) {
        hItem = _c(self)->getListEntry(self, me);

        rcDraw.left = rcAll.left + (i % self->nrCol) * self->defItemWidth;
        rcDraw.right = rcDraw.left + self->defItemWidth;

        rcDraw.top = rcAll.top + (i / self->nrCol) * self->defItemHeight;
        rcDraw.bottom = rcDraw.top + self->defItemHeight;

        if (rcDraw.bottom < rcVis.top) {
            i++;
            continue;
        }
        if (rcDraw.top > rcVis.bottom)
            break;

        if ( self->itemOps.drawItem ) {
            self->itemOps.drawItem ((mItemView *)self, hItem, hdc, &rcDraw);
        }
        i++;
    }
}

#include "iconview_content_data.c"

static BOOL mIconView_setProperty(mIconView* self, int id, DWORD value)
{
#ifdef _MGNCS_GUIBUILDER_SUPPORT
    if(id == NCSP_DEFAULT_CONTENT)
    {
        NCS_ICONV_ITEMINFO ivii;
        char szText[100];
        int pos = 0;
        memset(&ivii, 0, sizeof(ivii));
        ivii.bmp = get_iconview_content();
        ivii.label = szText;
        _c(self)->setIconSize(self, 55,65);
        for(ivii.index = 0; ivii.index < 3; ivii.index ++)
        {
            pos = 0;
            sprintf(szText,"Item %d", ivii.index + 1);
            _c(self)->addItem(self, &ivii, &pos);
        }
    }
#endif
    if( id >= NCSP_ICONV_MAX)
        return FALSE;

    switch(id)
    {
        case NCSP_ICONV_DEFICONHEIGHT:
            if (value <= 0)
                return FALSE;
            _c(self)->setIconSize(self, self->defItemWidth, value);
            return TRUE;

        case NCSP_ICONV_DEFICONWIDTH:
            if (value <= 0)
                return FALSE;
            _c(self)->setIconSize(self, value, self->defItemHeight);
            return TRUE;
    }

    return Class(mItemView).setProperty((mItemView*)self, id, value);
}

static DWORD mIconView_getProperty(mIconView* self, int id)
{
    if( id >= NCSP_ICONV_MAX)
        return -1;

    switch(id)
    {
        case NCSP_ICONV_DEFICONWIDTH:
            return self->defItemWidth;
    }

    return Class(mItemView).getProperty((mItemView*)self, id);
}

static int mIconView_getItemHeight(mIconView *self, HITEM hItem)
{
    if (_c(self)->getItemCount(self)%self->nrCol == 1) {
        return Class(mItemView).getItemHeight((mItemView*)self, hItem);
    }

    return 0;
}

BEGIN_CMPT_CLASS(mIconView, mItemView)
    CLASS_METHOD_MAP(mIconView, construct);
    CLASS_METHOD_MAP(mIconView, setProperty);
    CLASS_METHOD_MAP(mIconView, getProperty);
    CLASS_METHOD_MAP(mIconView, onSizeChanged);
    CLASS_METHOD_MAP(mIconView, onPaint);
    CLASS_METHOD_MAP(mIconView, onKeyDown);
    CLASS_METHOD_MAP(mIconView, getRect);
    CLASS_METHOD_MAP(mIconView, inItem);
    CLASS_METHOD_MAP(mIconView, addItem);
    CLASS_METHOD_MAP(mIconView, getFirstVisItem);
    CLASS_METHOD_MAP(mIconView, setIconSize);
    CLASS_METHOD_MAP(mIconView, getItemHeight);
    SET_DLGCODE(DLGC_WANTARROWS);
END_CMPT_CLASS

#endif        //_MGNCSCTRL_ICONVIEW
