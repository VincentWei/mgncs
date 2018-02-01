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
#include "mspinnerpiece.h"

#include "mlayoutpiece.h"
#include "mpairpiece.h"

#include "mabstractbuttonpiece.h"
#include "marrowbuttonpiece.h"

#ifdef _MGNCSCTRL_SPINNER

#define IncPiece  (_c(self)->getIncPiece(self))
#define DecPiece  (_c(self)->getDecPiece(self))

static inline void update_inc_dec(mSpinnerPiece* self)
{
	if(!IncPiece || !DecPiece)
		return ;
    if (self->loop_flag == 1){
        _c(DecPiece)->enable(DecPiece, TRUE);
        _c(IncPiece)->enable(IncPiece, TRUE);
    } else {
        _c(DecPiece)->enable(DecPiece, self->cur_pos != (self->line_step > 0 ? self->min : self->max));
        _c(IncPiece)->enable(IncPiece, self->cur_pos != (self->line_step > 0 ? self->max : self->min));
    }
}

static mHotPiece* mSpinnerPiece_getIncPiece(mSpinnerPiece *self)
{
	if (self->body)
	{
		mPairPiece *p = (mPairPiece *)self->body; 
		if (mPairPiece_isVert(p)){
			return p->first;
		} else {
			return p->second;
		}
	}
	return NULL;
	//return self->body ? ((mPairPiece*)(self->body))->second : NULL;
}

static mHotPiece* mSpinnerPiece_getDecPiece(mSpinnerPiece *self)
{
	if (self->body)
	{
		mPairPiece *p = (mPairPiece *)self->body; 
		if (mPairPiece_isVert(p)){
			return p->second;
		} else {
			return p->first;
		}
	}
	return NULL;
	//return self->body ? ((mPairPiece*)(self->body))->first : NULL;
}


static BOOL mSpinnerPiece_onIncDec(mSpinnerPiece *self, mHotPiece *sender, int event_id, DWORD param)
{
	if(!IncPiece || !DecPiece)
		return FALSE;

	_c(self)->lineStep(self, sender == DecPiece);

	return FALSE;
}

static void mSpinnerPiece_construct(mSpinnerPiece* self, DWORD add_data)
{
	Class(mContainerPiece).construct((mContainerPiece*)self, add_data);

	//add_ata
	if(add_data != 0 && add_data != 1) {//add_data is a mHotPiece pointer
		self->body = (mHotPiece*)add_data;
	} else {
		mPairPiece * pair = NEWPIECE(mPairPiece);
		if(add_data == 1) {//vertial
			mPairPiece_setVert(pair);
			pair->first  = (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, NCS_ARROWPIECE_UP);
			pair->second = (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, NCS_ARROWPIECE_DOWN);
		} else {
			pair->first  = (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, NCS_ARROWPIECE_LEFT);
			pair->second = (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, NCS_ARROWPIECE_RIGHT);
		}
		self->body = (mHotPiece*)pair;
	}

	self->max = 100;
	self->min = 0;
	self->cur_pos = 0;
	self->line_step = 5;

	update_inc_dec(self);

	ncsAddEventListener((mObject*)IncPiece, (mObject*)self, 
			(NCS_CB_ONPIECEEVENT)mSpinnerPiece_onIncDec, NCSN_ABP_PUSHED);
	ncsAddEventListener((mObject*)DecPiece, (mObject*)self, 
			(NCS_CB_ONPIECEEVENT)mSpinnerPiece_onIncDec, NCSN_ABP_PUSHED);
}

