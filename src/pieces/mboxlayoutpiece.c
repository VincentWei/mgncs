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
#include "mlayoutpiece.h"
#include "mboxlayoutpiece.h"

static void delete_cell_infos(mBoxLayoutPiece *self)
{
	if(self->count>0)
	{
		free(self->cell_info);
		free(self->cells);
	}
}

static void resize_boxlayout(mBoxLayoutPiece *self, int count)
{
	unsigned short* cell_info;
	mHotPiece ** cells ;
	if((unsigned short)count == self->count)
		return;

	cell_info = (unsigned short*)calloc(count, sizeof(unsigned short));
	cells = (mHotPiece **)calloc(count, sizeof(mHotPiece*));

	if(self->count > 0)
	{
		int i;
		memcpy(cell_info, self->cell_info, (self->count<count?self->count:count)*sizeof(unsigned short));
		memcpy(cells, self->cells, (self->count<count?self->count:count)*sizeof(mHotPiece*));
		for(i=count; i<self->count; i++)
		{
			DELPIECE(self->cells[i]);
		}
	}

	delete_cell_infos(self);

	self->count = count;
	self->cells = cells;
	self->cell_info = cell_info;

}

static void mBoxLayoutPiece_construct(mBoxLayoutPiece* self, DWORD add_data)
{
	Class(mLayoutPiece).construct((mLayoutPiece*)self, add_data);

	self->left = self->top = self->right = self->bottom = 0;
	self->count = 0;
	self->margin = 0;
	self->space = 0;
	self->cell_info = NULL;
	self->cells = NULL;
	if((int)add_data > 0)
		resize_boxlayout(self, (int)add_data);
}

static void mBoxLayoutPiece_destroy(mBoxLayoutPiece* self)
{
	int i;
	for(i=0; i<self->count; i++)
	{
		DELPIECE(self->cells[i]);
	}

	delete_cell_infos(self);

	Class(mLayoutPiece).destroy((mLayoutPiece*)self);

}

static BOOL mBoxLayoutPiece_setRenderer(mBoxLayoutPiece * self, const char* rdr_name)
{
	int i;
	if(rdr_name == NULL)
		return FALSE;

	for(i=0; i<self->count; i++)
	{
		if(self->cells[i])
			_c(self->cells[i])->setRenderer(self->cells[i], rdr_name);
	}

	return TRUE;
}

static mHotPiece * mBoxLayoutPiece_hitTest(mBoxLayoutPiece* self, int x, int y)
{
	mHotPiece * hp = NULL;
	int i;

	if(x > self->left && x < self->right
		&& y > self->top && y < self->bottom)
	{
		for(i=0; i<self->count; i++)
		{
			if(self->cells[i] && (hp=_c(self->cells[i])->hitTest(self->cells[i], x, y)))
			{
				return hp;
			}
		}

		//return (mHotPiece*)self;
	}

	return NULL;
}

BOOL mBoxLayoutPiece_calcSize(mBoxLayoutPiece *self, int total_size, int *sizes)
{
	int left_size ;
	int i;
	int auto_count = 0;
	unsigned short * cinfo = self->cell_info;

	if(self->space > 0  && self->count > 1)
		total_size -= self->space * (self->count - 1);

	total_size -= self->margin*2;

	left_size = total_size;

	for(i = 0; i<self->count; i++)
	{
		int size = mBoxLayoutPiece_getSize(cinfo[i]);
		if(size > 0)
		{
			switch(mBoxLayoutPiece_getSizeType(cinfo[i]) )
			{
			case NCS_LAYOUTPIECE_ST_FIXED:
				sizes[i] = size;
				break;
			case NCS_LAYOUTPIECE_ST_PERCENT:
				sizes[i] = (total_size * size) / 100;
				break;
			default:
				sizes[i] = _c(self)->getCellAutoSize(self, i);
				break;
			}
		}
		else{
			sizes[i] = 0;
			auto_count ++;
		}
		left_size -= sizes[i];
	}

	if(left_size > 0 && auto_count > 0)
	{
		int avg = left_size / auto_count;
		for(i=0; i<self->count; i++)
		{
			if(sizes[i] == 0)
			{
				sizes[i] = avg;
			}
		}
	}
	return TRUE;
}


static BOOL mBoxLayoutPiece_setRect(mBoxLayoutPiece *self, const RECT *prc)
{
	if(prc == NULL)
		return FALSE;

	self->left = (short)prc->left;
	self->top = (short)prc->top;
	self->right = (short)prc->right;
	self->bottom = (short)prc->bottom;

	_c(self)->recalcBox(self);

	return TRUE;
}

