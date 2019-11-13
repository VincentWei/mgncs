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
    The implementation of mListLayout class.

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

#ifdef _MGNCSCTRL_LIST

#define pList (self->list)
/*=============================================================*/
extern BOOL mList_adjustNodesHeight(mList *self, int diff);
extern BOOL mList_adjustNodesWidth(mList *self, int diff);
extern void mList_drawFocusFrame(mList *self, mNode *node, HDC hdc, RECT *rcNode);
#define _ADJUST_NODES_HEIGHT(self, diff)    mList_adjustNodesHeight((mList*)self, diff)
#define _ADJUST_NODES_WIDTH(self, diff)    mList_adjustNodesHeight((mList*)self, diff)
#define _DRAW_FOCUS_FRAME(self, node, hdc, rcNode)       \
    mList_drawFocusFrame((mList*)self, node, hdc, rcNode);

static void mListLayout_construct(mListLayout *self, DWORD  addData)
{
    Class(mObject).construct((mObject*)self, addData);
    self->list = NULL;
    self->itemHeight = 18;
    self->itemWidth = 0;
}

static void mListLayout_destroy(mListLayout *self)
{
    Class(mObject).destroy((mObject*)self);
}

#define _LAYOUT_BAK_VAL(self, refresh, oldVal, isH) \
    if(!refresh) oldVal = _c(pList)->getProperty(pList, isH?NCSP_SWGT_CONTY:NCSP_SWGT_CONTX);

#define _LAYOUT_UPDATE_VAL(self, refresh, oldVal, isH)  \
    if(!refresh && oldVal != _c(pList)->getProperty(pList, isH?NCSP_SWGT_CONTY:NCSP_SWGT_CONTX)) refresh = TRUE;

//If has called moveContent, it will return TRUE; otherwise return FALSE;
static BOOL mListLayout_recalcContSize(mListLayout *self, int flags)
{
    HDC     hdc = GetClientDC(pList->hwnd);
    SIZE    size;
    BOOL    update = FALSE;
    mNode*  first ;
    int     oldVal = 0, width = 0, maxWidth = 0, totalHeight = 0;

    if (!self || !pList || !flags)
        return FALSE;

    first = _c(pList)->getNode(pList, 0);

    while(first) {
        _c(first)->computeSize(first, hdc, &size);
        width = size.cx;
        if (maxWidth < width)
            maxWidth = width;
        totalHeight += size.cy;
        first = _c(first)->getNext(first);
    }
    ReleaseDC(hdc);

    maxWidth = maxWidth  > pList->visWidth ? maxWidth : pList->visWidth;
    if (flags & NCSF_LIST_LAYOUT_ITEMWCHANGED) {
        _LAYOUT_BAK_VAL(self, update, oldVal, FALSE);
        if (_c(pList)->setProperty(pList, NCSP_SWGT_CONTWIDTH, maxWidth)) {
            _LAYOUT_UPDATE_VAL(self, update, oldVal, FALSE);
        }
    }

    if (flags & NCSF_LIST_LAYOUT_ITEMHCHANGED) {
        _LAYOUT_BAK_VAL(self, update, oldVal, TRUE);
        if (_c(pList)->setProperty(pList, NCSP_SWGT_CONTHEIGHT, totalHeight)) {
            _LAYOUT_UPDATE_VAL(self, update, oldVal, TRUE);
        }
    }

    return update;
}

//return update flags
static inline BOOL _init_layout(mListLayout *self, mAbstractList *list,
        int defItemW, int defItemH)
{
    if (!self || !list)
        return FALSE;

    self->list = list;
    if (defItemH > 0 && self->itemHeight != defItemH) {
        self->itemHeight= defItemH;
    }

    if (defItemW > 0 && self->itemWidth != defItemW) {
        self->itemWidth = defItemW;
    }
    return TRUE;
}

static void mListLayout_init(mListLayout *self,
        mAbstractList *list, int defItemW, int defItemH)
{
    if (!_init_layout(self, list, defItemW, defItemH))
        return;

    if (!_c(self)->recalcContSize(self, NCSF_LIST_LAYOUT_ITEMHCHANGED | NCSF_LIST_LAYOUT_ITEMWCHANGED) && pList)
        InvalidateRect(pList->hwnd, NULL, TRUE);
}

static BOOL mListLayout_setProperty(mListLayout *self, int id, DWORD value)
{
    if( id >= NCSP_LIST_LAYOUT_MAX || !self)
        return FALSE;

    switch (id) {
        case NCSP_LIST_LAYOUT_DEFITEMHEIGHT:
            if (value > 0 && value != self->itemHeight) {
                self->itemHeight = value;
                if (!_c(self)->recalcContSize(self, NCSF_LIST_LAYOUT_ITEMHCHANGED)) {
                    InvalidateRect(pList->hwnd, NULL, TRUE);
                }
                return TRUE;
            }
            break;
    }
    return FALSE;
}

static DWORD mListLayout_getProperty(mListLayout *self, int id)
{
    if( id >= NCSP_LIST_LAYOUT_MAX || !self)
        return -1;

    switch (id) {
        case NCSP_LIST_LAYOUT_DEFITEMHEIGHT:
            return self->itemHeight;

        case NCSP_LIST_LAYOUT_DEFITEMWIDTH:
            return self->itemWidth;
    }
    return -1;
}

static int mListLayout_getNodeRect(mListLayout *self, mNode *node, RECT *rcNode)
{
    mNode *first;
    int height = 0;
    BOOL found = FALSE;

    if (!self || !pList || !rcNode || !node)
        return -1;

    first  = _c(pList)->getNode(pList, 0);

    while (first) {
        if (first == node) {
            found = TRUE;
            break;
        }

        height += _c(first)->getProperty(first, NCSP_NODE_HEIGHT);
        first = _c(first)->getNext(first);
    }

    if (found) {
        rcNode->top = height;
        rcNode->left = 0;
        rcNode->right = pList->visWidth > pList->realContWidth ?
            rcNode->left + pList->visWidth : rcNode->left + pList->contWidth;
        rcNode->bottom = rcNode->top + _c(node)->getProperty(node, NCSP_NODE_HEIGHT);

        return 0;
    }
    return -1;
}

static mNode* mListLayout_onMouseHit(mListLayout *self, int mouseX, int mouseY)
{
    int nodesHeight;
    mNode* tmp;
    int h;
    if (!self || !pList)
        return NULL;

    nodesHeight = _c(pList)->getProperty(pList, NCSP_SWGT_CONTHEIGHT);
    if (mouseY < 0 || mouseY >= nodesHeight)
        return NULL;

    tmp = _c(pList)->getNode(pList, 0);
    h = 0;

    while (tmp) {
        h += _c(tmp)->getProperty(tmp, NCSP_NODE_HEIGHT);

        if (h > mouseY) {
            return tmp;
        }
        tmp = _c(tmp)->getNext(tmp);
    }

    return NULL;
}

static void mListLayout_update(mListLayout *self,
        HDC hdc, const RECT* rcVis, RECT *rcCont)
{
    int height;
    mNode *node;
    if (!self || !pList || !rcVis || !rcCont || RECTWP(rcCont) <= 0)
        return;

    height = 0;
    node = _c(pList)->getNode(pList, 0);

    while (node) {
        rcCont->top += height;
        height = _c(node)->getProperty(node, NCSP_NODE_HEIGHT);
        rcCont->bottom = rcCont->top + height;
        if (rcCont->bottom < rcVis->top) {
            node = _c(node)->getNext(node);
            continue;
        }

        if (rcCont->top > rcVis->bottom)
            break;

        if (_c(node)->paint && RECTHP(rcCont) > 0) {
            _c(node)->paint(node, hdc, rcCont, (mObject*)pList);
        }

        _DRAW_FOCUS_FRAME(pList, node, hdc, rcCont);
        node = _c(node)->getNext(node);
    }
}

static mNode* mListLayout_onDirKey(mListLayout *self,
        mNode *refNode, int direct)
{
    BOOL loop = direct & NCSF_LIST_LOOP;
    mNode *node = NULL;
    int direct_flags = direct & NCSF_LIST_DIRMASK;
    if (!self)
        return 0;

    switch (direct_flags) {
        case NCSF_LIST_UP:
        case NCSF_LIST_LEFT:
            if (refNode)
                node = _c(refNode)->getPrev(refNode);

            if (loop && !node) {
                node = (mNode*)_c(pList->root)->getProperty(pList->root, NCSP_NODE_LASTCHILD);
            }
            break;

        case NCSF_LIST_DOWN:
        case NCSF_LIST_RIGHT:
            if (refNode)
                node = _c(refNode)->getNext(refNode);

            if (loop && !node)
                node = _c(pList)->getNode(pList, 0);
            break;

        case NCSF_LIST_HOME:
            node = _c(pList)->getNode(pList, 0);
            break;

        case NCSF_LIST_END:
            node = (mNode*)_c(pList->root)->getProperty(pList->root, NCSP_NODE_LASTCHILD);
            break;
    }

    while (node && _c(node)->getProperty(node, NCSP_NODE_HEIGHT) <= 0) {
        node = (direct_flags == NCSF_LIST_DOWN || direct_flags == NCSF_LIST_HOME)
                ? _c(node)->getNext(node) :_c(node)->getPrev(node);
    }

    if (!node && loop && (direct_flags == NCSF_LIST_DOWN))
        node = _c(pList)->getNode(pList, 0);

    return node;
}

