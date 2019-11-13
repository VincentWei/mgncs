/* 
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
    <http://www.minigui.com/blog/minigui-licensing-policy/>.
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

/*for page handle*/
#define IDC_PAGE    20

static void mPropSheet_construct (mPropSheet *self, DWORD addData)
{
    RECT rcClient;
	g_stmWidgetCls.construct((mWidget*)self,  addData);

    GetClientRect (self->hwnd, &rcClient);

    self->minTabWidth = 16;
    self->active = self->firstView = NULL;
    self->pageCount = 0;
    self->tabMargin = 8;
    self->btnShow = FALSE;
    self->scrollTabWidth = RECTW(rcClient);
}

#define NEXTPAGE(self, page) \
    _c(self)->getNextPage(self, page)

#define PREVPAGE(self, page) \
    _c(self)->getPrevPage(self, page)

static mPage* createPage (mPropSheet* self,
        PDLGTEMPLATE dlgTemplate, DWORD style,
        const NCS_EVENT_HANDLER *handlers)
{
    mPage*  page;
    DWORD   exStyle;
    RECT    rcClient, rcPage;

    if (GetWindowExStyle(self->hwnd) & WS_EX_TRANSPARENT)
        exStyle = WS_EX_TRANSPARENT | WS_EX_USEPARENTRDR;
    else
        exStyle = WS_EX_USEPARENTRDR;

    GetClientRect(self->hwnd, &rcClient);
    self->renderer->getRect(self, &rcClient, &rcPage, NCSF_PRPSHT_PAGE);

    page = (mPage*)ncsCreateWindow (
                        NCSCTRL_PAGE,
                        dlgTemplate->caption,
                        WS_NONE, /* WS_VISIBLE */
                        exStyle,
                        IDC_PAGE,
                        rcPage.left, rcPage.top,
                        RECTW(rcPage), RECTH(rcPage),
                        self->hwnd,
                        NULL,
                        NULL,
                        (NCS_EVENT_HANDLER*)handlers,
                        dlgTemplate->dwAddData);

    if (!page) {
        fprintf (stderr, "createPage Error> create page window error. \n ");
        return NULL;
    }

    _c(page)->setIcon(page, dlgTemplate->hIcon);

    if (!_c(page)->addIntrinsicControls(page,
                dlgTemplate->controls, dlgTemplate->controlnr)) {
        fprintf (stderr, "createPage Error> set controls error. \n ");
        DestroyWindow(page->hwnd);
        return NULL;
    }

    SendMessage (page->hwnd, MSG_INITPAGE, 0, dlgTemplate->dwAddData);

    return page;
}

static void updatePropSheet (mPropSheet* self)
{
    mPage   *page, *tmp, *list;
    int     tabWidth = 0;

    list = NEXTPAGE(self, NULL);

    /*set btnShow status*/
    page = list;
    self->btnShow = FALSE;

    while (page) {
        tabWidth += page->titleWidth;

        if (tabWidth > self->scrollTabWidth) {
            self->btnShow = TRUE;
            self->maxTabWidth = self->scrollTabWidth;
            break;
        }
        page = NEXTPAGE(self, page);
    }

    if (self->firstView == NULL || self->btnShow == FALSE) {
        self->firstView = list;
    }

    //firstView should be previous to active page
    if (self->active) {
        page = NEXTPAGE(self, self->active);

        while (page) {
            if (page == self->firstView) {
                self->firstView = self->active;
                break;
            }
            page = NEXTPAGE(self, page);
        }
    }

    page = self->firstView;

    while (page) {
        tmp = page;
        tabWidth = 0;

        while (tmp) {
            tabWidth += tmp->titleWidth;

            if (tmp == self->active) {
                if (tabWidth <= self->maxTabWidth) {
                    return;
                }
            }
            tmp = NEXTPAGE(self, tmp);
        }

        self->firstView = NEXTPAGE(self, self->firstView);
        page = self->firstView;
    }
}

static void calcScrollPageTitleWidth (HDC hdc, mPropSheet* self, mPage* page)
{
    _c(page)->getTitleWidth(page, hdc);

    page->titleWidth += self->tabMargin;

    if (page->titleWidth < self->minTabWidth) {
        page->titleWidth = self->minTabWidth;
    }

    if (page->titleWidth > self->scrollTabWidth) {
        page->titleWidth = self->scrollTabWidth;
    }
}

