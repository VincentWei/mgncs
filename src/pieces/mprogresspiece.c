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
