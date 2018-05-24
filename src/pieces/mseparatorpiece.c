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
#include "mseparatorpiece.h"

static inline void line(HDC hdc, int x0,int y0, int x1, int y1, gal_pixel color)
{
	gal_pixel old = SetPenColor(hdc, color);
	MoveTo(hdc, x0, y0);
	LineTo(hdc, x1, y1);
	SetPenColor(hdc, old);
}

static void mSeparatorPiece_paint(mSeparatorPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	gal_pixel darker_pixel;
	gal_pixel lighter_pixel;
	{
		DWORD bkcolor = GetWindowElementAttr (owner->hwnd, WE_FGC_ACTIVE_WND_BORDER);
		DWORD darkercolor  = ncsCommRDRCalc3dboxColor(bkcolor, NCSR_COLOR_DARKER);
		DWORD lightercolor = ncsCommRDRCalc3dboxColor(bkcolor, NCSR_COLOR_LIGHTER);
		darker_pixel  = ncsColor2Pixel(hdc, darkercolor);
		lighter_pixel = ncsColor2Pixel(hdc, lightercolor);
	}
	//get color
	if(NCS_PIECE_PAINT_VERT&add_data)
	{
		int x = (self->left+self->right)/2;
		line(hdc, x, self->top+1, x, self->bottom-1, darker_pixel);
		line(hdc, x+1, self->top+1, x+1, self->bottom-1, lighter_pixel);
	}
	else
	{
		int y = (self->top + self->bottom)/2;
		line(hdc, self->left+1, y, self->right-1, y, darker_pixel);
		line(hdc, self->left+1, y+1, self->right-1, y+1, lighter_pixel);
	}
}

static BOOL mSeparatorPiece_autoSize(mSeparatorPiece *self, mWidget *owner, const SIZE *pszMin, const SIZE *pszMax)
{
	int def_cx = 12;
	int def_cy = 12;

	if(pszMin)
	{
		if(pszMin->cx > def_cx)
			def_cx = pszMin->cx;
		if(pszMin->cy > def_cy)
			def_cy = pszMin->cy;
	}
	if(pszMax)
	{
		if(pszMax->cx < def_cx)
			def_cx = pszMax->cx;
		if(pszMax->cy < def_cy)
			def_cy = pszMax->cy;
	}

	self->right = self->left + def_cx;
	self->bottom = self->top + def_cy;

	return TRUE;
}

BEGIN_MINI_CLASS(mSeparatorPiece, mStaticPiece)
	CLASS_METHOD_MAP(mSeparatorPiece, paint)
	CLASS_METHOD_MAP(mSeparatorPiece, autoSize)
END_MINI_CLASS