static BOOL mBoxLayoutPiece_getRect(mBoxLayoutPiece* self, PRECT prc)
{
	if(!prc )
		return FALSE;

	prc->left = self->left;
	prc->top  = self->top;
	prc->right = self->right;
	prc->bottom = self->bottom;

	return TRUE;
}

static BOOL mBoxLayoutPiece_setProperty(mBoxLayoutPiece *self, int id, DWORD value)
{
	int i = 0;
	switch(id)
	{
	case NCSP_BOXLAYOUTPIECE_SPACE:
		if(self->space == (unsigned char)value)
			return FALSE;
		self->space = (unsigned char)value;
		_c(self)->recalcBox(self);
		return TRUE;
	case NCSP_BOXLAYOUTPIECE_MARGIN:
		if(self->margin == (unsigned char)value)
			return FALSE;
		self->margin = (unsigned char)value;
		_c(self)->recalcBox(self);
		return TRUE;
	case NCSP_BOXLAYOUTPIECE_COUNT:
		if(self->count == (unsigned short)value)
			return FALSE;
		resize_boxlayout(self, (int)value);
		return TRUE;
	}

	for(i=0; i<self->count; i++)
	{
		if(self->cells[i])
			_c(self->cells[i])->setProperty(self->cells[i], id, value);
	}

	return Class(mLayoutPiece).setProperty((mLayoutPiece*)self, id, value);

}

static DWORD mBoxLayoutPiece_getProperty(mBoxLayoutPiece *self, int id)
{
	switch(id)
	{
	case NCSP_BOXLAYOUTPIECE_SPACE:
		return self->space;

	case NCSP_BOXLAYOUTPIECE_MARGIN:
		return self->margin;

	case NCSP_BOXLAYOUTPIECE_COUNT:
		return self->count;
	}

	return Class(mLayoutPiece).getProperty((mLayoutPiece*)self, id);
}

static void mBoxLayoutPiece_paint(mBoxLayoutPiece *self, HDC hdc, mWidget* owner, DWORD add_data)
{
	int i;

	for(i = 0; i<self->count; i++)
	{
		if(self->cells[i])
			_c(self->cells[i])->paint(self->cells[i], hdc, (mObject*)owner, add_data);
	}

}

static int mBoxLayoutPiece_setCellInfo(mBoxLayoutPiece *self, int idx, int size, int type, BOOL bupdate)
{
	if(idx < 0 || idx >= self->count)
		return 0;

	mBoxLayoutPiece_setSize(self->cell_info[idx], size);
	mBoxLayoutPiece_setSizeType(self->cell_info[idx], type);

	if(bupdate)
		_c(self)->recalcBox(self);

	return 1;
}

static mHotPiece * mBoxLayoutPiece_setCell(mBoxLayoutPiece *self, int idx, mHotPiece *cell)
{
	mHotPiece *old;
	if(idx < 0 || idx >= self->count)
		return NULL;

	old = self->cells[idx] ;
	self->cells[idx] = cell;

	return old;
}

static mHotPiece * mBoxLayoutPiece_getCell(mBoxLayoutPiece *self, int idx)
{
	if(idx < 0 || idx >= self->count)
		return NULL;

	return self->cells[idx];
}

static BOOL mBoxLayoutPiece_autoSize(mBoxLayoutPiece *self, mWidget *owner, const SIZE *pszMin, const SIZE *pszMax)
{

	return FALSE;
}


BEGIN_MINI_CLASS(mBoxLayoutPiece, mLayoutPiece)
	CLASS_METHOD_MAP(mBoxLayoutPiece, construct  )
	CLASS_METHOD_MAP(mBoxLayoutPiece, destroy    )
	CLASS_METHOD_MAP(mBoxLayoutPiece, setRenderer)
	CLASS_METHOD_MAP(mBoxLayoutPiece, hitTest    )
	CLASS_METHOD_MAP(mBoxLayoutPiece, setRect    )
	CLASS_METHOD_MAP(mBoxLayoutPiece, getRect    )
	CLASS_METHOD_MAP(mBoxLayoutPiece, setProperty)
	CLASS_METHOD_MAP(mBoxLayoutPiece, getProperty)
	CLASS_METHOD_MAP(mBoxLayoutPiece, paint    )
	CLASS_METHOD_MAP(mBoxLayoutPiece, setCellInfo)
	CLASS_METHOD_MAP(mBoxLayoutPiece, setCell    )
	CLASS_METHOD_MAP(mBoxLayoutPiece, getCell    )
	CLASS_METHOD_MAP(mBoxLayoutPiece, autoSize   )
END_MINI_CLASS


