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
