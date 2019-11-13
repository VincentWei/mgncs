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
#include "mimagepiece.h"

static void mImagePiece_construct(mImagePiece *self, DWORD add_data)
{
	Class(mStaticPiece).construct((mStaticPiece*)self, add_data);

	ncsInitDrawInfo(&self->img);
	mImagePiece_setAlign(self, NCS_ALIGN_CENTER);
	mImagePiece_setVAlign(self, NCS_VALIGN_CENTER);
}

static void mImagePiece_destroy(mImagePiece *self)
{
	ncsCleanImageDrawInfo(&self->img);
	Class(mStaticPiece).destroy((mStaticPiece*)self);
}

static void mImagePiece_paint(mImagePiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	//draw Text
	RECT rc;
	_c(self)->getRect(self, &rc);
	ncsImageDrawInfoDraw(&self->img, hdc,  &rc, mImagePiece_getAlign(self), mImagePiece_getVAlign(self));
}

static BOOL mImagePiece_setProperty(mImagePiece *self, int id, DWORD value)
{
	unsigned char revert;
	switch(id)
	{
	case NCSP_IMAGEPIECE_IMAGE:
		revert = self->img.revert;
		ncsSetImageDrawInfo(&self->img, (void*)value, self->img.drawMode, IMG_TYPE_BITMAP);
		self->img.revert = revert;
		break;
	case NCSP_IMAGEPIECE_ALIGN:
		mImagePiece_setAlign(self, value);
		break;
	case NCSP_IMAGEPIECE_VALIGN:
		mImagePiece_setVAlign(self, value);
		break;
	case NCSP_IMAGEPIECE_ICON:
		revert = self->img.revert;
		ncsSetImageDrawInfo(&self->img, (void*)value, self->img.drawMode, IMG_TYPE_ICON);
		self->img.revert = revert;
		break;
	case NCSP_IMAGEPIECE_MYBITMAP:
		revert = self->img.revert;
		ncsSetImageDrawInfo(&self->img, (void*)value, self->img.drawMode, IMG_TYPE_MYBITMAP);
		self->img.revert = revert;
		break;
	case NCSP_IMAGEPIECE_IMAGEFILE:
		revert = self->img.revert;
		ncsSetImageDrawInfoByFile(&self->img, (const char*)value, self->img.drawMode, FALSE);
		self->img.revert = revert;
		break;
	case NCSP_IMAGEPIECE_DRAWMODE:
		self->img.drawMode = value&0xFF;
		break;
	default:
		return Class(mStaticPiece).setProperty((mStaticPiece*)self, id, value);
	}
	return TRUE;
}

static DWORD mImagePiece_getProperty(mImagePiece* self, int id)
{
	switch(id)
	{
	case NCSP_IMAGEPIECE_IMAGE:
		if(self->img.img_type == IMG_TYPE_BITMAP)
			return (DWORD)self->img.img.pbmp;
		return (DWORD)-1;
	case NCSP_IMAGEPIECE_ALIGN:
		return mImagePiece_getAlign(self);
	case NCSP_IMAGEPIECE_VALIGN:
		return mImagePiece_getVAlign(self);
	case NCSP_IMAGEPIECE_ICON:
		if(self->img.img_type == IMG_TYPE_ICON)
			return (DWORD)self->img.img.hIcon;
		return (DWORD)-1;
	case NCSP_IMAGEPIECE_MYBITMAP:
		if(self->img.img_type == IMG_TYPE_MYBITMAP)
			return (DWORD)self->img.img.pmybmp;
		return (DWORD)-1;
	case NCSP_IMAGEPIECE_DRAWMODE:
		return self->img.drawMode;
	}

	return Class(mStaticPiece).getProperty((mStaticPiece*)self, id);
}

 static BOOL mImagePiece_autoSize(mImagePiece *self, mWidget *owner, const SIZE *pszMin, const SIZE *pszMax)
{
	SIZE size;
	if(!owner)
		return FALSE;

	if(!ncsImageDrawInfoGetImageSize(&self->img, &size.cx, &size.cy))
		return FALSE;

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

BEGIN_MINI_CLASS(mImagePiece, mStaticPiece)
	CLASS_METHOD_MAP(mImagePiece, construct)
	CLASS_METHOD_MAP(mImagePiece, destroy)
	CLASS_METHOD_MAP(mImagePiece, paint)
	CLASS_METHOD_MAP(mImagePiece, setProperty)
	CLASS_METHOD_MAP(mImagePiece, getProperty)
	CLASS_METHOD_MAP(mImagePiece, autoSize)
END_MINI_CLASS

