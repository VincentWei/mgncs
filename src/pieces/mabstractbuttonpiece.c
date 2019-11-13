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

static void mAbstractButtonPiece_construct(mAbstractButtonPiece* self, DWORD add_data)
{
	Class(mContainerPiece).construct((mContainerPiece*)self, add_data);

	self->flags = 0;
	self->state = NCS_ABP_NORMAL;
	self->check_state = NCS_ABP_UNCHECKED;
	self->revert = 0;
}

static void set_check(mAbstractButtonPiece* self)
{
	int new_state;
	if(!mAbstractButtonPiece_isCheckable(self))
		return;

	if(!mAbstractButtonPiece_isAutoCheck(self))
	{
		if(self->check_state == NCS_ABP_UNCHECKED)
		{
			self->check_state = NCS_ABP_CHECKED;
			RAISE_EVENT(self, NCSN_ABP_STATE_CHANGED, self->check_state);
		}
		return ;
	}
	
	switch(self->check_state)
	{
	case NCS_ABP_UNCHECKED:
		if(mAbstractButtonPiece_is3DCheck(self))
			new_state = NCS_ABP_HALFCHECKED;
		else
			new_state = NCS_ABP_CHECKED;
		break;
	case NCS_ABP_HALFCHECKED:
		new_state = NCS_ABP_CHECKED;
		break;
	case NCS_ABP_CHECKED:
	default:
		new_state = NCS_ABP_UNCHECKED;
		break;
	}

	self->check_state = new_state;
	RAISE_EVENT(self, NCSN_ABP_STATE_CHANGED, new_state);
}

static void set_state(mAbstractButtonPiece* self, int new_state, int event_id, mWidget *owner)
{
	if(new_state == self->state)
		return;

	self->state = new_state;

	if(owner && new_state == NCS_ABP_HILIGHT)
		mWidget_setHoveringFocus(owner, (mObject*)self);

	mHotPiece_update((mHotPiece*)self, (mObject*)owner, TRUE);

	//cancel the auto repeade message
	if(new_state != NCS_ABP_PUSHED
            && mAbstractButtonPiece_needRepeatMsg (self)){
		SetAutoRepeatMessage(0, 0, 0, 0);
    }

	if(event_id != 0)
		RAISE_EVENT(self, event_id, 0);
}

static BOOL mAbstractButtonPiece_isEnabled(mAbstractButtonPiece * self)
{
//	return mAbstractButtonPiece_isEnableFlag(self);
	return self->state != NCS_ABP_DISABLED;
}

static BOOL mAbstractButtonPiece_enable(mAbstractButtonPiece* self, BOOL benable)
{
	if((self->state!=NCS_ABP_DISABLED && benable) ||
		(self->state == NCS_ABP_DISABLED && !benable))
		return TRUE;

//	mAbstractButtonPiece_setEnable(self, benable);
	if(self->state == NCS_ABP_PUSHED)
	{
		mWidget_releaseCapturedHotPiece();
        if(mAbstractButtonPiece_needRepeatMsg (self)){
            SetAutoRepeatMessage(0, 0, 0, 0);
        }
	}
	else if(self->state == NCS_ABP_HILIGHT)
	{
		mWidget_releaseHoveringFocus();
	}

	set_state(self, benable?NCS_ABP_NORMAL:NCS_ABP_DISABLED, 0, NULL);
	RAISE_EVENT(self, NCSN_ABP_DISABLED, benable);
	return benable;
}

