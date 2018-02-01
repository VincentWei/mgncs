/* 
    This file is part of mGNCS, a component for MiniGUI.

    Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.
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
#include "mcontainerpiece.h"
#include "msliderpiece.h"

#if defined (_MGNCSCTRL_SCROLLBAR) || defined (_MGNCSCTRL_TRACKBAR)

#define SLPF_SET_LOGIC     0x01
#define SLPF_FORCE_UPDATE  0x02

static void mSliderPiece_construct(mSliderPiece *self, DWORD add_data)
{
	Class(mContainerPiece).construct((mContainerPiece*)self, add_data);
	self->line_step = 1;
	self->page_step = 3;
	self->min = 0;
	self->max = 10;
	self->cur_pos = 0,
	self->thumb_state = PIECE_STATE_NORMAL;
	self->flags = NCSS_SLIDERPIECE_MIN|NCSS_SLIDERPIECE_TICK;
}

static void mSliderPiece_destroy(mSliderPiece* self)
{
	DELPIECE(self->thumb);
	Class(mContainerPiece).destroy((mContainerPiece*)self);
}

static void set_logic_pos(mSliderPiece *self, int thumb_pos, int total_size/*=-1*/)
{
	if(self->min >= self->max)
		return;

	if(total_size <= 0)
	{
		RECT rc, rc_thumb;

		if(!(self->thumb
			&& _c(self)->getRect(self, &rc)
			&& _c(self->thumb)->getRect(self->thumb, &rc_thumb)))
			return ;

		if(mSliderPiece_isVert(self))
			total_size = RECTH(rc) - RECTH(rc_thumb);
		else
			total_size = RECTW(rc) - RECTW(rc_thumb);
	}

	if(thumb_pos >= total_size)
		self->cur_pos = self->max;
	else if(thumb_pos <= 0)
		self->cur_pos = self->min;
	else {
		//same as int((thumb_pos/total_size) * (self->max - self->min) + 0.5)
		self->cur_pos = ((((thumb_pos <<16) / total_size) * (self->max - self->min) + (1<<15)) >> 16) + self->min;
	}
}

static void _change_status(mSliderPiece *self)
{
    if (self->cur_pos <= self->min) {
        mSliderPiece_setMin(self);
        mSliderPiece_clearMax(self);
    }
    else if (self->cur_pos >= self->max) {
        mSliderPiece_setMax(self);
        mSliderPiece_clearMin(self);
    }
    else {
        mSliderPiece_clearMin(self);
        mSliderPiece_clearMax(self);
    }
}