static void _list_refresh_nodes(mListLayout *self, mNode *node)
{
    if (self && pList && !_c(pList)->isFrozen(pList) && node) {
        RECT rect;
        if (_c(pList)->getRect(pList, node, &rect, TRUE) == 0) {
            rect.right = rect.left + pList->contWidth;
            rect.bottom = rect.top + pList->visHeight;
            InvalidateRect(pList->hwnd, &rect, TRUE);
        }
    }
}

static void mListLayout_nodeEvent(mListLayout *self,
        int eventId, DWORD eventInfo)
{
    if (!self || !pList)
        return;

    switch(eventId) {
        case NCSE_LIST_ROOTCHANGED:
        case NCSE_LIST_SIZECHANGED:
            if (!_c(self)->recalcContSize(self, NCSF_LIST_LAYOUT_ITEMWCHANGED | NCSF_LIST_LAYOUT_ITEMHCHANGED))
                InvalidateRect(pList->hwnd, NULL, TRUE);
            break;

        case NCSE_LIST_FONTCHANGED:
            if (!_c(self)->recalcContSize(self, NCSF_LIST_LAYOUT_ITEMWCHANGED))
                InvalidateRect(pList->hwnd, NULL, TRUE);

            break;

        case NCSE_NODE_CONTENTCHANGED:
        {
            _c(self)->recalcContSize(self, NCSF_LIST_LAYOUT_ITEMWCHANGED);
            _c(pList)->refreshNode(pList, (mNode *)eventInfo, NULL);
            break;
        }

        case NCSE_NODE_HEIGHTCHANGED:
        {
            mNodeEventData *eventData = (mNodeEventData*)eventInfo;
            if (eventData && eventData->data) {
                int oldVal;
                BOOL update = FALSE;

                _LAYOUT_BAK_VAL(self, update, oldVal, TRUE);
                if (_ADJUST_NODES_HEIGHT(pList, eventData->data))
                    _LAYOUT_UPDATE_VAL(self, update, oldVal, TRUE);

                //need refresh the following node
                if (!update) {
                    _list_refresh_nodes(self, eventData->source);
                }
            }
            break;
        }

        case NCSE_NODE_NODEADDED:
        {
            mNode   *node = (mNode *)eventInfo;
            SIZE    size;
            int     oldVal;
            BOOL    update = FALSE;
            HDC     hdc = GetClientDC(pList->hwnd);

            _c(node)->computeSize(node, hdc, &size);
            ReleaseDC(hdc);

            //change content width
            if (pList->realContWidth < size.cx) {
                _c(pList)->setProperty(pList, NCSP_SWGT_CONTWIDTH, size.cx);
            }
            node = _c(node)->getNext(node);

            _LAYOUT_BAK_VAL(self, update, oldVal, TRUE);
            if (_ADJUST_NODES_HEIGHT(pList, size.cy))
                _LAYOUT_UPDATE_VAL(self, update, oldVal, TRUE);

            if (node && !update) {
                //refresh the following node
                _list_refresh_nodes(self, node);
            }
            break;
        }

        case NCSE_NODE_NODEDELETED:
        {
            if (eventInfo && !_M(pList, isFrozen)) {
                //refresh the following node
                if (!_c(self)->recalcContSize(self, NCSF_LIST_LAYOUT_ITEMWCHANGED | NCSF_LIST_LAYOUT_ITEMHCHANGED))
                    _list_refresh_nodes(self, _c((mNode*)eventInfo)->getNext((mNode*)eventInfo));
            }
            break;
        }
    }
}

BEGIN_MINI_CLASS(mListLayout, mObject)
    CLASS_METHOD_MAP(mListLayout, construct)
    CLASS_METHOD_MAP(mListLayout, init)
    CLASS_METHOD_MAP(mListLayout, destroy)
    CLASS_METHOD_MAP(mListLayout, setProperty)
    CLASS_METHOD_MAP(mListLayout, getProperty)
    CLASS_METHOD_MAP(mListLayout, onMouseHit)
    CLASS_METHOD_MAP(mListLayout, onDirKey)
    CLASS_METHOD_MAP(mListLayout, getNodeRect)
    CLASS_METHOD_MAP(mListLayout, nodeEvent)
    CLASS_METHOD_MAP(mListLayout, update)
    CLASS_METHOD_MAP(mListLayout, recalcContSize)
END_MINI_CLASS

/*============================mLVIconLayout=======================*/

#define GETRECT_EX(func, clstype, member, op1, op2)             \
    void func(clstype* self, int index, RECT* rc)               \
    {                                                           \
        rc->top = (index op1 self->member) * self->itemHeight;  \
        rc->left = (index op2 self->member) * self->itemWidth;  \
        rc->right = rc->left + self->itemWidth;                 \
        rc->bottom = rc->top + self->itemHeight;                \
    }

GETRECT_EX(_lvicon_getrect_byidx, mLVIconLayout, nrCol, /, %);
GETRECT_EX(_lhicon_getrect_byidx, mLHIconLayout, nrRow, %, /);

#define GETFUNC_ICON_REFRESH_NODES(func, clstype, t, r, b, l, defsize,  cb_get_rect_byidx)\
    void func(clstype *self, mNode *node, int lastIdx, int offTop, int offIdx) \
    {                                                                   \
        if (self && pList && !_c(pList)->isFrozen(pList)) {                    \
            if(!node) {                                                 \
                RECT lastrc;                                            \
                cb_get_rect_byidx(self, lastIdx, &lastrc);              \
                if (offTop)                                             \
                    OffsetRect(&lastrc, 0, offTop);                     \
                _c(pList)->contentToWindow(pList, &lastrc.left, &lastrc.top);  \
                _c(pList)->contentToWindow(pList, &lastrc.right, &lastrc.bottom);\
                InvalidateRect(pList->hwnd, &lastrc, TRUE);             \
            }                                                           \
            else {                                                      \
                RECT rect;                                              \
                if (_c(node)->indexOf(node) - offIdx == lastIdx) {            \
                    _c(pList)->refreshNode(pList, node, NULL);                 \
                    return;                                             \
                }                                                       \
                if (_c(pList)->getRect(pList, node, &rect, FALSE) == 0) {      \
                    RECT lastrc;                                        \
                    cb_get_rect_byidx(self, lastIdx, &lastrc);          \
                    if (offTop)                                         \
                        OffsetRect(&lastrc, 0, offTop);                 \
                    if (rect.t== lastrc.t) {                            \
                        rect.r= lastrc.r;                               \
                    }                                                   \
                    else {                                              \
                        rect.r= pList->defsize;                         \
                        if (rect.b== lastrc.t) {                        \
                            lastrc.l= 0;                                \
                        }                                               \
                        else {                                          \
                            RECT tmprc;                                 \
                            tmprc.t = rect.b;                           \
                            tmprc.r = pList->defsize;                   \
                            tmprc.b = lastrc.t;                         \
                            tmprc.l = 0;                                \
                            _c(pList)->contentToWindow(pList, &tmprc.left, &tmprc.top);    \
                            _c(pList)->contentToWindow(pList, &tmprc.right, &tmprc.bottom);\
                            InvalidateRect(pList->hwnd, &tmprc, TRUE);  \
                        }   \
                        _c(pList)->contentToWindow(pList, &lastrc.left, &lastrc.top);      \
                        _c(pList)->contentToWindow(pList, &lastrc.right, &lastrc.bottom);  \
                        InvalidateRect(pList->hwnd, &lastrc, TRUE);                 \
                    }\
                    _c(pList)->contentToWindow(pList, &rect.left, &rect.top);      \
                    _c(pList)->contentToWindow(pList, &rect.right, &rect.bottom);  \
                    InvalidateRect(pList->hwnd, &rect, TRUE);               \
                }   \
            }       \
        }           \
    }


GETFUNC_ICON_REFRESH_NODES(_lvicon_refresh_nodes, mLVIconLayout, top, right, bottom, left, visWidth, _lvicon_getrect_byidx);
GETFUNC_ICON_REFRESH_NODES(_lhicon_refresh_nodes, mLHIconLayout, left, bottom, right, top, visHeight, _lhicon_getrect_byidx);

