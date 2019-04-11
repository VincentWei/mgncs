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

#include "mime.h"
#include "mime-wordsel.h"

#ifdef _MGNCSCTRL_IMWORDSEL

#define ARROW_WIDTH  8

static inline mHotPiece* get_from_cell(mHotPiece* p, int n)
{
    mBoxLayoutPiece* blp = (mBoxLayoutPiece*)p;
    if(!blp)
        return NULL;
    return _c(blp)->getCell(blp, n);
}

static inline mLineLayoutPiece* get_line_layout_piece(mHotPiece* p)
{
    mSimpleListPiece* slp = (mSimpleListPiece*)(get_from_cell(p, 1));
    return mSimpleListPiece_getLineLayout(slp);
}

static void free_label_piece(mLabelPiece* lp)
{
    if(lp->str)
        FreeFixStr((char*)lp->str);
    DELPIECE(lp);
}
static void reset_words(mIMWordSel* self)
{
    mSimpleListPiece* slp = (mSimpleListPiece*)get_from_cell((mHotPiece*)self->body, 1);
    if(slp)
    {
        _c(slp)->reset(slp, (void(*)(mHotPiece*))free_label_piece);
    }
}

static void notify_sel_changed(mIMWordSel* self)
{
    int idx;
    if(!self->ime_it)
        ncsNotifyParentEx((mWidget*)self, NCSN_IMWS_SELCHANGED, -1);

    idx = self->page_start + self->cur_sel;
//overflow
    if (idx > self->page_end - 1)
        idx = self->page_end - 1;

    _c(self->ime_it)->locate(self->ime_it, idx, SEEK_SET);
    ncsNotifyParentEx((mWidget*)self, NCSN_IMWS_SELCHANGED, idx);
}

static BOOL set_cur_sel(mIMWordSel* self, int idx)
{
    int index;
    mSimpleListPiece* slp = (mSimpleListPiece*)get_from_cell((mHotPiece*)self->body, 1);
    if(self->ime_it == NULL)
        return FALSE;
//    if(self->cur_sel == idx)
//        return FALSE;
    if(idx < 0)
        return FALSE;

//overflow
    index = self->page_start + self->cur_sel;
    if (index > self->page_end - 1)
        idx = self->page_end - self->page_start - 1;

    if(_c(slp)->setProperty(slp, NCSP_SIMPLELISTPIECE_CURINDEX, idx))
    {
        self->cur_sel = idx;
        mHotPiece_update((mHotPiece*)get_line_layout_piece((mHotPiece*)(self->body)), (mObject*)self, TRUE);
        return TRUE;
    }
    return FALSE;
}

static inline mLabelPiece* new_lable_piece(const char* word)
{
    return NEWPIECEEX(mLabelPiece, (DWORD)FixStrDup(word));
}

