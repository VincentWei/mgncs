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
#include "mlayoutpiece.h"
#include "mpairpiece.h"

static void mPairPiece_construct(mPairPiece* self, DWORD add_data)
{
	mHotPiece ** pieces = (mHotPiece**)add_data;
	Class(mLayoutPiece).construct((mLayoutPiece*)self, add_data);

	self->first = NULL;
	self->second = NULL;
	self->flags = 0;

	if(pieces){
		self->first = pieces[0];
		self->second = pieces[1];
	}
}

static void mPairPiece_destroy(mPairPiece* self)
{
	DELPIECE(self->first);
	DELPIECE(self->second);

	Class(mLayoutPiece).destroy((mLayoutPiece*)self);

}

static BOOL mPairPiece_setRenderer(mPairPiece * self, const char* rdr_name)
{
	if(rdr_name == NULL)
		return FALSE;

	if(self->first)
		_c(self->first)->setRenderer(self->first, rdr_name);

	if(self->second)
		_c(self->second)->setRenderer(self->second, rdr_name);

	return TRUE;
}

static mHotPiece * mPairPiece_hitTest(mPairPiece* self, int x, int y)
{
	mHotPiece * hp = NULL;
	if(self->first)
		hp = _c(self->first)->hitTest(self->first, x, y);

	if(!hp && self->second)
		hp = _c(self->second)->hitTest(self->second, x, y);

	return hp;
}

static void inline calc_horz_first_second_size(const RECT *prc,
		RECT *pfirst_rc, RECT *psecond_rc,
		int first_size, int margin, int space,
		BOOL breverse)
{

	pfirst_rc->top    = prc->top + margin;
	pfirst_rc->bottom = prc->bottom - margin;
	psecond_rc->top = pfirst_rc->top;
	psecond_rc->bottom = pfirst_rc->bottom;

	if(breverse)
	{
		pfirst_rc->right  = prc->right - margin;
		pfirst_rc->left   = pfirst_rc->right - first_size;

		psecond_rc->left  = prc->left + margin;
		psecond_rc->right = pfirst_rc->left - space;
	}
	else
	{
		pfirst_rc->left   = prc->left + margin;
		pfirst_rc->right  = pfirst_rc->left + first_size;

		psecond_rc->left = pfirst_rc->right + space;
		psecond_rc->right = prc->right - margin;
	}
}

static void inline calc_vert_first_second_size(const RECT *prc,
		RECT *pfirst_rc, RECT *psecond_rc,
		int first_size, int margin, int space,
		BOOL breverse)
{

	pfirst_rc->left    = prc->left + margin;
	pfirst_rc->right   = prc->right - margin;
	psecond_rc->left   = pfirst_rc->left;
	psecond_rc->right  = pfirst_rc->right;

	if(breverse)
	{
		pfirst_rc->bottom  = prc->bottom - margin;
		pfirst_rc->top     = pfirst_rc->bottom - first_size;

		psecond_rc->top    = prc->top + margin;
		psecond_rc->bottom = pfirst_rc->top - space;
	}
	else
	{
		pfirst_rc->top     = prc->top + margin;
		pfirst_rc->bottom  = pfirst_rc->top + first_size;

		psecond_rc->top    = pfirst_rc->bottom + space;
		psecond_rc->bottom = prc->bottom - margin;
	}
}

static BOOL mPairPiece_setRect(mPairPiece *self, const RECT *prc)
{
	RECT first_rc;
	RECT second_rc;
	int first_size;
	int space;
	int margin;
	mHotPiece * first, * second;
	int total_size ;

	if(prc == NULL || (!self->first && !self->second))
		return FALSE;

	total_size = mPairPiece_isVert(self)?RECTHP(prc):RECTWP(prc);
	//calc the first rect of the two
	first_size = mPairPiece_getFirstSize(self);
	space = mPairPiece_getSpace(self);
	margin = mPairPiece_getMargin(self);

	switch(mPairPiece_getFirstSizeType(self))
	{
	case NCS_PAIRPIECE_ST_PERCENT:
		if(first_size == 0)
			first_size = total_size/2;
		else
			first_size = ((total_size - margin*2 - space) *  first_size) / 100;
		break;
	case NCS_PAIRPIECE_ST_AUTO:
		first_size = 0;
		if(self->first && _c(self->first)->getRect(self->first, &first_rc))
		{
			first_size = mPairPiece_isVert(self)?RECTH(first_rc):RECTW(first_rc);
			break;
		}
		//dono't break, continue trade 0
	case NCS_PAIRPIECE_ST_FIXED:
		if(first_size == 0)
			first_size = total_size/2;
		break;
	}

	if(mPairPiece_isVert(self))
	{
		calc_vert_first_second_size(prc,
				&first_rc,
				&second_rc,
				first_size,
				margin,
				space,
				mPairPiece_reversed(self));
	}
	else
	{
		calc_horz_first_second_size(prc,
				&first_rc,
				&second_rc,
				first_size,
				margin,
				space,
				mPairPiece_reversed(self));
	}

	first = mPairPiece_getFirst(self);
	second = mPairPiece_getSecond(self);

	if(first)
		_c(first)->setRect(first, &first_rc);

	if(second)
		_c(second)->setRect(second, &second_rc);

	return TRUE;
}