static BOOL mLVIconLayout_recalcContSize(mLVIconLayout *self, int flags)
{
    int height = 0, count, oldVal = 0;
    BOOL update = FALSE;

    if (!self || !pList || !flags)
        return FALSE;

    if (flags & NCSF_LIST_LAYOUT_ITEMWCHANGED) {
        _LAYOUT_BAK_VAL(self, update, oldVal, FALSE);
        if (_c(pList)->setProperty(pList, NCSP_SWGT_CONTWIDTH, pList->visWidth)) {
            _LAYOUT_UPDATE_VAL(self, update, oldVal, FALSE);
        }
    }

    self->nrCol = pList->visWidth > self->itemWidth ? pList->visWidth/self->itemWidth : 1;
    count = _c(pList->root)->getProperty(pList->root, NCSP_NODE_CHILDRENCOUNT);
    if (count >= 0) {
        height = self->itemHeight * (count / self->nrCol + ((count % self->nrCol) ? 1 : 0));
    }

    _LAYOUT_BAK_VAL(self, update, oldVal, TRUE);
    if (_c(pList)->setProperty(pList, NCSP_SWGT_CONTHEIGHT, height)) {
        _LAYOUT_UPDATE_VAL(self, update, oldVal, TRUE);
    }

    return update;
}

static void mLVIconLayout_construct(mLVIconLayout *self, DWORD addData)
{
    Class(mListLayout).construct((mListLayout*)self, addData);
    self->itemHeight = 64;
    self->itemWidth = 64;
    self->nrCol = 1;
}

static BOOL mLVIconLayout_setProperty(mLVIconLayout *self, int id, DWORD value)
{
    if( id >= NCSP_LIST_LAYOUT_MAX || !self)
        return FALSE;

    switch (id) {
        case NCSP_LIST_LAYOUT_DEFITEMHEIGHT:
            return Class(mListLayout).setProperty((mListLayout*)self, id, value);

        case NCSP_LIST_LAYOUT_DEFITEMWIDTH:
            if (value > 0 && value != self->itemWidth) {
                self->itemWidth = value;
                if (!_c(self)->recalcContSize(self, NCSF_LIST_LAYOUT_ITEMWCHANGED))
                    InvalidateRect(pList->hwnd, NULL, TRUE);
                return TRUE;
            }
            break;
    }
    return FALSE;
}

static int mLVIconLayout_getNodeRect(mLVIconLayout *self, mNode *node, RECT *rcNode)
{
    int index;
    if (!self || !pList || !rcNode || !node)
        return -1;

    index = _c(pList)->indexOf(pList, node);
    if (index < 0)
        return -1;

    _lvicon_getrect_byidx(self, index, rcNode);
    return 0;
}

static mNode* mLVIconLayout_onMouseHit(mLVIconLayout *self, int mouseX, int mouseY)
{
    int index;
    if (!self || !pList || mouseY < 0 || mouseX < 0)
        return NULL;

    if (mouseX > self->itemWidth * self->nrCol)
        return NULL;

    index = (mouseY/self->itemHeight)* self->nrCol + mouseX / self->itemWidth;
    return _c(pList)->getNode(pList, index);
}

static void mLVIconLayout_update(mLVIconLayout *self,
        HDC hdc, const RECT* rcVis, RECT *rcCont)
{
    int i = 0;
    mNode *node;
    RECT rcDraw;
    if (!self || !pList || !rcVis || !rcCont || RECTWP(rcCont) <= 0)
        return;

    node = _c(pList)->getNode(pList, 0);

    while(node) {
        _lvicon_getrect_byidx(self, i, &rcDraw);
        OffsetRect(&rcDraw, rcCont->left, rcCont->top);

        if (rcDraw.bottom < rcVis->top) {
            node = _c(node)->getNext(node);
            i++;
            continue;
        }

        if (rcDraw.top > rcVis->bottom)
            break;

        if (_c(node)->paint) {
            _c(node)->paint(node, hdc, &rcDraw, (mObject*)pList);
        }

        _DRAW_FOCUS_FRAME(pList, node, hdc, &rcDraw);
        node = _c(node)->getNext(node);
        i++;
    }
}

static mNode* mLVIconLayout_onDirKey(mLVIconLayout *self,
        mNode *refNode, int direct)
{
    BOOL loop = direct & NCSF_LIST_LOOP;
    mNode *node = NULL;
    int direct_flags, dstIdx;
    int count;
    if (!self)
        return NULL;
    count = _c(pList->root)->getProperty(pList->root, NCSP_NODE_CHILDRENCOUNT);
    if (count <= 0)
        return refNode;

    direct_flags = direct & NCSF_LIST_DIRMASK;

    switch (direct_flags) {
        case NCSF_LIST_UP:
        {
            if (refNode) {
                int index = _c(refNode)->indexOf(refNode);
                dstIdx = index - self->nrCol;
                if (dstIdx < 0) {
                    if (!loop) {
                        return refNode;
                    }
                    dstIdx = (count/self->nrCol) * self->nrCol + index;
                    while (dstIdx >= count) {
                        dstIdx -= self->nrCol;
                    }
                }

                if (dstIdx != index) node = _c(pList)->getNode(pList, dstIdx);
                return node ? node : refNode;
            }
            break;
        }

        case NCSF_LIST_LEFT:
            if (refNode)
                node = _c(refNode)->getPrev(refNode);

            if (loop && !node) {
                node = (mNode*)_c(pList->root)->getProperty(pList->root, NCSP_NODE_LASTCHILD);
            }
            break;

        case NCSF_LIST_DOWN:
        {
            if (refNode) {
                int index = _c(refNode)->indexOf(refNode);
                dstIdx = index + self->nrCol;
                if (dstIdx >= count) {
                    if (!loop)
                        return refNode;
                    dstIdx = index%self->nrCol;
                }

                if (dstIdx != index) node = _c(pList)->getNode(pList, dstIdx);
                return node ? node : refNode;
            }
            break;
        }

        case NCSF_LIST_RIGHT:
            if (refNode)
                node = _c(refNode)->getNext(refNode);

            if (loop && !node)
                node = _c(pList)->getNode(pList, 0);
            break;

        case NCSF_LIST_HOME:
            node = _c(pList)->getNode(pList, 0);
            break;

        case NCSF_LIST_END:
            node = (mNode*)_c(pList->root)->getProperty(pList->root, NCSP_NODE_LASTCHILD);
            break;
    }

    while (node && _c(node)->getProperty(node, NCSP_NODE_HEIGHT) <= 0) {
        node = (direct_flags == NCSF_LIST_DOWN || direct_flags == NCSF_LIST_HOME)
                ? _c(node)->getNext(node) :_c(node)->getPrev(node);
    }

    if (!node && loop && (direct_flags == NCSF_LIST_DOWN))
        node = _c(pList)->getNode(pList, 0);

    return node;
}

static void mLVIconLayout_nodeEvent(mLVIconLayout *self,
        int eventId, DWORD eventInfo)
{
    if (!self || !pList)
        return;
    switch(eventId) {
        case NCSE_LIST_FONTCHANGED:
        case NCSE_NODE_HEIGHTCHANGED:
            //nothing to do
            break;

        case NCSE_LIST_ROOTCHANGED:
        case NCSE_LIST_SIZECHANGED:
            return Class(mListLayout).nodeEvent((mListLayout*)self, eventId, eventInfo);

        case NCSE_NODE_CONTENTCHANGED:
            _c(pList)->refreshNode(pList, (mNode*)eventInfo, NULL);
            break;

        case NCSE_NODE_NODEADDED:
        {
            mNode *node = (mNode*)eventInfo;

            if (node) {
                BOOL refresh = FALSE;
                int count = _c(pList->root)->getProperty(pList->root, NCSP_NODE_CHILDRENCOUNT);

                if (self->nrCol == 1 || count%self->nrCol == 1) {
                    int oldVal;
                    _LAYOUT_BAK_VAL(self, refresh, oldVal, TRUE);
                    if (_ADJUST_NODES_HEIGHT(pList, self->itemHeight)) {
                        _LAYOUT_UPDATE_VAL(self, refresh, oldVal, TRUE);
                    }
                }

                if (!refresh)
                    _lvicon_refresh_nodes(self, node, count - 1, 0, 0);
            }
            break;
        }

        case NCSE_NODE_NODEDELETED:
        {
            if (eventInfo) {
                mNode *parentNode, *node = (mNode*)eventInfo;
                int oldVal, count;
                BOOL refresh = FALSE;

                parentNode = (mNode*)_c(node)->getProperty(node, NCSP_NODE_PARENTNODE);
                count = _c(parentNode)->getProperty(parentNode, NCSP_NODE_CHILDRENCOUNT);

                if (count%self->nrCol == 0) {
                    _LAYOUT_BAK_VAL(self, refresh, oldVal, TRUE);
                    if (_ADJUST_NODES_HEIGHT(pList, -(self->itemHeight))) {
                        _LAYOUT_UPDATE_VAL(self, refresh, oldVal, TRUE);
                    }
                }

                node = _c(node)->getNext(node);
                if (!refresh && (node || count%self->nrCol != 0)) {
                    _lvicon_refresh_nodes(self, node, count, 0, 0);
                }
            }
        }
    }
}

