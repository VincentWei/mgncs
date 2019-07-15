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
    The implementation of ScrollWidget.

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

static void mScrollWidget_construct (mScrollWidget *self, DWORD addData)
{
	g_stmWidgetCls.construct((mWidget*)self, addData);

    _c(self)->initMargins(self, 0, 0, 0, 0);

    self->contX = 0;
    self->contY = 0;
    self->hStepVal = 5;
    self->vStepVal = 5;
    self->drawMode = NCSID_SWGT_AUTO;
}

static int mScrollWidget_setHScrollInfo (mScrollWidget* self,
        BOOL reDraw)
{
    SCROLLINFO si;

    if (self->drawMode == NCSID_SWGT_NEVER) {
        ShowScrollBar (self->hwnd, SB_HORZ, FALSE);
        return 0;
    }

    if (self->contWidth <= self->visWidth) {
        SetScrollPos (self->hwnd, SB_HORZ, 0);
        
        if (self->drawMode == NCSID_SWGT_AUTO) {
            ShowScrollBar (self->hwnd, SB_HORZ, FALSE);
        }
        else if (self->drawMode == NCSID_SWGT_ALWAYS) {
            EnableScrollBar(self->hwnd, SB_HORZ, TRUE);

            si.fMask = SIF_PAGE;
            si.nMax = 1;
            si.nMin = 0;
            si.nPage = 0;
            SetScrollInfo (self->hwnd, SB_HORZ, &si, reDraw);

            ShowScrollBar (self->hwnd, SB_HORZ, TRUE);
        }

        return 0;
    }

    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = self->contWidth - 1;
    si.nPage = self->visWidth;
    si.nPos = self->contX;

    SetScrollInfo (self->hwnd, SB_HORZ, &si, reDraw);

    if (self->drawMode == NCSID_SWGT_AUTO) {
        ShowScrollBar (self->hwnd, SB_HORZ, TRUE);
    }
    else if (self->drawMode == NCSID_SWGT_ALWAYS) {
        EnableScrollBar(self->hwnd, SB_HORZ, TRUE);
        ShowScrollBar (self->hwnd, SB_HORZ, TRUE);
    }

    return 0;
}

static int mScrollWidget_setVScrollInfo (mScrollWidget* self,
        BOOL reDraw)
{
    SCROLLINFO si;

    if (self->drawMode == NCSID_SWGT_NEVER) {
        ShowScrollBar (self->hwnd, SB_VERT, FALSE);
        return 0;
    }

    if (self->contHeight <= self->visHeight) {
        SetScrollPos (self->hwnd, SB_VERT, 0);

        if (self->drawMode == NCSID_SWGT_AUTO) {
            ShowScrollBar (self->hwnd, SB_VERT, FALSE);
        }
        else if (self->drawMode == NCSID_SWGT_ALWAYS) {
            EnableScrollBar(self->hwnd, SB_VERT, TRUE);

            si.fMask = SIF_PAGE;
            si.nMax = 1;
            si.nMin = 0;
            si.nPage = 0;
            SetScrollInfo (self->hwnd, SB_VERT, &si, reDraw);

            ShowScrollBar (self->hwnd, SB_VERT, TRUE);
        }
        return 0;
    }

    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = self->contHeight - 1;
    si.nPage = self->visHeight;
    si.nPos = self->contY;

    SetScrollInfo (self->hwnd, SB_VERT, &si, reDraw);
    if (self->drawMode == NCSID_SWGT_AUTO) {
        ShowScrollBar (self->hwnd, SB_VERT, TRUE);
    }
    else if (self->drawMode == NCSID_SWGT_ALWAYS) {
        EnableScrollBar(self->hwnd, SB_VERT, TRUE);
        ShowScrollBar (self->hwnd, SB_VERT, TRUE);
    }

    return 0;
}

static void mScrollWidget_setScrollInfo (mScrollWidget* self, BOOL reDraw)
{
    mScrollWidget_setHScrollInfo(self, reDraw);
    mScrollWidget_setVScrollInfo(self, reDraw);
}

