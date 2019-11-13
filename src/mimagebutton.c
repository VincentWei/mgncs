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

#ifdef _MGNCSCTRL_IMAGEBUTTON

static mHotPiece* mImageButton_createButtonBody(mImageButton* self, DWORD dwStyle, mHotPiece* content)
{
	mAbstractButtonPiece* body = NEWPIECE(mAbstractButtonPiece);
	body->body = content;

    if(dwStyle & NCSS_BUTTON_CHECKABLE)
		_c(body)->setProperty(body, NCSP_ABP_CHECKABLE, 1);
	if(dwStyle & NCSS_BUTTON_AUTOCHECK)
		_c(body)->setProperty(body,NCSP_ABP_AUTOCHECK, 1);
	if(dwStyle & NCSS_BUTTON_3DCHECK)
		_c(body)->setProperty(body,NCSP_ABP_3DCHECK, 1);

    _c(body)->setProperty(body, NCSP_ABP_NOREPEATMSG, 1);

	return (mHotPiece *)body;
}

static mObject* mImageButton_createContent(mImageButton* self, DWORD dwStyle)
{
	return (mObject *)NEWPIECE(mToolImageItemPiece);
}

static BOOL mImageButton_setProperty(mImageButton* self, int id, DWORD value)
{
	if(id >= NCSP_IMGBTN_MAX)
		return FALSE;
	if(NCSP_IMGBTN_TOOLIMAGEFILE == id)
	{
		mToolImage* toolimg = ncsNewToolImageFromFile((const char*)value, 4, TRUE, FALSE);
		return SetBodyProp(NCSP_TOOLIMAGEPIECE_TOOLIMAGE, (DWORD)toolimg);
	}
	else if(NCSP_IMGBTN_IMAGE == id)
	{
		if(value == 0)
			return FALSE;
		return SetBodyProp(NCSP_TOOLIMAGEPIECE_TOOLIMAGE, (DWORD)ncsNewToolImage((PBITMAP)value, 4, FALSE, FALSE));
	}
	else if(NCSP_IMGBTN_TOOLIMAGE == id) {
		return SetBodyProp(NCSP_TOOLIMAGEPIECE_TOOLIMAGE, value);
	}
	return Class(mButton).setProperty((mButton*)self, id, value);
}

static DWORD mImageButton_getProperty(mImageButton* self, int id)
{
	if(id > NCSP_IMGBTN_MAX)
		return 0;
	if(NCSP_IMGBTN_TOOLIMAGEFILE == id || NCSP_IMGBTN_IMAGE == id)
		return 0;
	else if(NCSP_IMGBTN_TOOLIMAGE == id)
		return GetBodyProp(NCSP_TOOLIMAGEPIECE_TOOLIMAGE);
	return Class(mButton).getProperty((mButton*)self, id);
}

static void mImageButton_onPaint(mImageButton* self, HDC hdc, const PCLIPRGN p)
{
	return Class(mWidget).onPaint((mWidget*)self, hdc, p);
}

BEGIN_CMPT_CLASS(mImageButton, mButton)
	CLASS_METHOD_MAP(mImageButton, createButtonBody)
	CLASS_METHOD_MAP(mImageButton, createContent)
	CLASS_METHOD_MAP(mImageButton, setProperty)
	CLASS_METHOD_MAP(mImageButton, getProperty)
	CLASS_METHOD_MAP(mImageButton, onPaint)
END_CMPT_CLASS

#endif		//_MGNCSCTRL_IMAGEBUTTON