static BOOL mPairPiece_getRect(mPairPiece* self, PRECT prc)
{
	RECT first_rc, second_rc;
	int margin;

	if(!prc || (!self->first && !self->second))
		return FALSE;


	margin = mPairPiece_getMargin(self);
	memset(&first_rc, 0, sizeof(RECT));
	memset(&second_rc, 0, sizeof(RECT));

	if(self->first)
		_c(self->first)->getRect(self->first, &first_rc);

	if(self->second)
		_c(self->second)->getRect(self->second, &second_rc);

	*prc = first_rc;

	if(RECTW(second_rc) > 0 && RECTH(second_rc) > 0)
	{
		if(prc->left > second_rc.left)
			prc->left = second_rc.left;
		if(prc->right < second_rc.right)
			prc->right = second_rc.right;

		if(prc->top > second_rc.top)
			prc->top = second_rc.top;
		if(prc->bottom < second_rc.bottom)
			prc->bottom = second_rc.bottom;
	}

	if(RECTWP(prc) > 0 && RECTHP(prc) > 0)
	{
		if(margin > 0)
		{
			prc->left   -= margin;
			prc->top    -= margin;
			prc->right  += margin;
			prc->bottom += margin;
		}
		return TRUE;
	}

	return FALSE;
}

static void switch_first_second(mPairPiece *self)
{
	mHotPiece * first = self->first;
	mHotPiece * second = self->second;

	RECT rc_first, rc_second;
	int  size_first, size_second;
	int space;

	if(!first || !second)
		return ;

	if(!_c(first)->getRect(first, &rc_first) || !_c(second)->getRect(second, &rc_second))
		return ;

	space = mPairPiece_getSpace(self);
	if(mPairPiece_isVert(self))
	{
		size_first = RECTH(rc_first);
		size_second = RECTH(rc_second);
		if(rc_first.top > rc_second.top)
		{
			rc_first.top = rc_second.top;
			rc_first.bottom = rc_first.top + size_first;
			rc_second.top = rc_first.bottom + space;
			rc_second.bottom = rc_second.top + size_second;
		}
		else
		{
			rc_second.top = rc_first.top;
			rc_second.bottom = rc_second.top + size_second;
			rc_first.top = rc_second.bottom + space;
			rc_first.bottom = rc_first.top + size_first;
		}
	}
	else
	{
		size_first = RECTW(rc_first);
		size_second = RECTW(rc_second);

		if(rc_first.left > rc_second.left)
		{
			rc_first.left = rc_second.left;
			rc_first.right = rc_first.left + size_first;
			rc_second.left = rc_first.right + space;
			rc_second.right = rc_second.left + size_second;
		}
		else
		{
			rc_second.left = rc_first.left;
			rc_second.right = rc_second.left + size_second;
			rc_first.left = rc_second.right + space;
			rc_first.right = rc_first.left + size_first;
		}
	}

	_c(first)->setRect(first, &rc_first);
	_c(second)->setRect(second, &rc_second);

}

static BOOL mPairPiece_setProperty(mPairPiece *self, int id, DWORD value)
{
	mHotPiece *first, *second;

	switch(id)
	{
	case NCSP_PAIRPIECE_DIRECTION:
		if(value)
			mPairPiece_setVert(self);
		else
			mPairPiece_setHorz(self);
		return TRUE;

	case NCSP_PAIRPIECE_SECOND_AS_FIRST:
		if((value && mPairPiece_reversed(self))
			|| (! value && !mPairPiece_reversed(self)))
			return TRUE;
		if(value)
			mPairPiece_setReverse(self);
		else
			mPairPiece_cancelReverse(self);
		switch_first_second(self);
		return TRUE;
	case NCSP_PAIRPIECE_FIRST_SIZE_TYPE:
		mPairPiece_setFirstSizeType(self,value);
		return TRUE;
	case NCSP_PAIRPIECE_FIRST_SIZE:
		mPairPiece_setFirstSize(self, value);
		return TRUE;
	case NCSP_PAIRPIECE_SPACE:
		mPairPiece_setSpace(self, value);
		return TRUE;
	case NCSP_PAIRPIECE_MARGIN:
		mPairPiece_setMargin(self, value);
		return TRUE;
	case NCSP_PAIRPIECE_FIRST:
		mPairPiece_setFirst(self, (mHotPiece*)value);
		return TRUE;
	case NCSP_PAIRPIECE_SECOND:
		mPairPiece_setSecond(self, (mHotPiece*)value);
		return TRUE;
	}

	first = mPairPiece_getFirst(self);
	if(first && (_c(first)->setProperty(first, id, value)))
			return TRUE;

	second = mPairPiece_getSecond(self);
		if(second && _c(second)->setProperty(second, id, value))
			return TRUE;


	return Class(mLayoutPiece).setProperty((mLayoutPiece*)self, id, value);

}