static void set_thumb_pos(mSliderPiece *self, int x, int y, mWidget * owner, DWORD flags )
{
	RECT rc_thumb, rc_old;
	RECT rc;
	int size, min, max;
	int pos = 0;

	if(!self->thumb)
		return ;

	if(!_c(self->thumb)->getRect(self->thumb, &rc_thumb))
		return ;

	if(!_c(self)->getRect(self, &rc))
		return ;

	rc_old = rc_thumb;

	if(mSliderPiece_isVert(self))
	{
		size = RECTH(rc_thumb);
		min = rc.top + size/2;
		max = rc.bottom - size/2;

		if( y <= min){
			y = min;
		}
		else if (y >= max){
			y = max;
		}

		rc_thumb.top = y - size/2;
		rc_thumb.bottom = rc_thumb.top + size;
		if(rc_thumb.bottom > rc.bottom){
			rc_thumb.bottom = rc.bottom;
			rc_thumb.top = rc_thumb.bottom - size;
		}
		pos = rc_thumb.top - rc.top;

		if(flags&SLPF_SET_LOGIC)
			set_logic_pos(self, pos , RECTH(rc) - size);

        _change_status(self);

		size = RECTW(rc_thumb);
		rc_thumb.left = (rc.left + rc.right - size)/2;
		rc_thumb.right = rc_thumb.left + size;

		_c(self->thumb)->setRect(self->thumb, &rc_thumb);

	}
	else
	{
		size = RECTW(rc_thumb);
		min = rc.left + size/2;
		max = rc.right - size/2;

		if(x <= min){
			x = min;
		}
		else if (x >= max){
			x = max;
		}

		rc_thumb.left = x - size/2;
		rc_thumb.right = rc_thumb.left + size;
		if(rc_thumb.right > rc.right){
			rc_thumb.right = rc.right;
			rc_thumb.left = rc_thumb.right - size;
		}
		pos = rc_thumb.left - rc.left;

		if(flags&SLPF_SET_LOGIC)
			set_logic_pos(self, pos, RECTW(rc) - size);

        _change_status(self);
		size = RECTH(rc_thumb);
		rc_thumb.top = (rc.top + rc.bottom - size)/2;
		rc_thumb.bottom = rc_thumb.top + size;
		_c(self->thumb)->setRect(self->thumb, &rc_thumb);

	}

    if(self->min < self->max)
    {
        RAISE_EVENT(self, NCSN_SLIDERPIECE_POSCHANGED, (DWORD)self->cur_pos);
        if(mSliderPiece_isMin(self))
            RAISE_EVENT(self, NCSN_SLIDERPIECE_REACHMIN, self->min);
        else if(mSliderPiece_isMax(self))
            RAISE_EVENT(self, NCSN_SLIDERPIECE_REACHMAX, self->max);
    }

    if(owner){
#if 1
        RECT rc;
        GetBoundRect (&rc, &rc_old, &rc_thumb);
        InvalidateRect(owner->hwnd, &rc, TRUE);
#else
        InvalidateRect(owner->hwnd, &rc_old, TRUE);
        InvalidateRect(owner->hwnd, &rc_thumb, TRUE);
#endif 
    }
}

static void set_thumb_state(mSliderPiece *self, int new_state, int event, mWidget *owner)
{
	if(self->thumb_state == new_state)
		return;
	self->thumb_state = new_state;

	if(owner && new_state == PIECE_STATE_HILIGHT)
		mWidget_setHoveringFocus(owner, (mObject*)self);

	if(event!=0)
		RAISE_EVENT(self, event, 0);

	mHotPiece_update((mHotPiece*)self, (mObject*)owner, TRUE);
}

static void set_thumb_logic_pos(mSliderPiece *self, int pos, mWidget *owner);

static inline void thumb_inc(mSliderPiece *self, int offset, mWidget *owner)
{
	set_thumb_logic_pos(self, self->cur_pos + offset, owner);
}