static BOOL mAbstractButtonPiece_setProperty(mAbstractButtonPiece* self, int id, DWORD value)
{
	switch(id)
	{
	case NCSP_ABP_CHECKABLE:
		mAbstractButtonPiece_setCheckable(self, value);
		return TRUE;
	case NCSP_ABP_AUTOCHECK:
		mAbstractButtonPiece_setAutoCheck(self, value);
		return TRUE;
	case NCSP_ABP_3DCHECK:
		mAbstractButtonPiece_set3DCheck(self, value);
		return TRUE;
	case NCSP_ABP_CHECKSTATE:
		if(value >= NCS_ABP_UNCHECKED && value <= NCS_ABP_CHECKED)
		{
			if(!mAbstractButtonPiece_isCheckable(self) || self->check_state == (char)value)
				return TRUE;

			self->check_state = (char)value;
		//	RAISE_EVENT(self,NCSN_ABP_STATE_CHANGED ,0);
			return TRUE;
		}
		return FALSE;
	case NCSP_ABP_DISABLE:
		_c(self)->enable(self, value);
		return TRUE;
	case NCSP_ABP_FLAT:
		if(value)
			self->flags|=NCSS_ABP_FLAT;
		else
			self->flags&=NCSS_ABP_FLAT;
		return TRUE;
    case NCSP_ABP_NOREPEATMSG:
        return mAbstractButtonPiece_setNoRepeatMsg(self, value);
	}

	return Class(mContainerPiece).setProperty((mContainerPiece*)self, id, value);
}

static DWORD mAbstractButtonPiece_getProperty(mAbstractButtonPiece* self, int id)
{
	switch(id)
	{
	case NCSP_ABP_CHECKABLE:
		return mAbstractButtonPiece_isCheckable(self);
	case NCSP_ABP_AUTOCHECK:
		return mAbstractButtonPiece_isAutoCheck(self);
	case NCSP_ABP_3DCHECK:
		return mAbstractButtonPiece_is3DCheck(self);
	case NCSP_ABP_STATE:
		return self->state;
	case NCSP_ABP_CHECKSTATE:
		return self->check_state;
	case NCSP_ABP_DISABLE:
		return mAbstractButtonPiece_isEnableFlag(self);
	case NCSP_ABP_FLAT:
		return self->flags&NCSP_ABP_FLAT;
    case NCSP_ABP_NOREPEATMSG:
        return !mAbstractButtonPiece_needRepeatMsg(self);
	}

	return Class(mContainerPiece).getProperty((mContainerPiece*)self, id);
}


