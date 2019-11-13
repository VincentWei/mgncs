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
    The implementation of mAbstractList class.
   
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
#include "mdblist.h"
#include "mnode.h"
#include "mabstractlist.h"

#define NCSF_ASTLST_FROZEN          0x0001

static void mAbstractList_construct(mAbstractList *self, DWORD addData)
{
	Class(mScrollWidget).construct((mScrollWidget*)self, addData);

    self->nodeCmp    = _ncs_defcmp_node;
    self->flags      = 0;
    self->root       = ncsCreateNode((mObject*)self, "", NULL, 0, 0, 0);
    self->contHeight = 0;
    self->vStepVal 	 = GetWindowFont(self->hwnd)->size + 2;
}

static void mAbstractList_destroy(mAbstractList *self)
{
	if (self->root){
        DELETE(self->root);
	}
	Class(mScrollWidget).destroy((mScrollWidget*)self);
}

static BOOL mAbstractList_setProperty(mAbstractList *self, int id, DWORD value)
{
	if( id >= NCSP_ASTLST_MAX || !self)
		return -1;

	switch (id)
    {
        case NCSP_ASTLST_TEXTINDENT:
            self->textIndent = value;
            return TRUE;

        case NCSP_ASTLST_IMAGEINDENT:
            self->imageIndent = value;
            return TRUE;

        case NCSP_ASTLST_CMPNODEFUNC:
            if (value && (NCS_CB_CMPNODE)value != self->nodeCmp) {
                self->nodeCmp = (NCS_CB_CMPNODE)value;
                _M(self, sortNodes, self->nodeCmp, NULL);
                return TRUE;
            }
            return FALSE;
    }

	return Class(mScrollWidget).setProperty((mScrollWidget*)self, id, value);
}

static DWORD mAbstractList_getProperty(mAbstractList* self, int id)
{
	if( id >= NCSP_ASTLST_MAX || !self)
		return -1;

	switch (id)
    {
        case NCSP_ASTLST_TEXTINDENT:
            return self->textIndent;

        case NCSP_ASTLST_IMAGEINDENT:
            return self->imageIndent;

        case NCSP_ASTLST_CMPNODEFUNC:
            return (DWORD)self->nodeCmp;

        case NCSP_ASTLST_AUTOSORT:
            return GetWindowStyle(self->hwnd) & NCSS_ASTLST_AUTOSORT;

        case NCSP_ASTLST_TRANSPARENT:
            return GetWindowExStyle(self->hwnd) & WS_EX_TRANSPARENT;
            
        case NCSP_ASTLST_ROOTNODE:
            return (DWORD)self->root;

        case NCSP_ASTLST_FONTSIZE:
            return GetWindowFont(self->hwnd)->size;
    }

	return Class(mScrollWidget).getProperty((mScrollWidget*)self, id);
}

static int mAbstractList_insertNode(mAbstractList *self, 
        mNode *node, mNode *prev, mNode *next, int index)
{
    if (self && self->root)
        return _M(self->root, insertNode, node, prev, next, index);

    return -1;
}

static int mAbstractList_addNode(mAbstractList *self, mNode *node)
{
    return _M(self, insertNode, node, NULL, NULL, -1);
}

static int mAbstractList_removeNode(mAbstractList *self, mNode *node)
{
    if (self && self->root && node) {
        mNode *parentNode = (mNode*)_M(node, getProperty, NCSP_NODE_PARENTNODE);
        if (parentNode)
            return _M(parentNode, removeNode, node);
    }
    return -1;
}

static void mAbstractList_freeze(mAbstractList *self, BOOL lock)
{
    if (lock) {
        self->flags |= NCSF_ASTLST_FROZEN;
    } else {
        self->flags &= ~NCSF_ASTLST_FROZEN;
    }
}

static BOOL mAbstractList_isFrozen(mAbstractList *self)
{
    return self->flags & NCSF_ASTLST_FROZEN;
}

static BOOL mAbstractList_showNode(mAbstractList *self, mNode *node)
{
    RECT rcNode;
    int pos_y, pos_x;

    if (!self || !node) return FALSE;


    if (_M(self, getRect, node, &rcNode, FALSE) < 0)
        return FALSE;

    if (rcNode.top >= self->contY && rcNode.bottom <= self->contY + self->visHeight)
    {
        if (rcNode.right < self->contX) {
            return _M(self, makePosVisible, rcNode.left, rcNode.top); 
        } else if (rcNode.left >= self->contX + self->visWidth
                || (rcNode.right > self->contX + self->visWidth && rcNode.left != 0)) {
            return _M(self, makePosVisible, rcNode.right, rcNode.top); 
        } else if (rcNode.left < self->contX) {
            return _M(self, makePosVisible, rcNode.left, rcNode.top); 
        }

        _M(self, contentToWindow, &rcNode.left, &rcNode.top);
        _M(self, contentToWindow, &rcNode.right, &rcNode.bottom);
        InvalidateRect(self->hwnd, &rcNode, TRUE);
        return TRUE;
    }

    pos_y = rcNode.top;
    pos_x = self->contX;

    if (rcNode.bottom <= self->contY) {
        pos_y = rcNode.top;
    } else if (rcNode.top > self->contY) {
        pos_y = rcNode.bottom;
    }

    if (rcNode.right <= self->contX){
        pos_x = rcNode.left;
	} else if (rcNode.left >= self->contX + self->visWidth 
            || (rcNode.right  > self->contX + self->visWidth && rcNode.left != 0)){
        pos_x = rcNode.right;
	}

    return _M(self, makePosVisible, pos_x, pos_y); 
}

