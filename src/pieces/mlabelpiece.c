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
#include "mlabelpiece.h"

static void mLabelPiece_construct(mLabelPiece *self, DWORD add_data)
{
	Class(mHotPiece).construct((mHotPiece*)self, add_data);
	self->str = (const char*)add_data;
	self->align = NCS_ALIGN_CENTER;
	self->valign = NCS_VALIGN_CENTER;
	mLabelPiece_setAutoWrap(self, 0);
}

static void mLabelPiece_paint(mLabelPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	//draw Text
    RECT rcClient;
    DWORD uFormat = DT_WORDBREAK;
	DWORD color = 0;
	//gal_pixel old_color;
    const char* str = self->str;


    if(owner == NULL || str == NULL)
        return ;

    SelectFont(hdc, GetWindowFont(owner->hwnd));
	_c(self)->getRect(self, &rcClient);

    if ((NCS_PIECE_PAINT_STATE_MASK&add_data) == PIECE_STATE_DISABLE) //is disable
        color = ncsGetElement(owner, NCS_FGC_DISABLED_ITEM);
    else
	{
		if(mLabelPiece_isSelected(self))
		{
			color = ncsGetElement(owner, NCS_FGC_SELECTED_ITEM);
		}
		else
		{
			color = ncsGetElement(owner, NCS_FGC_3DBODY);
		}
	}

	/* old_color = */SetTextColor(hdc, ncsColor2Pixel(hdc, color));

    if(self->align == NCS_ALIGN_CENTER)
        uFormat |= DT_CENTER ;
    else if(self->align == NCS_ALIGN_RIGHT)
        uFormat |= DT_RIGHT ;
    else
        uFormat |= DT_LEFT;

    if(self->valign == NCS_VALIGN_CENTER)
        uFormat |= DT_VCENTER ;
    else if(self->valign == NCS_VALIGN_BOTTOM)
        uFormat |= DT_BOTTOM ;
    else
        uFormat |= DT_TOP;

    if(!mLabelPiece_isAutoWrap(self))
        uFormat |= DT_SINGLELINE;

    if(!mLabelPiece_isPrefix(self))
        uFormat |= DT_NOPREFIX;
/*
 * for vertical text
 */	
	if(mLabelPiece_isWordBreak(self))
		uFormat |= DT_WORDBREAK;

    SetBkMode(hdc, BM_TRANSPARENT);
    
	DrawText(hdc, str, -1, &rcClient, uFormat);
}

static BOOL mLabelPiece_setProperty(mLabelPiece *self, int id, DWORD value)
{
	switch(id)
	{
	case NCSP_LABELPIECE_LABEL:
		self->str = (const char*)value;
		break;
	case NCSP_LABELPIECE_SELECT:
		mLabelPiece_setSelected(self, value);
		break;
	case NCSP_LABELPIECE_ALIGN:
		self->align = (char)(value&0xFF);
		break;
	case NCSP_LABELPIECE_VALIGN:
		self->valign = (char)(value&0xFF);
		break;
	case NCSP_LABELPIECE_AUTOWRAP:
		mLabelPiece_setAutoWrap(self, value);
		break;
	case NCSP_LABELPIECE_PREFIX:
		mLabelPiece_setPrefix(self, value);
		break;
	case NCSP_LABELPIECE_WORDBREAK:
		mLabelPiece_setWordBreak(self, value);
		break;
	default:
		return Class(mStaticPiece).setProperty((mStaticPiece*)self, id, value);
	}
	return TRUE;
}

static DWORD mLabelPiece_getProperty(mLabelPiece* self, int id)
{
	switch(id)
	{
	case NCSP_LABELPIECE_LABEL:
		return (DWORD)self->str;
	case NCSP_LABELPIECE_SELECT:
		return mLabelPiece_isSelected(self);
	case NCSP_LABELPIECE_ALIGN:
		return self->align;
	case NCSP_LABELPIECE_VALIGN:
		return self->valign;
	case NCSP_LABELPIECE_AUTOWRAP:
		return mLabelPiece_isAutoWrap(self);
	case NCSP_LABELPIECE_PREFIX:
		return mLabelPiece_isPrefix(self);
	case NCSP_LABELPIECE_WORDBREAK:
		return mLabelPiece_isWordBreak(self);
	}

	return Class(mStaticPiece).getProperty((mStaticPiece*)self, id);
}

static BOOL mLabelPiece_autoSize(mLabelPiece* self, mWidget *owner, const SIZE *pszMin, const SIZE *pszMax)
{
	SIZE size;
	HDC hdc;
	if(!owner || !self->str)
		return FALSE;

	hdc = GetClientDC(owner->hwnd);
	GetTextExtent(hdc, self->str, strlen(self->str),&size);
	ReleaseDC(hdc);

	if(pszMin)
	{
		if(size.cx < pszMin->cx)
			size.cx = pszMin->cx;
		if(size.cy < pszMin->cy)
			size.cy = pszMin->cy;
	}

	if(pszMax)
	{
		if(size.cx > pszMax->cx)
			size.cx = pszMax->cx;
		if(size.cy > pszMax->cy)
			size.cy = pszMax->cy;
	}
	
	self->right = self->left + size.cx;
	self->bottom = self->top + size.cy;

	return TRUE;
}

BEGIN_MINI_CLASS(mLabelPiece, mStaticPiece)
	CLASS_METHOD_MAP(mLabelPiece, construct)
	CLASS_METHOD_MAP(mLabelPiece, paint)
	CLASS_METHOD_MAP(mLabelPiece, setProperty)
	CLASS_METHOD_MAP(mLabelPiece, getProperty)
	CLASS_METHOD_MAP(mLabelPiece, autoSize)
END_MINI_CLASS