static int mAbstractButtonPiece_processMessage(mAbstractButtonPiece* self, int message, WPARAM wParam, LPARAM lParam, mWidget * owner)
{
	if(self->state == NCS_ABP_DISABLED)
		return 0;
	
	switch(self->state)
	{
		case NCS_ABP_NORMAL:
			if(message == MSG_MOUSEMOVEIN && wParam)
			{
				set_state(self, NCS_ABP_HILIGHT, 0, owner);
			}
			else if (message == MSG_LBUTTONDOWN
					|| (message == MSG_KEYDOWN &&( wParam == SCANCODE_SPACE || wParam == SCANCODE_ENTER)))
			{
				set_state(self, NCS_ABP_PUSHED, NCSN_ABP_PUSHED, owner);
				if(mAbstractButtonPiece_needRepeatMsg (self)
                        && (message == MSG_LBUTTONDOWN || wParam == SCANCODE_ENTER)){
					SetAutoRepeatMessage(owner->hwnd, message, wParam, lParam);
                }
			}
			break;
		case NCS_ABP_HILIGHT:
			if((message == MSG_MOUSEMOVEIN && wParam == 0)) //move out
			{
				set_state(self, NCS_ABP_NORMAL, 0, owner);
			}
			else if(message == MSG_LBUTTONDOWN
				|| (message == MSG_KEYDOWN && (wParam == SCANCODE_SPACE || wParam == SCANCODE_ENTER)))
			{
				mWidget_captureHotPiece(owner, (mObject*)self);
				set_state(self, NCS_ABP_PUSHED, NCSN_ABP_PUSHED, owner);
				if(mAbstractButtonPiece_needRepeatMsg (self)
                        && (message == MSG_LBUTTONDOWN || wParam == SCANCODE_ENTER)){
					SetAutoRepeatMessage(owner->hwnd, message, wParam, lParam);
                }
			}
			break;
		case NCS_ABP_PUSHED:
			if(message == MSG_MOUSEMOVE)
			{
				int x = LOSWORD(lParam);
				int y = HISWORD(lParam);
				if(GetCapture() == owner->hwnd)
					ScreenToClient(owner->hwnd, &x, &y);

				if(_c(self)->hitTest(self, x, y) != (mHotPiece*)self)
					set_state(self, NCS_ABP_CAPTURED, 0, owner);
			}
			else if(message == MSG_LBUTTONUP)
			{
				mWidget_releaseCapturedHotPiece();

				set_check(self);
				set_state(self, NCS_ABP_HILIGHT, NCSN_ABP_CLICKED, owner);
			}
			else if(message == MSG_KEYUP && 
                    (wParam == SCANCODE_SPACE || wParam == SCANCODE_ENTER))
			{
				POINT pt;
				GetCursorPos(&pt);
				if(mWidget_getCapturedHotPiece(owner) == (mObject*)self)
				{
					ScreenToClient(owner->hwnd, &pt.x, &pt.y);
					mWidget_releaseCapturedHotPiece();
				}
				
				if(_c(self)->hitTest(self, pt.x, pt.y) != (mHotPiece*)self)
				{
					set_state(self, NCS_ABP_NORMAL, 0, owner);
				}
				else
				{
					set_state(self, NCS_ABP_HILIGHT, NCSN_ABP_CLICKED, owner);
				}
				
				set_check(self);
				mHotPiece_update((mHotPiece*)self, (mObject*)owner, TRUE);
			}
			else if(message == MSG_LBUTTONDOWN || 
                    (message == MSG_KEYDOWN && wParam == SCANCODE_ENTER)) //process auto repeate message
			{
				RAISE_EVENT(self, NCSN_ABP_PUSHED, 0);
			}
			break;

		case NCS_ABP_CAPTURED:
			if(message == MSG_MOUSEMOVE)
			{
				int x = LOSWORD(lParam);
				int y = HISWORD(lParam);
				ScreenToClient(owner->hwnd, &x, &y);

				if(_c(self)->hitTest(self, x, y) == (mHotPiece*)self)
				{
					set_state(self, NCS_ABP_PUSHED, 0, owner);
                    if(mAbstractButtonPiece_needRepeatMsg (self)){
                        SetAutoRepeatMessage(owner->hwnd, MSG_LBUTTONDOWN, wParam, lParam);
                    }
				}
			}
			else if(message == MSG_LBUTTONUP
				|| (message == MSG_KEYUP && 
                    (wParam == SCANCODE_SPACE || wParam == SCANCODE_SPACE)))
			{
				mWidget_releaseCapturedHotPiece();
				set_state(self, NCS_ABP_NORMAL, 0, owner);
			}
			break;
	}
	return 0;
}

static void mAbstractButtonPiece_paint(mAbstractButtonPiece* self, HDC hdc, mWidget *onwer, DWORD add_data)
{
	if(self->body)
		_c(self->body)->paint(self->body, hdc, (mObject*)onwer,
			(add_data)|(self->state)|(NCS_PIECE_PAINT_MAKE_CHECK(self->check_state))
				      |((self->flags&NCSS_ABP_FLAT)?NCS_PIECE_PAINT_FLAT:0));
}

BEGIN_MINI_CLASS(mAbstractButtonPiece, mContainerPiece)
	CLASS_METHOD_MAP(mAbstractButtonPiece, construct)
	CLASS_METHOD_MAP(mAbstractButtonPiece, processMessage)
	CLASS_METHOD_MAP(mAbstractButtonPiece, isEnabled)
	CLASS_METHOD_MAP(mAbstractButtonPiece, enable)
	CLASS_METHOD_MAP(mAbstractButtonPiece, setProperty)
	CLASS_METHOD_MAP(mAbstractButtonPiece, getProperty)
	CLASS_METHOD_MAP(mAbstractButtonPiece, paint)
END_MINI_CLASS

