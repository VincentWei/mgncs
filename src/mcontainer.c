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
    The implementation of mContainer control.

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

#include "mscroll_widget.h"
#include "mcontainer.h"

#ifdef _MGNCSCTRL_CONTAINER

static void mContainer_construct (mContainer *self,DWORD addData)
{
	g_stmScrollWidgetCls.construct((mScrollWidget*)self, addData);

    self->hPanel = HWND_INVALID;

}

static void mContainer_moveContent (mContainer* self)
{
	int _x = -self->contX, _y = -self->contY;
	
	_M(self, viewportToWindow, &_x, &_y);
    
	MoveWindow (self->hPanel, _x, _y,
            self->contWidth, self->contHeight, TRUE);
}

static int createPanel(mContainer *self)
{
    RECT rcClient;
    GetClientRect (self->hwnd, &rcClient);
    self->hPanel = CreateWindowEx2 (NCSCTRL_PANEL,
                        "",
                        WS_VISIBLE,
                        WS_EX_TRANSPARENT,
                        10,
                        0, 0,
                        RECTW(rcClient), RECTH(rcClient),
                        self->hwnd,
                        NULL, 0, 0);

    if (self->hPanel == HWND_INVALID) {
        return 1;
    }

    return 0;
}


static BOOL mContainer_addChildren(mContainer *self,
        const NCS_WND_TEMPLATE* pCtrl, int nCount)
{
    int             i;
    HWND            hCtrl;
    RECT            rcClient;
    int             maxWidth, maxHeight;

    if (nCount < 0 || !self) {
        return FALSE;
    }

    if (self->hPanel == HWND_INVALID) {
        if (createPanel(self))
            return FALSE;
    }

    if (!pCtrl || nCount == 0) {
        return TRUE;
    }

    GetClientRect (self->hwnd, &rcClient);

    maxWidth = 0;
    maxHeight = 0;

    for (i = 0; i < nCount; i++) {
        mWidget * widget = ncsCreateWindowIndirect(pCtrl, self->hPanel);

		if (widget != NULL) 
		{
			hCtrl = widget->hwnd;

			if ((pCtrl->x + pCtrl->w) > maxWidth) {
				maxWidth = pCtrl->x + pCtrl->w;
			}

			/* vertical scrollbar */
			if ((pCtrl->y + pCtrl->h) > maxHeight) {
				maxHeight = pCtrl->y + pCtrl->h;
			}
		}

        pCtrl ++;
    }

    ncsSetProperty (self->hwnd, NCSP_SWGT_CONTWIDTH, maxWidth);

    ncsSetProperty (self->hwnd, NCSP_SWGT_CONTHEIGHT, maxHeight);

    hCtrl = GetNextDlgTabItem (self->hPanel, (HWND)0, FALSE);
    SetFocus(hCtrl);

    return TRUE;
}

static BOOL mContainer_addIntrinsicControls(mContainer *self, PCTRLDATA pCtrl, int nCount)
{
    int             i;
    HWND            hCtrl;
    int             maxWidth, maxHeight;

    if (nCount < 0 || !self)
        return FALSE;

    if (self->hPanel == HWND_INVALID) {
        if (createPanel(self))
            return FALSE;
    }

    if (!pCtrl || nCount == 0) {
        return TRUE;
    }

    maxWidth = 0;
    maxHeight = 0;
    for (i = 0; i < nCount; i++) {
        hCtrl = CreateWindowEx2 (pCtrl->class_name,
                                pCtrl->caption,
                                pCtrl->dwStyle | WS_CHILD,
                                pCtrl->dwExStyle,
                                pCtrl->id,
                                pCtrl->x, pCtrl->y, pCtrl->w, pCtrl->h,
                                self->hPanel,
                                pCtrl->werdr_name,
                                pCtrl->we_attrs,
                                pCtrl->dwAddData);

        if (hCtrl != HWND_INVALID) 
		{
			if ((pCtrl->x + pCtrl->w) > maxWidth) {
				maxWidth = pCtrl->x + pCtrl->w;
			}

			/* vertical scrollbar */
			if ((pCtrl->y + pCtrl->h) > maxHeight) {
				maxHeight = pCtrl->y + pCtrl->h;
			}
		}

        pCtrl ++;
    }

    ncsSetProperty (self->hwnd, NCSP_SWGT_CONTWIDTH, maxWidth);

    ncsSetProperty (self->hwnd, NCSP_SWGT_CONTHEIGHT, maxHeight);

    hCtrl = GetNextDlgTabItem (self->hPanel, (HWND)0, FALSE);
    SetFocus(hCtrl);

    return TRUE;
}

