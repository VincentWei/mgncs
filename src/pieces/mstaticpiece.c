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
#include "mstaticpiece.h"

static void mStaticPiece_construct(mStaticPiece *self, DWORD add_data)
{
	Class(mHotPiece).construct((mHotPiece*)self, add_data);

	self->left = self->top = self->right = self->bottom = 0;
}

static BOOL mStaticPiece_setRect(mStaticPiece *self, RECT *prc)
{
	if(!prc)
		return FALSE;

	SetStaticPieceRect(self, prc);

	return TRUE;
}

static BOOL mStaticPiece_getRect(mStaticPiece *self, RECT *prc)
{
	if(!prc)
		return FALSE;

	SetRect(prc, self->left, self->top, self->right, self->bottom);
	return TRUE;
}

static mHotPiece * mStaticPiece_hitTest(mStaticPiece *self, int x, int y)
{
	if(x > self->left && x < self->right
		&& y > self->top && y < self->bottom)
		return (mHotPiece*)self;
	return NULL;
}

BEGIN_MINI_CLASS(mStaticPiece, mHotPiece)
	CLASS_METHOD_MAP(mStaticPiece, construct)
	CLASS_METHOD_MAP(mStaticPiece, getRect)
	CLASS_METHOD_MAP(mStaticPiece, setRect)
	CLASS_METHOD_MAP(mStaticPiece, hitTest)
END_MINI_CLASS
