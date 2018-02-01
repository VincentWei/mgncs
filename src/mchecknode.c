/*
    The implementation of mCheckNode class.
   
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

#include "mscroll_widget.h"
#include "mdblist.h"
#include "mnode.h"
#include "mabstractlist.h"
#include "mchecknode.h"

static void _set_text_color(mAbstractList *self, HDC hdc, const char* text, int clrId)
{
    gal_pixel textColor = 0;
    if (text) {
        textColor = ncsColor2Pixel(hdc, ncsGetElement((mWidget*)self, clrId));
        SetTextColor(hdc, textColor);
    }
}

static void _default_draw_bkgnd(mNode *self, HDC hdc, const RECT *rect, mObject *owner)
{
    mAbstractList *control = (mAbstractList*)(owner);

    SetBkMode (hdc, BM_TRANSPARENT);
    if (control) {
        if (!_c(self)->isEnabled(self)) {
            _set_text_color(control, hdc, self->text, NCS_FGC_DISABLED_ITEM);
            control->renderer->drawItem(control, hdc, rect, NCSRS_DISABLE);
        }
        else if (_c(self)->isSelected(self)){
            _set_text_color(control, hdc, self->text, NCS_FGC_HILIGHT_ITEM);
            control->renderer->drawItem(control, hdc, rect, NCSRS_HIGHLIGHT);
        }
        else {
            _set_text_color(control, hdc, self->text, NCS_FGC_WINDOW);
            if (_c(control)->getProperty(control, NCSP_ASTLST_TRANSPARENT) <= 0)
                control->renderer->drawItem(control, hdc, rect, NCSRS_NORMAL);
        }
    }
}

#define IS_VISIBLE_TEXT(self)  \
    (!(self->flags & NCSF_NODE_HIDETEXT) && self->text)
#define IS_VISIBLE_IMAGE(self) \
    (!(self->flags & NCSF_NODE_HIDEIMAGE) && self->image)

static void _textrs_paint(mNode *self, HDC hdc, const RECT *rect, mObject *owner)
{
    int indent = -1, inter = 2, offset = 0;
    RECT rcText , rcCheck;
	mAbstractList *control;

    _default_draw_bkgnd(self, hdc, rect, owner);

    // draw Check rect
    CopyRect(&rcCheck, rect);
    rcCheck.left += inter;
    rcCheck.right = rcCheck.left + RECTH(rcCheck);

    control = (mAbstractList*)(owner);

    if (((mCheckNode *)self)->isChecked) {
        control->renderer->drawCheckbox(control, hdc,
                &rcCheck, NCSRF_FRAME | NCSRF_FILL | NCSRS_SELECTED);
    } else {
        control->renderer->drawCheckbox(control, hdc,
                &rcCheck,  NCSRF_FRAME | NCSRF_FILL | NCSRS_NORMAL);
    }

    offset += RECTW(rcCheck) + inter;

    if (IS_VISIBLE_IMAGE(self)) {
		int top = rect->top, left;
        int w = RECTWP(rect), h = RECTWP(rect);

        if (RECTHP(rect) > self->image->bmHeight)
            top += (rect->top + rect->bottom < self->image->bmHeight) >>1;

        if (owner)
            indent = _c((mAbstractList*)owner)->getProperty((mAbstractList*)owner, NCSP_ASTLST_IMAGEINDENT);

        left = offset + rect->left + (indent > 0 ? indent : inter);
        if (w > self->image->bmWidth)
            w = self->image->bmWidth;

        if (h > self->image->bmHeight)
            h = self->image->bmHeight;

        FillBoxWithBitmap (hdc,  left, top, w, h, self->image);

        if (self->text)
            offset += (inter + w);
    }

    if (IS_VISIBLE_TEXT(self)) {
        CopyRect(&rcText, rect);
        indent = _c((mAbstractList*)owner)->getProperty((mAbstractList*)owner, NCSP_ASTLST_TEXTINDENT);
        if (indent >= 0 && self->image)
            rcText.left += indent;
        else
            rcText.left += (offset + inter);
        DrawText(hdc, self->text, -1, &rcText, DT_VCENTER | DT_SINGLELINE);
    }
}

NCS_NODE_LAYOUTOPS _textrs_checknodeops =
{
    "list",
    NCSS_NODE_LTEXTRS,
    _textrs_paint,
};

static void mCheckNode_construct(mCheckNode *self, DWORD addData)
{
	Class(mNode).construct((mNode *)self, addData);

    if ((self->style & NCSS_NODE_LAYOUT_MASK) == NCSS_NODE_LTEXTRS)
    {
        self->layout = &_textrs_checknodeops;
    }
}

static BOOL mCheckNode_setCheck(mCheckNode *self, BOOL isCheck)
{
    self->isChecked = isCheck;

    return TRUE;
}

static void mCheckNode_paint(mCheckNode *self,
        HDC hdc, const RECT *rect, mObject *owner)
{
    _c(self)->super->paint((mNode *)self, hdc, rect, owner);
}

BEGIN_MINI_CLASS(mCheckNode, mNode)
	CLASS_METHOD_MAP(mCheckNode, construct)
	CLASS_METHOD_MAP(mCheckNode, setCheck)
	CLASS_METHOD_MAP(mCheckNode, paint)
END_MINI_CLASS