static BOOL calc_and_add(mIMWordSel* self, BOOL bFromEnd)
{
    RECT rc;
    HDC  hdc;
    SIZE size;
    mLabelPiece * lp;
    mLineLayoutPiece * l;
    int  pos;
    char szWord[128];

//    self->cur_sel = -1;
    if(!self->ime_it || _c(self->ime_it)->count(self->ime_it) <= 0)
        return FALSE;

    hdc = GetClientDC(self->hwnd);

    l = (mLineLayoutPiece*)get_line_layout_piece((mHotPiece*)(self->body));

    _c(l)->getRect(l, &rc);

    if(bFromEnd)
    {
        int start_idx = -1;
        pos = rc.right;
        _c(self->ime_it)->locate(self->ime_it, self->page_end - 1, SEEK_SET);
        do{
            const char* word = _c(self->ime_it)->word(self->ime_it);
            if(self->ime_it->charset_converter)
                word = ncsIMConvertWord(self->ime_it->charset_converter, word, szWord, sizeof(szWord)-1);
            if(word)
            {
                RECT rc_piece;
                GetTextExtent(hdc, word, -1, &size);
                if(pos - size.cx < rc.left)
                    break;
                //new lp
                lp = new_lable_piece(word);
                if(!lp)
                    continue;
                rc_piece.left = 0;
                rc_piece.right = size.cx + 8;
                rc_piece.top   = 0;
                rc_piece.bottom = 20;
                _c(lp)->setRect(lp, &rc_piece);
                _c(l)->addPiece(l,(mHotPiece*)lp, NCS_LINELAYOUTPIECENODE_FLAG_FIXED, 0, 0);
                pos -= size.cx;
                pos -= 8;
                start_idx = _c(self->ime_it)->curIndex(self->ime_it);

            }
        }while(_c(self->ime_it)->prev(self->ime_it)>=0 && pos >= rc.left);
        self->page_start = start_idx;
    }
    else
    {
        int end_idx = 0;
        pos = rc.left;
        self->page_start = _c(self->ime_it)->locate(self->ime_it, self->page_start, SEEK_SET);
        do{
            const char* word = _c(self->ime_it)->word(self->ime_it);
            if(self->ime_it->charset_converter)
                word = ncsIMConvertWord(self->ime_it->charset_converter, word, szWord, sizeof(szWord)-1);
            if(word)
            {
                RECT rc_piece;
                GetTextExtent(hdc, word, -1, &size);
                if(pos + size.cx > rc.right)
                    break;
                //new lp
                lp = new_lable_piece(word);
                if(!lp)
                    continue;
                rc_piece.left  = 0;
                rc_piece.right = size.cx + 8;
                rc_piece.top   = 0;
                rc_piece.bottom= 20;
                _c(lp)->setRect(lp, &rc_piece);
                _c(l)->appendPiece(l, (mHotPiece*)lp,NCS_LINELAYOUTPIECENODE_FLAG_FIXED, 0, 0);
                pos += size.cx;
                pos += 8;
            }
        }while((end_idx =  _c(self->ime_it)->next(self->ime_it))>=0 && pos <= rc.right);
        if(end_idx >= 0)
            self->page_end = end_idx;
        else
            self->page_end = _c(self->ime_it)->count(self->ime_it);
    }

    ReleaseDC(hdc);

    //set_cur_sel(self, 0);
    _c(l)->recalc(l);
    mHotPiece_update((mHotPiece*)l, (mObject*)self, TRUE);
    return TRUE;
}

static void enable_arrow(mIMWordSel* self, mArrowButtonPiece* arrow, BOOL benable)
{
    BOOL bold_enable = _c(arrow)->isEnabled(arrow);
    if((bold_enable && benable) || (!bold_enable && !benable))
        return ;
    _c(arrow)->enable(arrow, benable);
    mHotPiece_update((mHotPiece*)arrow, (mObject*)self, TRUE);
}

static void change_page_updown(mIMWordSel* self)
{
    //get left and right
    mArrowButtonPiece* left_arrow  = (mArrowButtonPiece*)get_from_cell((mHotPiece*)(self->body), 0);
    mArrowButtonPiece* right_arrow = (mArrowButtonPiece*)get_from_cell((mHotPiece*)(self->body), 2);

    enable_arrow(self, left_arrow,  self->ime_it?self->page_start > 0:FALSE);
    enable_arrow(self, right_arrow, self->ime_it?(self->page_end < _c(self->ime_it)->count(self->ime_it)):FALSE);
}

////////////////////////////////////////
static BOOL mIMWordSel_onPageUp(mIMWordSel* self, mHotPiece* send, int id, DWORD value)
{
    if(self->ime_it == NULL)
        return FALSE;

    if(self->page_start > 0)
    {
        int sel = self->cur_sel;
        reset_words(self);
        self->page_end = self->page_start ;
        calc_and_add(self, TRUE);
        change_page_updown(self);

        if(send == NULL && id == -1) //use the old sel
        {
            if (sel > self->page_end - self->page_start - 1)
                sel= self->page_end - self->page_start - 1;
        }
        else
        {
            sel = self->page_end - self->page_start - 1;
        }

        if(set_cur_sel(self, sel))
            notify_sel_changed(self);
        return TRUE;
    }
    return FALSE;
}

static BOOL mIMWordSel_onPageDown(mIMWordSel* self, mHotPiece* send, int id, DWORD value)
{
    if(self->ime_it == NULL)
        return FALSE;

    if(self->page_end < _c(self->ime_it)->count(self->ime_it))
    {
        int sel = self->cur_sel;
        reset_words(self);
        self->page_start = self->page_end;
        calc_and_add(self, FALSE);
        change_page_updown(self);

        if ( send == NULL && id == -1 && sel > 0)
        {
            if( sel > self->page_end - self->page_start - 1)
                sel = self->page_end - self->page_start - 1;
        }
        else
        {
            sel = 0;
        }

        if(set_cur_sel(self, sel))
            notify_sel_changed(self);
        return TRUE;
    }
    return FALSE;
}

