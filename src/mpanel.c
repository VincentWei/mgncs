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
    The implementation of mPanel control.

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

static int
mPanel_onKeyDown (mPanel* self, int scancode, int state)
{
    HWND hCurFocus;
    int dlgCode = 0;

    hCurFocus = GetFocusChild (self->hwnd);

    if (hCurFocus) {
        dlgCode = ncsGetDlgCode(hCurFocus);
        if (dlgCode & DLGC_WANTALLKEYS)
            /* continue*/
            return 1;
    }

    switch (scancode) {
        case SCANCODE_KEYPADENTER:
        case SCANCODE_ENTER:
        {
            HWND hDef;

            if (dlgCode & DLGC_PUSHBUTTON)
                break;

            hDef = GetDlgDefPushButton (self->hwnd);
            if (hDef) {
                SendMessage (self->hwnd, MSG_COMMAND, GetDlgCtrlID (hDef), 0L);
                return 0;
            }
            break;
        }

        case SCANCODE_ESCAPE:
            SendMessage (self->hwnd, MSG_COMMAND, IDCANCEL, 0L);
            return 0;

        case SCANCODE_TAB:
        {
            HWND hNewFocus;

            if (dlgCode & DLGC_WANTTAB)
                break;

            if (state & KS_SHIFT)
                hNewFocus = GetNextDlgTabItem (self->hwnd, hCurFocus, TRUE);
            else
                hNewFocus = GetNextDlgTabItem (self->hwnd, hCurFocus, FALSE);

            if (hNewFocus != hCurFocus) {
                SetFocus (hNewFocus);
				InvalidateRect(hNewFocus, NULL, FALSE);
            }

            return 0;
        }

        case SCANCODE_CURSORBLOCKDOWN:
        case SCANCODE_CURSORBLOCKRIGHT:
        case SCANCODE_CURSORBLOCKUP:
        case SCANCODE_CURSORBLOCKLEFT:
        {
            HWND hNewFocus;

            if (ncsGetDlgCode(hCurFocus) & DLGC_WANTARROWS)
                    break;

            if (scancode == SCANCODE_CURSORBLOCKDOWN
                    || scancode == SCANCODE_CURSORBLOCKRIGHT)
                hNewFocus = GetNextDlgGroupItem (self->hwnd, hCurFocus, FALSE);
            else
                hNewFocus = GetNextDlgGroupItem (self->hwnd, hCurFocus, TRUE);

            if (hNewFocus != hCurFocus) {
                if (dlgCode & DLGC_STATIC)
                    return 0;

                SetFocus (hNewFocus);

                if (ncsGetDlgCode(hNewFocus) & DLGC_RADIOBUTTON) {
                    /* simulate user clicking event */
                    //SendMessage (hNewFocus, BM_CLICK, 0, 0L);
                    ExcludeWindowStyle (hCurFocus, WS_TABSTOP);
                    IncludeWindowStyle (hNewFocus, WS_TABSTOP);
                }
            }

            return 0;
        }
    }

    return 1;
}

static int mPanel_onKeyUp(mPanel* self, int scancode, int status)
{
	if(scancode == SCANCODE_TAB)
	{
		HWND hwndFocus = GetFocus(self->hwnd);
		if(hwndFocus && !(ncsGetDlgCode(hwndFocus)&DLGC_WANTTAB))
			return 0;
	}
	return 1;
}

static LRESULT mPanel_wndProc(mPanel* self, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case MSG_COMMAND:
            SendNotifyMessage (GetParent(self->hwnd),
                    MSG_COMMAND, wParam, lParam);
            break;
		case MSG_CHAR:
			if(wParam == '\t')
			{
				HWND hwndFocus = GetFocus(self->hwnd);
				if(hwndFocus && !(ncsGetDlgCode(hwndFocus)&DLGC_WANTTAB))
					return 0;
			}
			break;
    }

	return Class(mWidget).wndProc((mWidget*)self, message, wParam, lParam);
}

BEGIN_CMPT_CLASS(mPanel, mWidget)
    CLASS_METHOD_MAP(mPanel, wndProc)
    CLASS_METHOD_MAP(mPanel, onKeyDown)
    CLASS_METHOD_MAP(mPanel, onKeyUp)
	SET_DLGCODE(DLGC_WANTALLKEYS)
END_CMPT_CLASS