static void recalcTabWidths (mPropSheet* self, DWORD style)
{
    mPage*  page;
    HDC     hdc;

    if (self->pageCount == 0)
        return;

    if (style & NCSS_PRPSHT_COMPACTTAB) {
        int totalWidth = 0;

        hdc = GetClientDC (self->hwnd);
        page = NEXTPAGE(self, NULL);

        while (page) {
            page->titleWidth = 0;
            _c(page)->getTitleWidth(page, hdc);
            page->titleWidth += self->tabMargin;

            if (page->titleWidth < self->minTabWidth) {
                page->titleWidth = self->minTabWidth;
            }

            totalWidth += page->titleWidth;

            if (totalWidth > RECTW(self->headRect)) {
                int tmpWidth;
                totalWidth -= page->titleWidth;
                tmpWidth = RECTW(self->headRect) - totalWidth;
                if (tmpWidth >= self->minTabWidth)
                    page->titleWidth = tmpWidth;
                break;
            }
            page = NEXTPAGE(self, page);
        }
        ReleaseDC (hdc);
        self->maxTabWidth = totalWidth;
    }
    else {
        int width = RECTW(self->headRect)/self->pageCount;
        int remainder = RECTW(self->headRect)%self->pageCount;
        int i = 0;

        page = NEXTPAGE(self, NULL);
        while(page) {
            page->titleWidth = width;
            if (i < remainder) {
                page->titleWidth ++;
                i++;
            }
            page = NEXTPAGE(self, page);
        }

        self->maxTabWidth = RECTW(self->headRect);
    }
}

static BOOL changeActivePage(mPropSheet* self,
        mPage* page, DWORD style, BOOL reCount)
{
    HDC hdc;
    if (!page)
        return FALSE;

    if (page != self->active) {
        /*hide active page*/
        if (self->active) {
            _c((self->active))->showPage (self->active, SW_HIDE);
        }

        /*change active page*/
        self->active = page;

        /*change firstView page for scrollable style*/
        if ((style&NCSS_PRPSHT_BTNMASK) == NCSS_PRPSHT_SCROLLABLE) {

            hdc = GetClientDC (self->hwnd);
            calcScrollPageTitleWidth (hdc, self, page);
            ReleaseDC (hdc);

            updatePropSheet(self);
        }

        /*notify parent activeChanged*/
        ncsNotifyParent ((mWidget*)self, NCSN_PRPSHT_ACTIVECHANGED);

        /*show new active page*/
        _c(page)->showPage (page, SW_SHOW);

        if (((style&NCSS_PRPSHT_BTNMASK) != NCSS_PRPSHT_SCROLLABLE)
                && reCount) {
            recalcTabWidths (self, style);
        }

        InvalidateRect (self->hwnd, &(self->headRect), TRUE);
        return TRUE;
    }
    return FALSE;
}

static int mPropSheet_onSizeChanged(mPropSheet* self, RECT* prcClient)
{
    DWORD   style = GetWindowStyle (self->hwnd);
    mPage   *page = NEXTPAGE(self, NULL);
    RECT    rcPage;
	RECT    rcClient = {0, 0, RECTWP(prcClient), RECTHP(prcClient)};

    self->renderer->resetHeadArea(self, &rcClient, style);

    if ((style&NCSS_PRPSHT_BTNMASK) != NCSS_PRPSHT_SCROLLABLE) {
        recalcTabWidths (self, style);
    }
    else {
        self->scrollTabWidth = RECTW(self->headRect);
        updatePropSheet(self);
    }

    if (page)
        self->renderer->getRect (self, &rcClient, &rcPage, NCSF_PRPSHT_PAGE);

    while(page) {
        MoveWindow (page->hwnd, rcPage.left, rcPage.top,
                RECTW(rcPage), RECTH(rcPage), page == self->active);
        page = NEXTPAGE(self, page);
    }

    InvalidateRect (self->hwnd, &(self->headRect), TRUE);
    return 0;
}