static mComponent* mContainer_getChild(mContainer* self, int id)
{
    mWidget* panel = ncsObjFromHandle(self->hPanel);
    return Class(mWidget).getChild(panel, id);
}

static HWND mContainer_setFocus(mContainer* self, int id)
{
    mWidget* child = (mWidget*) _c(self)->getChild(self, id);

    if (child) {
        return SetFocus(child->hwnd);
    }
    return 0;
}

static HWND mContainer_getFocus(mContainer* self)
{
    return GetFocus(self->hPanel);
}

static LRESULT mContainer_wndProc (mContainer* self, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case MSG_KEYDOWN:
        {
            HWND hNewFocus;
            RECT rcFocus;

            SendAsyncMessage (self->hPanel, MSG_KEYDOWN, wParam, lParam);
            hNewFocus = GetFocus (self->hPanel);

            if (hNewFocus) {
                GetWindowRect (hNewFocus, &rcFocus);
                if (!_c(self)->isVisible(self, rcFocus.left, rcFocus.top)) {
                    _c(self)->makePosVisible (self, rcFocus.left, rcFocus.top);
                }

                if (!_c(self)->isVisible(self, rcFocus.right, rcFocus.bottom)) {
                    _c(self)->makePosVisible (self, rcFocus.right, rcFocus.bottom);
                }
            }
            return 0;
        }
    }

	return Class(mScrollWidget).wndProc((mScrollWidget*)self,
                               message, wParam, lParam);
}

HWND mContainer_getPanel(mContainer* self)
{
	if(!IsWindow(self->hPanel))
		createPanel(self);
	return self->hPanel;
}

void mContainer_adjustContent(mContainer *self)
{
	HWND hchild;
    int  maxwidth = 0, maxheight = 0;
	if(!IsWindow(self->hPanel))
		return;

	for(hchild = GetNextChild(self->hPanel, HWND_NULL);
		IsWindow(hchild);
		hchild = GetNextChild(self->hPanel,hchild))
	{
		RECT rctmp;
		GetWindowRect(hchild, &rctmp);
        if(maxwidth < rctmp.right)
            maxwidth = rctmp.right;
        if(maxheight < rctmp.bottom)
            maxheight = rctmp.bottom;
	}

	_c(self)->setProperty(self, NCSP_SWGT_CONTWIDTH, maxwidth);

	_c(self)->setProperty(self, NCSP_SWGT_CONTHEIGHT, maxheight);

	_c(self)->moveContent(self);
}

BEGIN_CMPT_CLASS(mContainer, mScrollWidget)
    CLASS_METHOD_MAP(mContainer, construct);
    CLASS_METHOD_MAP(mContainer, wndProc);
    CLASS_METHOD_MAP(mContainer, moveContent);
    CLASS_METHOD_MAP(mContainer, addIntrinsicControls);
    CLASS_METHOD_MAP(mContainer, addChildren);
    CLASS_METHOD_MAP(mContainer, getChild);
    CLASS_METHOD_MAP(mContainer, getFocus);
    CLASS_METHOD_MAP(mContainer, setFocus);
    CLASS_METHOD_MAP(mContainer, getPanel);
    CLASS_METHOD_MAP(mContainer, adjustContent);
	SET_DLGCODE(DLGC_WANTALLKEYS)
END_CMPT_CLASS

#endif		//_MGNCSCTRL_CONTAINER

