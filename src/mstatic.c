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

/*
static BOOL mStatic_onCreate(mStatic *self, LPARAM lParam)
{
	if(self->body) {
		SetBodyProp(NCSP_LABELPIECE_LABEL, 
			(DWORD)GetWindowCaption(self->hwnd));
		InvalidateRect(self->hwnd, NULL, TRUE);
	}
	return TRUE;
}

static void mStatic_onLButtonDown(mStatic *self)
{
    ncsNotifyParent((mWidget*)self, NCSN_WIDGET_CLICKED);
}
*/

static mObject* mStatic_createBody(mStatic *self)
{
	return (mObject*)NEWPIECE(mLabelPiece);

}

static BOOL mStatic_setProperty(mStatic *self, int id, DWORD value)
{
	if(id >= NCSP_STATIC_MAX)
		return FALSE;
	
	switch(id)
    {
        case NCSP_STATIC_ALIGN:
			if(Body && SetBodyProp(NCSP_LABELPIECE_ALIGN, value))
			{
	            InvalidateRect(self->hwnd, NULL, TRUE);
				return TRUE;
			}
            return FALSE;
        case NCSP_STATIC_VALIGN:
			if(Body&& SetBodyProp(NCSP_LABELPIECE_VALIGN, value)){
    	        InvalidateRect(self->hwnd, NULL, TRUE);
				return TRUE;
			}
            return FALSE;
        case NCSP_STATIC_AUTOWRAP:
			if(Body && SetBodyProp(NCSP_LABELPIECE_AUTOWRAP, value))
			{
            	InvalidateRect(self->hwnd, NULL, TRUE);
				return TRUE;
			}
            return FALSE;
    }

	return Class(mWidget).setProperty((mWidget*)self, id, value);
}

static DWORD mStatic_getProperty(mStatic *self, int id)
{
	if(id >= NCSP_STATIC_MAX)
		return 0;
	
	switch(id)
    {
        case NCSP_STATIC_ALIGN:
			return Body?GetBodyProp(NCSP_LABELPIECE_ALIGN):0;
        case NCSP_STATIC_VALIGN:
			return Body?GetBodyProp(NCSP_LABELPIECE_VALIGN):0;
        case NCSP_STATIC_AUTOWRAP:
			return Body?GetBodyProp(NCSP_LABELPIECE_AUTOWRAP):0;
    }
	
	return Class(mWidget).getProperty((mWidget*)self, id);
}

static LRESULT mStatic_wndProc(mStatic* self, UINT message, WPARAM wParam, LPARAM lParam)
{	
    switch(message){
        case MSG_LBUTTONDOWN:
            ncsNotifyParent((mWidget *)self, NCSN_WIDGET_CLICKED);
			break;
        case MSG_LBUTTONDBLCLK:
            ncsNotifyParent((mWidget *)self, NCSN_WIDGET_DBCLICKED);
			break;
        case MSG_FONTCHANGED:
            InvalidateRect (self->hwnd, NULL, TRUE);
            return 0;

        case MSG_SETTEXT:
            SetWindowCaption (self->hwnd, (const char*)lParam);
			if(self->body)
			{
				SetBodyProp(NCSP_LABELPIECE_LABEL, 
					(DWORD)GetWindowCaption(self->hwnd));
				InvalidateRect(self->hwnd, NULL, TRUE);
			}
            return 0;

        default:
            break;
    }
	
	return Class(mWidget).wndProc((mWidget*)self, message, wParam, lParam);
}

BEGIN_CMPT_CLASS(mStatic, mWidget)
	CLASS_METHOD_MAP(mStatic, createBody)
//	CLASS_METHOD_MAP(mStatic, onCreate)
	CLASS_METHOD_MAP(mStatic, setProperty)
	CLASS_METHOD_MAP(mStatic, getProperty)
//	CLASS_METHOD_MAP(mStatic, onLButtonDown)
	CLASS_METHOD_MAP(mStatic, wndProc)
END_CMPT_CLASS