static int mSliderPiece_processMessage(mSliderPiece *self,
		int message,
		WPARAM wParam,
		LPARAM lParam,
		mWidget *owner)
{
	int x, y;
	mHotPiece * thumb = self->thumb;
	if(!thumb)
		return 0;

	switch(message)
	{
	case MSG_LBUTTONDOWN:
		//hittest
		x = LOSWORD(lParam);
		y = HISWORD(lParam);
		if(_c(thumb)->hitTest(thumb, x, y) == thumb) //mouse on thumb
		{
			mWidget_captureHotPiece(owner, (mObject*)self);
			set_thumb_state(self, PIECE_STATE_PUSHED, 0, owner);
		}
		else
		{
			set_thumb_pos(self, x, y, owner, SLPF_SET_LOGIC);
		}
		break;
	case MSG_LBUTTONUP:
		x = LOSWORD(lParam);
		y = HISWORD(lParam);
		mWidget_releaseCapturedHotPiece();
		ScreenToClient(owner->hwnd, &x, &y);
		if(_c(thumb)->hitTest(thumb, x, y) == thumb)
			set_thumb_state(self, PIECE_STATE_HILIGHT, 0, owner);
		else
			set_thumb_state(self, PIECE_STATE_NORMAL, 0, owner);
		break;
	case MSG_MOUSEMOVE:
		x = LOSWORD(lParam);
		y = HISWORD(lParam);
		if(self->thumb_state == PIECE_STATE_CAPTURED
			|| self->thumb_state == PIECE_STATE_PUSHED)
		{
			ScreenToClient(owner->hwnd, &x, &y);
			if(_c(thumb)->hitTest(thumb, x, y) != thumb)
				set_thumb_state(self, PIECE_STATE_CAPTURED, 0, owner);
			else
				set_thumb_state(self, PIECE_STATE_PUSHED, 0, owner);
			set_thumb_pos(self, x, y, owner,SLPF_SET_LOGIC);
		}
		else
		{
			if(_c(thumb)->hitTest(thumb, x, y) == thumb){
				set_thumb_state(self, PIECE_STATE_HILIGHT, 0, owner);
			}
			else
			{
				set_thumb_state(self, PIECE_STATE_NORMAL, 0, owner);
			}
		}
		break;
	case MSG_MOUSEMOVEIN:
		if(!wParam) //mouse move out
		{
			set_thumb_state(self, PIECE_STATE_NORMAL, 0, owner);
		}
		break;
	case MSG_KEYDOWN:
		if(!mSliderPiece_isVert(self))
		{
			if(wParam == SCANCODE_CURSORBLOCKLEFT)
			{
				thumb_inc(self, -self->line_step, owner);
			}
			else if(wParam == SCANCODE_CURSORBLOCKRIGHT)
			{
				thumb_inc(self, self->line_step, owner);
			}
		}
		else
		{
			if(wParam == SCANCODE_CURSORBLOCKUP)
			{
				thumb_inc(self, -self->line_step, owner);
			}
			else if(wParam == SCANCODE_CURSORBLOCKDOWN)
			{
				thumb_inc(self, self->line_step, owner);
			}
		}
		if(wParam == SCANCODE_PAGEUP)
		{
			thumb_inc(self, -self->page_step, owner);
		}
		else if(wParam == SCANCODE_PAGEDOWN)
		{
			thumb_inc(self, self->page_step, owner);
		}
		break;
	}
	return 0;
}

static void mSliderPiece_paint(mSliderPiece *self, HDC hdc, mWidget * owner, DWORD add_data)
{
    DWORD newadd = 0L;

    HDC tmpDC = CreateCompatibleDC (hdc);

    /*
     * resave the add data for trackbar paint
     *
     * bits 0x0000FFFF  : the tick
     * bits 0x00FF0000  : horz or vert
     * bits 0xFF000000  : enable or disable
     */

    if (add_data & PIECE_STATE_DISABLE)
        newadd |= (PIECE_STATE_DISABLE << 24);
    if (mSliderPiece_isVert(self))
        newadd |= NCS_PIECE_PAINT_VERT;
    if (mSliderPiece_isTick(self))
        newadd |= (self->max - self->min)&0xFFFF;
    
    BitBlt (hdc, 0, 0, 0, 0, tmpDC, 0, 0, 0);
	Class(mContainerPiece).paint((mContainerPiece*)self,
            tmpDC, (mObject*)owner, newadd);
    BitBlt (tmpDC, 0, 0, 0, 0, hdc, 0, 0, 0);
    DeleteMemDC (tmpDC);

	if(self->thumb)
		_c(self->thumb)->paint(self->thumb, hdc, (mObject*)owner,
			add_data|(self->thumb_state)
			  |(mSliderPiece_isVert(self)?NCS_PIECE_PAINT_VERT:0));
}