BEGIN_MINI_CLASS(mLVIconLayout, mListLayout)
    CLASS_METHOD_MAP(mLVIconLayout, construct)
    CLASS_METHOD_MAP(mLVIconLayout, setProperty)
    CLASS_METHOD_MAP(mLVIconLayout, onMouseHit)
    CLASS_METHOD_MAP(mLVIconLayout, onDirKey)
    CLASS_METHOD_MAP(mLVIconLayout, getNodeRect)
    CLASS_METHOD_MAP(mLVIconLayout, nodeEvent)
    CLASS_METHOD_MAP(mLVIconLayout, update)
    CLASS_METHOD_MAP(mLVIconLayout, recalcContSize)
END_MINI_CLASS

/*==========================mLHIconLayout==================================*/
static BOOL mLHIconLayout_recalcContSize(mLHIconLayout *self, int flags)
{
    int count, oldVal = 0;
    BOOL update = FALSE;

    if (!self || !pList || !flags)
        return FALSE;

    if (flags & NCSF_LIST_LAYOUT_ITEMHCHANGED) {
        _LAYOUT_BAK_VAL(self, update, oldVal, TRUE);
        if (_c(pList)->setProperty(pList, NCSP_SWGT_CONTHEIGHT, self->nrRow * self->itemHeight)) {
            _LAYOUT_UPDATE_VAL(self, update, oldVal, TRUE);
        }
    }

    self->nrRow = pList->visHeight > self->itemHeight ? pList->visHeight/self->itemHeight : 1;

    count = _c(pList->root)->getProperty(pList->root, NCSP_NODE_CHILDRENCOUNT);
    if (count >= 0) {
        int width;
        int col = ((count % self->nrRow) ? 1 : 0) + (count / self->nrRow);
        width = col * self->itemWidth;
        width = width > pList->visWidth ? width : pList->visWidth;

        _LAYOUT_BAK_VAL(self, update, oldVal, FALSE);

        if (_c(pList)->setProperty(pList, NCSP_SWGT_CONTWIDTH, width)) {
            _LAYOUT_BAK_VAL(self, update, oldVal, FALSE);
        }
    }

    return update;
}

static void mLHIconLayout_construct(mLHIconLayout *self, DWORD addData)
{
    Class(mListLayout).construct((mListLayout*)self, addData);
    self->nrRow = 1;
    self->itemHeight = 64;
    self->itemWidth = 64;
}

static BOOL mLHIconLayout_setProperty(mLHIconLayout *self, int id, DWORD value)
{
    if( id >= NCSP_LIST_LAYOUT_MAX || !self)
        return FALSE;

    switch (id) {
        case NCSP_LIST_LAYOUT_DEFITEMHEIGHT:
            return Class(mListLayout).setProperty((mListLayout*)self, id, value);

        case NCSP_LIST_LAYOUT_DEFITEMWIDTH:
            if (value > 0 && value != self->itemWidth) {
                self->itemWidth = value;
                _c(self)->recalcContSize(self, NCSF_LIST_LAYOUT_ITEMWCHANGED);
                return TRUE;
            }
            break;
    }
    return FALSE;
}

static int mLHIconLayout_getNodeRect(mLHIconLayout *self, mNode *node, RECT *rcNode)
{
    int index;
    if (!self || !pList || !rcNode || !node)
        return -1;

    index = _c(pList)->indexOf(pList, node);
    if (index < 0)
        return -1;

    _lhicon_getrect_byidx(self, index, rcNode);
    return 0;
}

static mNode* mLHIconLayout_onMouseHit(mLHIconLayout *self, int mouseX, int mouseY)
{
    int row, col, index;
    if (!self || !pList || mouseY < 0 || mouseX < 0 || mouseX > pList->contWidth)
        return NULL;

    row = mouseY / self->itemHeight;
    col = mouseX / self->itemWidth;
    index = row + self->nrRow * col;

    return _c(pList)->getNode(pList, index);
}

static void mLHIconLayout_update(mLHIconLayout *self,
        HDC hdc, const RECT* rcVis, RECT *rcCont)
{
    int i = 0;
    mNode *node;
    RECT rcDraw;
    if (!self || !pList || !rcVis || !rcCont || RECTWP(rcCont) <= 0)
        return;

    node = _c(pList)->getNode(pList, 0);

    while(node) {
        _lhicon_getrect_byidx(self, i, &rcDraw);
        OffsetRect(&rcDraw, rcCont->left, rcCont->top);

        if (rcDraw.right < rcVis->left || rcDraw.top > rcVis->bottom) {
            node = _c(node)->getNext(node);
            i++;
            continue;
        }

        if (rcDraw.left > rcVis->right)
            break;

        if (_c(node)->paint) {
            _c(node)->paint(node, hdc, &rcDraw, (mObject*)pList);
        }

        _DRAW_FOCUS_FRAME(pList, node, hdc, &rcDraw);
        node = _c(node)->getNext(node);
        i++;
    }
}

static mNode* mLHIconLayout_onDirKey(mLHIconLayout *self,
        mNode *refNode, int direct)
{
    BOOL loop = direct & NCSF_LIST_LOOP;
    mNode *node = NULL;
    int direct_flags, dstIdx;
    int count;

    if (!self)
        return 0;
    count = _c(pList->root)->getProperty(pList->root, NCSP_NODE_CHILDRENCOUNT);
    if (count <= 0)
        return refNode;

    direct_flags = direct & NCSF_LIST_DIRMASK;

    switch (direct_flags) {
        case NCSF_LIST_UP:
        {
            if (self->nrRow == 1)
                return refNode;

            if (refNode) {
                int index = _c(refNode)->indexOf(refNode);
                int col = index/self->nrRow;
                dstIdx = index - 1;
                if (index == 0 || dstIdx/self->nrRow != col) {
                    if (loop)
                        node = _c(pList)->getNode(pList, index + self->nrRow - 1);
                    else
                        node = refNode;
                }
                else
                    node = _c(refNode)->getPrev(refNode);
            }

            break;
        }

        case NCSF_LIST_LEFT:
        {
            if (refNode) {
                int index = _c(refNode)->indexOf(refNode);
                int row = index % self->nrRow;
                int col, maxNrCol = count/self->nrRow + (count%self->nrRow ? 1 : 0);

                if (index == 0) {
                    if (loop) {
                        dstIdx = self->nrRow * maxNrCol - 1;
                        while (dstIdx >= count)
                            dstIdx -= self->nrRow;

                        node = _c(pList)->getNode(pList, dstIdx);
                    }
                    else
                        node = refNode;
                }
                else {
                    //first column
                    if (index/self->nrRow == 0) {
                        if (row - 1 <= (count - 1)%self->nrRow)
                            col = maxNrCol - 1;
                        else
                            col = maxNrCol - 2;

                        col = col > 0 ? col : 0;

                        dstIdx = self->nrRow * col + row -1;
                        node = _c(pList)->getNode(pList, dstIdx);
                    }
                    else {
                        dstIdx = index - self->nrRow;
                        node = _c(pList)->getNode(pList, dstIdx);
                    }
                }
            }
            break;
        }

        case NCSF_LIST_DOWN:
        {
            if (self->nrRow == 1)
                return refNode;

            if (refNode) {
                int index = _c(refNode)->indexOf(refNode);
                int col = index / self->nrRow;
                dstIdx = index + 1;
                if (dstIdx >= count || dstIdx / self->nrRow != col) {
                    if (dstIdx < count && loop) {
                        dstIdx -= self->nrRow;
                        node = _c(pList)->getNode(pList, dstIdx);
                    }
                    else
                        node = refNode;
                }
                else {
                    node = _c(refNode)->getNext(refNode);
                }
            }
            break;
        }

        case NCSF_LIST_RIGHT:
        {
            if (refNode) {
                int index = _c(refNode)->indexOf(refNode);
                dstIdx = index + self->nrRow;
                if (dstIdx >= count) {
                    int row = index%self->nrRow;
                    //last row
                    if (row == self->nrRow - 1) {
                        if (loop) //get first node
                            node = _c(pList)->getNode(pList, 0);
                        else
                            node = refNode;
                    }
                    else {
                        node = _c(pList)->getNode(pList, row + 1);
                    }
                }
                else {
                    node = _c(pList)->getNode(pList, dstIdx);
                }
            }
            break;
        }

        case NCSF_LIST_HOME:
            node = _c(pList)->getNode(pList, 0);
            break;

        case NCSF_LIST_END:
            node = (mNode*)_c(pList->root)->getProperty(pList->root, NCSP_NODE_LASTCHILD);
            break;
    }

    return node;
}