static BOOL mIMWordSel_onChanged(mIMWordSel* self, mHotPiece* sender, int id, DWORD value)
{
    if(self->ime_it == NULL)
        return TRUE;
    self->cur_sel = (int)value;
    notify_sel_changed(self);
    return TRUE;
}


static mHotPiece* mIMWordSel_createBody(mIMWordSel* self)
{
    mHotPiece* left_arrow, *right_arrow, *simple_list;

    mBoxLayoutPiece*  boxLayout = (mBoxLayoutPiece*)NEWPIECEEX(mHBoxLayoutPiece, 3);
    //left is the page up arrow
    _c(boxLayout)->setCellInfo(boxLayout, 0, ARROW_WIDTH, 0, 0);
    left_arrow = (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, NCS_ARROWPIECE_LEFT);
    _c(boxLayout)->setCell(boxLayout, 0, left_arrow);
    ncsAddEventListener((mObject*)left_arrow, (mObject*)self, (NCS_CB_ONPIECEEVENT)mIMWordSel_onPageUp, NCSN_ABP_CLICKED);
    //right is the page down arrow
    _c(boxLayout)->setCellInfo(boxLayout, 2, ARROW_WIDTH, 0, 0);
    right_arrow = (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, NCS_ARROWPIECE_RIGHT);
    _c(boxLayout)->setCell(boxLayout, 2, right_arrow);
    ncsAddEventListener((mObject*)right_arrow, (mObject*)self, (NCS_CB_ONPIECEEVENT)mIMWordSel_onPageDown, NCSN_ABP_CLICKED);
    //center is the mSimpleList piece
    simple_list = (mHotPiece*)NEWPIECE(mSimpleListPiece);
    _c(boxLayout)->setCell(boxLayout, 1, simple_list);
    ncsAddEventListener((mObject*)simple_list, (mObject*)self, (NCS_CB_ONPIECEEVENT)mIMWordSel_onChanged, NCSN_SIMPLELISTPIECE_CHANGED);
    ncsAddEventListener((mObject*)simple_list, (mObject*)self, (NCS_CB_ONPIECEEVENT)mIMWordSel_onPageUp, NCSN_SIMPLELISTPIECE_REACHHEAD);
    ncsAddEventListener((mObject*)simple_list, (mObject*)self, (NCS_CB_ONPIECEEVENT)mIMWordSel_onPageDown, NCSN_SIMPLELISTPIECE_REACHTAIL);

    return (mHotPiece*)boxLayout;
}

static void set_ime_iterator(mIMWordSel* self, mIMIterator* it)
{
    reset_words(self);
    self->ime_it = it;
    self->cur_sel = -1;
    self->page_start = 0;
    self->page_end = 0;
    if(it)
    {
        calc_and_add(self, FALSE);
    }
    else
    {
        self->cur_sel = self->page_start = self->page_end  = 0;
    }

    change_page_updown(self);

    InvalidateRect(self->hwnd, NULL, TRUE);
}

static BOOL mIMWordSel_setProperty(mIMWordSel* self, int id, DWORD value)
{
    if(id >= NCSP_IMWS_MAX)
        return FALSE;

    switch(id)
    {
    case NCSP_IMWS_ITERATOR:
        set_ime_iterator(self, (mIMIterator*)value);
        return TRUE;
    case NCSP_IMWS_CURSEL:
        return set_cur_sel(self, (int)value);
    case NCSP_IMWS_ISACTIVE:
        return self->isactive = (int)value;
    }

    return _SUPER(mWidget, self, setProperty, id, value);
}

static DWORD mIMWordSel_getProperty(mIMWordSel* self, int id)
{

    if(id >= NCSP_IMWS_MAX)
        return 0;

    switch(id)
    {
    case NCSP_IMWS_ITERATOR:
        if(self->ime_it == NULL)
            return 0;
        return (DWORD)(self->ime_it);
    case NCSP_IMWS_CURSEL:
        if(self->ime_it == NULL)
            return 0;
        return (DWORD)(self->cur_sel);
    case NCSP_IMWS_CURWORD:
        if(self->ime_it)
        {
            mSimpleListPiece *slp = (mSimpleListPiece*)get_from_cell((mHotPiece*)(self->body), 1);
            if(slp && slp->cur_node)
                return _c(slp->cur_node->piece)->getProperty(slp->cur_node->piece, NCSP_LABELPIECE_LABEL);
        }
        return 0;
    case NCSP_IMWS_CURRAWWORD:
        if(self->ime_it)
        {
            //locate
            _c(self->ime_it)->locate(self->ime_it, self->page_start + self->cur_sel ,SEEK_SET);
            return (DWORD)_c(self->ime_it)->word(self->ime_it);
        }
        return 0;
    case NCSP_IMWS_ISEMPTY:
        return self->ime_it?(_c(self->ime_it)->count(self->ime_it)<=0):TRUE;
    case NCSP_IMWS_ISACTIVE:
        return self->isactive;
    }


    return _SUPER(mWidget, self, getProperty, id);
}

