/*
    The implementation of mPropsheet control.

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