static void mAbstractList_refreshNode(mAbstractList *self, 
        mNode *node, const RECT *rcInv)
{
    RECT rcNode, rcTmp;

    if (!self || !node || _c(self)->isFrozen(self))
        return;

    if (_M(self, getRect, node, &rcNode, TRUE) < 0)
        return;

    if (rcInv) {
        rcTmp = *rcInv;
        OffsetRect (&rcTmp, rcNode.left, rcNode.top);

        if (IntersectRect(&rcTmp, &rcNode, &rcTmp))
            InvalidateRect (self->hwnd, &rcTmp, TRUE);
    }
    else 
        InvalidateRect(self->hwnd, &rcNode, TRUE);
}

static mNode* mAbstractList_findNode(mAbstractList *self, DWORD info, 
        int type, BOOL recursion, int startIndex)
{
    if (!self || !self->root)
        return NULL;

    return _M(self->root, findNode, info, type, recursion, startIndex);
}

mNode* mAbstractList_getNode(mAbstractList *self, int index)
{
    return _M(self->root, getNode, index);
}

int mAbstractList_indexOf(mAbstractList *self, mNode *node)
{
    return _M(node, indexOf);
}

static void _sort_children(mNode *self, NCS_CB_CMPNODE func)
{
    int ret;
    mNode *tmp, *first, *child1, *child2;

    if (!self || !func)
        return;

    if (_M(self, getProperty, NCSP_NODE_CHILDRENCOUNT) <= 1)
        return;

    child1 = first = (mNode*)_M(self, getProperty, NCSP_NODE_FIRSTCHILD);

    while (first) {
        _sort_children(first, func);
        first = _M(first, getNext);
    }
    
    while (child1) {
        ret = 0;

        for (child2 = (mNode*)_M(self, getProperty, NCSP_NODE_FIRSTCHILD);
                child2 != NULL; child2 = _M(child2, getNext) )
        {
            if (child1 == child2)
                break;

            ret = func(child1, child2);
            if (ret < 0)
                break;
        }

        tmp = _M(child1, getNext);

        if (ret < 0) {
           _M(self, moveNode, child1, 1, child2);
        }
                
        child1 = tmp;
    }
}

static void mAbstractList_sortNodes(mAbstractList *self, NCS_CB_CMPNODE func, mNode *parentNode)
{
    if (self && func) {
        _sort_children(parentNode ? parentNode: self->root, func);
        if (!_M(self, isFrozen))
            InvalidateRect(self->hwnd, NULL, TRUE);
    }
}

static void mAbstractList_notifyEvent(mAbstractList* self, int eventId, DWORD eventInfo)
{
    switch (eventId) {
        case NCSE_NODE_NODEADDED:
        {
            mNode *node = (mNode*)eventInfo;
            if (node && _M(node, getProperty, NCSP_NODE_CHILDRENCOUNT) > 0
                    && GetWindowStyle(self->hwnd) & NCSS_ASTLST_AUTOSORT) {
                _M(self, sortNodes, self->nodeCmp, node);
            }
            break;
        }
        default:
            break;
    }
}

BEGIN_CMPT_CLASS(mAbstractList, mScrollWidget)
	CLASS_METHOD_MAP(mAbstractList, construct)
	CLASS_METHOD_MAP(mAbstractList, destroy)
	CLASS_METHOD_MAP(mAbstractList, getProperty)
	CLASS_METHOD_MAP(mAbstractList, setProperty)
	CLASS_METHOD_MAP(mAbstractList, getNode)
	CLASS_METHOD_MAP(mAbstractList, indexOf)
	CLASS_METHOD_MAP(mAbstractList, findNode)
	CLASS_METHOD_MAP(mAbstractList, addNode)
	CLASS_METHOD_MAP(mAbstractList, insertNode)
	CLASS_METHOD_MAP(mAbstractList, removeNode)
	CLASS_METHOD_MAP(mAbstractList, showNode)
	CLASS_METHOD_MAP(mAbstractList, refreshNode)
	CLASS_METHOD_MAP(mAbstractList, freeze)
	CLASS_METHOD_MAP(mAbstractList, isFrozen)
	CLASS_METHOD_MAP(mAbstractList, sortNodes)
	CLASS_METHOD_MAP(mAbstractList, notifyEvent)
END_CMPT_CLASS

mAbstractList* mAbstractList_getControl(mNode *node)
{
	mNode *pNode;
    if (!node)
        return NULL;

    pNode = node;
    while (pNode && INSTANCEOF(pNode->parent, mNode))
        pNode = (mNode*)pNode->parent;

    if (INSTANCEOF(pNode->parent, mAbstractList))
        return (mAbstractList*)pNode->parent;

    return NULL;
}
