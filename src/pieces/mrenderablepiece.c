
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