static void recalc_thumb_logic_pos(mSliderPiece *self, mWidget * owner)
{
	int total_size ;
	int thumb_pos ;
	RECT rc, rc_thumb;
	int size;

	if(!(self->thumb
		&& _c(self)->getRect(self, &rc)
		&& _c(self->thumb)->getRect(self->thumb, &rc_thumb)))
		return ;

	if(self->cur_pos > self->max)
		self->cur_pos = self->max;
	else if(self->cur_pos < self->min)
		self->cur_pos = self->min;

	if(mSliderPiece_isVert(self))
	{
		size = RECTH(rc_thumb) ;
		total_size = RECTH(rc) - size;
		if(total_size <= 0)
			return;

		if(self->max <= self->min)
			thumb_pos = 0;
		else
			thumb_pos = (total_size * ((self->cur_pos - self->min) *100 / (self->max - self->min))) / 100;
		//printf("cur_pos=%d, min=%d, max=%d, total_size=%d, thumb_pos=%d\n", self->cur_pos, self->min, self->max, thumb_pos, total_size);
		set_thumb_pos(self, 0, thumb_pos + rc.top + size/2, owner, SLPF_FORCE_UPDATE);
	}
	else{
		size = RECTW(rc_thumb);
		total_size = RECTW(rc) - size;
		if(total_size <= 0)
			return ;

		if(self->max <= self->min)
			thumb_pos = 0;
		else
			thumb_pos  = (total_size * ((self->cur_pos -self->min) * 100 / (self->max - self->min))) / 100;
		//printf("cur_pos=%d, min=%d, max=%d, total_size=%d, thumb_pos=%d\n", self->cur_pos, self->min, self->max, thumb_pos, total_size);
		set_thumb_pos(self, thumb_pos + rc.left + size/2, 0, owner, SLPF_FORCE_UPDATE);
	}
}

static void set_thumb_logic_pos(mSliderPiece *self, int new_pos, mWidget *owner)
{

	if(self->min >= self->max)
		return;

	if(new_pos > self->max)
		new_pos = self->max;
	else if(new_pos < self->min)
		new_pos = self->min;

	if(self->cur_pos == new_pos)
		return;

	self->cur_pos = new_pos;

	recalc_thumb_logic_pos(self, owner);
}



static BOOL mSliderPiece_setProperty(mSliderPiece *self, int id, DWORD value)
{
	switch(id)
	{
	case NCSP_SLIDERPIECE_POS:
		{
			set_thumb_logic_pos(self, (int)value, NULL);
			return TRUE;
		}
	case NCSP_SLIDERPIECE_LINESTEP:
        if ((unsigned short)value == 0)
            return FALSE;

		self->line_step = value & 0xFFFF;
		return TRUE;

	case NCSP_SLIDERPIECE_PAGESTEP:
        if ((unsigned short)value == 0)
            return FALSE;

		self->page_step = value & 0xFFFF;
		return TRUE;

	case NCSP_SLIDERPIECE_MIN:
		if((unsigned short)value == self->min)
			return TRUE;
		if((unsigned short)value > self->max)
			self->min = self->max;
		else
			self->min = (unsigned short)value;
		recalc_thumb_logic_pos(self, NULL);
		return TRUE;
	case NCSP_SLIDERPIECE_MAX:
		if((unsigned short)value == self->max)
			return TRUE;
		if((unsigned short)value < self->min)
			self->max = self->min;
		else
			self->max = (unsigned short)value;
		recalc_thumb_logic_pos(self, NULL);
		return TRUE;
	case NCSP_SLIDERPIECE_TICK:
		if(value && !mSliderPiece_isTick(self))
			mSliderPiece_setTick(self);
		else if(value == 0 && mSliderPiece_isTick(self))
			mSliderPiece_clearTick(self);
		else
			return FALSE;
		return TRUE;
	case NCSP_SLIDERPIECE_DIRECTION:
		if(value)
			mSliderPiece_setVert(self);
		else
			mSliderPiece_setHorz(self);
		return TRUE;
	}
	return Class(mContainerPiece).setProperty((mContainerPiece*)self, id, value);
}

static DWORD mSliderPiece_getProperty(mSliderPiece *self, int id)
{
	switch(id)
	{
	case NCSP_SLIDERPIECE_POS:
		return self->cur_pos;
	case NCSP_SLIDERPIECE_LINESTEP:
		return self->line_step;
	case NCSP_SLIDERPIECE_PAGESTEP:
		return self->page_step;
	case NCSP_SLIDERPIECE_MAX:
		return self->max;
	case NCSP_SLIDERPIECE_MIN:
		return self->min;
	case NCSP_SLIDERPIECE_TICK:
		return mSliderPiece_isTick(self);

	case NCSP_SLIDERPIECE_DIRECTION:
		return mSliderPiece_isVert(self);

	}
	return Class(mContainerPiece).getProperty((mContainerPiece*)self, id);
}

