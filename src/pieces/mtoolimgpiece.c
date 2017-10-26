#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncsconfig.h"
#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#include "mwidget.h"
#include "mrdr.h"

#include "mhotpiece.h"
#include "mstaticpiece.h"
#include "mtoolimgpiece.h"

static void mToolImagePiece_construct(mToolImagePiece *self, DWORD add_data)
{
	Class(mStaticPiece).construct((mStaticPiece*)self, add_data);

	self->toolImg = (mToolImage*)add_data;
	if(self->toolImg)
		self->toolImg->ref ++;
}

static void mToolImagePiece_destroy(mToolImagePiece *self)
{
	if((--(self->toolImg->ref)) == 0){
		ncsFreeToolImage(self->toolImg);
	}
	Class(mStaticPiece).destroy((mStaticPiece*)self);
}

static void mToolImagePiece_paint(mToolImagePiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	RECT rc;
	int idx = (int)add_data;
	if(idx >= self->toolImg->cell_count)
			idx = self->toolImg->cell_count - 1;

	_c(self)->getRect(self, &rc);
	//printf("ToolImage RC:%d,%d,%d,%d\n",rc.left, rc.top, rc.right, rc.bottom);

	//draw part image
	ncsDrawToolImageCell(self->toolImg, hdc, idx, &rc);
}

static BOOL mToolImagePiece_setProperty(mToolImagePiece *self, int id, DWORD value)
{
	switch(id)
	{
	case NCSP_TOOLIMAGEPIECE_TOOLIMAGE:
		if(self->toolImg != (mToolImage*)value)
		{
			if(self->toolImg && (--(self->toolImg->ref))==0)
				ncsFreeToolImage(self->toolImg);
			self->toolImg = (mToolImage*)value;
			if(self->toolImg)
				self->toolImg->ref ++;
			return TRUE;
		}
		return FALSE;

	default:
		return Class(mStaticPiece).setProperty((mStaticPiece*)self, id, value);
	}
	return TRUE;
}

static DWORD mToolImagePiece_getProperty(mToolImagePiece* self, int id)
{
	switch(id)
	{
	case NCSP_TOOLIMAGEPIECE_TOOLIMAGE:
		return (DWORD)self->toolImg;
	}

	return Class(mStaticPiece).getProperty((mStaticPiece*)self, id);
}

 static BOOL mToolImagePiece_autoSize(mToolImagePiece *self, mWidget *owner, const SIZE *pszMin, const SIZE *pszMax)
{
	SIZE size;
	if(!owner)
		return FALSE;

	if(!self->toolImg || !self->toolImg->pbmp)
		return FALSE;

	if(self->toolImg->flags & TOOLIMGF_VERTCELL)
	{
		size.cx = self->toolImg->pbmp->bmWidth;
		size.cy = self->toolImg->pbmp->bmHeight / self->toolImg->cell_count;
	}
	else
	{
		size.cx = self->toolImg->pbmp->bmWidth / self->toolImg->cell_count;
		size.cy = self->toolImg->pbmp->bmHeight;
	}

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

BEGIN_MINI_CLASS(mToolImagePiece, mStaticPiece)
	CLASS_METHOD_MAP(mToolImagePiece, construct)
	CLASS_METHOD_MAP(mToolImagePiece, destroy)
	CLASS_METHOD_MAP(mToolImagePiece, paint)
	CLASS_METHOD_MAP(mToolImagePiece, setProperty)
	CLASS_METHOD_MAP(mToolImagePiece, getProperty)
	CLASS_METHOD_MAP(mToolImagePiece, autoSize)
END_MINI_CLASS