static void mLHIconLayout_nodeEvent(mLHIconLayout *self,
        int eventId, DWORD eventInfo)
{
    if (!self || !pList)
        return;
    switch(eventId) {
        case NCSE_LIST_FONTCHANGED:
        case NCSE_NODE_HEIGHTCHANGED:
            //nothing to do
            break;

        case NCSE_LIST_ROOTCHANGED:
        case NCSE_LIST_SIZECHANGED:
            return Class(mListLayout).nodeEvent((mListLayout*)self, eventId, eventInfo);
            break;

        case NCSE_NODE_CONTENTCHANGED:
            _c(pList)->refreshNode(pList, (mNode*)eventInfo, NULL);
            break;

        case NCSE_NODE_NODEADDED:
        {
            mNode *node = (mNode*)eventInfo;

            if (node) {
                BOOL refresh = FALSE;
                int count = _c(pList->root)->getProperty(pList->root, NCSP_NODE_CHILDRENCOUNT);

                if (self->nrRow == 1 || count % self->nrRow == 1) {
                    int width = (count/self->nrRow + 1) * self->itemWidth;
                    int oldVal;

                    _LAYOUT_BAK_VAL(self, refresh, oldVal, FALSE);
                    if (_c(pList)->setProperty(pList, NCSP_SWGT_CONTWIDTH, width)) {
                        _LAYOUT_UPDATE_VAL(self, refresh, oldVal, FALSE);
                    }
                }
                if (!refresh) {
                    _lhicon_refresh_nodes(self, node, count - 1, 0, 0);
                }
            }
            break;
        }

        case NCSE_NODE_NODEDELETED:
        {
            if (eventInfo) {
                mNode *parentNode, *node = (mNode*)eventInfo;
                int count;
                BOOL refresh = FALSE;
                parentNode = (mNode*)_c(node)->getProperty(node, NCSP_NODE_PARENTNODE);
                count = _c(parentNode)->getProperty(parentNode, NCSP_NODE_CHILDRENCOUNT);

                if (count%self->nrRow == 0) {
                    int oldVal, width;
                    width = (count/self->nrRow) * self->itemWidth;

                    _LAYOUT_BAK_VAL(self, refresh, oldVal, FALSE);
                    if (_c(pList)->setProperty(pList, NCSP_SWGT_CONTWIDTH, width)) {
                        _LAYOUT_UPDATE_VAL(self, refresh, oldVal, FALSE);
                    }
                }
                node = _c(node)->getNext(node);
                if (!refresh && (node || count%self->nrRow != 0)) {
                    _lhicon_refresh_nodes(self, node, count, 0, 0);
                }
            }
            break;
        }
    }
}

BEGIN_MINI_CLASS(mLHIconLayout, mListLayout)
    CLASS_METHOD_MAP(mLHIconLayout, construct)
    CLASS_METHOD_MAP(mLHIconLayout, setProperty)
    CLASS_METHOD_MAP(mLHIconLayout, onMouseHit)
    CLASS_METHOD_MAP(mLHIconLayout, onDirKey)
    CLASS_METHOD_MAP(mLHIconLayout, getNodeRect)
    CLASS_METHOD_MAP(mLHIconLayout, nodeEvent)
    CLASS_METHOD_MAP(mLHIconLayout, update)
    CLASS_METHOD_MAP(mLHIconLayout, recalcContSize)
END_MINI_CLASS

static void mLHCenterBoxLayout_construct(mLHCenterBoxLayout *self, DWORD addData)
{
    Class(mLHIconLayout).construct((mLHIconLayout*)self, addData);
    self->nrRow = 1;
    self->offx = 0;
}

static BOOL mLHCenterBoxLayout_recalcContSize(mLHCenterBoxLayout *self, int flags)
{
    BOOL update = FALSE;
    int oldVal = 0;

    if (!self || !pList || !flags)
        return FALSE;

    if (flags & NCSF_LIST_LAYOUT_ITEMHCHANGED) {
        _LAYOUT_BAK_VAL(self, update, oldVal, TRUE);
        if (_c(pList)->setProperty(pList, NCSP_SWGT_CONTHEIGHT, self->itemHeight)) {
            _LAYOUT_UPDATE_VAL(self, update, oldVal, TRUE);
        }
    }

    if (flags & NCSF_LIST_LAYOUT_ITEMWCHANGED) {
        int width = 0, count;
        count = _c(pList->root)->getProperty(pList->root, NCSP_NODE_CHILDRENCOUNT);
        if (count >= 0) {
            width = count * self->itemWidth;
            width = width > pList->visWidth ? width : pList->visWidth;
        }

        _LAYOUT_BAK_VAL(self, update, oldVal, FALSE);
        if (_c(pList)->setProperty(pList, NCSP_SWGT_CONTWIDTH, width)) {
            _LAYOUT_UPDATE_VAL(self, update, oldVal, FALSE);
        }
    }

    return update;
}

static mNode* mLHCenterBoxLayout_onDirKey(mLHCenterBoxLayout *self,
        mNode *refNode, int direct)
{
    mNode *node = Class(mLHIconLayout).onDirKey((mLHIconLayout*)self, refNode, direct);
    if (node && node != refNode) {
        _c(pList)->selectNode(pList, node, TRUE);
        InvalidateRect(pList->hwnd, NULL, TRUE);
    }
    return NULL;
}

static void mLHCenterBoxLayout_update(mLHCenterBoxLayout *self,
        HDC hdc, const RECT* rcVis, RECT *rcCont)
{
    int i = 0, centerIdx, firstIdx, nrCol;
    mNode *node;
    RECT rcDraw;
    POINT pt;
    //int centerX, centerY;
    int loop;

    if (!self || !pList || !rcVis || !rcCont || RECTWP(rcCont) <= 0)
        return;

    nrCol = RECTWP(rcVis) > self->itemWidth ? RECTWP(rcVis)/self->itemWidth + 1: 1;

    loop = nrCol >>1;
    /*centerX = */pt.x = RECTWP(rcVis)>>1;
    /*centerY = */pt.y = RECTHP(rcVis)>>1;

    node = _c(pList)->getCurSel(pList);
    if (!node) {
        centerIdx = 0;
    }
    else
        centerIdx = _c(pList)->indexOf(pList, node);

    if (centerIdx > loop) {
        firstIdx = centerIdx - loop;
        pt.x -= self->itemWidth * loop;
    }
    else {
        firstIdx = 0;
        pt.x -= self->itemWidth * centerIdx;
    }
    node = _c(pList)->getNode(pList, firstIdx);

    rcDraw.top = pt.y - (self->itemHeight >>1);
    rcDraw.bottom = pt.y + (self->itemHeight >>1);

    for (i = 0; i < nrCol && node; i++) {
        rcDraw.left = pt.x - (self->itemWidth>>1);
        rcDraw.right = pt.x + (self->itemWidth>>1);

        //TODO: for visible part node
        if (rcDraw.left < rcVis->left) {
            pt.x += self->itemWidth;
            node = _c(node)->getNext(node);
            continue;
        }

        if (rcDraw.left > rcVis->right)
            break;

        if (_c(node)->paint) {
            _c(node)->paint(node, hdc, &rcDraw, (mObject*)pList);
        }

        _DRAW_FOCUS_FRAME(pList, node, hdc, &rcDraw);
        pt.x += self->itemWidth;
        node = _c(node)->getNext(node);
    }
}

static int mLHCenterBoxLayout_getNodeRect(mLHCenterBoxLayout *self, mNode *node, RECT *rcNode)
{
    int index;
    RECT rc;
    if (!self || !pList || !rcNode || !node)
        return -1;

    index = _c(pList)->indexOf(pList, node);
    if (index < 0)
        return -1;

    GetClientRect(pList->hwnd, &rc);
    rcNode->top = (RECTH(rc) - self->itemHeight)>>1;
    rcNode->bottom = rcNode->top + self->itemHeight;

    rcNode->left = index * self->itemWidth;
    rcNode->right = rcNode->left + self->itemWidth;

    return 0;
}

static mNode* mLHCenterBoxLayout_onMouseHit(mLHCenterBoxLayout *self, int mouseX, int mouseY)
{
    //not support mouse event
    return NULL;
}

BEGIN_MINI_CLASS(mLHCenterBoxLayout, mLHIconLayout)
    CLASS_METHOD_MAP(mLHCenterBoxLayout, construct)
    CLASS_METHOD_MAP(mLHCenterBoxLayout, onMouseHit)
    CLASS_METHOD_MAP(mLHCenterBoxLayout, update)
    CLASS_METHOD_MAP(mLHCenterBoxLayout, getNodeRect)
    CLASS_METHOD_MAP(mLHCenterBoxLayout, onDirKey)
    CLASS_METHOD_MAP(mLHCenterBoxLayout, recalcContSize)
