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
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "mhotpiece.h"
#include "mcontainerpiece.h"
#include "mabstractbuttonpiece.h"
#include "mpushbuttonpiece.h"

#include "mstaticpiece.h"
#include "mrenderablepiece.h"
#include "mbuttonboxpiece.h"

static void mPushButtonPiece_construct(mPushButtonPiece *self, DWORD add_data)
{
	Class(mAbstractButtonPiece).construct((mAbstractButtonPiece*)self, add_data);
	if(add_data)
		self->body = (mHotPiece*)add_data;
	else
	{
		self->body = (mHotPiece*)NEWPIECE(mButtonBoxPiece);
	}
}

static void mPushButtonPiece_destroy(mPushButtonPiece* self)
{
	DELPIECE(self->content);
	Class(mAbstractButtonPiece).destroy((mAbstractButtonPiece *)self);
}

static void mPushButtonPiece_paint(mPushButtonPiece *self, HDC hdc, mWidget * owner, DWORD add_data)
{
	Class(mAbstractButtonPiece).paint((mAbstractButtonPiece*)self, hdc, (mObject*)owner, add_data);

	if(self->content)
	{
		add_data = (add_data)|(self->state)|(NCS_PIECE_PAINT_MAKE_CHECK(self->check_state))
							      |((self->flags&NCSS_ABP_FLAT)?NCS_PIECE_PAINT_FLAT:0);
		if(self->state == NCS_ABP_PUSHED)
		{
			mHotPiece* content = self->content;
			RECT rc;
			_c(content)->getRect(content, &rc);
			OffsetRect(&rc, 1,1);
			_c(content)->setRect(content, &rc);
			_c(content)->paint(self->content, hdc, (mObject*)owner, add_data);
			OffsetRect(&rc, -1,-1);
			_c(content)->setRect(content, &rc);
		}
		else
			_c(self->content)->paint(self->content, hdc, (mObject*)owner, add_data);
	}
}

static BOOL mPushButtonPiece_setProperty(mPushButtonPiece *self, int id, DWORD value)
{
	if(self->content && _c(self->content)->setProperty(self->content, id, value))
		return TRUE;

	return Class(mAbstractButtonPiece).setProperty((mAbstractButtonPiece*)self, id, value);
}

static DWORD mPushButtonPiece_getProperty(mPushButtonPiece *self, int id)
{
	DWORD value;
	if(self->content && (value=_c(self->content)->getProperty(self->content, id))!=(DWORD)-1)
		return value;

	return Class(mAbstractButtonPiece).getProperty((mAbstractButtonPiece*)self, id);
}

static BOOL mPushButtonPiece_setRect(mPushButtonPiece *self, const RECT *prc)
{
	Class(mAbstractButtonPiece).setRect((mAbstractButtonPiece*)self, prc);

	if(self->content)
		_c(self->content)->setRect(self->content, prc);

	return TRUE;
}

static BOOL mPushButtonPiece_setRenderer(mPushButtonPiece* self, const char* rdr_name)
{
	Class(mAbstractButtonPiece).setRenderer((mAbstractButtonPiece*)self, rdr_name);

	if(self->content)
		_c(self->content)->setRenderer(self->content, rdr_name);
	return TRUE;
}

static BOOL mPushButtonPiece_autoSize(mPushButtonPiece *self, mWidget *owner, const SIZE *pszMin, const SIZE *pszMax)
{
	if(self->content)
	{
		if(_c(self->content)->autoSize(self->content, (mObject*)owner, pszMin, pszMax) && self->body)
		{
			RECT rc ;
			_c(self->content)->getRect(self->content, &rc);
			_c(self->body)->setRect(self->body, &rc);
		}
	}
	return Class(mAbstractButtonPiece).autoSize((mAbstractButtonPiece*)self, (mObject*)owner, pszMin, pszMax);
}


BEGIN_MINI_CLASS(mPushButtonPiece, mAbstractButtonPiece)
	CLASS_METHOD_MAP(mPushButtonPiece, construct)
	CLASS_METHOD_MAP(mPushButtonPiece, destroy)
	CLASS_METHOD_MAP(mPushButtonPiece, paint)
	CLASS_METHOD_MAP(mPushButtonPiece, setProperty)
	CLASS_METHOD_MAP(mPushButtonPiece, getProperty)
	CLASS_METHOD_MAP(mPushButtonPiece, setRect)
	CLASS_METHOD_MAP(mPushButtonPiece, setRenderer)
	CLASS_METHOD_MAP(mPushButtonPiece, autoSize)
END_MINI_CLASS