static BOOL mScrollWidget_SetContWidth (mScrollWidget* self, int width)
{
    int oldVal, diff, oldX;
    RECT rect;

    oldVal = self->contWidth;
    self->contWidth = width > self->visWidth ? width : self->visWidth;
    self->realContWidth = width;

    if (oldVal == self->contWidth)
        return FALSE;

    oldX = self->contX;
    diff = self->contWidth - self->visWidth;
    if (diff <= 0)
        self->contX = 0;
    else {
        if (self->contX > diff)
            self->contX = diff;
    }

    mScrollWidget_setHScrollInfo (self, TRUE);

    if (oldX != self->contX) {
        if (_c(self)->moveContent) {
            _c(self)->moveContent(self);
        }
    }
    else {
        rect.left = MIN(oldVal, self->contWidth);
        rect.top = self->contY;
        _c(self)->contentToWindow(self, &rect.left, &rect.top); 
        rect.right = rect.left + ABS(oldVal - self->contWidth);
        rect.bottom = rect.top + self->visHeight;

        InvalidateRect(self->hwnd, &rect, TRUE);
    }

    return TRUE;
}

static BOOL mScrollWidget_SetContHeight (mScrollWidget* self, int height)
{
    int oldVal, diff, oldY;
    RECT rect;

    oldVal = self->contHeight;
    self->contHeight = height >= self->visHeight ? height : self->visHeight;
    /*contHeight can be lower than visHeight*/
    self->realContHeight = height;

    if (oldVal == self->contHeight)
        return FALSE;

    diff = self->contHeight - self->visHeight;
    oldY = self->contY;

    if (diff <= 0)
        self->contY = 0;
    else {
        if (self->contY > diff)
            self->contY = diff;
    }

    mScrollWidget_setVScrollInfo (self, TRUE);

    if (oldY != self->contY) {
        if (_c(self)->moveContent) {
            _c(self)->moveContent(self);
        }
    }
    else {
        rect.left = self->contX;
        rect.top = MIN(oldVal, self->contHeight);
        _c(self)->contentToWindow(self, &rect.left, &rect.top); 

        rect.right = rect.left + self->visWidth;
        rect.bottom = rect.top + ABS(oldVal - self->contHeight);

        InvalidateRect(self->hwnd, &rect, TRUE);
    } 
    
    return TRUE;
}

static BOOL mScrollWidget_SetContX (mScrollWidget* self, int value)
{
    BOOL bChanged = FALSE;
    int maxContX;

    if (self->contX == value)
        return bChanged;

    maxContX = self->contWidth - self->visWidth;
    
    if (value > maxContX)
        value = maxContX;

    if (value >= 0 && value <= maxContX && value != self->contX) {
        self->contX = value;
        SetScrollPos (self->hwnd, SB_HORZ, value);
        if (_c(self)->moveContent) {
            _c(self)->moveContent(self);
        }
        bChanged = TRUE;
    }
    return bChanged;
}

static int mScrollWidget_SetContY (mScrollWidget* self, int value)
{
    BOOL bChanged = FALSE;
    int maxContY;

    if (self->contY == value)
        return bChanged;

    maxContY = self->contHeight - self->visHeight;
    
    if (value > maxContY)
        value = maxContY;

    if (value >= 0 && value <= maxContY && value != self->contY) {
        self->contY = value;
        SetScrollPos (self->hwnd, SB_VERT, value);
        if (_c(self)->moveContent) {
            _c(self)->moveContent(self);
        }
        bChanged = TRUE;
    }
    return bChanged;
}

static BOOL 
mScrollWidget_SetMargins (mScrollWidget* self, const RECT* rcMargin)
{
    RECT rcWnd;

    if (!rcMargin)
        return FALSE;

    if (EqualRect(rcMargin, (const RECT*)&self->leftMargin))
        return FALSE;

    if (rcMargin->left >= 0)
        self->leftMargin = rcMargin->left;
    if (rcMargin->top >= 0)
        self->topMargin = rcMargin->top;
    if (rcMargin->right >= 0)
        self->rightMargin = rcMargin->right;
    if (rcMargin->bottom >= 0)
        self->bottomMargin = rcMargin->bottom;

    GetClientRect (self->hwnd, &rcWnd);

    _c(self)->onSizeChanged (self, &rcWnd);
    //margin changed, should refresh whole window.
    if (_c(self)->moveContent) {
        _c(self)->moveContent(self);
    }
	return TRUE;
}

