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

static void mGroupBox_onPaint(mGroupBox *self, HDC hdc, const PCLIPRGN pinv_clip)
{
    RECT        rcClient;
	const char *spCaption = NULL;
    PLOGFONT    font;

    GetClientRect (self->hwnd, &rcClient);
 
    font = GetWindowFont(self->hwnd);
    spCaption = GetWindowCaption (self->hwnd);

    if (spCaption)
	{
		SIZE size;
		RECT rect;
		DWORD fgcolor;
		if (GetWindowExStyle (self->hwnd) & WS_EX_TRANSPARENT)
			SetBkMode(hdc, BM_TRANSPARENT);
		else
			SetBkMode (hdc, BM_OPAQUE);

		if(GetWindowStyle(self->hwnd) & WS_DISABLED)
			fgcolor = ncsGetElement(self, NCS_FGC_DISABLED_ITEM);
		else
			fgcolor = ncsGetElement(self, NCS_FGC_3DBODY);
		
		SetTextColor(hdc, ncsColor2Pixel(hdc,fgcolor));

		SetBkColor(hdc, GetWindowBkColor (self->hwnd));
		TextOut (hdc, font->size, 2, spCaption);

		GetTextExtent (hdc, spCaption, -1, &size);
		rect.left   = font->size - 1;
		rect.right  = rect.left + size.cx + 2;
		rect.top    = 0;
		rect.bottom = size.cy;
		ExcludeClipRect (hdc, &rect);
	}

	rcClient.top += (font->size >> 1);

	((mGroupBoxRenderer*)(self->renderer))->drawGroupbox(self, hdc, &rcClient);
}

static int mGroupBox_hitTest(mGroupBox *self, int x, int y)
{
	RECT rc;
	GetWindowRect(self->hwnd, &rc);
	if(!PtInRect(&rc, x, y))
		return HT_OUT;

	if(RECTW(rc) >20 && RECTH(rc) > 20)
	{
		rc.left += 10;
		rc.top += 10;
		rc.right -= 10;
		rc.bottom -= 10;
		if(PtInRect(&rc, x, y))
			return HT_TRANSPARENT;
	}
	
	return HT_CLIENT;
	
}

static LRESULT mGroupBox_wndProc(mGroupBox* self, UINT message, WPARAM wParam, LPARAM lParam)
{	
    if (message == MSG_HITTEST) {
        return ((mGroupBoxClass*)_c(self))->hitTest(self, (int)wParam, (int)lParam);
    }
	
	return Class(mStatic).wndProc((mStatic *)self, message, wParam, lParam);
}

BEGIN_CMPT_CLASS(mGroupBox, mStatic)
	CLASS_METHOD_MAP(mGroupBox, onPaint)
	CLASS_METHOD_MAP(mGroupBox, hitTest)
	CLASS_METHOD_MAP(mGroupBox, wndProc)
END_CMPT_CLASS