END_MINI_CLASS

/*===========================mLGroupLayout=========================*/
static void mLGroupLayout_construct(mLGroupLayout *self, DWORD  addData)
{
    Class(mLVIconLayout).construct((mLVIconLayout*)self, addData);
    self->cbInGroup = NULL;
    self->cbCmpNode = NULL;
    self->cbCmpGroup= _ncs_defcmp_node;
    self->groupRoot = NULL;
    self->defGroupH = 18;
    self->defaultGroup = TRUE;
}

static void mLGroupLayout_destroy(mLGroupLayout *self)
{
    if (self->groupRoot) {
        _c(self->groupRoot)->removeAll(self->groupRoot);
        DELETE(self->groupRoot);
    }

    //reset list node cmparation
    pList->nodeCmp = self->cbCmpNode;
    Class(mLVIconLayout).destroy((mLVIconLayout*)self);
}

static int is_current_group(mGroupNode *node, DWORD id)
{
    if (node && node->id == id)
        return 0;

    return 1;
}

static mNode* get_groupnode_byid(mLGroupLayout *self, int id)
{
    if (!self->groupRoot)
        return NULL;

    return _c(self->groupRoot)->findNodeEx(self->groupRoot, id, NCS_NODE_FTOTHER, FALSE, 0, (void*)is_current_group);
}


static int mLGroupLayout_nodeCmpFunc(mNode *node1, mNode *node2)
{
    mList *list = (mList *)mAbstractList_getControl(node1);
    mLGroupLayout *self = (mLGroupLayout*) list->layout;

    //first group compare, get node group information.
    if (self->cbInGroup && self->groupRoot && !self->defaultGroup) {
        int group1, group2;
        group1 = self->cbInGroup(node1);
        group2 = self->cbInGroup(node2);

        if (group1 == group2) {
            return self->cbCmpNode(node1, node2);
        }
        else {
            mNode *groupNode1, *groupNode2;
            groupNode1 = get_groupnode_byid(self, group1);
            groupNode2 = get_groupnode_byid(self, group2);

            return self->cbCmpGroup(groupNode1, groupNode2);
        }
    }
    return self->cbCmpNode(node1, node2);
}

NCS_CB_CMPNODE mLGroupLayout_decorateNodeCmp(mLGroupLayout *self,
        NCS_CB_CMPNODE func)
{
    if (!self || !func)
        return NULL;

    self->cbCmpNode = func;
    return mLGroupLayout_nodeCmpFunc;
}

//update content region setting
static BOOL mLGroupLayout_recalcContSize(mLGroupLayout *self, int flags)
{
    int oldVal = 0, height = 0;
    BOOL update = FALSE;

    if (!self || !pList || !flags)
        return FALSE;

    if (flags & NCSF_LIST_LAYOUT_ITEMWCHANGED) {
        _LAYOUT_BAK_VAL(self, update, oldVal, FALSE);
        if (_c(pList)->setProperty(pList, NCSP_SWGT_CONTWIDTH, pList->visWidth)) {
            _LAYOUT_UPDATE_VAL(self, update, oldVal, FALSE);
        }
    }

    self->nrCol = pList->visWidth > self->itemWidth ? pList->visWidth/self->itemWidth : 1;

    if (self->groupRoot) {
        int nrChild;
        mNode *groupNode = _c(self->groupRoot)->getNode(self->groupRoot, 0);

        while(groupNode) {
            nrChild = _c(groupNode)->getProperty(groupNode, NCSP_NODE_CHILDRENCOUNT);
            if (nrChild > 0) {
                height += self->defGroupH;
                height += self->itemHeight * (nrChild / self->nrCol + ((nrChild % self->nrCol) ? 1 : 0));
            }
            groupNode = _c(groupNode)->getNext(groupNode);
        }
    }

    _LAYOUT_BAK_VAL(self, update, oldVal, TRUE);
    if (_c(pList)->setProperty(pList, NCSP_SWGT_CONTHEIGHT, height)) {
        _LAYOUT_UPDATE_VAL(self, update, oldVal, TRUE);
    }

    return update;
}

static void _update_list_and_layout(mLGroupLayout *self)
{
    BOOL frozen = _c(pList)->isFrozen(pList);
    int count = 0;
    if (!frozen)
        _c(pList)->freeze(pList, TRUE);

    //sort nodes and update group node counting
    if (self && self->cbInGroup && self->groupRoot) {
        //no set group information
        if (pList) {
            count = _c(pList->root)->getProperty(pList->root, NCSP_NODE_CHILDRENCOUNT);
        }

        if (self->defaultGroup) {
            mNode *unnamedNode = _c(self->groupRoot)->getNode(self->groupRoot, 0);
            if (unnamedNode)
                unnamedNode->count = count;
        }
        else if (count > 0) {
            int groupId;
            mGroupNode *groupNode;
            mNode *first;
            _c(pList)->sortNodes(pList, mLGroupLayout_nodeCmpFunc, NULL);

            //reset count
            first = _c(pList)->getNode(pList, 0);

            groupNode = (mGroupNode*)_c(self->groupRoot)->getNode(self->groupRoot, 0);
            groupNode->count = 0;

            while (first) {
                groupId = self->cbInGroup(first);
                if (groupId == groupNode->id) {
                    groupNode->count++;
                }
                else {
                    groupNode = (mGroupNode*)_c(groupNode)->getNext(groupNode);
                    if (!groupNode)
                        break;

                    groupNode->count = 0;
                    continue;
                }

                first = _c(first)->getNext(first);
            }

            if (groupNode)
                groupNode = (mGroupNode*)_c(groupNode)->getNext(groupNode);

            while (groupNode) {
                groupNode->count = 0;
                groupNode = (mGroupNode*)_c(groupNode)->getNext(groupNode);
            }
        }
    }

    _c(self)->recalcContSize(self, NCSF_LIST_LAYOUT_ITEMWCHANGED | NCSF_LIST_LAYOUT_ITEMHCHANGED);

    if (!frozen)
        _c(pList)->freeze(pList, FALSE);
}

static void mLGroupLayout_init(mLGroupLayout *self,
        mAbstractList *list, int defItemW, int defItemH)
{
    if (!self || !list || !_c(self)->getProperty(self, NCSP_ASTLST_AUTOSORT))
        return;

    if (!_init_layout((mListLayout*)self, list, defItemW, defItemH))
        return;

    self->defGroupH = GetWindowFont(list->hwnd)->size + 4;

    if (self->cbCmpNode != pList->nodeCmp) {
        self->cbCmpNode = pList->nodeCmp;
    }

    if (!self->groupRoot) {
        mNode *node;
        int nodeCounts = 0;

        self->groupRoot = ncsCreateNode((mObject*)list, "categories",
                NULL, self->defGroupH, NCSS_NODE_HIDEIMAGE, 0);
        node = ncsAddGroupNode((mObject*)self->groupRoot, NCSID_UNNAMED_GROUPTEXT,
                NULL, self->defGroupH, NCSS_NODE_HIDEIMAGE, 0, NCSID_UNNAMED_GROUP);

        nodeCounts = _c(pList->root)->getProperty(pList->root, NCSP_NODE_CHILDRENCOUNT);

        if (node && nodeCounts > 0) {
            node->count = nodeCounts;
        }
    }

    _update_list_and_layout(self);
}

static BOOL mLGroupLayout_setGroupInfo(mLGroupLayout *self,
        NCS_CB_INGROUP inGroup, NCS_GROUP_INFO *groupInfo, int groupSize)
{
    BOOL update = FALSE;
    if (!self)
        return FALSE;

    if (inGroup != self->cbInGroup) {
        self->cbInGroup = inGroup;
        update = TRUE;
    }

    if (pList && self->groupRoot) {
        //set new group information
        if (groupInfo && groupSize > 0) {
            mNode *unnamedNode = NULL;
            int i = 0;

            //delete old group node
            _c(self->groupRoot)->removeAll(self->groupRoot);

            pList->nodeCmp = self->cbCmpGroup;
            //add new group node
            while (i < groupSize) {
                ncsAddGroupNode((mObject*)self->groupRoot, groupInfo[i].text,
                        NULL, self->defGroupH, NCSS_NODE_HIDEIMAGE,
                        groupInfo[i].addData, groupInfo[i].id);
                i++;
            }
            //add unnamed group node
            unnamedNode = ncsAddGroupNode((mObject*)self->groupRoot,
                    NCSID_UNNAMED_GROUPTEXT,
                    NULL, self->defGroupH, NCSS_NODE_HIDEIMAGE,
                    0, NCSID_UNNAMED_GROUP);
            unnamedNode->count = _c(pList->root)->getProperty(pList->root, NCSP_NODE_CHILDRENCOUNT);

            //update list's comparation function
            pList->nodeCmp = mLGroupLayout_nodeCmpFunc;
            //clear the flags of defaultGroup
            self->defaultGroup = FALSE;

            update = TRUE;
        }
        else if (!self->defaultGroup) {
            //delete old group node
            _c(self->groupRoot)->removeAll(self->groupRoot);
            pList->nodeCmp = self->cbCmpNode;

            //reset defaultGroup mode
            ncsAddGroupNode((mObject*)self->groupRoot, NCSID_UNNAMED_GROUPTEXT,
                    NULL, self->defGroupH, NCSS_NODE_HIDEIMAGE, 0,
                    NCSID_UNNAMED_GROUP);
            self->defaultGroup = TRUE;

            //resort nodes
            _c(pList)->sortNodes(pList, pList->nodeCmp, NULL);
            update = TRUE;
        }
    }

    if (update) {
        _update_list_and_layout(self);
        return TRUE;
    }

    return FALSE;
}