static int mPropSheet_onLButtonDown(mPropSheet* self,
        int x, int y, DWORD key_flags)
{
    DWORD   style;
    mPage*  page = NULL;
    RECT    rcClient, rcResult, rcTab;

    if (!PtInRect (&(self->headRect), x, y))
        return 1;

    style = GetWindowStyle (self->hwnd);
    GetClientRect(self->hwnd, &rcClient);

    self->renderer->getRect(self, &rcClient, &rcResult, NCSF_PRPSHT_TAB);

    if (!PtInRect (&rcResult, x, y)) {
        if (!self->btnShow) {
            return 1;
        }
        self->renderer->getRect(self, &rcClient, &rcResult, NCSF_PRPSHT_LEFT);
        if (!PtInRect (&rcResult, x, y)) {
            self->renderer->getRect(self, &rcClient, &rcResult, NCSF_PRPSHT_RIGHT);
            if (!PtInRect (&rcResult, x, y)) {
                return 1;
            }
            else {
                /*right*/
                page = NEXTPAGE(self, self->active);
            }
        }
        else {
            /*left*/
            page = PREVPAGE(self, self->active);
        }
    }
    else {
        /*tab*/
        if ((style & NCSS_PRPSHT_BTNMASK) == NCSS_PRPSHT_SCROLLABLE) {
            page = self->firstView;
        }
        else  {
            page = NEXTPAGE(self, NULL);
        }

        rcTab.right = rcResult.left;
        rcTab.top = rcResult.top;
        rcTab.bottom = rcResult.bottom;
        while (page) {
            rcTab.left = rcTab.right;
            rcTab.right += page->titleWidth;

            if (rcTab.right > rcResult.right) {
                page = NULL;
                break;
            }

            if (PtInRect (&rcTab, x, y)) {
                break;
            }

            page = NEXTPAGE(self, page);
        }
    }

    if (!page)
        return 1;

    changeActivePage(self, page, style, FALSE);
	return 0;
}

static int mPropSheet_onKeyDown(mPropSheet* self, int scancode, int state)
{
    mPage   *activePage = NULL;
    DWORD   style;

    if (!(state & KS_CTRL) || !NEXTPAGE(self, NULL))
        return 1;

    style = GetWindowStyle (self->hwnd);

    switch (scancode) {
        case SCANCODE_CURSORBLOCKDOWN:
        case SCANCODE_CURSORBLOCKRIGHT:
            activePage = NEXTPAGE(self, self->active);
            break;

        case SCANCODE_CURSORBLOCKUP:
        case SCANCODE_CURSORBLOCKLEFT:
            activePage = PREVPAGE(self, self->active);
            break;
    }

    if (activePage)
        changeActivePage(self, activePage, style, FALSE);

    return 0;
}

static mPage* mPropSheet_addPage(mPropSheet* self,
        const PDLGTEMPLATE dlgTemplate,
        const NCS_EVENT_HANDLER * handlers)
{
    mPage*  page;
    DWORD   style;

    if ((dlgTemplate ->controlnr > 0) && !dlgTemplate->controls) {
        fprintf (stderr, "addPage Error> controls info error.\n");
        return NULL;
    }

    style = GetWindowStyle (self->hwnd);

    if ((style&NCSS_PRPSHT_BTNMASK) != NCSS_PRPSHT_SCROLLABLE) {
        if (RECTW(self->headRect) /(self->pageCount + 1) < self->minTabWidth) {
            fprintf (stderr, "addPage Error> headRect width error.\n");
            return NULL;
        }
    }

    if (!(page = createPage (self, dlgTemplate, style, handlers))) {
        fprintf (stderr, "addPage Error> create page error.\n");
        return NULL;
    }

    self->pageCount++;

    if (self->pageCount == 1) {
        /* only set the first page as the active page */
        changeActivePage(self, page, style, TRUE);
    }

    return page;
}

