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
#include "mcontainerpiece.h"

static void mContainerPiece_construct(mContainerPiece* self, DWORD add_data)
{
	Class(mHotPiece).construct((mHotPiece*)self, add_data);

	self->body = NULL;
}

static void mContainerPiece_destroy(mContainerPiece* self)
{
	DELPIECE(self->body);

	Class(mHotPiece).destroy((mHotPiece*)self);
}

static mHotPiece* mContainerPiece_hitTest(mContainerPiece*self, int x, int y)
{
	RECT rc;
	if(self->body && _c(self->body)->getRect(self->body, &rc) && PtInRect(&rc, x, y))
	{
		return (mHotPiece*)self; //return itself
	}
	return NULL;
}

static BOOL mContainerPiece_setRect(mContainerPiece* self, const PRECT prc)
{
	return self->body?
		(_c(self->body)->setRect(self->body, prc))
		:FALSE;
}

static BOOL mContainerPiece_getRect(mContainerPiece* self, PRECT prc)
{
	return self->body?
		(_c(self->body)->getRect(self->body, prc))
		: FALSE;
}

static BOOL mContainerPiece_setRenderer(mContainerPiece* self, const char* rdr_name)
{
	return self->body?
		(_c(self->body)->setRenderer(self->body, rdr_name))
		:FALSE;
}

static void mContainerPiece_paint(mContainerPiece* self, HDC hdc, mWidget * widget, DWORD add_data)
{
	if(self->body)
		_c(self->body)->paint(self->body, hdc, (mObject*)widget, add_data);
}

static BOOL mContainerPiece_setProperty(mContainerPiece *self, int id, DWORD value)
{
	if(self->body)
		return _c(self->body)->setProperty(self->body, id, value);
	return FALSE;
}

static DWORD mContainerPiece_getProperty(mContainerPiece *self, int id)
{
	if(self->body)
		return _c(self->body)->getProperty(self->body, id);
	return (DWORD)-1;
}

static BOOL mContainerPiece_autoSize(mContainerPiece* self, mWidget *owner, const SIZE *pszMin, const SIZE *pszMax)
{
	if(self->body)
		return _c(self->body)->autoSize(self->body, (mObject*)owner, pszMin, pszMax);
	return TRUE;
}

static void mContainerPiece_raiseEvent(mContainerPiece *self, int event_id, DWORD param)
{
	ncsRaiseEvent((mObject*)self, event_id, param);
}

BEGIN_MINI_CLASS(mContainerPiece, mHotPiece)
	CLASS_METHOD_MAP(mContainerPiece, construct)
	CLASS_METHOD_MAP(mContainerPiece, destroy)
	CLASS_METHOD_MAP(mContainerPiece, hitTest)
	CLASS_METHOD_MAP(mContainerPiece, setRect)
	CLASS_METHOD_MAP(mContainerPiece, getRect)
	CLASS_METHOD_MAP(mContainerPiece, setRenderer)
	CLASS_METHOD_MAP(mContainerPiece, setProperty)
	CLASS_METHOD_MAP(mContainerPiece, getProperty)
	CLASS_METHOD_MAP(mContainerPiece, paint)
	CLASS_METHOD_MAP(mContainerPiece, autoSize)
	CLASS_METHOD_MAP(mContainerPiece, raiseEvent)
END_MINI_CLASS