static BOOL mLGroupLayout_resetGroup(mLGroupLayout *self,
        NCS_GROUP_INFO *groupInfo, int groupSize)
{
    return mLGroupLayout_setGroupInfo(self, NULL, groupInfo, groupSize);
}

static NCS_CB_INGROUP
mLGroupLayout_setInGroupFunc(mLGroupLayout *self, NCS_CB_INGROUP func)
{
    NCS_CB_INGROUP oldFunc;
    if (!self || (func == self->cbInGroup))
        return NULL;

    oldFunc = self->cbInGroup;
    self->cbInGroup = func;
    _update_list_and_layout(self);

    return oldFunc;
}

static int mLGroupLayout_getNodeRect(mLGroupLayout *self, mNode *node, RECT *rcNode)
{
    mNode *groupNode;
    int offh = 0, index, count = 0, nrChild;

    if (!self || !pList || !rcNode || !node)
        return -1;

    //no current visible node
    if ((mObject*)pList->root != node->parent)
        return -1;

    groupNode = _c(self->groupRoot)->getNode(self->groupRoot, 0);

    index = _c(pList)->indexOf(pList, node);

    while(groupNode) {
        nrChild = groupNode->count;

        if (nrChild <= 0) {
            groupNode = _c(groupNode)->getNext(groupNode);
            continue;
        }

        count += nrChild;
        offh += self->defGroupH;

        //in this group
        if (index < count) {
            index -= (count - nrChild);
            break;
        }
        else {
            offh += self->itemHeight * (nrChild / self->nrCol + ((nrChild % self->nrCol) ? 1 : 0));
        }

        groupNode = _c(groupNode)->getNext(groupNode);
    }

    if (index < 0)
        return -1;

    rcNode->top = offh + (index/self->nrCol) * self->itemHeight;
    rcNode->left = (index%self->nrCol) * self->itemWidth;
    rcNode->right = rcNode->left + self->itemWidth;
    rcNode->bottom = rcNode->top + self->itemHeight;
    return 0;
}

static mNode* mLGroupLayout_onMouseHit(mLGroupLayout *self, int mouseX, int mouseY)
{
    int head = 0, offh = 0, index = -1, count = 0, nrChild;
    mNode* groupNode;
    if (!self || !pList || mouseY < 0 || mouseX < 0)
        return NULL;

    if (mouseX > self->itemWidth * self->nrCol)
        return NULL;

    groupNode = _c(self->groupRoot)->getNode(self->groupRoot, 0);

    while(groupNode) {
        nrChild = _c(groupNode)->getProperty(groupNode, NCSP_NODE_CHILDRENCOUNT);
        if (nrChild > 0) {
            head = offh + self->defGroupH;
            offh = head +
                self->itemHeight * (nrChild / self->nrCol + ((nrChild % self->nrCol) ? 1 : 0));
        }
        //in group, no support
        if (mouseY < head) {
            return NULL;
        }
        else if (mouseY < offh) {
            index = ((mouseY - head)/self->itemHeight)* self->nrCol + mouseX / self->itemWidth;
            //show first category
            if (index + count < self->nrCol)
                _c(pList)->makePosVisible(pList, 0, 0);

            return _c(pList)->getNode(pList, index + count);
        }

        count += nrChild;
        groupNode = _c(groupNode)->getNext(groupNode);
    }
    return NULL;
}

static void mLGroupLayout_update(mLGroupLayout *self,
        HDC hdc, const RECT* rcVis, RECT *rcCont)
{
    int i = 0;
    mNode *node, *groupNode;
    RECT rcDraw;
    int head, height = 0, count = 0, nrChild = 0;
    SIZE size;

    if (!self || !pList || !rcVis || !rcCont || RECTWP(rcCont) <= 0)
        return;

    head = rcCont->top;
    groupNode = _c(self->groupRoot)->getNode(self->groupRoot, 0);
    while(groupNode) {

        nrChild = _c(groupNode)->getProperty(groupNode, NCSP_NODE_CHILDRENCOUNT);
        if (nrChild > 0) {
            head += height + self->defGroupH;
            height = self->itemHeight * (nrChild / self->nrCol + ((nrChild % self->nrCol) ? 1 : 0));


            //under visible region
            if (head + rcCont->top - self->defGroupH > rcVis->bottom)
                break;

            //upper visible region
            if (head + height < rcVis->top) {
                groupNode = _c(groupNode)->getNext(groupNode);
                count += nrChild;
                continue;
            }

            //visible group node
            if (head >= rcVis->top) {
                _c(groupNode)->computeSize(groupNode, hdc, &size);
                rcDraw.left = rcCont->left;
                rcDraw.right = rcDraw.left + size.cx;
                rcDraw.bottom = head;
                rcDraw.top = rcDraw.bottom - self->defGroupH;

                if (_c(groupNode)->paint) {
                    _c(groupNode)->paint(groupNode, hdc, &rcDraw, (mObject*)pList);
                }
            }

            //loop node
            node = _c(pList)->getNode(pList, count);
            i = 0;
            while(node && i < nrChild) {
                rcDraw.left = rcCont->left + (i%self->nrCol) * self->itemWidth;
                rcDraw.right = rcDraw.left + self->itemWidth;

                rcDraw.top = head + (i/self->nrCol) * self->itemHeight;
                rcDraw.bottom = rcDraw.top + self->itemHeight;

                if (_c(node)->paint) {
                    _c(node)->paint(node, hdc, &rcDraw, (mObject*)pList);
                }
                _DRAW_FOCUS_FRAME(pList, node, hdc, &rcDraw);
                node = _c(node)->getNext(node);
                i++;
            }
            count += nrChild;
        }
        groupNode = _c(groupNode)->getNext(groupNode);
    }
}

static mNode* _get_group_node(mLGroupLayout *self, mNode* node)
{
    if (node) {
        int groupId = NCSID_UNNAMED_GROUP;
        if (self->cbInGroup && !self->defaultGroup) {
            groupId = self->cbInGroup(node);
        }

        return get_groupnode_byid(self, groupId);
    }
    return NULL;
}

static mNode* mLGroupLayout_onDirKey(mLGroupLayout *self,
        mNode *refNode, int direct)
{
    mNode *node;
    if (direct & NCSF_LIST_UP) {
        direct |= ~NCSF_LIST_UP;
        direct &= NCSF_LIST_LEFT;
    }
    if (direct & NCSF_LIST_DOWN) {
        direct |= ~NCSF_LIST_DOWN;
        direct &= NCSF_LIST_RIGHT;
    }

    node = Class(mLVIconLayout).onDirKey((mLVIconLayout*)self, refNode, direct);
    if (node && node != refNode) {
        int index = _c(pList)->indexOf(pList, node);
        if (index >= 0 && index < self->nrCol)
            _c(pList)->makePosVisible(pList, 0, 0);
    }

    return node;
}

/*
static int _get_node_index_ingroup(mLGroupLayout *self, mNode *node)
{
    if (!self || !pList || !node)
        return -1;

    mNode *groupNode = _c(self->groupRoot)->getNode(self->groupRoot, 0);
    int index, count = 0, nrChild;

    index = _c(pList)->indexOf(pList, node);

    while(groupNode) {
        nrChild = groupNode->count;

        if (nrChild <= 0) {
            groupNode = _c(groupNode)->getNext(groupNode);
            continue;
        }

        count += nrChild;

        //in this group
        if (index < count) {
            index -= (count - nrChild);
            break;
        }

        groupNode = _c(groupNode)->getNext(groupNode);
    }

    return index;
}
*/

