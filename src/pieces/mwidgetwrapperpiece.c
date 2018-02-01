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
#include "mwidgetwrapperpiece.h"

static void mWidgetWrapperPiece_construct(mWidgetWrapperPiece* self, DWORD add_data)
{
	Class(mHotPiece).construct((mHotPiece*)self, add_data);

	self->widget = (mObject*)add_data;
}

/** Should we delete the self->widget? No! widget would be delete
 * by its parent
static void mWidgetWrapperPiece_destroy(mWidgetWrapperPiece* self)
{
	DELETE(self->widget);
	Class(mHotPiece).destroy((mHotPiece*)self);
}*/

static BOOL mWidgetWrapperPiece_setRect(mWidgetWrapperPiece* self, const PRECT prc)
{
	if(!prc)
		return FALSE;

	if(self->widget)
	{
		mWidget * widget = (mWidget*)(self->widget);
		return MoveWindow(widget->hwnd, prc->left, prc->top, RECTWP(prc), RECTHP(prc), TRUE);
	}
	return FALSE;
}

static BOOL mWidgetWrapperPiece_getRect(mWidgetWrapperPiece* self, PRECT prc)
{
	if(!prc)
		return FALSE;

	if(self->widget)
	{
		mWidget * widget = (mWidget*)(self->widget);
		return GetWindowRect(widget->hwnd, prc);
	}
	return FALSE;
}

static BOOL mWidgetWrapperPiece_setRenderer(mWidgetWrapperPiece* self, const char* rdr_name)
{
	if(!rdr_name)
		return FALSE;

	if(self->widget){
		mWidget * widget = (mWidget*)(self->widget);
		widget->renderer = ncsRetriveCtrlRDR(rdr_name, _c(widget)->className);
		SetWindowElementRenderer(widget->hwnd, rdr_name, NULL);
		return widget->renderer != NULL;
	}
	return FALSE;
}

static BOOL mWidgetWrapperPiece_setProperty(mWidgetWrapperPiece* self, int id, DWORD value)
{
	if(self->widget)
	{
		return SendMessage(((mWidget*)(self->widget))->hwnd, MSG_SETPIECE_PROPERTY, (WPARAM)id, (LPARAM)value);
	}
	return FALSE;
}

static DWORD mWidgetWrapperPiece_getProperty(mWidgetWrapperPiece* self, int id)
{
	if(self->widget)
	{
		return SendMessage(((mWidget*)(self->widget))->hwnd, MSG_GETPIECE_PROPERTY, (WPARAM)id, 0);
	}
	return -1;
}



BEGIN_MINI_CLASS(mWidgetWrapperPiece, mHotPiece)
	CLASS_METHOD_MAP(mWidgetWrapperPiece, construct)
	CLASS_METHOD_MAP(mWidgetWrapperPiece, setRect)
	CLASS_METHOD_MAP(mWidgetWrapperPiece, getRect)
	CLASS_METHOD_MAP(mWidgetWrapperPiece, setRenderer)
	CLASS_METHOD_MAP(mWidgetWrapperPiece, setProperty)
	CLASS_METHOD_MAP(mWidgetWrapperPiece, getProperty)
END_MINI_CLASS