static void 
mScrollWidget_resetViewPort (mScrollWidget* self, unsigned int visWidth, unsigned int visHeight)
{
    RECT rcWnd;
    BOOL changed = FALSE;
    int oldContW, oldContH;
    
    //make value correct.
    GetClientRect (self->hwnd, &rcWnd);
    if (visWidth > RECTW(rcWnd))
        visWidth = RECTW(rcWnd);

    if (visHeight > RECTH(rcWnd))
        visHeight = RECTH(rcWnd);

    /* no change */
    if ((self->visWidth == visWidth) && (self->visHeight == visHeight))
        return;

    self->visWidth = visWidth;
    self->visHeight = visHeight; 

    oldContW = self->contWidth;
    oldContH = self->contHeight;

    if (visWidth >= 0 && self->realContWidth < visWidth)
        self->contWidth = visWidth;
    else
        self->contWidth = self->realContWidth;

    if (visHeight >= 0 && self->realContHeight < visHeight)
        self->contHeight = visHeight;
    else
        self->contHeight = self->realContHeight;

    if(oldContW != self->contWidth
            || oldContH != self->contHeight)
        changed = TRUE;

    //change cont pos
    if (self->contWidth == self->visWidth && self->contX != 0) {
        self->contX = 0;
        changed = TRUE;
    }
    if (self->realContHeight == self->visHeight && self->contY != 0) {
        self->contY = 0;
        changed = TRUE;
    }
    
    if (changed && _c(self)->moveContent) {
        _c(self)->moveContent(self);
    }

    mScrollWidget_setScrollInfo(self, TRUE);
}

/*------------------- class method declaration -----------------------*/
static BOOL mScrollWidget_setContPos (mScrollWidget* self, int x, int y)
{
    BOOL bChange = FALSE;
    int maxContX, maxContY;

    maxContX = self->contWidth - self->visWidth;
    maxContY = self->contHeight - self->visHeight;
    
    if (x > maxContX)
        x = maxContX;

    if (y > maxContY)
        y = maxContY;

    if (x >= 0 && x <= maxContX && self->contX != x) {
        self->contX = x;
        SetScrollPos (self->hwnd, SB_HORZ, x);
        bChange = TRUE;
    }
    if (y >= 0 && y <= maxContY && y != self->contY) {
        self->contY = y;
        SetScrollPos (self->hwnd, SB_VERT, y);
        bChange = TRUE;
    }
    if (bChange && _c(self)->moveContent) {
        _c(self)->moveContent(self);
    }

    return bChange;
}

static void mScrollWidget_svScroll (mScrollWidget* self, 
        int scrollVal, int newPos, BOOL bHScroll)
{
    int scrollBoundMax;
    int scrollBoundMin;
    int scroll = 0;
    BOOL bScroll = FALSE;
    int nOffset;
    
    if (bHScroll) {
        scrollBoundMax = self->contWidth - self->visWidth;
        nOffset = self->contX;
    }
    else {
        scrollBoundMax = self->contHeight - self->visHeight;
        nOffset = self->contY;
    }
    scrollBoundMin = 0;

    if (newPos >= 0) { // SB_THUMBTRACK
        scrollVal = newPos - nOffset;
    }

    scroll = (scrollVal > 0 ? scrollVal : -scrollVal);

    if (scrollVal > 0 && nOffset < scrollBoundMax) {
        if ((nOffset + scroll) > scrollBoundMax)
            nOffset = scrollBoundMax;
        else
            nOffset += scroll;
        bScroll = TRUE;
    }
    else if ( scrollVal < 0 && nOffset > scrollBoundMin) {
        if ((nOffset - scroll) < scrollBoundMin)
            nOffset = scrollBoundMin;
        else
            nOffset -= scroll;
        bScroll = TRUE;
    }

    if (bScroll) {
        mScrollWidget_setContPos (self, 
                (bHScroll ? nOffset : -1), (bHScroll ? -1 : nOffset));
    }
}

static void mScrollWidget_moveContent (mScrollWidget* self)
{
    InvalidateRect(self->hwnd, NULL, TRUE);
}