static BOOL mPropSheet_removePage(mPropSheet* self, mPage* delPage)
{
    mPage   *next, *prev = NULL;
    DWORD   style;
    BOOL    activeChanged = FALSE;

    if (!delPage)
        return FALSE;

    style = GetWindowStyle(self->hwnd);

    next = NEXTPAGE(self, delPage);

    if ((style & NCSS_PRPSHT_BTNMASK) != NCSS_PRPSHT_SCROLLABLE) {
        if (!next)
            prev = PREVPAGE(self, delPage);

        DestroyWindow (delPage->hwnd);
        self->pageCount --;
        recalcTabWidths (self, style);

        if (self->active == delPage) {
            if (!next)
                self->active = prev;
            else
                self->active = next;

            ncsNotifyParent ((mWidget*)self, NCSN_PRPSHT_ACTIVECHANGED);
            if (self->active) {
                _c(self->active)->showPage(self->active, SW_SHOW);
            }
        }

        InvalidateRect (self->hwnd, &(self->headRect), TRUE);
    }
    else {
        if (self->active == delPage) {
            if (next) {
                self->active = next;
                if (self->firstView == delPage)
                    self->firstView = next;
            }
            else {
                prev = PREVPAGE(self, delPage);
                self->active = prev;
                if (self->firstView == delPage)
                    self->firstView = prev;
            }
            activeChanged = TRUE;
        }
        else {
            /*delPage isn't active page*/
            if (self->firstView == delPage) {
                if(next)
                    self->firstView = next;
                else
                    self->firstView = PREVPAGE(self, delPage);
            }
        }
        DestroyWindow (delPage->hwnd);
        self->pageCount--;

        if (NEXTPAGE(self, NULL))  updatePropSheet(self);

        if (activeChanged)
            ncsNotifyParent ((mWidget*)self, NCSN_PRPSHT_ACTIVECHANGED);

        if (self->active) {
            _c(self->active)->showPage(self->active, SW_SHOW);
        }

        InvalidateRect (self->hwnd, &(self->headRect), TRUE);
    }
    return TRUE;
}

static mPage* mPropSheet_getPrevPage(mPropSheet* self, mPage* page)
{
    HWND child, next;

    if (!page)
        return NULL;

    child = GetNextChild(self->hwnd, HWND_NULL);

    while (child && (child != HWND_INVALID)) {
        next = GetNextChild(self->hwnd, child);
        if (next == page->hwnd)
            return (mPage*)GetWindowAdditionalData2(child);
        child = next;
    }

    return NULL;
}

static mPage* mPropSheet_getNextPage(mPropSheet* self, mPage* page)
{
    HWND child;

    if (!page) /*for first page*/
        child = GetNextChild(self->hwnd, HWND_NULL);
    else
        child = GetNextChild(self->hwnd, page->hwnd);

    if (child && (child != HWND_INVALID))
        return (mPage*)GetWindowAdditionalData2(child);
    else
        return NULL;
}


static int mPropSheet_broadCastMsg(mPropSheet* self, DWORD info1, DWORD info2)
{
    int     idx = 0;
    mPage*  page = NEXTPAGE(self, NULL);

    while (page) {
        if (SendMessage (page->hwnd, MSG_SHEETCMD, info1, info2))
            return idx + 1;
        idx++;
        page = NEXTPAGE(self, page);
    }
    return 0;
}

static BOOL mPropSheet_setActivePage(mPropSheet* self, mPage *page)
{
    DWORD style;

    if (!page)
        return FALSE;

    style = GetWindowStyle(self->hwnd);
    return changeActivePage(self, page, style, FALSE);
}

static int mPropSheet_getPageIndex(mPropSheet* self, mPage* page)
{
    int     idx = 0;
    HWND    list = GetNextChild (self->hwnd, HWND_NULL);

    while (list && (list != HWND_INVALID)) {
        if (list == page->hwnd)
            return idx;
        idx ++;
        list = GetNextChild(self->hwnd, list);
    }
    return -1;
}

static mPage* mPropSheet_getPageByIndex(mPropSheet* self, int pageIndex)
{
    int     idx = 0;
    HWND    list = GetNextChild (self->hwnd, HWND_NULL);

    while (list && (list != HWND_INVALID)) {
        if (idx == pageIndex) {
            return (mPage*)GetWindowAdditionalData2(list);
        }
        idx ++;
        list = GetNextChild(self->hwnd, list);
    }
    return NULL;
}

static BOOL mPropSheet_setActivePageByIndex(mPropSheet* self, int pageIndex)
{
    mPage*  page;
    DWORD style;

    page = _c(self)->getPageByIndex(self, pageIndex);
    if (!page)
        return FALSE;

    style = GetWindowStyle(self->hwnd);

    return changeActivePage(self, page, style, FALSE);
}

