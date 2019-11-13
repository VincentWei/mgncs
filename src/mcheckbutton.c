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
    <http://www.minigui.com/blog/minigui-licensing-policy/>.
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

static mHotPiece * mCheckButton_createButtonBody(mCheckButton *self, DWORD dwStyle, mHotPiece * content)
{
	mHotPiece *body;
	mHotPiece *pieces[2]={NULL, content};

	body = (mHotPiece*)NEWPIECEEX(mCheckButtonPiece, pieces);
	_c(body)->setProperty(body, NCSP_ABP_CHECKABLE, 1);
    _c(body)->setProperty(body, NCSP_LABELPIECE_ALIGN, NCS_ALIGN_LEFT);
	if(dwStyle & NCSS_BUTTON_AUTOCHECK)
		_c(body)->setProperty(body, NCSP_ABP_AUTOCHECK, 1);
	if(dwStyle & NCSS_BUTTON_3DCHECK)
		_c(body)->setProperty(body, NCSP_ABP_3DCHECK,1);
			
	return (mHotPiece*)body;
}

static void mCheckButton_onPaint(mCheckButton *self, HDC hdc, const PCLIPRGN pinv_clip)
{
	mHotPiece* content = NULL;
    _SUPER(mWidget, self, onPaint, hdc, pinv_clip);
 
	content = (mHotPiece*)GetBodyProp(NCSP_CHECKBUTTONPIECE_CONTENT);
	if(GetFocus(GetParent(self->hwnd)) == self->hwnd)
	{
		RECT rc;

		if(content && !(_c(content)->getRect(content, &rc)))
			GetClientRect(self->hwnd, &rc);
		if(self->renderer)
			self->renderer->drawFocusFrame(self, hdc, &rc);
		else
			ncsCommRDRDrawFocusFrame(self->hwnd, hdc, &rc);
	}
}

static BOOL mCheckButton_onEraseBkgnd(mCheckButton* self, HDC hdc, const RECT *pinv)
{
	return Class(mWidget).onEraseBkgnd((mWidget*)self, hdc, pinv);
}

BEGIN_CMPT_CLASS(mCheckButton, mButton)
	CLASS_METHOD_MAP(mCheckButton, createButtonBody)
	CLASS_METHOD_MAP(mCheckButton, onPaint)
	CLASS_METHOD_MAP(mCheckButton, onEraseBkgnd)
END_CMPT_CLASS
