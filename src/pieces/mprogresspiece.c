
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
#include "mrenderablepiece.h"
#include "mprogresspiece.h"

#ifdef _MGNCSCTRL_PROGRESSBAR

static void mProgressPiece_construct(mProgressPiece *self, DWORD add_data)
{
	Class(mRenderablePiece).construct((mRenderablePiece*)self, add_data);

	self->max = 100;
	self->min = 0;
	self->cur = 20;
	self->step = 10;
}

static void adjust_pos(mProgressPiece *self)
{
	if(self->min > self->max)
		self->min = self->max;
	if(self->cur > self->max)
		self->cur = self->max;
	else if(self->cur < self->min)
		self->cur = self->min;
}

static BOOL mProgressPiece_setProperty(mProgressPiece *self, int id, DWORD value)
{
	switch(id)
	{
	case NCSP_PROGRESSPIECE_MAXPOS:
        if ((int)value <= self->min)
            return FALSE;

		self->max = (int)value;
		adjust_pos(self);
		return TRUE;

	case NCSP_PROGRESSPIECE_MINPOS:
        if ((int)value >= self->max)
            return FALSE;

		self->min = (int)value;
		adjust_pos(self);
		return TRUE;

	case NCSP_PROGRESSPIECE_LINESTEP:
    {
        int maxStep;

        if ((int)value == 0)
            return FALSE;

        maxStep = self->max - self->min;
        if ((int)value > maxStep)
            self->step = maxStep;
        else if ((int)value < -maxStep)
            self->step = -maxStep;
        else
            self->step = (int)value;
		return TRUE;
    }

	case NCSP_PROGRESSPIECE_CURPOS:
		self->cur = (int)value;
		adjust_pos(self);
		return TRUE;
	}

	return Class(mRenderablePiece).setProperty((mRenderablePiece*)self, id, value);
}

static DWORD mProgressPiece_getProperty(mProgressPiece *self, int id)
{
	switch(id)
	{
	case NCSP_PROGRESSPIECE_MAXPOS:
		return self->max;
	case NCSP_PROGRESSPIECE_MINPOS:
		return self->min;
	case NCSP_PROGRESSPIECE_LINESTEP:
		return self->step;
	case NCSP_PROGRESSPIECE_CURPOS:
		return self->cur;
	}

	return Class(mRenderablePiece).getProperty((mRenderablePiece*)self, id);
}

static int mProgressPiece_step(mProgressPiece *self, int offset, BOOL dec)
{
	if(offset == 0)
		offset = dec?-self->step:self->step;

	self->cur += offset;
	adjust_pos(self);
	return self->cur;
}


BEGIN_MINI_CLASS(mProgressPiece, mRenderablePiece)
	CLASS_METHOD_MAP(mProgressPiece, construct)
	CLASS_METHOD_MAP(mProgressPiece, setProperty)
	CLASS_METHOD_MAP(mProgressPiece, getProperty)
	CLASS_METHOD_MAP(mProgressPiece, step)
END_MINI_CLASS

#endif //_MGNCSCTRL_PROGRESSBAR