static BOOL mPropSheet_removePageByIndex(mPropSheet* self, int pageIndex)
{
    mPage*  page = _c(self)->getPageByIndex(self, pageIndex);
    return _c(self)->removePage(self, page);
}

static void recalcPageWidth(mPropSheet* self)
{
    mPage   *page = NEXTPAGE(self, NULL);
    HDC hdc;
    DWORD style = GetWindowStyle(self->hwnd);

    if ((style&NCSS_PRPSHT_BTNMASK) == NCSS_PRPSHT_SCROLLABLE) {
        hdc = GetClientDC (self->hwnd);
        while(page) {
            calcScrollPageTitleWidth (hdc, self, page);
            page = NEXTPAGE(self, page);
        }

        ReleaseDC (hdc);
        updatePropSheet(self);
    }
    else {
        recalcTabWidths (self, style);
    }

    InvalidateRect (self->hwnd, &(self->headRect), TRUE);
}

static BOOL mPropSheet_setProperty(mPropSheet *self, int id, DWORD value)
{
	if( id >= NCSP_PRPSHT_MAX)
		return FALSE;

	switch(id)
	{
        case NCSP_PRPSHT_ACTIVEPAGE:
            return mPropSheet_setActivePage(self, (mPage*)value);

        case NCSP_PRPSHT_ACTIVEPAGEIDX:
            return mPropSheet_setActivePageByIndex(self, value);

        case NCSP_PRPSHT_TABMARGIN:
            if ((int)value > 0) {
                self->tabMargin = (int)value;
                recalcPageWidth(self);
                return TRUE;
            }

        case NCSP_PRPSHT_MINTABWIDTH:
            if ((int)value > 0) {
                self->minTabWidth = (int)value;
                recalcPageWidth(self);
                return TRUE;
            }
	}

	return Class(mWidget).setProperty((mWidget*)self, id, value);
}

static DWORD mPropSheet_getProperty(mPropSheet* self, int id)
{
	if( id >= NCSP_PRPSHT_MAX)
		return -1;

	switch (id)
    {
        case NCSP_PRPSHT_TABMARGIN:
            return self->tabMargin;

        case NCSP_PRPSHT_PAGECOUNT:
            return self->pageCount;

        case NCSP_PRPSHT_MINTABWIDTH:
            return self->minTabWidth;

        case NCSP_PRPSHT_ACTIVEPAGE:
            return (DWORD)self->active;

        case NCSP_PRPSHT_ACTIVEPAGEIDX:
            return _c(self)->getPageIndex(self, self->active);

        case NCSP_PRPSHT_FIRSTVIEWPAGE:
            return (DWORD)self->firstView;

        case NCSP_PRPSHT_FIRSTVIEWPAGEIDX:
            return _c(self)->getPageIndex(self, self->firstView);
    }

	return Class(mWidget).getProperty((mWidget*)self, id);
}

static void mPropSheet_onPaint (mPropSheet* self, HDC hdc, const PCLIPRGN pinv_clip)
{
    mPage   *page;
    DWORD   style;
    RECT    rcClient, destRect, tabRect;

    style = GetWindowStyle(self->hwnd);

    GetClientRect (self->hwnd, &rcClient);

    /* draw border */
    self->renderer->getRect(self, &rcClient, &destRect, NCSF_PRPSHT_BORDER);
    self->renderer->drawBorder(self, hdc, &destRect);

    if ((style & NCSS_PRPSHT_BTNMASK) == NCSS_PRPSHT_SCROLLABLE) {
        page = self->firstView;
        if (self->btnShow) {
            self->renderer->getRect(self, &rcClient, &destRect, NCSF_PRPSHT_LEFT);

            if (self->active == NEXTPAGE(self, NULL))
                self->renderer->drawScrollBtn(self, hdc, &destRect, NCSR_ARROW_LEFT | NCSRS_DISABLE);
            else
                self->renderer->drawScrollBtn(self, hdc, &destRect, NCSR_ARROW_LEFT);

            self->renderer->getRect(self, &rcClient, &destRect, NCSF_PRPSHT_RIGHT);

            if (NEXTPAGE(self, self->active) == NULL)
                self->renderer->drawScrollBtn(self, hdc, &destRect, NCSR_ARROW_RIGHT | NCSRS_DISABLE);
            else
                self->renderer->drawScrollBtn(self, hdc, &destRect, NCSR_ARROW_RIGHT);
        }
    }
    else  {
        page = NEXTPAGE(self, NULL);
    }

    //get tab area
    self->renderer->getRect(self, &rcClient, &tabRect, NCSF_PRPSHT_TAB);
    destRect.right = tabRect.left;
    destRect.top = tabRect.top;
    destRect.bottom = tabRect.bottom;

    while (page) {
        destRect.left = destRect.right;
        destRect.right += page->titleWidth;

        if (destRect.right > tabRect.right) //&& !(self->firstView == page))
            break;

        self->renderer->drawTab(self, hdc, &destRect,
                GetWindowCaption(page->hwnd), _c(page)->getIcon(page),
                page == self->active);

        page = NEXTPAGE(self, page);
    }
}

