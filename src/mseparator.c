
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

static mObject* mSeparator_createBody(mSeparator * self)
{
	return (mObject*)NEWPIECE(mSeparatorPiece);
}

static void mSeparator_onPaint(mSeparator *self, HDC hdc, const CLIPRGN *inv)
{
	if(Body)
		_c(Body)->paint(Body, hdc, (mObject*)self, GetWindowStyle(self->hwnd)&NCSS_SPRTR_VERT?NCS_PIECE_PAINT_VERT:0);
}

BEGIN_CMPT_CLASS(mSeparator, mStatic)
	CLASS_METHOD_MAP(mSeparator, createBody)
	CLASS_METHOD_MAP(mSeparator, onPaint)
END_CMPT_CLASS

