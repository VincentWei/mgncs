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
#include "manimateframes.h"
#include "manimatepiece.h"
#include "mgifanimateframes.h"
#include "mbmparrayanimateframes.h"
#include "mmemanimateframes.h"

#ifdef _MGNCSCTRL_ANIMATE

static void mAnimatePiece_construct(mAnimatePiece *self, DWORD add_data)
{
	Class(mStaticPiece).construct((mStaticPiece*)self, add_data);
	self->align = NCS_ALIGN_CENTER;
	self->valign = NCS_VALIGN_CENTER;
	self->frame = NULL;
}

static void mAnimatePiece_destroy(mAnimatePiece *self)
{
	DELETE(self->frame);
	Class(mStaticPiece).destroy((mStaticPiece*)self);
}

static void mAnimatePiece_paint(mAnimatePiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	RECT rc;
	if(self->frame) {
		_c(self)->getRect(self, &rc);
		_c(self->frame)->drawFrame(self->frame, hdc, (mObject*)owner, &rc, self->align, self->valign, mAnimatePiece_isScale(self));

		if(add_data && (mAnimatePiece_isPlay(self) || mAnimatePiece_isAutoplay(self)))
		{
			int ret;
			ret = _c(self->frame)->nextFrame(self->frame);
			if(add_data && !mAnimatePiece_isAutoloop(self) && ret == NCSR_ANIMATEFRAME_LASTFRAME)
			{
				mAnimatePiece_setPlay(self, FALSE);
				mAnimatePiece_setAutoplay(self, FALSE);
			}
		}
	}
}

static void mAnimatePiece_autofit(mAnimatePiece *self, int *width, int *height)
{
	int max_width = 0, max_height = 0;
	RECT rc;
	_c(self)->getRect(self, &rc);

	if(self->frame){
		_c(self->frame)->getMaxFrameSize(self->frame, &max_width, &max_height);
	}

	if(width)
		*width = max_width;
	if(height)
		*height = max_height;
}

static void mAnimatePiece_reset(mAnimatePiece *self)
{
	if (self->frame == NULL)
		return;
	self->frame->cur_frame = NULL;
	mAnimatePiece_setPlay(self, 1);
}

static BOOL set_new_frame(mAnimatePiece*self, mAnimateFrames* frame)
{
	if(!frame)
		return FALSE;
	DELETE(self->frame);
	self->frame = frame;
	return self->frame != NULL;
}

static inline void clean_frame(mAnimatePiece* self)
{
	DELETE(self->frame);
	self->frame = NULL;
}

static BOOL mAnimatePiece_setProperty(mAnimatePiece *self, int id, DWORD value)
{
	switch(id)
	{
	case NCSP_ANIMATEPIECE_MEM:
		if(!value)
		{
			clean_frame(self);
			return TRUE;
		}
		return set_new_frame(self, (mAnimateFrames*)ncsCreateAnimateFramesFromGIFMem((const void*)value, 1));
	case NCSP_ANIMATEPIECE_GIFFILE:
		if(!value)
		{
			clean_frame(self);
			return TRUE;
		}
		return set_new_frame(self, (mAnimateFrames*)ncsCreateAnimateFramesFromGIFFile((const char*)value));
	case NCSP_ANIMATEPIECE_DIR:
		if(!value)
		{
			clean_frame(self);
			return TRUE;
		}
		return set_new_frame(self, (mAnimateFrames*)ncsCreateAnimateFramesFromDIR((const char*)value));
	case NCSP_ANIMATEPIECE_AUTOLOOP:
		mAnimatePiece_setAutoloop(self, value);
		return TRUE;
	case NCSP_ANIMATEPIECE_BMPARRAY:
		if(!value) {
			clean_frame(self);
			return TRUE;
		}
		return set_new_frame(self, (mAnimateFrames*)ncsCreateAnimateFramesFromBmpArray((PBITMAP*)value, 0, FALSE));
	case NCSP_ANIMATEPIECE_AFRAME:
		if(!value) {
			clean_frame(self);
			return TRUE;
		}
		self->frame = (mAnimateFrames*)value;
		return TRUE;
	case NCSP_ANIMATEPIECE_SCALE:
		mAnimatePiece_setScale(self, value);
		return TRUE;
	case NCSP_ANIMATEPIECE_AUTOFIT:
		mAnimatePiece_setAutofit(self, value);
		return TRUE;
	case NCSP_ANIMATEPIECE_AUTOPLAY:
		mAnimatePiece_setAutoplay(self, value);
		return TRUE;
	case NCSP_ANIMATEPIECE_ALIGN:
		self->align = value & 3;
		if(self->align > NCS_ALIGN_CENTER)
			self->align = NCS_ALIGN_CENTER;
		return TRUE;
	case NCSP_ANIMATEPIECE_VALIGN:
		self->valign = value & 3;
		if(self->valign > NCS_VALIGN_CENTER)
			self->valign = NCS_VALIGN_CENTER;
		return TRUE;
	default:
		return Class(mStaticPiece).setProperty((mStaticPiece*)self, id, value);
	}
	return TRUE;
}

static DWORD mAnimatePiece_getProperty(mAnimatePiece* self, int id)
{
	switch(id)
	{
	case NCSP_ANIMATEPIECE_AUTOLOOP:
		return mAnimatePiece_isAutoloop(self);
	case NCSP_ANIMATEPIECE_SCALE:
		return mAnimatePiece_isScale(self);
	case NCSP_ANIMATEPIECE_AUTOPLAY:
		return mAnimatePiece_isAutoplay(self);
	case NCSP_ANIMATEPIECE_AUTOFIT:
		return mAnimatePiece_isAutofit(self);
	case NCSP_ANIMATEPIECE_ALIGN:
		return self->align;
	case NCSP_ANIMATEPIECE_VALIGN:
		return self->valign;
	}
	return Class(mStaticPiece).getProperty((mStaticPiece*)self, id);
}


BEGIN_MINI_CLASS(mAnimatePiece, mStaticPiece)
	CLASS_METHOD_MAP(mAnimatePiece, construct)
	CLASS_METHOD_MAP(mAnimatePiece, destroy)
	CLASS_METHOD_MAP(mAnimatePiece, paint)
	CLASS_METHOD_MAP(mAnimatePiece, reset)
	CLASS_METHOD_MAP(mAnimatePiece, autofit)
	CLASS_METHOD_MAP(mAnimatePiece, getProperty)
	CLASS_METHOD_MAP(mAnimatePiece, setProperty)
END_MINI_CLASS

#endif		//_MGNCSCTRL_ANIMATE