static void mIMWordSel_destroy(mIMWordSel* self)
{
    reset_words(self);
    _SUPER(mWidget, self, destroy);
}

static BOOL mIMWordSel_wndProc(mIMWordSel * self, UINT message,  WPARAM wParam, LPARAM lParam)
{
    if(message == MSG_KEYDOWN)
    {
        if(!self->ime_it)
            return FALSE;

        switch(wParam)
        {
            case SCANCODE_PAGEDOWN:
            case SCANCODE_CURSORBLOCKDOWN:
                mIMWordSel_onPageDown(self, NULL, -1, 0);
                break;
            case SCANCODE_PAGEUP:
            case SCANCODE_CURSORBLOCKUP:
                mIMWordSel_onPageUp(self, NULL, -1, 0);
                                break;
            case SCANCODE_CURSORBLOCKLEFT:
                if(self->cur_sel <= 0/*self->page_start*/)
                {
                    if (mIMWordSel_onPageUp(self, NULL, 0, 0))
                        _c(self)->setProperty(self, NCSP_IMWS_CURSEL, self->page_end - self->page_start -1);
                    break;
                }
                if(set_cur_sel(self, self->cur_sel - 1))
                    notify_sel_changed(self);
                break;
            case SCANCODE_CURSORBLOCKRIGHT:
                if(self->cur_sel >= self->page_end - self->page_start - 1)
                {
                    if (mIMWordSel_onPageDown(self, NULL, 0, 0))
                        _c(self)->setProperty(self, NCSP_IMWS_CURSEL, 0);
                    break;
                }
                if(set_cur_sel(self, self->cur_sel + 1))
                    notify_sel_changed(self);
                break;
            case SCANCODE_HOME:
                if(self->page_start > 0)
                {
                    self->page_start = 0;
                    calc_and_add(self, FALSE);
                    change_page_updown(self);
                }
                if(set_cur_sel(self, 0))
                    notify_sel_changed(self);
                break;
            case SCANCODE_END:
                if(self->page_end < _c(self->ime_it)->count(self->ime_it))
                {
                    self->page_end = _c(self->ime_it)->count(self->ime_it) - 1;
                    calc_and_add(self, TRUE);
                    change_page_updown(self);
                }
                if(set_cur_sel(self, self->page_end - self->page_start - 1))
                    notify_sel_changed(self);
                break;
            default:
                return FALSE;
        }
        return TRUE;
    }

    return _SUPER(mWidget, self, wndProc, message, wParam, lParam);
}

BOOL mIMWordSel_nextPage(mIMWordSel* self)
{
    return mIMWordSel_onPageDown(self, NULL, 0, 0);
}

BOOL mIMWordSel_prevPage(mIMWordSel* self)
{
    return mIMWordSel_onPageUp(self, NULL, 0, 0);
}

BOOL mIMWordSel_selNext(mIMWordSel* self)
{
    if(set_cur_sel(self, self->cur_sel + 1))
    {
        notify_sel_changed(self);
        return TRUE;
    }
    return FALSE;
}

BOOL mIMWordSel_selPrev(mIMWordSel* self)
{
    if(set_cur_sel(self, self->cur_sel - 1))
    {
        notify_sel_changed(self);
        return TRUE;
    }
    return FALSE;
}

BEGIN_CMPT_CLASS(mIMWordSel, mWidget)
    CLASS_METHOD_MAP(mIMWordSel, createBody)
    CLASS_METHOD_MAP(mIMWordSel, setProperty)
    CLASS_METHOD_MAP(mIMWordSel, getProperty)
    CLASS_METHOD_MAP(mIMWordSel, destroy)
    CLASS_METHOD_MAP(mIMWordSel, wndProc)
    CLASS_METHOD_MAP(mIMWordSel, nextPage)
    CLASS_METHOD_MAP(mIMWordSel, prevPage)
    CLASS_METHOD_MAP(mIMWordSel, selNext)
    CLASS_METHOD_MAP(mIMWordSel, selPrev)
END_CMPT_CLASS

#endif        //_MGNCSCTRL_IMWORDSEL
