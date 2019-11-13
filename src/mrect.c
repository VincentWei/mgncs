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

#ifdef _MGNCSCTRL_RECTANGLE

static mObject * mRectangle_createBody(mRectangle *self)
{
	return (mObject*)NEWPIECE(mRectPiece);
}

static DWORD mRectangle_setProperty(mRectangle* self, int id, DWORD value)
{
	if( id >= NCSP_RECTANGLE_MAX)
		return 0;

	switch(id)
	{
	case NCSP_RECTANGLE_BORDERSIZE:
		if(!_c(HP(self->body))->setProperty(HP(self->body), NCSP_RECTPIECE_BORDERSIZE, value))
			return FALSE;
		break;
	case NCSP_RECTANGLE_BORDERCOLOR:
		if(!_c(HP(self->body))->setProperty(HP(self->body), NCSP_RECTPIECE_BORDERCOLOR, value))
			return FALSE;
		break;
	case NCSP_RECTANGLE_FILLCOLOR:
		if(!_c(HP(self->body))->setProperty(HP(self->body), NCSP_RECTPIECE_FILLCOLOR, value))
			return FALSE;
		break;
	case NCSP_RECTANGLE_XRADIUS:
		if(!_c(HP(self->body))->setProperty(HP(self->body), NCSP_RECTPIECE_XRADIUS, value))
			return FALSE;
		break;
	case NCSP_RECTANGLE_YRADIUS:
		if(!_c(HP(self->body))->setProperty(HP(self->body), NCSP_RECTPIECE_YRADIUS, value))
			return FALSE;
		break;
	case NCSP_RECTANGLE_FILLCLR_RED:
		if(!_c(HP(self->body))->setProperty(HP(self->body), NCSP_RECTPIECE_FILLCLR_RED, value))
			return FALSE;
		break;
	case NCSP_RECTANGLE_FILLCLR_GREEN:
		if(!_c(HP(self->body))->setProperty(HP(self->body), NCSP_RECTPIECE_FILLCLR_GREEN, value))
			return FALSE;
		break;
	case NCSP_RECTANGLE_FILLCLR_BLUE:
		if(!_c(HP(self->body))->setProperty(HP(self->body), NCSP_RECTPIECE_FILLCLR_BLUE, value))
			return FALSE;
		break;
	case NCSP_RECTANGLE_FILLCLR_ALPHA:
		if(!_c(HP(self->body))->setProperty(HP(self->body), NCSP_RECTPIECE_FILLCLR_ALPHA, value))
			return FALSE;
		break;
	case NCSP_RECTANGLE_BRDCLR_RED:
		if(!_c(HP(self->body))->setProperty(HP(self->body), NCSP_RECTPIECE_BRDCLR_RED, value))
			return FALSE;
		break;
	case NCSP_RECTANGLE_BRDCLR_GREEN:
		if(!_c(HP(self->body))->setProperty(HP(self->body), NCSP_RECTPIECE_BRDCLR_GREEN, value))
			return FALSE;
		break;
	case NCSP_RECTANGLE_BRDCLR_BLUE:
		if(!_c(HP(self->body))->setProperty(HP(self->body), NCSP_RECTPIECE_BRDCLR_BLUE, value))
			return FALSE;
		break;
	case NCSP_RECTANGLE_BRDCLR_ALPHA:
		if(!_c(HP(self->body))->setProperty(HP(self->body), NCSP_RECTPIECE_BRDCLR_ALPHA, value))
			return FALSE;
		break;
	default:
		return Class(mWidget).setProperty((mWidget*)self, id, value);
	}

	InvalidateRect(self->hwnd, NULL, FALSE);
	return TRUE;

}