static void mScrollWidget_onHScroll (mScrollWidget* self, int code, int mouseX)
{
    int hScroll = 0, newPos = -1;

    switch (code) {
        case SB_LINERIGHT:
            hScroll = self->hStepVal;
            break;
        case SB_LINELEFT:
            hScroll = -self->hStepVal;
            break;
        case SB_PAGERIGHT:
            hScroll = self->visWidth - self->hStepVal;
            break;
        case SB_PAGELEFT:
            hScroll = -(self->visWidth - self->hStepVal);
            break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            newPos = mouseX;
            break;
    }
    mScrollWidget_svScroll(self, hScroll, newPos, TRUE);
}

static void mScrollWidget_onVScroll (mScrollWidget* self, int code, int mouseY)
{
    int hScroll = 0, newPos = -1;

    switch (code) {
        case SB_LINEDOWN:
            hScroll = self->vStepVal;
            break;
        case SB_LINEUP:
            hScroll = -self->vStepVal;
            break;
        case SB_PAGEDOWN:
            hScroll = self->visHeight - self->vStepVal;
            break;
        case SB_PAGEUP:
            hScroll = -(self->visHeight - self->vStepVal);
            break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            newPos = mouseY;
            break;
    }
    mScrollWidget_svScroll(self, hScroll, newPos, FALSE);
}

static int
mScrollWidget_onKeyDown (mScrollWidget* self, int scancode, int state)
{
    int code = -1;
    BOOL hScroll = TRUE;

    switch (scancode) 
    {
        case SCANCODE_CURSORBLOCKUP:
            code = SB_LINEUP;
            hScroll = FALSE;
            break;

        case SCANCODE_CURSORBLOCKDOWN:
            code = SB_LINEDOWN;
            hScroll = FALSE;
            break;

        case SCANCODE_CURSORBLOCKLEFT:
            code = SB_LINELEFT;
            hScroll = TRUE;
            break;

        case SCANCODE_CURSORBLOCKRIGHT:
            code = SB_LINERIGHT;
            hScroll = TRUE;
            break;

        case SCANCODE_PAGEDOWN:
            code = SB_PAGEDOWN;
            hScroll = FALSE;
            break;

        case SCANCODE_PAGEUP:
            code = SB_PAGEUP;
            hScroll = FALSE;
            break;

        default:
            break;
    }

    if (code != -1) {
        if (hScroll)
            _c(self)->onHScroll(self, code, 0);
        else
            _c(self)->onVScroll(self, code, 0);
    }
    return 1;
}

static BOOL mScrollWidget_isVisible (mScrollWidget* self, int x, int y)
{
    if (x < self->contX || x > self->contX + self->visWidth 
            || y < self->contY || y > self->contY + self->visHeight)
        return FALSE;

    return TRUE;
}

static BOOL mScrollWidget_makePosVisible (mScrollWidget* self, int x, int y)
{
    int cont_x = -1, cont_y = -1;

    if (x >= 0) {
        if (x >= self->contWidth)
            x = self->contWidth - 1;
        if (x < self->contX)
            cont_x = x;
        else if (x > self->contX + self->visWidth - 1)
            cont_x = x - self->visWidth + 1;
    }

    if (y >= 0) {
        if (y >= self->contHeight)
            y = self->contHeight - 1;
        if (y < self->contY)
            cont_y = y;
        else if (y > self->contY + self->visHeight - 1)
            cont_y = y - self->visHeight + 1;
    }
    return mScrollWidget_setContPos (self, cont_x, cont_y);
}

static int mScrollWidget_onSizeChanged (mScrollWidget* self, RECT* rcClient)
{
    int newVisW, newVisH;

    newVisW = RECTWP(rcClient) - self->leftMargin - self->rightMargin;
    newVisH = RECTHP(rcClient) - self->topMargin - self->bottomMargin;

    _c(self)->resetViewPort (self, newVisW, newVisH);
    return 0;
}

static LRESULT mScrollWidget_wndProc(mScrollWidget* self, UINT message, WPARAM wParam, LPARAM lParam)
{	
    switch(message){
        case MSG_CREATE:
            mScrollWidget_setScrollInfo(self, TRUE);
            break;

        case MSG_HSCROLL:
            _c(self)->onHScroll(self, wParam, lParam);
            break;

        case MSG_VSCROLL:
            _c(self)->onVScroll(self, wParam, lParam);
            break;
    }
	
	return Class(mWidget).wndProc((mWidget*)self, message, wParam, lParam);
}

