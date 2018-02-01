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
 *   <http://www.minigui.com/en/about/licensing-policy/>.
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
#include "mrenderablepiece.h"

static void mRenderablePiece_construct(mRenderablePiece * self, DWORD add_data)
{
	Class(mStaticPiece).construct((mStaticPiece*)self, add_data);

	self->rdr_func = NULL;
}

static void mRenderablePiece_paint(mRenderablePiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	if(self->rdr_func)
		self->rdr_func((mHotPiece*)self, hdc, (mObject*)owner, add_data);
}

static BOOL mRenderablePiece_setRenderer(mRenderablePiece *self, const char* rdr_name)
{
	PHotPieceRdrFunc rdr_func;	
	if(rdr_name == NULL)
		return FALSE;

	rdr_func = (PHotPieceRdrFunc)(void*)ncsRetriveCtrlRDR(rdr_name, TYPENAME(self));
	if(!rdr_func)
		return FALSE;

	self->rdr_func = rdr_func;
	return TRUE;
}

BEGIN_MINI_CLASS(mRenderablePiece, mStaticPiece)
	CLASS_METHOD_MAP(mRenderablePiece, construct)
	CLASS_METHOD_MAP(mRenderablePiece, paint)
	CLASS_METHOD_MAP(mRenderablePiece, setRenderer)
END_MINI_CLASS