static BOOL mPropSheet_addChildren(mPropSheet* self, NCS_WND_TEMPLATE * children, int count)
{
	DWORD exStyle = WS_EX_USEPARENTRDR;
	DWORD style;
	mPage *page, *activePage = NULL;
	RECT  rcClient, rcPage;
	int i;

	if(!children || count <= 0)
		return FALSE;

	style = GetWindowStyle(self->hwnd);

	if(GetWindowExStyle(self->hwnd) & WS_EX_TRANSPARENT)
		exStyle |= WS_EX_TRANSPARENT;

	GetClientRect(self->hwnd, &rcClient);
	self->renderer->getRect(self, &rcClient, &rcPage, NCSF_PRPSHT_PAGE);

	//create pages
	for(i=0; i<count; i++)
	{
		NCS_WND_TEMPLATE tmpl;
		if(!ncsIsChildClass(children[i].class_name, NCSCTRL_PAGE))
			continue;

		tmpl = children[i];
		tmpl.x = rcPage.left;
		tmpl.y = rcPage.top;
		tmpl.w = RECTW(rcPage);
		tmpl.h = RECTH(rcPage);
		tmpl.style &= ~WS_VISIBLE;
		tmpl.ex_style |= exStyle;

		page = (mPage*)ncsCreateWindowIndirect(&tmpl, self->hwnd);
		if(!page)
			continue;

		if(activePage == NULL)
			activePage = page;
        else
        {
            HDC hdc = GetClientDC (self->hwnd);
            calcScrollPageTitleWidth (hdc, self, page);
            ReleaseDC (hdc);
        }

		SendMessage(page->hwnd, MSG_INITPAGE, 0, tmpl.user_data);
		self->pageCount ++;

	}

	if(!activePage)
		return FALSE;

	changeActivePage(self, activePage, style, TRUE);
	return TRUE;
}

BEGIN_CMPT_CLASS(mPropSheet, mWidget)
    CLASS_METHOD_MAP(mPropSheet, construct)
    CLASS_METHOD_MAP(mPropSheet, onPaint)
    CLASS_METHOD_MAP(mPropSheet, onLButtonDown)
    CLASS_METHOD_MAP(mPropSheet, onSizeChanged)
    CLASS_METHOD_MAP(mPropSheet, onKeyDown)
    CLASS_METHOD_MAP(mPropSheet, addPage)
    CLASS_METHOD_MAP(mPropSheet, removePage)
    CLASS_METHOD_MAP(mPropSheet, removePageByIndex)
    CLASS_METHOD_MAP(mPropSheet, getNextPage)
    CLASS_METHOD_MAP(mPropSheet, getPrevPage)
    CLASS_METHOD_MAP(mPropSheet, getPageIndex)
    CLASS_METHOD_MAP(mPropSheet, getPageByIndex)
    CLASS_METHOD_MAP(mPropSheet, broadCastMsg)
    CLASS_METHOD_MAP(mPropSheet, setProperty)
    CLASS_METHOD_MAP(mPropSheet, getProperty)
    CLASS_METHOD_MAP(mPropSheet, addChildren)
	SET_DLGCODE( DLGC_WANTTAB | DLGC_WANTARROWS)
END_CMPT_CLASS

#endif //_MGNCSCTRL_PROPSHEET