static BOOL mScrollWidget_setProperty(mScrollWidget* self, int id, DWORD value)
{
	if( id >= NCSP_SWGT_MAX)
		return FALSE;

	switch(id)
	{
	case NCSP_SWGT_HSTEPVALUE:
        self->hStepVal = (unsigned int)value;
        return TRUE;

	case NCSP_SWGT_VSTEPVALUE:
        self->vStepVal = (unsigned int)value;
        return TRUE;

	case NCSP_SWGT_CONTX:
		return mScrollWidget_SetContX(self, (int)value);
        
	case NCSP_SWGT_CONTY:
		return mScrollWidget_SetContY(self, (int)value);
        
	case NCSP_SWGT_CONTWIDTH:
		return mScrollWidget_SetContWidth(self, (int)value);
        
	case NCSP_SWGT_CONTHEIGHT:
		return mScrollWidget_SetContHeight(self, (int)value);
        
	case NCSP_SWGT_VISWIDTH:
        if (value >= 0)
            _c(self)->resetViewPort(self, (int)value, self->visHeight);
        return self->visWidth;
        
	case NCSP_SWGT_VISHEIGHT:
        if (value >= 0)
            _c(self)->resetViewPort(self, self->visWidth, (int)value);
        return self->visHeight;

	case NCSP_SWGT_MARGINRECT:
		return mScrollWidget_SetMargins(self, (const RECT*)value);

	case NCSP_SWGT_DRAWMODE:
        if (self->drawMode == (unsigned int)value)
            return FALSE;

        self->drawMode = (unsigned int)value;
        InvalidateRect(self->hwnd, NULL, TRUE);
        mScrollWidget_setScrollInfo(self, TRUE);

        return TRUE;
	}

	return Class(mWidget).setProperty((mWidget*)self, id, value);
}

static DWORD mScrollWidget_getProperty(mScrollWidget* self, int id)
{
	if( id >= NCSP_SWGT_MAX)
		return -1;

	switch (id)
    {
        case NCSP_SWGT_HSTEPVALUE:
            return self->hStepVal;

        case NCSP_SWGT_VSTEPVALUE:
            return self->vStepVal;

        case NCSP_SWGT_CONTX:
            return self->contX;

        case NCSP_SWGT_CONTY:
            return self->contY;

        case NCSP_SWGT_CONTWIDTH:
            return self->contWidth;

        case NCSP_SWGT_CONTHEIGHT:
            return self->contHeight;

        case NCSP_SWGT_VISWIDTH:
            return self->visWidth;

        case NCSP_SWGT_VISHEIGHT:
            return self->visHeight;

        case NCSP_SWGT_MARGINRECT:
            return (DWORD)&(self->leftMargin);

        case NCSP_SWGT_DRAWMODE:
            return self->drawMode;
    }

	return Class(mWidget).getProperty((mWidget*)self, id);
}

static void mScrollWidget_getVisRect(mScrollWidget* self, RECT *rcVis)
{
    if (rcVis) {
        rcVis->left = self->leftMargin;
        rcVis->top = self->topMargin;
        rcVis->right = self->leftMargin + self->visWidth;
        rcVis->bottom = self->topMargin + self->visHeight;
    }
}


static inline void mScrollWidget_viewportToWindow (mScrollWidget* self, int *x, int *y)
{
    if (x)
        *x += self->leftMargin;
    if (y)
        *y += self->topMargin;
}

static inline void mScrollWidget_windowToViewport (mScrollWidget* self, int *x, int *y)
{
    if (x)
        *x -= self->leftMargin;
    if (y)
        *y -= self->topMargin;
}

static inline int mScrollWidget_viewportToContent (mScrollWidget* self, int *x, int *y)
{
    if (x)
        *x = *x + self->contX;
    if (y)
        *y = *y + self->contY;

    return 0;
}

static inline int mScrollWidget_contentToViewport (mScrollWidget* self, int *x, int *y)
{
    if (x)
        *x = *x - self->contX;

    if (y)
        *y = *y - self->contY;

    if (!x || !y)
        return -1;

    if (*x < 0 || *x > self->visWidth ||
               *y < 0 || *y > self->visHeight)
        return -1;
    return 0;
}