static BOOL mSliderPiece_setRect(mSliderPiece *self, const RECT *prc)
{
	RECT rcThumb;
	if(self->thumb && mSliderPiece_isCenterThumb(self) && _c(self->thumb)->getRect(self->thumb, &rcThumb))
	{
		RECT rcbar = *prc;
		if(mSliderPiece_isVert(self))
		{
			if(RECTH(rcbar) < RECTH(rcThumb))
			{
				rcbar.top = rcbar.bottom = (rcThumb.top + rcThumb.bottom) / 2;
			}
			else {
				rcbar.top += RECTH(rcThumb) / 2;
				rcbar.bottom -= RECTH(rcThumb) / 2;
			}
		}
		else {
			if(RECTW(rcbar) < RECTW(rcThumb))
			{
				rcbar.left = rcbar.right = (rcThumb.left + rcThumb.right) / 2;
			}
			else {
				rcbar.left += RECTW(rcThumb) / 2;
				rcbar.right -= RECTW(rcThumb) / 2;
			}
		}
		Class(mContainerPiece).setRect((mContainerPiece*)self, &rcbar);
	}
	else
	{
		Class(mContainerPiece).setRect((mContainerPiece*)self, prc);
	}

	//calc the thumb rect
	recalc_thumb_logic_pos(self, NULL);

	return TRUE;
}

static BOOL mSliderPiece_getRect(mSliderPiece* self, RECT *prc)
{
	RECT rcThumb;
	BOOL bret;

	bret = Class(mContainerPiece).getRect((mContainerPiece*)self, prc);
	if(bret && self->thumb && mSliderPiece_isCenterThumb(self) && _c(self->thumb)->getRect(self->thumb, &rcThumb))
	{
		if(mSliderPiece_isVert(self))
		{
			prc->top -= RECTH(rcThumb) / 2;
			prc->bottom += RECTH(rcThumb) / 2;
		}
		else {
			prc->left -= RECTW(rcThumb) / 2;
			prc->right += RECTW(rcThumb) / 2;
		}
	}
	return bret;
}

static BOOL mSliderPiece_setRenderer(mSliderPiece *self, const char* rdr_name)
{
	Class(mContainerPiece).setRenderer((mContainerPiece*)self, rdr_name);

	if(self->thumb)
		_c(self->thumb)->setRenderer(self->thumb, rdr_name);

	return TRUE;
}

int mSliderPiece_stepLine(mSliderPiece* self, BOOL forward)
{
	thumb_inc(self, forward?-self->line_step:self->line_step, NULL);
	return self->cur_pos;
}

int mSliderPiece_stepPage(mSliderPiece* self, BOOL forward)
{
	thumb_inc(self, forward?-self->page_step:self->page_step, NULL);
	return self->cur_pos;
}

BEGIN_MINI_CLASS(mSliderPiece, mContainerPiece)
	CLASS_METHOD_MAP(mSliderPiece, construct )
	CLASS_METHOD_MAP(mSliderPiece, destroy   )
	CLASS_METHOD_MAP(mSliderPiece, setProperty )
	CLASS_METHOD_MAP(mSliderPiece, getProperty )
	CLASS_METHOD_MAP(mSliderPiece, paint       )
	CLASS_METHOD_MAP(mSliderPiece, processMessage )
	CLASS_METHOD_MAP(mSliderPiece, setRenderer    )
	CLASS_METHOD_MAP(mSliderPiece, setRect    )
	CLASS_METHOD_MAP(mSliderPiece, stepLine   )
	CLASS_METHOD_MAP(mSliderPiece, stepPage   )
	CLASS_METHOD_MAP(mSliderPiece, getRect    )
END_MINI_CLASS

#endif //_MGNCSCTRL_SCROLLBAR _MGNCSCTRL_TRACKBAR