static DWORD mPairPiece_getProperty(mPairPiece *self, int id)
{
	mHotPiece *first, *second;
	DWORD value;

	switch(id)
	{
	case NCSP_PAIRPIECE_DIRECTION:
		return mPairPiece_isVert(self);

	case NCSP_PAIRPIECE_SECOND_AS_FIRST:
		return mPairPiece_reversed(self);

	case NCSP_PAIRPIECE_FIRST_SIZE_TYPE:
		return mPairPiece_getFirstSizeType(self);

	case NCSP_PAIRPIECE_FIRST_SIZE:
		return mPairPiece_getFirstSize(self);

	case NCSP_PAIRPIECE_SPACE:
		return mPairPiece_getSpace(self);

	case NCSP_PAIRPIECE_MARGIN:
		return mPairPiece_getMargin(self);

	case NCSP_PAIRPIECE_FIRST:
		return (DWORD)mPairPiece_getFirst(self);

	case NCSP_PAIRPIECE_SECOND:
		return (DWORD)mPairPiece_getSecond(self);
	}

	first = mPairPiece_getFirst(self);
	if(first && (value = _c(first)->getProperty(first, id))!=-1)
		return value;

	second = mPairPiece_getSecond(self);
	if(second && (value = _c(second)->getProperty(second, id))!=-1)
		return value;

	return Class(mLayoutPiece).getProperty((mLayoutPiece*)self, id);
}

static void mPairPiece_paint(mPairPiece *self, HDC hdc, mWidget* owner, DWORD add_data)
{
	mHotPiece* first, *second;

	first = mPairPiece_getFirst(self);
	second = mPairPiece_getSecond(self);

	if(first)
		_c(first)->paint(first, hdc, (mObject*)owner, add_data);

	if(second)
		_c(second)->paint(second, hdc, (mObject*)owner, add_data);
}

static BOOL mPairPiece_autoSize(mPairPiece *self, mWidget *owner, const SIZE *pMinSize, const SIZE *pMaxSize)
{
	RECT rc={0,0,0,0};
	if(self->first ){
		if(mPairPiece_getFirstSizeType(self) == NCS_PAIRPIECE_ST_FIXED)
		{
			_c(self->first)->getRect(self->first, &rc);
			if(mPairPiece_isVert(self))
				rc.bottom = rc.top + mPairPiece_getFirstSize(self);
			else
				rc.right = rc.left + mPairPiece_getFirstSize(self);
			_c(self->first)->setRect(self->first, &rc);
		}
		else
		{
			_c(self->first)->autoSize(self->first, (mObject*)owner, pMinSize, pMaxSize);
			_c(self->first)->getRect(self->first, &rc);
		}
	}

	if(self->second){
		RECT rc_sec;
		if(mPairPiece_getFirstSizeType(self)== NCS_PAIRPIECE_ST_PERCENT )
		{
			int first_size = mPairPiece_getFirstSize(self);
			//cacl from first size
			if(mPairPiece_isVert(self))
			{
				rc_sec.left = rc.left;
				rc_sec.right = rc.right;
				rc_sec.top = rc.bottom + mPairPiece_getSpace(self);
				rc_sec.bottom = rc.top + (RECTH(rc)) * (100 - first_size) / first_size;
			}
			else
			{
				rc_sec.top = rc.top;
				rc_sec.bottom = rc.bottom;
				rc_sec.left = rc.right + mPairPiece_getSpace(self);
				rc_sec.right = rc.left + (RECTW(rc)) * (100 - first_size) / first_size;
			}
			_c(self->second)->setRect(self->second, &rc_sec);
		}
		else{
			int size;
			_c(self->second)->autoSize(self->second, (mObject*)owner, pMinSize, pMaxSize);
			_c(self->second)->getRect(self->second, &rc_sec);
			if(mPairPiece_isVert(self))
			{
				size = RECTH(rc_sec);
				rc_sec.top = rc.bottom +  mPairPiece_getSpace(self);
				rc_sec.bottom = rc_sec.top + size;
			}
			else
			{
				size = RECTW(rc_sec);
				rc_sec.left = rc.right +  mPairPiece_getSpace(self);
				rc_sec.right = rc_sec.left + size;
			}
			_c(self->second)->setRect(self->second, &rc_sec);
		}
	}

	return TRUE;
}

BEGIN_MINI_CLASS(mPairPiece, mLayoutPiece)
	CLASS_METHOD_MAP(mPairPiece, construct  )
	CLASS_METHOD_MAP(mPairPiece, destroy    )
	CLASS_METHOD_MAP(mPairPiece, setRenderer)
	CLASS_METHOD_MAP(mPairPiece, hitTest    )
	CLASS_METHOD_MAP(mPairPiece, setRect    )
	CLASS_METHOD_MAP(mPairPiece, getRect    )
	CLASS_METHOD_MAP(mPairPiece, setProperty)
	CLASS_METHOD_MAP(mPairPiece, getProperty)
	CLASS_METHOD_MAP(mPairPiece, paint    )
	CLASS_METHOD_MAP(mPairPiece, autoSize   )
END_MINI_CLASS