static void mScrollWidget_contentToWindow(mScrollWidget* self, int *x, int *y)
{
    _c(self)->contentToViewport (self, x, y);
    _c(self)->viewportToWindow (self, x, y);
}

static void mScrollWidget_windowToContent(mScrollWidget* self, int *x, int *y)
{
    _c(self)->windowToViewport (self, x, y);
    _c(self)->viewportToContent (self, x, y);
}

static void mScrollWidget_refreshRect (mScrollWidget* self, const RECT *rc)
{
    RECT rcUpdate;

    if (rc) {
        CopyRect(&rcUpdate, rc);
        _c(self)->contentToWindow(self, &rcUpdate.left, &rcUpdate.top);
        _c(self)->contentToWindow(self, &rcUpdate.right, &rcUpdate.bottom);
    }
    else {
        _c(self)->getVisRect(self, &rcUpdate);
    }

    InvalidateRect ( self->hwnd, &rcUpdate, TRUE);
}

static void 
mScrollWidget_initMargins(mScrollWidget* self, 
        int left, int top, int right, int bottom)
{
    RECT rcWnd;
    self->leftMargin = left;
    self->topMargin = top;
    self->rightMargin = right;
    self->bottomMargin = bottom;

    GetClientRect (self->hwnd, &rcWnd);
    self->visWidth = RECTW(rcWnd) - self->leftMargin - self->rightMargin;
    self->visHeight = RECTH(rcWnd) - self->topMargin - self->bottomMargin;

    self->contWidth = self->visWidth; 
    self->contHeight = self->visHeight; 

    //save user set value
    self->realContWidth = 0; 
    self->realContHeight = 0; 
}

static void mScrollWidget_getContRect(mScrollWidget *self, RECT *rcCont)
{
    rcCont->left = 0;
    rcCont->top = 0;

    if (self->realContWidth)
        rcCont->right = self->realContWidth;
    else
        rcCont->right = self->contWidth;

    if (self->realContHeight)
        rcCont->bottom = self->realContHeight;
    else
        rcCont->bottom = self->contHeight;

    _c(self)->contentToWindow(self, &rcCont->left, &rcCont->top);
    _c(self)->contentToWindow(self, &rcCont->right, &rcCont->bottom);
}

BEGIN_CMPT_CLASS(mScrollWidget, mWidget)
	CLASS_METHOD_MAP(mScrollWidget, construct)
	CLASS_METHOD_MAP(mScrollWidget, initMargins)
    CLASS_METHOD_MAP(mScrollWidget, onHScroll);
    CLASS_METHOD_MAP(mScrollWidget, onVScroll);
    CLASS_METHOD_MAP(mScrollWidget, onKeyDown);
    CLASS_METHOD_MAP(mScrollWidget, onSizeChanged);
    CLASS_METHOD_MAP(mScrollWidget, wndProc);
    CLASS_METHOD_MAP(mScrollWidget, moveContent);
    CLASS_METHOD_MAP(mScrollWidget, resetViewPort);
    CLASS_METHOD_MAP(mScrollWidget, setScrollInfo);
    CLASS_METHOD_MAP(mScrollWidget, makePosVisible);
    CLASS_METHOD_MAP(mScrollWidget, isVisible);
    CLASS_METHOD_MAP(mScrollWidget, setProperty);
    CLASS_METHOD_MAP(mScrollWidget, getProperty);
    CLASS_METHOD_MAP(mScrollWidget, getVisRect);
    CLASS_METHOD_MAP(mScrollWidget, getContRect);
    CLASS_METHOD_MAP(mScrollWidget, windowToContent);
    CLASS_METHOD_MAP(mScrollWidget, contentToWindow);
    CLASS_METHOD_MAP(mScrollWidget, refreshRect);
    CLASS_METHOD_MAP(mScrollWidget, viewportToContent);
    CLASS_METHOD_MAP(mScrollWidget, viewportToWindow);
    CLASS_METHOD_MAP(mScrollWidget, contentToViewport);
    CLASS_METHOD_MAP(mScrollWidget, windowToViewport);
END_CMPT_CLASS

