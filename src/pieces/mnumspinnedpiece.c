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
#include "mnumspinnedpiece.h"

static void mNumSpinnedPiece_construct(mNumSpinnedPiece *self, DWORD add_data)
{
	Class(mLabelPiece).construct((mLabelPiece*)self, add_data);

	self->str = (const char*)malloc(32);
	((char*)(self->str))[0] = 0;
}

static void mNumSpinnedPiece_destroy(mNumSpinnedPiece *self)
{
	Class(mLabelPiece).destroy((mLabelPiece*)self);

	if(self->str)
		free((void*)(self->str));
}

static BOOL mNumSpinnedPiece_setProperty(mNumSpinnedPiece *self, int id, DWORD value)
{
	switch(id)
	{
	case NCSP_LABELPIECE_LABEL:
		return FALSE;
	case PIECECOMM_PROP_POS:
		sprintf((char*)self->str, "%d", (int)value);
		return TRUE;
	}
	return Class(mLabelPiece).setProperty((mLabelPiece*)self, id, value);
}

BEGIN_MINI_CLASS(mNumSpinnedPiece, mLabelPiece)
	CLASS_METHOD_MAP(mNumSpinnedPiece, construct)
	CLASS_METHOD_MAP(mNumSpinnedPiece, destroy)
	CLASS_METHOD_MAP(mNumSpinnedPiece, setProperty)
END_MINI_CLASS