static BOOL mSpinnerPiece_setProperty(mSpinnerPiece * self, int id, DWORD value)
{
	switch(id)
	{
	case NCSP_SPNRPIECE_MAXPOS:
        if ((int)value < self->min)
            return FALSE;

        self->max = (int)value;
        if ((int)value < self->cur_pos)
		{
            self->cur_pos = self->max;
			if(_c(self)->onPosChanged)
				_c(self)->onPosChanged(self);
		}

		update_inc_dec(self);
		return TRUE;

	case NCSP_SPNRPIECE_MINPOS:
        if ((int)value > self->max)
            return FALSE;

        self->min = (int)value;
        if ((int)value > self->cur_pos)
		{
            self->cur_pos = self->min;
			if(_c(self)->onPosChanged)
				_c(self)->onPosChanged(self);
		}

		update_inc_dec(self);
		return TRUE;

	case NCSP_SPNRPIECE_LINESTEP:
        if ((int)value == 0)
            return FALSE;

		self->line_step = (int)value;
		return TRUE;

	case NCSP_PAIRPIECE_DIRECTION:
		return _c(self->body)->setProperty(self->body, id, value);

	case NCSP_SPNRPIECE_CURPOS:
		if((int)value == self->cur_pos)
			return TRUE;

		self->cur_pos = (int)value;
		if(self->cur_pos > self->max)
			self->cur_pos = self->max;
		else if(self->cur_pos < self->min)
			self->cur_pos = self->min;
		if(_c(self)->onPosChanged)
			_c(self)->onPosChanged(self);
		update_inc_dec(self);
		return TRUE;
    case NCSP_SPNRPIECE_LOOP:
        if ((BOOL)value == TRUE) {
            self->loop_flag = 1;
        } else {
            self->loop_flag = 0;
        }
		update_inc_dec(self);
        return TRUE;
	}

	return FALSE;
}

static DWORD mSpinnerPiece_getProperty(mSpinnerPiece * self, int id)
{
	switch(id)
	{
	case NCSP_SPNRPIECE_MAXPOS:
		return self->max;
	case NCSP_SPNRPIECE_MINPOS:
		return self->min;
	case NCSP_SPNRPIECE_LINESTEP:
		return self->line_step;
	case NCSP_SPNRPIECE_CURPOS:
		return self->cur_pos;
    case NCSP_SPNRPIECE_LOOP:
        return (DWORD)(self->loop_flag == 1);
	default:
		return _c(self->body)->getProperty(self->body, id);
	}
	return (DWORD)-1;
}

static mHotPiece * mSpinnerPiece_hitTest(mSpinnerPiece *self, int x, int y)
{
	if(self->body)
		return _c(self->body)->hitTest(self->body, x, y);
	return NULL;
}

static int mSpinnerPiece_lineStep(mSpinnerPiece *self, BOOL prev)
{
	int reach_id = 0;
	int dec_inc_id = 0;
    int old = self->cur_pos;
	
	if(prev) {
		self->cur_pos -= self->line_step;
		dec_inc_id = NCSN_SPNRPIECE_DEC;
	} else { // inc
		self->cur_pos += self->line_step;
		dec_inc_id = NCSN_SPNRPIECE_INC;
	}

    //adjust current position
    if(self->cur_pos <= self->min) {
        self->cur_pos = self->min;
        reach_id = NCSN_SPNRPIECE_REACHMIN;
    }

    if(self->cur_pos >= self->max) {
        self->cur_pos = self->max;
        reach_id = NCSN_SPNRPIECE_REACHMAX;
    }

    if (self->loop_flag == 1 && old == self->cur_pos)
    {
        if (self->cur_pos == self->max) {
			reach_id = NCSN_SPNRPIECE_REACHMIN;
            self->cur_pos = self->min;
        } else {
            self->cur_pos = self->max;
			reach_id = NCSN_SPNRPIECE_REACHMAX;
        }
    }

	if(_c(self)->onPosChanged)
		_c(self)->onPosChanged(self);

	update_inc_dec(self);

	if(reach_id != 0)
		RAISE_EVENT(self, reach_id, self->cur_pos);

	RAISE_EVENT(self, dec_inc_id, self->cur_pos);

	return self->cur_pos;
}


BEGIN_MINI_CLASS(mSpinnerPiece, mContainerPiece)
	CLASS_METHOD_MAP(mSpinnerPiece, construct  )
	CLASS_METHOD_MAP(mSpinnerPiece, getProperty )
	CLASS_METHOD_MAP(mSpinnerPiece, setProperty )
	CLASS_METHOD_MAP(mSpinnerPiece, getIncPiece )
	CLASS_METHOD_MAP(mSpinnerPiece, getDecPiece )
	CLASS_METHOD_MAP(mSpinnerPiece, hitTest )
	CLASS_METHOD_MAP(mSpinnerPiece, lineStep )
END_MINI_CLASS

#endif //_MGNCSCTRL_SPINNER