static int _lgroup_get_group_info(mLGroupLayout *self, mNode *group, int *offTop, int *startIdx)
{
    int offh = 0, index = 0, nrChild;
    mNode *groupNode;
    if (!self || !group || !self->groupRoot || !(offTop || startIdx))
        return -1;

    groupNode = _c(self->groupRoot)->getNode(self->groupRoot, 0);

    while(groupNode) {

        if (groupNode == group)
            break;

        nrChild = groupNode->count;
        if (nrChild <= 0) {
            groupNode = _c(groupNode)->getNext(groupNode);
            continue;
        }

        index += nrChild;
        if (offTop) {
            offh += self->defGroupH;
            offh += self->itemHeight * (nrChild / self->nrCol + ((nrChild % self->nrCol) ? 1 : 0));
        }

        groupNode = _c(groupNode)->getNext(groupNode);
    }

    if (offTop)
        *offTop = offh;

    if (startIdx)
        *startIdx = index;
    return 0;
}

//not include group category
static void _lgroup_refresh_onegroup(mLGroupLayout *self,
        mNode *groupNode, mNode *node, int lastIdx)
{
    int offTop = 0, offIdx = 0;
    if (!self || !groupNode || !pList)
        return;

    if (!_lgroup_get_group_info(self, groupNode, &offTop, &offIdx)) {
        offTop += self->defGroupH;
        _lvicon_refresh_nodes((mLVIconLayout*)self, node, lastIdx, offTop, offIdx);
    }
}

static void _lgroup_refresh_groups(mLGroupLayout *self, mNode *groupNode)
{
    int offTop = 0, offIdx = 0;
    RECT rc;
    mNode *first = NULL;
    if (!self || !groupNode || !pList)
        return;

    while (groupNode) {

        if (groupNode->count <= 0) {
            groupNode = _c(groupNode)->getNext(groupNode);
            continue;
        }

        if (!_lgroup_get_group_info(self, groupNode, &offTop, &offIdx)) {
            //invalidate category
            rc.left = 0;
            rc.right = pList->visWidth;
            rc.top = offTop;
            rc.bottom = rc.top + self->defGroupH;
            _c(pList)->contentToWindow(pList, &rc.left, &rc.top);
            _c(pList)->contentToWindow(pList, &rc.right, &rc.bottom);
            InvalidateRect(pList->hwnd, &rc, TRUE);

            //invalidate nodes in current group
            first = _c(pList)->getNode(pList, offIdx);
            offTop += self->defGroupH;
            _lvicon_refresh_nodes((mLVIconLayout*)self, first, groupNode->count - 1, offTop, offIdx);
        }
        groupNode = _c(groupNode)->getNext(groupNode);
    }
}

static void mLGroupLayout_nodeEvent(mLGroupLayout *self,
        int eventId, DWORD eventInfo)
{
    if (!self || !pList)
        return;

    switch(eventId) {
        case NCSE_LIST_FONTCHANGED:
        case NCSE_NODE_HEIGHTCHANGED:
            break;

        case NCSE_LIST_SIZECHANGED:
        case NCSE_NODE_CONTENTCHANGED:
            return Class(mLVIconLayout).nodeEvent((mLVIconLayout*)self, eventId, eventInfo);

        case NCSE_LIST_ROOTCHANGED:
            _update_list_and_layout(self);
            break;

        case NCSE_NODE_NODEADDED:
        {
            mNode *node = (mNode*)eventInfo;
            mNode *groupNode = _get_group_node(self, node);
            if (groupNode) {
                BOOL update = _c(pList)->isFrozen(pList) ? TRUE : FALSE;
                int oldVal = 0;

                groupNode->count++;
                if (groupNode->count == 1) {
                    _LAYOUT_BAK_VAL(self, update, oldVal, TRUE);
                    if (_ADJUST_NODES_HEIGHT(pList, self->itemHeight + self->defGroupH)) {
                        _LAYOUT_UPDATE_VAL(self, update, oldVal, TRUE);
                    }
                    if (!update)
                        _lgroup_refresh_groups(self, groupNode);
                }
                else if (self->nrCol == 1 || (groupNode->count%self->nrCol == 1)) {
                    _LAYOUT_BAK_VAL(self, update, oldVal, TRUE);
                    if (_ADJUST_NODES_HEIGHT(pList, self->itemHeight)) {
                        _LAYOUT_UPDATE_VAL(self, update, oldVal, TRUE);
                    }
                    if (!update) {
                        _lgroup_refresh_onegroup(self, groupNode, node, groupNode->count - 1);
                        _lgroup_refresh_groups(self, _c(groupNode)->getNext(groupNode));
                    }
                }
                else {
                    //TODO:refresh current group from the specified node.
                    if (!update) {
                        _lgroup_refresh_onegroup(self, groupNode, node, groupNode->count-1);
                    }
                }
            }
            break;
        }

        case NCSE_NODE_NODEDELETED:
        {
            mNode *node = (mNode*)eventInfo;
            mNode *groupNode = _get_group_node(self, node);

            if (groupNode) {
                int oldVal = 0;
                BOOL update = _c(pList)->isFrozen(pList) ? TRUE : FALSE;

                groupNode->count--;
                if (groupNode->count == 0) {
                    _LAYOUT_BAK_VAL(self, update, oldVal, TRUE);
                    if (_ADJUST_NODES_HEIGHT(pList, -(self->itemHeight + self->defGroupH))) {
                        _LAYOUT_UPDATE_VAL(self, update, oldVal, TRUE);
                    }
                    if (!update)
                        _lgroup_refresh_groups(self, _c(groupNode)->getNext(groupNode));
                }
                else if (self->nrCol == 1 || groupNode->count%self->nrCol == 0) {
                    _LAYOUT_BAK_VAL(self, update, oldVal, TRUE);
                    if (_ADJUST_NODES_HEIGHT(pList, -(self->itemHeight))) {
                        _LAYOUT_UPDATE_VAL(self, update, oldVal, TRUE);
                    }

                    if (!update) {
                        node = _c(node)->getNext(node);
                        _lgroup_refresh_onegroup(self, groupNode, node, groupNode->count);
                        _lgroup_refresh_groups(self, _c(groupNode)->getNext(groupNode));
                    }
                }
                else {
                    //only refresh the current group
                    if (!update) {
                        node = _c(node)->getNext(node);
                        _lgroup_refresh_onegroup(self, groupNode, node, groupNode->count);
                    }
                }
            }
            break;
        }
    }
}

static BOOL mLGroupLayout_setProperty(mLGroupLayout *self, int id, DWORD value)
{
    if( id >= NCSP_LGROUP_LAYOUT_MAX || !self)
        return FALSE;

    switch (id) {
        case NCSP_LGROUP_LAYOUT_GROUPCMPFUNC:
            if (value && value != (DWORD)self->cbCmpGroup) {
                self->cbCmpGroup = (NCS_CB_CMPNODE)value;
                //sort group node
                _c(pList)->sortNodes(pList, self->cbCmpGroup, self->groupRoot);
                _update_list_and_layout(self);
                return TRUE;
            }
            return FALSE;

        default:
            break;
    }
    return Class(mLGroupLayout).getProperty((mLGroupLayout*)self, id);
}

static DWORD mLGroupLayout_getProperty(mLGroupLayout *self, int id)
{
    if( id >= NCSP_LGROUP_LAYOUT_MAX || !self)
        return -1;

    switch (id) {
        case NCSP_LGROUP_LAYOUT_NODECMPFUNC:
            return (DWORD)self->cbCmpNode;

        case NCSP_LGROUP_LAYOUT_GROUPCMPFUNC:
            return (DWORD)self->cbCmpGroup;
    }
    return Class(mLVIconLayout).getProperty((mLVIconLayout*)self, id);
}

BEGIN_MINI_CLASS(mLGroupLayout, mLVIconLayout)
    CLASS_METHOD_MAP(mLGroupLayout, construct)
    CLASS_METHOD_MAP(mLGroupLayout, destroy)
    CLASS_METHOD_MAP(mLGroupLayout, getProperty)
    CLASS_METHOD_MAP(mLGroupLayout, setProperty)
    CLASS_METHOD_MAP(mLGroupLayout, decorateNodeCmp)
    CLASS_METHOD_MAP(mLGroupLayout, resetGroup)
    CLASS_METHOD_MAP(mLGroupLayout, setInGroupFunc)
    CLASS_METHOD_MAP(mLGroupLayout, init)
    CLASS_METHOD_MAP(mLGroupLayout, onMouseHit)
    CLASS_METHOD_MAP(mLGroupLayout, getNodeRect)
    CLASS_METHOD_MAP(mLGroupLayout, update)
    CLASS_METHOD_MAP(mLGroupLayout, nodeEvent)
    CLASS_METHOD_MAP(mLGroupLayout, onDirKey)
    CLASS_METHOD_MAP(mLGroupLayout, setGroupInfo)
    CLASS_METHOD_MAP(mLGroupLayout, recalcContSize)
END_MINI_CLASS

#endif //_MGNCSCTRL_LIST

