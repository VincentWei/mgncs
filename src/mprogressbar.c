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

#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSCTRL_PROGRESSBAR

static int prop_map[]={
    NCSP_PROGRESSPIECE_MAXPOS,
    NCSP_PROGRESSPIECE_MINPOS,
    NCSP_PROGRESSPIECE_CURPOS,
    NCSP_PROGRESSPIECE_LINESTEP
};

static void update_prog_part(mProgressBar* self, int old_pos, int new_pos)
{
    int greater_pos, less_pos;
    int min_pos, max_pos;
    RECT rc;

    DWORD dwStyle;

    if(old_pos == new_pos)
        return ;

    min_pos = GetBodyProp(NCSP_PROGRESSPIECE_MINPOS);
    max_pos = GetBodyProp(NCSP_PROGRESSPIECE_MAXPOS);

    if(min_pos >= max_pos)
        return;

    if(old_pos > new_pos){
        greater_pos = old_pos;
        less_pos = new_pos;
    }
    else{
        greater_pos = new_pos;
        less_pos = old_pos;
    }

    dwStyle = GetWindowStyle(self->hwnd);


    GetClientRect(self->hwnd, &rc);

    if(dwStyle & NCSS_PRGBAR_VERTICAL)
    {
        int top = rc.top;
        int bottom = rc.bottom;
        int height = RECTH(rc);
        rc.top = bottom - (height * (greater_pos - min_pos) * 256 / (max_pos - min_pos) / 256);
        rc.bottom = bottom - (height * (less_pos - min_pos) * 256 / (max_pos - min_pos) / 256);

        if(dwStyle & NCSS_PRGBAR_SHOWPERCENT)
        {
            int text_top, text_bottom;
            int font_height ;
            FONTMETRICS fm;
            GetFontMetrics(GetWindowFont(self->hwnd), &fm);
            font_height = fm.font_height + 2;

            text_top = text_bottom = (top + bottom) / 2;
            text_top -= font_height / 2;
            text_bottom += font_height /2;


            if( rc.top > text_bottom || rc.bottom < text_top)
            {
                RECT rc_text = { rc.left, text_top, rc.right, text_bottom };
                InvalidateRect(self->hwnd, &rc_text, TRUE);
            }
            else{
                if(rc.top >= text_top && rc.top <= text_bottom)
                    rc.top = text_top;
                if(rc.bottom <= text_bottom && rc.bottom >= text_top)
                    rc.bottom = text_bottom;
            }
        }
    }
    else
    {
        int left = rc.left;
        int right = rc.right;
        int width = RECTW(rc);
        rc.left = left + (width * (less_pos - min_pos) * 256 / (max_pos - min_pos) / 256);
        rc.right = left + (width * (greater_pos - min_pos) * 256 / (max_pos - min_pos) / 256);

        if(dwStyle & NCSS_PRGBAR_SHOWPERCENT)
        {
            int text_left, text_right;

            int font_width;
            FONTMETRICS fm;
            GetFontMetrics(GetWindowFont(self->hwnd), &fm);
            font_width = fm.max_width * 3;

            text_left = text_right = (left + right) / 2;
            text_left -= font_width / 2;
            text_right += font_width / 2;

            if( rc.left > text_right  || rc.right < text_left)
            {
                RECT rc_text = { text_left, rc.top, text_right, rc.bottom};
                InvalidateRect(self->hwnd, &rc_text, TRUE);
            }
            else
            {
                if( rc.left >= text_left && rc.left <= text_right)
                    rc.left = text_left;
                if( rc.right >= text_left && rc.right <= text_right)
                    rc.right = text_right;
            }


        }
    }

    InvalidateRect(self->hwnd, &rc, TRUE);

}

static BOOL mProgressBar_setProperty (mProgressBar* self, int id, DWORD value)
{
    if (id >= NCSP_PROG_MAX)
        return FALSE;

    if(id >= NCSP_PROG_MAXPOS)
    {
        if(id == NCSP_PROG_CURPOS)
        {
            int old_value;
            if(!Body)
                return FALSE;

            old_value = (int)GetBodyProp(NCSP_PROGRESSPIECE_CURPOS);
            if(SetBodyProp(NCSP_PROGRESSPIECE_CURPOS, value))
            {
                update_prog_part(self, old_value, (int)value);
                return TRUE;
            }
        }
        else if(Body && SetBodyProp(prop_map[id-NCSP_PROG_MAXPOS], value))
        {
            if(NCSP_PROG_LINESTEP == id)
                return TRUE;

            InvalidateRect(self->hwnd, NULL, TRUE);
            return TRUE;
        }
        return FALSE;
    }

    return Class(mWidget).setProperty ((mWidget*)self, id, value);
}

