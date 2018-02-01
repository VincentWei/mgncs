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

#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSCTRL_SPINNER

static BOOL mSpinner_setProperty (mSpinner* self, int id, DWORD value)
{
    if (id >= NCSP_SPNR_MAX)
        return FALSE;

    switch (id)
    {
        case NCSP_SPNR_MAXPOS :
        {
			if(Body && SetBodyProp(NCSP_SPNRPIECE_MAXPOS,value))
			{
				InvalidateRect(self->hwnd, NULL, TRUE);
				return TRUE;
			}
			return FALSE;
        }
        case NCSP_SPNR_MINPOS :
        {
			if(Body && SetBodyProp(NCSP_SPNRPIECE_MINPOS,value))
			{
				InvalidateRect(self->hwnd, NULL, TRUE);
				return TRUE;
			}
			return FALSE;
        }
        case NCSP_SPNR_CURPOS :
        {
			if(Body && SetBodyProp(NCSP_SPNRPIECE_CURPOS,value))
			{
				InvalidateRect(self->hwnd, NULL, TRUE);
				return TRUE;
			}
			return FALSE;
        }
        case NCSP_SPNR_LINESTEP :
        {
			return Body?SetBodyProp(NCSP_SPNRPIECE_LINESTEP, value):FALSE;
        }
        case NCSP_SPNR_TARGET :
        {
            self->h_target = (HWND)value;
            return TRUE;
        }
    }

    return Class(mWidget).setProperty ((mWidget*)self, id, value);
}

static DWORD mSpinner_getProperty (mSpinner* self, int id)
{
    if (id >= NCSP_SPNR_MAX)
		return (DWORD)-1;

    switch (id)
	{
        case NCSP_SPNR_MAXPOS :
			return Body?GetBodyProp(NCSP_SPNRPIECE_MAXPOS):-1;
        case NCSP_SPNR_MINPOS :
			return Body?GetBodyProp(NCSP_SPNRPIECE_MINPOS):-1;
        case NCSP_SPNR_CURPOS :
			return Body?GetBodyProp(NCSP_SPNRPIECE_CURPOS):-1;
        case NCSP_SPNR_LINESTEP :
			return Body?GetBodyProp(NCSP_SPNRPIECE_LINESTEP):-1;
        case NCSP_SPNR_TARGET :
            return (DWORD)self->h_target;
	}

    return (DWORD)Class(mWidget).getProperty ((mWidget*)self, id);
}


static BOOL mSpinner_onPiece(mSpinner *self, mHotPiece * piece, int id, DWORD param)
{
	int newid = 0;
	int code;
	switch(id)
	{
	case NCSN_SPNRPIECE_INC:
		if(IsWindow(self->h_target))
		{
			code = (GetWindowStyle(self->hwnd)&NCSS_SPNR_HORIZONTAL) 
				? SCANCODE_CURSORBLOCKRIGHT : SCANCODE_CURSORBLOCKDOWN;
			PostMessage(self->h_target, MSG_KEYDOWN, code, KS_SPINPOST);
			PostMessage(self->h_target, MSG_KEYUP, code, KS_SPINPOST);
			return FALSE;

		}
		newid = NCSN_SPNR_CHANGED;
		break;
	case NCSN_SPNRPIECE_DEC:
		if(IsWindow(self->h_target))
		{
			code = (GetWindowStyle(self->hwnd)) & NCSS_SPNR_HORIZONTAL
				 ? SCANCODE_CURSORBLOCKLEFT : SCANCODE_CURSORBLOCKUP;
			PostMessage(self->h_target, MSG_KEYDOWN, code, KS_SPINPOST);
			PostMessage(self->h_target, MSG_KEYUP, code, KS_SPINPOST);
			return FALSE;
		}
		newid = NCSN_SPNR_CHANGED;
		break;
	case NCSN_SPNRPIECE_REACHMIN:
		newid = NCSN_SPNR_REACHMIN;
		break;
	case NCSN_SPNRPIECE_REACHMAX:
		newid = NCSN_SPNR_REACHMAX;
		break;
	default:
		return FALSE;
	}
	mHotPiece_update(piece, (mObject*)self, TRUE);

	ncsNotifyParentEx((mWidget*)self, newid, param);
	return FALSE;
}

static mObject * mSpinner_createBody(mSpinner * self)
{
	mHotPiece *piece;
	int events[]={
		NCSN_SPNRPIECE_INC,
		NCSN_SPNRPIECE_DEC,
		NCSN_SPNRPIECE_REACHMIN,
		NCSN_SPNRPIECE_REACHMAX,
		0
	};
    DWORD dwStyle = GetWindowStyle(self->hwnd);

	if(!Body)
	{
		piece  = (mHotPiece*)NEWPIECEEX(mSpinnerPiece, (dwStyle&NCSS_SPNR_HORIZONTAL) ? 0 : 1);
	}
	else
	{
		piece = Body;
	}
    if (dwStyle & NCSS_SPNR_AUTOLOOP)
    {
        _c(piece)->setProperty(piece, NCSP_SPNRPIECE_LOOP, TRUE);
    }

	ncsAddEventListeners((mObject*)piece, (mObject*)self, (NCS_CB_ONPIECEEVENT)mSpinner_onPiece, events);

	return (mObject*)piece;
}

#ifdef _MGNCS_GUIBUILDER_SUPPORT
static BOOL mSpinner_refresh(mSpinner *self)
{
	return TRUE;
}
#endif

BEGIN_CMPT_CLASS (mSpinner, mWidget)
	CLASS_METHOD_MAP (mSpinner, setProperty)
	CLASS_METHOD_MAP (mSpinner, getProperty)
	CLASS_METHOD_MAP (mSpinner, createBody)
#ifdef _MGNCS_GUIBUILDER_SUPPORT
	CLASS_METHOD_MAP (mSpinner, refresh)
#endif
END_CMPT_CLASS

#endif //_MGNCSCTRL_SPINNER