static DWORD mRectangle_getProperty(mRectangle* self, int id)
{
	if( id >= NCSP_RECTANGLE_MAX)
		return (DWORD)-1;

	switch(id)
    {
        case NCSP_RECTANGLE_BORDERSIZE:
			return _c(HP(self->body))->getProperty(HP(self->body), NCSP_RECTPIECE_BORDERSIZE);
        case NCSP_RECTANGLE_BORDERCOLOR:
			return _c(HP(self->body))->getProperty(HP(self->body), NCSP_RECTPIECE_BORDERCOLOR);
        case NCSP_RECTANGLE_FILLCOLOR:
			return _c(HP(self->body))->getProperty(HP(self->body), NCSP_RECTPIECE_FILLCOLOR);
        case NCSP_RECTANGLE_XRADIUS:
			return _c(HP(self->body))->getProperty(HP(self->body), NCSP_RECTPIECE_XRADIUS);
        case NCSP_RECTANGLE_YRADIUS:
			return _c(HP(self->body))->getProperty(HP(self->body), NCSP_RECTPIECE_YRADIUS);
		case NCSP_RECTANGLE_FILLCLR_RED:
			return _c(HP(self->body))->getProperty(HP(self->body), NCSP_RECTPIECE_FILLCLR_RED);
		case NCSP_RECTANGLE_FILLCLR_GREEN:
			return _c(HP(self->body))->getProperty(HP(self->body), NCSP_RECTPIECE_FILLCLR_GREEN);
		case NCSP_RECTANGLE_FILLCLR_BLUE:
			return _c(HP(self->body))->getProperty(HP(self->body), NCSP_RECTPIECE_FILLCLR_BLUE);
		case NCSP_RECTANGLE_FILLCLR_ALPHA:
			return _c(HP(self->body))->getProperty(HP(self->body), NCSP_RECTPIECE_FILLCLR_ALPHA);
		case NCSP_RECTANGLE_BRDCLR_RED:
			return _c(HP(self->body))->getProperty(HP(self->body), NCSP_RECTPIECE_BRDCLR_RED);
		case NCSP_RECTANGLE_BRDCLR_GREEN:
			return _c(HP(self->body))->getProperty(HP(self->body), NCSP_RECTPIECE_BRDCLR_GREEN);
		case NCSP_RECTANGLE_BRDCLR_BLUE:
			return _c(HP(self->body))->getProperty(HP(self->body), NCSP_RECTPIECE_BRDCLR_BLUE);
		case NCSP_RECTANGLE_BRDCLR_ALPHA:
			return _c(HP(self->body))->getProperty(HP(self->body), NCSP_RECTPIECE_BRDCLR_ALPHA);
    }

	return Class(mWidget).getProperty((mWidget*)self, id);
}

static BOOL mRectangle_onEraseBkgnd(mRectangle* self, HDC hdc, const RECT *pinv)
{
	int border_size;
	int rx, ry;
	RECT rc,rc_inv;
	PCLIPRGN poldrgn;

	if(!Body)
		return Class(mWidget).onEraseBkgnd((mWidget*)self, hdc, pinv);

	if(GetBodyProp(NCSP_RECTPIECE_FILLCLR_ALPHA) == 0) //don't fill color
	{
		return Class(mWidget).onEraseBkgnd((mWidget*)self, hdc, pinv);
	}

	rx = GetBodyProp(NCSP_RECTPIECE_XRADIUS);
	ry = GetBodyProp(NCSP_RECTPIECE_YRADIUS);

	if(rx <= 0 || ry <= 0)
		return TRUE;

	GetClientRect(self->hwnd, &rc);
	border_size = GetBodyProp(NCSP_RECTPIECE_BORDERSIZE);

	if(rx > (RECTW(rc) - border_size)/2)
		rx = (RECTW(rc) - border_size) /2;
	if(ry > (RECTH(rc) - border_size)/2)
		ry = (RECTH(rc) - border_size);

	if(rx <= 0 || ry <= 0)
		return TRUE;


	poldrgn = CreateClipRgn();
	GetClipRegion(hdc, poldrgn);


	memset(&rc_inv, 0, sizeof(rc_inv));
	SelectClipRect(hdc, &rc_inv);
	border_size /= 2;
	rx += border_size;
	ry += border_size;
	//clear the region
	//include 4 small rect
	rc_inv.left = rc.left;
	rc_inv.top = rc.top;
	rc_inv.right = rc.left + rx;
	rc_inv.bottom = rc.top + ry;
	IncludeClipRect(hdc, &rc_inv);
	rc_inv.left = rc.right - rx;
	rc_inv.right = rc.right;
	IncludeClipRect(hdc, &rc_inv);
	rc_inv.top = rc.bottom - ry;
	rc_inv.bottom = rc.bottom;
	IncludeClipRect(hdc, &rc_inv);
	rc_inv.left = rc.left;
	rc_inv.right = rc.left + rx;
	IncludeClipRect(hdc, &rc_inv);
	//intersect old region
	SelectClipRegionEx(hdc, poldrgn,RGN_AND);

	Class(mWidget).onEraseBkgnd((mWidget*)self, hdc, pinv);

	SelectClipRegion(hdc, poldrgn);

	DestroyClipRgn(poldrgn);


	return TRUE;
}


BEGIN_CMPT_CLASS(mRectangle, mWidget)
	CLASS_METHOD_MAP(mRectangle, createBody)
	CLASS_METHOD_MAP(mRectangle, setProperty);
	CLASS_METHOD_MAP(mRectangle, getProperty);
	CLASS_METHOD_MAP(mRectangle, onEraseBkgnd);
END_CMPT_CLASS

#endif //_MGNCSCTRL_RECTANGLE