static DWORD mProgressBar_getProperty (mProgressBar* self, int id)
{
    if (id >= NCSP_PROG_MAX)
        return 0;

    if(id >= NCSP_PROG_MAXPOS)
    {
        return Body?GetBodyProp(prop_map[id-NCSP_PROG_MAXPOS]):0;
    }

    return Class(mWidget).getProperty ((mWidget*)self, id);
}

static int mProgressBar_increase (mProgressBar *self, int delta)
{
    mProgressPiece * piece;
    int old_pos;
    if ( delta == 0)
        return -1;

    if(self == NULL || !INSTANCEOF(self, mProgressBar))
        return -1;

    piece = SAFE_CAST(mProgressPiece,self->body);
    if(piece == NULL)
        return -1;

    old_pos = (int)GetBodyProp(NCSP_PROGRESSPIECE_CURPOS);

    _c(piece)->step(piece, delta, 0);

    update_prog_part(self, old_pos, (int)GetBodyProp(NCSP_PROGRESSPIECE_CURPOS));

    return 0;
}

static int mProgressBar_stepIncrease (mProgressBar *self)
{
    mProgressPiece * piece;
    int old_pos;

    if(!INSTANCEOF(self, mProgressBar))
        return -1;

    piece = SAFE_CAST(mProgressPiece, self->body);


    old_pos = (int)GetBodyProp(NCSP_PROGRESSPIECE_CURPOS);

    _c(piece)->step(piece,0, FALSE);

    update_prog_part(self, old_pos, (int)GetBodyProp(NCSP_PROGRESSPIECE_CURPOS));

    return 0;
}

static mObject* mProgressBar_createBody(mProgressBar * self)
{
    return (mObject*)NEWPIECE(mProgressPiece);
}

static void mProgressBar_onPaint(mProgressBar *self, HDC hdc, const CLIPRGN * inv)
{
    if(Body)
    {
        DWORD dwStyle = GetWindowStyle(self->hwnd);
        _c(Body)->paint(Body, hdc, (mObject*)self,
            (dwStyle&NCSS_PRGBAR_VERTICAL?NCS_PIECE_PAINT_VERT:0)
            | (dwStyle&NCSS_PRGBAR_BLOCKS?NCS_PIECE_PAINT_BLOCK:0)
            | (dwStyle&NCSS_PRGBAR_SHOWPERCENT?NCS_PIECE_PAINT_PERCENT:0));
    }
}


static BOOL mProgressBar_onEraseBkgnd(mProgressBar *self, HDC hdc, const RECT *pinv)
{
    DWORD dwStyle = GetWindowStyle(self->hwnd);
    if(dwStyle & NCSS_PRGBAR_BLOCKS)
        return Class(mWidget).onEraseBkgnd((mWidget*)self, hdc, pinv);

    if(Body)
    {
        int max, min, cur;
        PCLIPRGN poldrgn;
        RECT rc;

        max = GetBodyProp(NCSP_PROGRESSPIECE_MAXPOS);
        min = GetBodyProp(NCSP_PROGRESSPIECE_MINPOS);
        cur = GetBodyProp(NCSP_PROGRESSPIECE_CURPOS);
        if(cur <= min || max <= min)
            return Class(mWidget).onEraseBkgnd((mWidget*)self, hdc, pinv);

        if(cur >= max)
            return TRUE;

        GetClientRect(self->hwnd, &rc);

        //set the clip rect
        poldrgn = CreateClipRgn();
        GetClipRegion(hdc, poldrgn);
        // get the bkground area
        if(dwStyle & NCSS_PRGBAR_VERTICAL)
        {
            rc.bottom = rc.bottom - (RECTH(rc)* (cur - min)  / (max - min));
        }
        else
        {
            rc.left = rc.left + (RECTW(rc)*(cur - min)  / (max - min) );
        }

        ClipRectIntersect(hdc, &rc);

        Class(mWidget).onEraseBkgnd((mWidget*)self, hdc, pinv);

        SelectClipRegion(hdc, poldrgn);

        DestroyClipRgn(poldrgn);

        return TRUE;
    }
    return FALSE;
}

BEGIN_CMPT_CLASS (mProgressBar, mWidget)
    CLASS_METHOD_MAP (mProgressBar, createBody)
    CLASS_METHOD_MAP (mProgressBar, onPaint)
    CLASS_METHOD_MAP (mProgressBar, onEraseBkgnd)
    CLASS_METHOD_MAP (mProgressBar, setProperty)
    CLASS_METHOD_MAP (mProgressBar, getProperty)
    CLASS_METHOD_MAP (mProgressBar, increase)
    CLASS_METHOD_MAP (mProgressBar, stepIncrease)
END_CMPT_CLASS

#endif //_MGNCSCTRL_PROGRESSBAR
