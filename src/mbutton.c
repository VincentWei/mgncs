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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "mstatic.h"
#include "mcheckbutton.h"
#include "mgroupbox.h"
#include "mbuttongroup.h"

extern BOOL mButtonGroup_removeButton(mButtonGroup *self, mButton* btn);
extern BOOL mButtonGroup_addButton(mButtonGroup *self, mButton* btn);

static void mButton_construct(mButton *self, DWORD param)
{
	Class(mWidget).construct((mWidget*)self, param);

	IncludeWindowStyle(self->hwnd, NCSS_NOTIFY);
}

static BOOL mButton_onPiece(mButton* self, mHotPiece *piece, int event_id, DWORD param)
{
	if(event_id == NCSN_ABP_CLICKED)
	{	
		ncsNotifyParent((mWidget*)self,NCSN_WIDGET_CLICKED);	
	}
    else if(event_id == NCSN_ABP_PUSHED)
	{	
		ncsNotifyParent((mWidget*)self,NCSN_BUTTON_PUSHED);	
	}
	else if(event_id == NCSN_ABP_STATE_CHANGED)
	{
		if(self->group)
			_M((mButtonGroup*)(self->group), checkBtn, (mWidget*)self);

		ncsNotifyParent((mWidget*)self,NCSN_BUTTON_STATE_CHANGED);
	}	
	return FALSE;
}

static mHotPiece * mButton_createButtonBody(mButton *self, DWORD dwStyle, mHotPiece *content)
{
	mPushButtonPiece * body = NEWPIECE(mPushButtonPiece);
	
    if(dwStyle & NCSS_BUTTON_CHECKABLE)
		_c(body)->setProperty(body, NCSP_ABP_CHECKABLE, 1);
	if(dwStyle & NCSS_BUTTON_AUTOCHECK)
		_c(body)->setProperty(body,NCSP_ABP_AUTOCHECK, 1);
	if(dwStyle & NCSS_BUTTON_3DCHECK)
		_c(body)->setProperty(body,NCSP_ABP_3DCHECK, 1);

    _c(body)->setProperty(body, NCSP_ABP_NOREPEATMSG, 1);

	body->content = content;
	return (mHotPiece*)body;
}

static mObject* mButton_createBody(mButton *self)
{
	DWORD dwStyle = GetWindowStyle(self->hwnd);
	
	mHotPiece *content = (mHotPiece*)(_c(self)->createContent(self, dwStyle));
	mHotPiece * body = (mHotPiece*)(_c(self)->createButtonBody(self, dwStyle, (mObject*)content));
	if(body)
	{
		//add event listner
		int event_ids[]={NCSN_ABP_CLICKED, NCSN_ABP_PUSHED, NCSN_ABP_STATE_CHANGED, 0};
		ncsAddEventListeners((mObject*)body, 
                (mObject*)self, (NCS_CB_ONPIECEEVENT)mButton_onPiece, event_ids);
	}

	return (mObject*)body;
}

static mObject * mButton_createContent(mButton *self, DWORD dwStyle)
{
	if(dwStyle & NCSS_BUTTON_IMAGE)
		return (mObject*)NEWPIECE(mImagePiece);
	else if(dwStyle & NCSS_BUTTON_IMAGELABEL)
	{
		mImageLabelPiece * piece = NEWPIECE(mImageLabelPiece);
		if(piece && (dwStyle & NCSS_BUTTON_VERTIMAGELABEL))
		{
			_c(piece)->setProperty(piece, NCSP_PAIRPIECE_DIRECTION, 1);
		}
		return (mObject*)piece;
	}
	
	return (mObject*)NEWPIECE(mLabelPiece);
}

static LRESULT mButton_wndProc(mButton *self, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message){
        case MSG_KILLFOCUS:
			InvalidateRect(self->hwnd, NULL, TRUE);
			return 0;

        case MSG_SETFOCUS:
			InvalidateRect(self->hwnd, NULL, TRUE);
            break;
    }
	return Class(mWidget).wndProc((mWidget*)self, message, wParam, lParam);
}

static void mButton_onPaint(mButton *self, HDC hdc, const PCLIPRGN pinv_clip)
{
    _SUPER(mWidget, self, onPaint, hdc, pinv_clip);

	//paint focus
	if(GetFocus(GetParent(self->hwnd)) == self->hwnd)
	{
		RECT rc;
		int pushdown = 0;
		if(!(Body && _c(Body)->getRect(Body, &rc)))
			GetClientRect(self->hwnd, &rc);
		rc.left +=2;
		rc.top +=2;
		rc.right -=2;
		rc.bottom -=2;

		if(Body)
			pushdown = GetBodyProp(NCSP_ABP_STATE);
		if(pushdown == NCS_ABP_PUSHED)
		{
			rc.left += 2;
			rc.top += 2;
		}

		if(self->renderer)
			self->renderer->drawFocusFrame(self, hdc, &rc);
		else
			ncsCommRDRDrawFocusFrame(self->hwnd, hdc, &rc);
	}
}


