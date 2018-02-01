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
    <http://www.minigui.com/en/about/licensing-policy/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "mhotpiece.h"

static void mHotPiece_destroy(mHotPiece *self)
{
//	ncsRemoveEventSource((mObject*)self);
	Class(mObject).destroy((mObject*)self);
}

static mHotPiece * mHotPiece_hitTest(mHotPiece* self, int x, int y)
{
	return NULL;
}

static void mHotPiece_paint(mHotPiece * self, HDC hdc, mWidget *owner, DWORD add_data)
{

}

static int mHotPiece_processMessage(mHotPiece *self, int message, WPARAM wParam, LPARAM lParam, mWidget *owner)
{
	return 0;
}

static BOOL mHotPiece_setProperty(mHotPiece * self, int id, DWORD value)
{
	return FALSE;
}

static DWORD mHotPiece_getProperty(mHotPiece *self, int id)
{
	return (DWORD)-1;
}

static BOOL mHotPiece_enable(mHotPiece * self, BOOL benable)
{
	return FALSE;
}

static BOOL mHotPiece_isEnabled(mHotPiece *self)
{
	return TRUE;
}

static BOOL mHotPiece_setRect(mHotPiece *self, const RECT *prc)
{
	return FALSE;
}

static BOOL mHotPiece_getRect(mHotPiece *self, RECT *prc)
{
	return FALSE;
}

static BOOL mHotPiece_setRenderer(mHotPiece *self, const char* rdr)
{
	return FALSE;
}

static BOOL mHotPiece_autoSize(mHotPiece *self, mWidget *owner, const SIZE *pszMin, const SIZE *pszMax)
{
	return FALSE;
}

BEGIN_MINI_CLASS(mHotPiece, mObject)
	CLASS_METHOD_MAP(mHotPiece, destroy       )
	CLASS_METHOD_MAP(mHotPiece, hitTest       )
	CLASS_METHOD_MAP(mHotPiece, paint         )
	CLASS_METHOD_MAP(mHotPiece, processMessage)
	CLASS_METHOD_MAP(mHotPiece, setProperty   )
	CLASS_METHOD_MAP(mHotPiece, getProperty   )
	CLASS_METHOD_MAP(mHotPiece, enable        )
	CLASS_METHOD_MAP(mHotPiece, isEnabled     )
	CLASS_METHOD_MAP(mHotPiece, setRenderer   )
	CLASS_METHOD_MAP(mHotPiece, setRect       )
	CLASS_METHOD_MAP(mHotPiece, getRect       )
	CLASS_METHOD_MAP(mHotPiece, autoSize      )
END_MINI_CLASS

void mHotPiece_update(mHotPiece *self, mObject *owner, BOOL bUpdate)
{
	RECT rc;

	if(!self || !owner)
		return;

	if(_c(self)->getRect(self, &rc))
		InvalidateRect(((mWidget*)(owner))->hwnd, &rc, bUpdate);
	else
		InvalidateRect(((mWidget*)(owner))->hwnd, NULL, bUpdate);

}


