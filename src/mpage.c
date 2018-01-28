/*
 ** $Id: mpage.c 1681 2017-10-26 06:46:31Z weiym $
 **
 ** The implementation of mPropsheet control.
 **
 ** Copyright (C) 2009 Feynman Software.
 **
 ** All rights reserved by Feynman Software.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSCTRL_PAGE

static void mPage_construct (mPage *self,DWORD addData)
{
	g_stmContainerCls.construct((mContainer*)self,  addData);

    self->titleWidth = 0;
    self->hIcon = 0;
}

static const char* mPage_getTitle(mPage* self)
{
    return GetWindowCaption(self->hwnd);
}

static void mPage_getTitleWidth (mPage* self, HDC hdc)
{
    int     width = 0;
    SIZE    ext;
    char*   title;

    title = (char*)GetWindowCaption(self->hwnd);

    if (title) {
        GetTextExtent (hdc, title, -1, &ext);
        width = ext.cx;
    }

    if (self->hIcon) {
        width += 16;
    }

    self->titleWidth = width;
}

BOOL mPage_setTitle(mPage* self, const char* title)
{
    return SetWindowCaption(self->hwnd, title);
}

int mPage_getTitleLength(mPage* self)
{
    return strlen(GetWindowCaption(self->hwnd));
}

static void mPage_showPage (mPage* self, int showCmd)
{
    HWND focus;

    ShowWindow (self->hwnd, showCmd);

    focus = GetNextDlgTabItem (self->hwnd, (HWND)0, 0);
    if (SendMessage (self->hwnd, MSG_SHOWPAGE, (WPARAM)focus, (LPARAM)showCmd)
            && showCmd == SW_SHOW) {
        if (focus) SetFocus(focus);
    }
}

BOOL mPage_setIcon(mPage* self, HICON icon)
{
    if (self == NULL)
        return FALSE;

    self->hIcon = icon;
    return TRUE;
}

HICON mPage_getIcon(mPage* self)
{
    if (self == NULL)
        return 0;

    return self->hIcon;
}

BOOL mPage_callUserHandler(mPage *self, void* handler, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *pret)
{
	if(handler == NULL)
		return FALSE;

	switch(message)
	{
	case MSG_INITPAGE:
		((NCS_CB_ONINITPAGE)handler)(self, lParam);
        return TRUE;

	case MSG_SHOWPAGE:
		*pret = ((NCS_CB_ONSHOWPAGE)handler)(self, (HWND)wParam, (int)lParam);
		return TRUE;

	case MSG_SHEETCMD:
		*pret = ((NCS_CB_ONSHEETCMD)handler)(self, (DWORD)wParam, (DWORD)lParam);
		return TRUE;
	}

	return Class(mContainer).callUserHandler((mContainer*)self, handler, message, wParam, lParam, pret);
}

BEGIN_CMPT_CLASS(mPage, mContainer)
    CLASS_METHOD_MAP(mPage, construct);
    CLASS_METHOD_MAP(mPage, showPage);
    CLASS_METHOD_MAP(mPage, getTitleWidth);
    CLASS_METHOD_MAP(mPage, setTitle);
    CLASS_METHOD_MAP(mPage, getTitle);
    CLASS_METHOD_MAP(mPage, setIcon);
    CLASS_METHOD_MAP(mPage, getIcon);
    CLASS_METHOD_MAP(mPage, getTitleLength);
	CLASS_METHOD_MAP(mPage, callUserHandler)
	SET_DLGCODE(DLGC_WANTTAB | DLGC_WANTARROWS)
END_CMPT_CLASS

#endif //_MGNCSCTRL_PAGE
