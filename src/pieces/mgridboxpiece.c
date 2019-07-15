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
#include "mgridboxpiece.h"

static void mGridBoxPiece_construct(mGridBoxPiece *self, DWORD add_data)
{
	Class(mRenderablePiece).construct((mRenderablePiece*)self, add_data);

	self->rows = mGridBoxPiece_getRow(add_data);
	self->cols = mGridBoxPiece_getCol(add_data);

}

static CELL_POINT mGridBoxPiece_hitCell(mGridBoxPiece *self, int x, int y)
{
	CELL_POINT cpoint= NULL_CELL_POINT;
	int w, h;
//	int cell_width, cell_height;

	if(!(self->rows > 0 && self->cols > 0))
		return cpoint;

	if(!_c(self)->hitTest(self, x, y))
		return cpoint;

	x -= self->left;
	y -= self->top;
	w = self->right - self->left;
	h = self->bottom - self->top;
		
//	cell_width =  w / self->cols;
//	cell_height = h / self->rows;

	cpoint.col = x * self->cols / w;
	cpoint.row = y * self->rows / h;

	return cpoint;
	
}

static RECT *mGridBoxPiece_getCellRect(mGridBoxPiece *self, int row, int col, RECT *prc)
{
	int w, h;
	int cell_width, cell_height;
	if(!prc)
		return NULL;
	if(!(self->rows > 0 && self->cols > 0))
		return NULL;

	
	w = self->right - self->left;
	h = self->bottom - self->top;

	cell_width =  w / self->cols;
	cell_height = h / self->rows;

	prc->left = self->left + cell_width * col;
	prc->top  = self->top  + cell_height * row;
	if(col == self->cols - 1)
		prc->right = self->right;
	else
		prc->right = prc->left + cell_width;
	if(row == self->rows - 1)
		prc->bottom = self->bottom;
	else
		prc->bottom = prc->top + cell_height;

	return prc;

}

static void mGridBoxPiece_updateNewCell(mGridBoxPiece *self, CELL_POINT old_cell, CELL_POINT new_cell, int old_mask, int new_mask, mWidget *owner)
{
	mGridBoxPiecePaintData paint_data;
	HDC hdc;
	RECT rc;
	paint_data.add_data = 0;

	hdc = GetClientDC(owner->hwnd);
	if(_c(self)->getRect(self, &rc))
		ClipRectIntersect(hdc, &rc);


	//paint
	if(old_mask != 0)
	{
		paint_data.mask = old_mask;
		paint_data.sel_cell = old_cell;
		_c(self)->paint(self, hdc, (mObject*)owner, (DWORD)&paint_data);
	}


	if(new_mask != 0)
	{
		paint_data.mask = new_mask;
		if(new_mask == NCS_GRIDBOXPIECE_PAINT_MASK_HOT)
		{
			paint_data.hot_cell = new_cell;
		}
		else
		{
			paint_data.sel_cell = new_cell;
		}

		//paint
		_c(self)->paint(self, hdc, (mObject*)owner, (DWORD)&paint_data);
	}
					
	ReleaseDC(hdc);

}

static BOOL mGridBoxPiece_setProperty(mGridBoxPiece *self, int id, DWORD value)
{
	switch(id)
	{
	case NCSP_GRIDBOXPIECE_ROWS:
		self->rows = (unsigned short)value;
		return TRUE;
	case NCSP_GRIDBOXPIECE_COLS:
		self->cols = (unsigned short)value;
		return TRUE;
	}

	return Class(mRenderablePiece).setProperty((mRenderablePiece*)self, id, value);
}


static DWORD mGridBoxPiece_getProperty(mGridBoxPiece *self, int id)
{
	switch(id)
	{
	case NCSP_GRIDBOXPIECE_ROWS:
		return self->rows;
	case NCSP_GRIDBOXPIECE_COLS:
		return self->cols;
	}

	return Class(mRenderablePiece).getProperty((mRenderablePiece*)self, id);
}


BEGIN_MINI_CLASS(mGridBoxPiece, mRenderablePiece)
	CLASS_METHOD_MAP(mGridBoxPiece, construct);
	CLASS_METHOD_MAP(mGridBoxPiece, hitCell);
	CLASS_METHOD_MAP(mGridBoxPiece, getCellRect);
	CLASS_METHOD_MAP(mGridBoxPiece, setProperty);
	CLASS_METHOD_MAP(mGridBoxPiece, getProperty);
	CLASS_METHOD_MAP(mGridBoxPiece, updateNewCell);
END_MINI_CLASS