static BOOL mButton_setProperty(mButton *self, int id, DWORD value)
{
//	if(id >= NCSP_BUTTON_MAX)
//		return FALSE;

	switch(id)
	{
	case NCSP_BUTTON_ALIGN:
		if(Body && SetBodyProp(NCSP_LABELPIECE_ALIGN,value)){
			InvalidateRect(self->hwnd, NULL, TRUE);
			return TRUE;
		}
		return FALSE;
	case NCSP_BUTTON_VALIGN:
		if(Body && SetBodyProp(NCSP_LABELPIECE_VALIGN,value)){
			InvalidateRect(self->hwnd, NULL, TRUE);
			return TRUE;
		}
		return FALSE;
	case NCSP_BUTTON_WORDWRAP:
		if(Body && SetBodyProp(NCSP_LABELPIECE_AUTOWRAP,value)){
			InvalidateRect(self->hwnd, NULL, TRUE);
			return TRUE;
		}
		return FALSE;
	case NCSP_BUTTON_IMAGE:
		if(Body && SetBodyProp(NCSP_IMAGEPIECE_IMAGE,value)){
			InvalidateRect(self->hwnd, NULL, TRUE);
			return TRUE;
		}
		return FALSE;
	case NCSP_BUTTON_CHECKSTATE:
		if(Body && GetBodyProp(NCSP_ABP_CHECKSTATE) == value)
			return TRUE;
#ifdef _MGNCS_GUIBUILDER_SUPPORT
		if(self->group 
				&& value == NCS_BUTTON_CHECKED)
		{
			mButton* selbtn = (mButton*)_c(self->group)->getProperty(self->group, NCSP_BTNGRP_SELOBJ);
			if(selbtn != NULL && selbtn != self)
				return FALSE;
		}
#endif
		if(Body && SetBodyProp(NCSP_ABP_CHECKSTATE, value)){
			if(value == NCS_BUTTON_CHECKED && self->group)
				_c((mButtonGroup*)(self->group))->checkBtn((mButtonGroup*)(self->group), (mWidget*)self);
			InvalidateRect(self->hwnd,NULL, TRUE);
			return TRUE;
		}
		return FALSE;
	case NCSP_BUTTON_IMAGE_SIZE_PERCENT:
		if(value < 15)
			value = 15;
		else if(value > 85)
			value = 85;
		if(Body && SetBodyProp(NCSP_PAIRPIECE_FIRST_SIZE, value))
		{
			RECT rc;
			GetClientRect(self->hwnd, &rc);
			_c(Body)->setRect(Body, &rc);
			InvalidateRect(self->hwnd, NULL, TRUE);
			return TRUE;
		}
		return FALSE;
	case NCSP_BUTTON_GROUPID:
		if(value == 0 || value == 0xFFFFFFFF)
			return FALSE;
		value = (DWORD)ncsGetChildObj(GetParent(self->hwnd), (int)value);
	case NCSP_BUTTON_GROUP:
		{
			mButtonGroup* group =  SAFE_CAST(mButtonGroup, value);
			if(group == NULL)
				return FALSE;
			self->group = (mWidget*)group;
			mButtonGroup_addButton((mButtonGroup *)(self->group), (mButton*)self);
			return TRUE;
		}
	}
	return Class(mWidget).setProperty((mWidget*)self, id, value);
}

static DWORD mButton_getProperty(mButton *self, int id)
{
	if(id >= NCSP_BUTTON_MAX)
		return 0;
	

	if(Body && id >= NCSP_BUTTON_ALIGN && id < NCSP_BUTTON_GROUPID)
	{
		int pieceids[] = {
			NCSP_LABELPIECE_ALIGN,
			NCSP_LABELPIECE_VALIGN,
			NCSP_LABELPIECE_AUTOWRAP,
			NCSP_IMAGEPIECE_IMAGE,
			NCSP_ABP_CHECKSTATE,
			NCSP_PAIRPIECE_FIRST_SIZE
		};
		DWORD value = GetBodyProp(pieceids[id - NCSP_BUTTON_ALIGN]);
		if(value !=(DWORD)-1)
			return value;
	}
	else
	{
		switch(id)
		{
		case NCSP_BUTTON_GROUPID:
			return self->group?_c(self->group)->getId(self->group):(DWORD)-1;
		case NCSP_BUTTON_GROUP:
			return (DWORD)(self->group);
		}
	}

	return Class(mWidget).getProperty((mWidget*)self, id);
}
/*  
static BOOL mButton_onEraseBkgnd(mButton *self, HDC hdc, const RECT *pinv)
{
	return TRUE;
}
*/

BEGIN_CMPT_CLASS(mButton, mWidget)
	CLASS_METHOD_MAP(mButton, construct)
	CLASS_METHOD_MAP(mButton, createBody)
	CLASS_METHOD_MAP(mButton, createButtonBody)
	CLASS_METHOD_MAP(mButton, createContent)
	CLASS_METHOD_MAP(mButton, wndProc)
	CLASS_METHOD_MAP(mButton, onPaint)
	CLASS_METHOD_MAP(mButton, setProperty)
	CLASS_METHOD_MAP(mButton, getProperty)
	//CLASS_METHOD_MAP(mButton, onEraseBkgnd)
END_CMPT_CLASS



