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
    The implementation of mNode class.

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

#include "mdblist.h"
#include "mnode.h"
#include "mabstractlist.h"

#define nodelist_for_each(pos, node) \
    list_for_each(pos, &((mNode*)node)->children)

#define nodelist_entry(ptr) \
    list_entry(ptr, mNode, sibling)

#define ISDEFHEIGHT(self)   self->flags & NCSF_NODE_DEFHEIGHT

#define IS_VISIBLE_TEXT(self) (!(self->flags & NCSF_NODE_HIDETEXT) && self->text) 
#define IS_VISIBLE_IMAGE(self) (!(self->flags & NCSF_NODE_HIDEIMAGE) && self->image) 


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

static void _reset_text_color(mNode *self, HDC hdc, const RECT *rect, mObject *owner) 
{
    mAbstractList *control = (mAbstractList*)(owner);

    SetBkMode (hdc, BM_TRANSPARENT);
    if (control) {
        if (!_c(self)->isEnabled(self)) {
            _set_text_color(control, hdc, self->text, NCS_FGC_DISABLED_ITEM);
        }
        else if (_c(self)->isSelected(self)){
            _set_text_color(control, hdc, self->text, NCS_FGC_HILIGHT_ITEM);
        }
        else {
            _set_text_color(control, hdc, self->text, NCS_FGC_WINDOW);
        }
    }
}

static void _textrs_paint(mNode *self, HDC hdc, const RECT *rect, mObject *owner)
{
    int indent = -1, inter = 2, offset = 0;
    RECT rcText;

    _default_draw_bkgnd(self, hdc, rect, owner);

    if (IS_VISIBLE_IMAGE(self)) {
		int top = rect->top, left;
        int w = RECTWP(rect), h = RECTWP(rect);

        if (RECTHP(rect) > self->image->bmHeight) 
            top += (rect->top + rect->bottom < self->image->bmHeight) >>1;

        if (owner)
            indent = _c((mAbstractList*)owner)->getProperty((mAbstractList*)owner, NCSP_ASTLST_IMAGEINDENT);

        left = rect->left + (indent > 0 ? indent : inter);
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

static void _iconview_paint(mNode *self, HDC hdc, const RECT *rect, mObject *owner)
{
    RECT rcText;
    if (!self || !rect)
        return;

    _default_draw_bkgnd(self, hdc, rect, owner);

    if (IS_VISIBLE_TEXT(self)) {
        CopyRect(&rcText, rect);

        if (owner) {
            rcText.top = rcText.bottom - _c((mComponent*)owner)->getProperty((mComponent*)owner, NCSP_ASTLST_FONTSIZE) - 2;
        }
        else
            rcText.top = rcText.bottom - 18;

        DrawText(hdc, self->text, -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    if (IS_VISIBLE_IMAGE(self)) {
        int x = rect->left, y = rect->top; 
        int half, bmpW = 0, bmpH = 0;

        half = (RECTWP(rect) - (int)self->image->bmWidth)>>1;
        if (half > 0) 
            x += half;
        else 
            bmpW = RECTWP(rect);
        
        half = (RECTHP(rect) - (int)self->image->bmHeight)>>1;
        if (half > 0)
            y += half;
        else 
            bmpH = RECTHP(rect);

        FillBoxWithBitmap (hdc, x, y, bmpW, bmpH, self->image);
    }
}

static void _tictactoemidlet_paint(mNode *self, HDC hdc, const RECT *rect, mObject *owner)
{
	RECT rcText;

    if (!self || !rect)
        return;

    _reset_text_color(self, hdc, rect, owner);

    if (IS_VISIBLE_TEXT(self)) {
        CopyRect(&rcText, rect);

        if (owner) {
            rcText.top = rcText.bottom - _c((mComponent*)owner)->getProperty((mComponent*)owner, NCSP_ASTLST_FONTSIZE) - 2;
        }
        else
            rcText.top = rcText.bottom - 18;

        DrawText(hdc, self->text, -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    if (IS_VISIBLE_IMAGE(self)) {
        int x = rect->left, y = rect->top; 
        int half, bmpW = 0, bmpH = 0, offset;
        BOOL sel = _c(self)->isSelected(self);

        bmpW = self->image->bmWidth >> 1;
        bmpH = self->image->bmHeight;
        offset = bmpW;

        half = (RECTWP(rect) - bmpW)>>1;
        if (half > 0) 
            x += half;
        else 
            bmpW = RECTWP(rect);
        
        half = (RECTHP(rect) - bmpH)>>1;
        if (half > 0)
            y += half;
        else 
            bmpH = RECTHP(rect);

        FillBoxWithBitmapPart (hdc, x, y, bmpW, bmpH, 0, 0, self->image, sel?offset: 0, 0);
    }
}

static void _smallicon_paint(mNode *self, HDC hdc, const RECT *rect, mObject *owner)
{
	RECT rcText;
    if (!self || !rect)
        return;

    _reset_text_color(self, hdc, rect, owner);
    if (IS_VISIBLE_TEXT(self)) {
        CopyRect(&rcText, rect);

        if (owner) {
            rcText.top = rcText.bottom - _c((mComponent*)owner)->getProperty((mComponent*)owner, NCSP_ASTLST_FONTSIZE) - 2;
        }
        else
            rcText.top = rcText.bottom - 18;

        DrawText(hdc, self->text, -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    if (IS_VISIBLE_IMAGE(self)) {
        int x = rect->left, y = rect->top, w, h; 
        BOOL sel = _c(self)->isSelected(self);

        w = RECTWP(rect);
        h = RECTHP(rect);

        if (!sel) {
            int bmpW = 0, bmpH = 0;
            bmpW = self->image->bmWidth;
            bmpH = self->image->bmHeight;
            if (bmpW < w && bmpH < h)
                FillBoxWithBitmap (hdc,  x + ((w - bmpW)>>1), y+((h - bmpH)>>1), bmpW, bmpH, self->image);
            else
                FillBoxWithBitmap (hdc,  x + (w>>2), y+(h>>2), w>>1, h>>1, self->image);
        }
        else
            FillBoxWithBitmap (hdc,  x, y, w, IS_VISIBLE_TEXT(self)? (h - RECTH(rcText)):h, self->image);
    }
}

NCS_NODE_LAYOUTOPS _textrs_nodeops = {
    "list", 
    NCSS_NODE_LTEXTRS,
    _textrs_paint,
};

NCS_NODE_LAYOUTOPS _textbs_nodeops = {
    "icon", 
    NCSS_NODE_LTEXTBS,
    _iconview_paint,
};

NCS_NODE_LAYOUTOPS _tictactoemidlet_nodeops = {
    "tictactoemidlet", 
    NCSS_NODE_TICTACTOEMIDLET,
    _tictactoemidlet_paint,
};

NCS_NODE_LAYOUTOPS _smallicon_nodeops = {
    "smallicon", 
    NCSS_NODE_SMALLICON,
    _smallicon_paint,
};

#define pNodeLayout   (self->layout)

BOOL mNode_changeSelectState(mNode *self, BOOL select)
{
	BOOL isSel;
    if (!self || !_c(self)->isEnabled(self))
        return FALSE;

    isSel = _c(self)->isSelected(self);
    if (isSel && !select) {
        self->flags &= ~NCSF_NODE_SELECTED;
        return TRUE;
    } else if (!isSel && select) {
        self->flags |= NCSF_NODE_SELECTED;
        return TRUE;
    }
    return FALSE;
}

static BOOL mNode_isSelected(mNode *self)
{
    if (!self)
        return FALSE;

    return _c(self)->getProperty(self, NCSP_NODE_SELECTED);
}

static BOOL mNode_isEnabled(mNode *self)
{
    if (!self)
        return FALSE;

    return _c(self)->getProperty(self, NCSP_NODE_ENABLED);
}

static mNode* mNode_findNodeEx(mNode *self, DWORD info, 
        int type, BOOL recursion, int startIndex, int (*cb)(mNode *node, DWORD info))
{
    list_t  *me;
    mNode   *pNode;
    int     i = 0, start = startIndex;

    if (!self)
        return NULL;

    if (start < 0 || start > self->count) 
        start = 0;

    nodelist_for_each(me, self) {
        pNode = list_entry(me, mNode, sibling);
        if (i < start) {
            i++;
            continue;
        }

        if (type == NCS_NODE_FTSTRING && pNode->text 
                && strncasecmp(pNode->text, (const char*)info, strlen((char*)info)) == 0) 
            return pNode;
        else if (type == NCS_NODE_FTDATA && pNode->addData == info)
            return pNode;
        else if (type == NCS_NODE_FTIMAGE && pNode->image == (PBITMAP)info)
            return pNode;
        else if (type == NCS_NODE_FTOTHER && cb && cb(pNode, info) == 0) {
            return pNode;
        }

        if (recursion && _c(pNode)->hasChildren(pNode)) {
            mNode *tmp = _c(pNode)->findNodeEx(pNode, info, type, recursion, startIndex, cb);
            if (tmp) return tmp;
        }
    }

    return NULL;
}

static mNode* mNode_findNode(mNode *self, DWORD info, 
        int type, BOOL recursion, int startIndex)
{
    return mNode_findNodeEx(self, info, type, recursion, startIndex, NULL);
}

GET_ENTRY_INDEX(ncsGetNodeIndex, mNode, sibling)
static int mNode_indexOf(mNode *self)
{
    if (!self || !self->parent)
        return -1;

    //not root node
    if (INSTANCEOF(self->parent, mNode)) {
        mNode *parent = (mNode*)self->parent;
        return ncsGetNodeIndex(&parent->children, self);
    }
    return -1;
}

GET_ENTRY_BY_INDEX(ncsGetNodeByIndex, mNode, sibling)
static mNode* mNode_getNode(mNode *self, int index)
{
    if (!self || index < 0 || index >= self->count)
        return NULL;

    return ncsGetNodeByIndex(&self->children, index);
}

static mNode* mNode_getLast(mNode *self)
{
	list_t *me;
    if (!self)
        return NULL;

    me = self->children.prev;
    return (me == &(self->children)) ? NULL : list_entry(me, mNode, sibling);
}

static mNode* mNode_getNext(mNode *self)
{
    if (!self || !self->parent)
        return NULL;

    //not root node
    if (INSTANCEOF(self->parent, mNode)) {
        mNode *parent = (mNode*)self->parent;
        list_t *me = self->sibling.next;
        return (me == &(parent->children)) ? NULL : list_entry(me, mNode, sibling);
    }
    return NULL;
}

static BOOL mNode_hasChildren(mNode *self)
{
    return !list_empty(&self->children);
}

static mNode* mNode_getPrev(mNode *self)
{
    if (!self || !self->parent)
        return NULL;

    //not root node
    if (INSTANCEOF(self->parent, mNode)) {
        list_t *me = self->sibling.prev;
        mNode *parent = (mNode*)self->parent;
        return (me == &(parent->children)) ? NULL : list_entry(me, mNode, sibling);
    }
    return NULL;
}

static mObject* mNode_getParent(mNode *self)
{
    return self ? self->parent : NULL;
}

static void mNode_sendEventEx(mNode *self, int eventId, DWORD info)
{
    mAbstractList *control = mAbstractList_getControl(self);

    if (control && _c(control)->notifyEvent) {
        mNodeEventData eventData;
        eventData.source = self;
        eventData.data   = info;
        _c(control)->notifyEvent(control, eventId, (DWORD)&eventData);
    }
}

static void mNode_sendEvent(mNode *self, int eventId)
{
    mAbstractList *control = mAbstractList_getControl(self);

    if (control && _c(control)->notifyEvent) {
        _c(control)->notifyEvent(control, eventId, (DWORD)self);
    }
}

static void mNode_paint(mNode *self, HDC hdc, const RECT *rect, mObject *owner)
{
    if (self && owner) {
        pNodeLayout->paint(self, hdc, rect, owner);
    }
}

static int mNode_msgHandler(mNode *self, int msg, 
        WPARAM wParam, LPARAM lParam, mObject *owner)
{
    return 1;
}

static BOOL _update_node_layout(mNode *self, int style)
{
    if (pNodeLayout && (style & 0x0F0F) == pNodeLayout->style)
        return FALSE;

    if (style & NCSS_NODE_LTEXTBS) {
        self->layout = &_textbs_nodeops;
    }
    else if (style & NCSS_NODE_TICTACTOEMIDLET) {
        self->layout = &_tictactoemidlet_nodeops;
    }
    else if (style & NCSS_NODE_SMALLICON) {
        self->layout = &_smallicon_nodeops;
    }
    else {
        self->layout = &_textrs_nodeops;
    }

    return TRUE;
}

static BOOL mNode_setProperty(mNode *self, int id, DWORD value)
{
    if (id >= NCSP_NODE_MAX || !self)
        return FALSE;

    switch (id) {
        case NCSP_NODE_HIDETEXT:
        {
            BOOL ret = FALSE;
            if (!value && (self->flags & NCSF_NODE_HIDETEXT)) {
                //show text
                self->flags |= NCSF_NODE_HIDETEXT;
                ret = TRUE;
            }
            else if (value && !(self->flags & NCSF_NODE_HIDETEXT)) {
                //hide text
                self->flags &= ~NCSF_NODE_HIDETEXT;
                ret = TRUE;
            }

            if (ret)
                _c(self)->sendEvent(self, NCSE_NODE_REFRESH);

            return ret; 
        }

        case NCSP_NODE_HIDEIMAGE:
        {
            BOOL ret = FALSE;
            if (!value && (self->flags & NCSF_NODE_HIDEIMAGE)) {
                //show image
                self->flags |= NCSF_NODE_HIDEIMAGE;
                ret = TRUE;
            }
            else if (value && !(self->flags & NCSF_NODE_HIDEIMAGE)) {
                //hide image 
                self->flags &= ~NCSF_NODE_HIDEIMAGE;
                ret = TRUE;
            }

            if (ret)
                _c(self)->sendEvent(self, NCSE_NODE_REFRESH);

            return ret; 
        }

        case NCSP_NODE_LAYOUT:
            if (_update_node_layout(self, value))
                _c(self)->sendEvent(self, NCSE_NODE_REFRESH);
            return TRUE;

        case NCSP_NODE_HEIGHT:
            if (value >= 0 && self) {
				int diff;
                if (self->height == value)
                    return FALSE;

                diff = value - _c(self)->getProperty(self, NCSP_NODE_HEIGHT);

                //new height
                self->height = value;
                self->flags = self->flags & ~NCSF_NODE_DEFHEIGHT;

                mNode_sendEventEx(self, NCSE_NODE_HEIGHTCHANGED, diff);

                return TRUE;
            }
            return FALSE;

        case NCSP_NODE_TEXT:
            return _c(self)->setText(self, (const char*)value);

        case NCSP_NODE_IMAGE:
            return _c(self)->setImage(self, (PBITMAP)value);

        case NCSP_NODE_ENABLED:
        {
            if (value) { //enable
                if (self->flags & NCSF_NODE_DISABLED) {
                    self->flags &= ~NCSF_NODE_DISABLED;
                    _c(self)->sendEvent(self, NCSE_NODE_REFRESH);
                }
            }
            else {  //disable
                if (!(self->flags & NCSF_NODE_DISABLED)) {
                    self->flags |= NCSF_NODE_DISABLED;
                    _c(self)->sendEvent(self, NCSE_NODE_REFRESH);
                }
            }
            return TRUE;
        }

        case NCSP_NODE_ADDDATA:
            self->addData = value;
			return TRUE;
    }
    return FALSE;
}

static DWORD mNode_getProperty(mNode *self, int id)
{
    if (id >= NCSP_NODE_MAX || !self)
        return -1;

    switch (id) {
        case NCSP_NODE_HIDETEXT:
            return self->flags & NCSF_NODE_HIDETEXT;

        case NCSP_NODE_HIDEIMAGE:
            return self->flags & NCSF_NODE_HIDEIMAGE;

        case NCSP_NODE_LAYOUT:
            return pNodeLayout->style;

        case NCSP_NODE_HEIGHT:
            if (ISDEFHEIGHT(self)) {
                mAbstractList *control = mAbstractList_getControl(self);
                if (control) {
                    return _c(control)->getProperty(control, NCSP_ASTLST_DEFNODEHEIGHT);
                }
                return -1;
            }
            else
                return self->height;

        case NCSP_NODE_TEXT:
            return (DWORD)_c(self)->getText(self);

        case NCSP_NODE_IMAGE:
            return (DWORD)_c(self)->getImage(self);

        case NCSP_NODE_ADDDATA:
            return self->addData;

        case NCSP_NODE_ENABLED:
            return !(self->flags & NCSF_NODE_DISABLED);

        case NCSP_NODE_SELECTED:
            return self->flags & NCSF_NODE_SELECTED;

        case NCSP_NODE_PARENTNODE:
            return (DWORD)mNode_getParent(self);

        case NCSP_NODE_FIRSTCHILD:
            return (DWORD)mNode_getNode(self, 0);

        case NCSP_NODE_LASTCHILD:
            return (DWORD)mNode_getLast(self);

        case NCSP_NODE_CHILDRENCOUNT:
            return self->count;

        default:
            break;
    }
    return -1;
}

static PBITMAP mNode_getImage(mNode *self)
{
    return self ? self->image : NULL;
}

static const char* mNode_getText(mNode *self)
{
    return (self && self->text) ? self->text : "";
}

static BOOL mNode_setImage(mNode *self, PBITMAP image)
{
    self->image = image;
    _c(self)->sendEvent(self, NCSE_NODE_CONTENTCHANGED);
    return TRUE;
}

static BOOL mNode_setText(mNode* self, const char* text)
{
    if (!text)
        return FALSE;

    if (self->text && strcmp(self->text, text) == 0) {
        return FALSE;
    }
    else if (self->text) {
        FreeFixStr (self->text);
    }

    self->text = FixStrAlloc (strlen(text));
    if (!self->text)
        return FALSE;

    strcpy (self->text, text);
    _c(self)->sendEvent(self, NCSE_NODE_CONTENTCHANGED);
    
    return TRUE;
}

static int _insert_sortnode(mNode *self, 
        mNode *node, NCS_CB_CMPNODE cmpFunc, BOOL add)
{
    list_t  *me;
    mNode   *tmp = NULL;
    int     i = 0;
    mNode   *parent = (mNode*)node->parent;

    nodelist_for_each(me, parent) {
        tmp = nodelist_entry(me);
        if (cmpFunc(node, tmp) < 0) {
            if (add)
                list_add_tail(&node->sibling, &tmp->sibling);
            return i;
        }
        i++;
    }

    if (add)
        list_add_tail(&node->sibling, &parent->children);
    return i;
}

static int mNode_insertNode(mNode *self, mNode *node, mNode *prev,
            mNode *next, int index)
{
    int ret;
	mAbstractList *control;

    if (!self || !node || !node->parent)
        return -1;

    node->parent = (mObject *)self;
    control = mAbstractList_getControl(self);

    if (control) {
        if (_c(control)->getProperty(control, NCSP_ASTLST_AUTOSORT) > 0) {
            NCS_CB_CMPNODE func = 
                (NCS_CB_CMPNODE)_c(control)->getProperty(control, NCSP_ASTLST_CMPNODEFUNC);
            if (func) {
                ret = _insert_sortnode(self, node, func, TRUE);
                self->count ++;
                _c(node)->sendEvent(node, NCSE_NODE_NODEADDED);
                return ret < 0 ? self->count -1 : ret;
            }
        }
    }

    if (prev) {
        list_add(&node->sibling, &prev->sibling);
        ret = _c(prev)->indexOf(prev) + 1;
    }
    else if (next) {
        list_add_tail (&node->sibling, &next->sibling);
        ret = _c(next)->indexOf(next) - 1;
    }
    else {
        ret = list_add_by_index (&node->sibling, &self->children, index);
    }
    
    self->count ++;
    _c(node)->sendEvent(node, NCSE_NODE_NODEADDED);
    return (ret < 0 ? self->count -1: ret);
}

static int mNode_addNode(mNode *self, mNode *node)
{
    return _c(self)->insertNode(self, node, NULL, NULL, -1);
}

static void mNode_removeAll(mNode *self)
{
    mNode *child;
    while (_c(self)->hasChildren(self)) {
        child = _c(self)->getNode(self, 0);
        _c(self)->removeNode(self, child);
        self->count --;
    }
    self->count = 0;
}

static int mNode_removeNode(mNode *self, mNode *node)
{
    if (!self || !node || node->parent != (mObject*)self)
        return -1;

    list_del(&node->sibling);
    _c(node)->removeAll(node);

    self->count --;
    _c(node)->sendEvent(node, NCSE_NODE_NODEDELETED);

    DELETE(node);
    return 0;
}

static int list_move_node (mNode* parentNode, mNode* curNode, mNode* prevNode)
{
    if (!curNode || !(parentNode || prevNode))
        return 0;

    list_del (&curNode->sibling);
    if (prevNode)
        list_add_tail(&curNode->sibling, &prevNode->sibling);
    else 
        list_add_tail(&curNode->sibling, &parentNode->children);

    /*
    if (curNode->parent && curNode->parent == prevNode->parent) {
        list_del (&curNode->sibling);

        if (prevNode)
            list_add_tail(&curNode->sibling, &prevNode->sibling);
        else {
            mNode *parent = (mNode*)curNode->parent;
            list_add_tail(&curNode->sibling, &parent->children);
        }
    }
    */

    return 0;
}

static int mNode_moveNode(mNode *self, mNode *curNode, int count, mNode *prevNode)
{
    list_t *first_me, *last_me;
    list_t *prev_me, *next_me;
    mNode *last;

    if (count <= 0 || !curNode)
        return -1;

    if (count == 1)
        return list_move_node(self, curNode, prevNode);

    first_me = &curNode->sibling;

    last = curNode;
    while (--count) {
        last = _c(last)->getNext(last);
    }

    last_me = &last->sibling;

    prev_me = first_me->prev;
    next_me = last_me->next;

    prev_me->next = next_me;
    next_me->prev = prev_me;

    if (prevNode) {
        next_me = &prevNode->sibling;
        prev_me = next_me->prev;
    }
    else {
        /*
        mNode *parent = (mNode *)curNode->parent;
        next_me = &parent->children;
        */
        next_me = &self->children;
        prev_me = next_me->prev;
    }

    prev_me->next = first_me;
    first_me->prev = prev_me;
    next_me->prev = last_me;
    last_me->next = next_me;

    return 0;
}

static BOOL mNode_computeSize(mNode *self, HDC hdc, SIZE *size)
{
	int inter = 2, width = inter;
    if (!self || !size || hdc == 0 || hdc == HDC_INVALID)
        return FALSE;

    if (IS_VISIBLE_IMAGE(self)) {
        width += self->image->bmWidth + inter;
    }

    if (IS_VISIBLE_TEXT(self)) {
        SIZE txt_size;
        GetTabbedTextExtent (hdc, self->text, -1, &txt_size);
        width += txt_size.cx + inter;
    }

    size->cx = width;
    size->cy = _c(self)->getProperty(self, NCSP_NODE_HEIGHT);

    return TRUE;
}

static BOOL mNode_canAsRoot(mNode *self)
{
    if (self && _c(self)->getProperty(self, NCSP_NODE_ENABLED)
             && _c(self)->hasChildren(self))
        return TRUE;

    return FALSE;
}

static void mNode_construct(mNode *self, DWORD addData)
{
	NCS_NODE_DATA *data;
	Class(mObject).construct((mObject*)self, addData);

	INIT_LIST_HEAD(&self->children);
	INIT_LIST_HEAD(&self->sibling);

    data = (NCS_NODE_DATA*)addData;
    if (data && data->parent) {
        self->addData = data->addData;
        self->image = data->image;
        self->style = data->style;

        if (data->height < 0) {
            self->flags = NCSF_NODE_DEFHEIGHT;
            self->height = 0;
        }
        else {
            self->height = data->height;
            self->flags = 0;
        }

        if (data->style & NCSS_NODE_DISABLED) {
            self->flags |= NCSF_NODE_DISABLED;
        }

        if (data->style & NCSS_NODE_HIDETEXT) {
            self->flags |= NCSF_NODE_HIDETEXT;
        }
        if (data->style & NCSS_NODE_HIDEIMAGE) {
            self->flags |= NCSF_NODE_HIDEIMAGE;
        }

        _update_node_layout(self, data->style);

        _c(self)->setText(self, data->text);

        self->parent  = data->parent;
    }
    else {
        self->parent = NULL;
        self->height = 0;
        self->flags = 0;
        self->addData = 0;
        self->count = 0;
        self->layout = (void*)&_textrs_nodeops;
    }
}

static void mNode_destroy(mNode *self)
{
    self->parent = NULL;
    self->height = 0;
    self->flags = 0;
    self->addData = 0;
    self->count = 0;
    self->layout = NULL;
	INIT_LIST_HEAD(&self->children);
	INIT_LIST_HEAD(&self->sibling);
	Class(mObject).destroy((mObject*)self);
}

BEGIN_MINI_CLASS(mNode, mObject)
	CLASS_METHOD_MAP(mNode, construct)
	CLASS_METHOD_MAP(mNode, destroy)
	CLASS_METHOD_MAP(mNode, getText)
	CLASS_METHOD_MAP(mNode, getImage)
	CLASS_METHOD_MAP(mNode, setText)
	CLASS_METHOD_MAP(mNode, setImage)
	CLASS_METHOD_MAP(mNode, setProperty)
	CLASS_METHOD_MAP(mNode, getProperty)
	CLASS_METHOD_MAP(mNode, sendEvent)
	CLASS_METHOD_MAP(mNode, paint)
	CLASS_METHOD_MAP(mNode, msgHandler)
	CLASS_METHOD_MAP(mNode, getPrev)
	CLASS_METHOD_MAP(mNode, getNext)
	CLASS_METHOD_MAP(mNode, getNode)
	CLASS_METHOD_MAP(mNode, indexOf)
	CLASS_METHOD_MAP(mNode, isSelected)
	CLASS_METHOD_MAP(mNode, isEnabled)
	CLASS_METHOD_MAP(mNode, hasChildren)
	CLASS_METHOD_MAP(mNode, addNode)
	CLASS_METHOD_MAP(mNode, moveNode)
	CLASS_METHOD_MAP(mNode, insertNode)
	CLASS_METHOD_MAP(mNode, removeNode)
	CLASS_METHOD_MAP(mNode, removeAll)
	CLASS_METHOD_MAP(mNode, findNode)
	CLASS_METHOD_MAP(mNode, findNodeEx)
	CLASS_METHOD_MAP(mNode, computeSize)
	CLASS_METHOD_MAP(mNode, canAsRoot)
END_MINI_CLASS

mNode* ncsCreateNode(mObject *parent, const char* text, 
        PBITMAP image, int height, int style, DWORD addData)
{
	NCS_NODE_DATA data;
    if (!parent || 
        (!INSTANCEOF(parent, mNode) && !INSTANCEOF(parent, mAbstractList)))
        return NULL;

    data.text = (char*)text;
    data.image = image;
    data.height = height;
    data.style = style;
    data.addData = addData;
    data.parent = parent;

    return NEWEX(mNode, (DWORD)&data);
}

mNode* ncsAddNode(mObject *parent, const char* text, 
        PBITMAP image, int height, int style, DWORD addData)
{
    mNode *newNode = 
        ncsCreateNode(parent, text, image, height, style, addData);
    int ret = -1;

    //addNode
    if (newNode && parent && INSTANCEOF(parent, mAbstractList)) {
        mAbstractList *list = (mAbstractList*)parent;
        if (_c(list)->addNode)
            ret = _c(list)->addNode(list, newNode);
    }
    else if (newNode && parent && INSTANCEOF(parent, mNode)) {
        mNode *list = (mNode *)parent;
        if (_c(list)->addNode)
            ret = _c(list)->addNode(list, newNode);
    }

    if (ret < 0) {
        DELETE(newNode);
        return NULL;
    }

    return newNode;
}

void ncsSetMenuData(mObject *self, NCS_MENU_DATA *pMenuData, int count)
{
    int     i = 0;
    mNode   *curNode;
    NCS_MENU_DATA *pData;

    if (!self || !pMenuData || count <= 0)
        return;

    for (i = 0; i < count; i++) {
        pData = pMenuData + i;
        pData->parent = (mObject*)self;
        curNode = ncsAddNode((mObject*)self, pData->text, pData->image, 
                        pData->height, pData->style, pData->addData);
        if (curNode && pData->nr_child > 0) {
            ncsSetMenuData((mObject*)curNode, pData->child, pData->nr_child);
        }
    }
}

void ncsSetMenu(mObject *self, NCS_MENU_INFO *pMenuInfo)
{
    if (!self || !pMenuInfo || pMenuInfo->count <= 0)
        return;

    ncsSetMenuData(self, pMenuInfo->data, pMenuInfo->count);
}

//////////////////////////////////////////
static void mGroupNode_construct(mGroupNode *self, DWORD addData)
{
	NCS_GROUPNODE_DATA *info;
	Class(mNode).construct((mNode*)self, addData);
    info = (NCS_GROUPNODE_DATA*)addData;
    if (info)
        self->id = info->id;
}

static BOOL mGroupNode_canAsRoot(mGroupNode *self)
{
    return FALSE;
}

BEGIN_MINI_CLASS(mGroupNode, mNode)
	CLASS_METHOD_MAP(mGroupNode, construct)
	CLASS_METHOD_MAP(mGroupNode, canAsRoot)
END_MINI_CLASS

static mNode* ncsCreateGroupNode(mObject *parent, const char* text, 
        PBITMAP image, int height, int style, DWORD addData, int id)
{
	NCS_GROUPNODE_DATA data;
    if (!parent || 
        (!INSTANCEOF(parent, mNode) && !INSTANCEOF(parent, mAbstractList)))
        return NULL;

    data.text = (char*)text;
    data.image = image;
    data.height = height;
    data.style = style;
    data.addData = addData;
    data.parent = parent;
    data.id = id;

    return (mNode*)NEWEX(mGroupNode, (DWORD)&data);
}

mNode* ncsAddGroupNode(mObject *parent, const char* text, 
        PBITMAP image, int height, int style, DWORD addData, int id)
{
    int ret = -1;
    mNode *newNode = ncsCreateGroupNode(parent, text, image, 
            height, style, addData, id);

    //addNode
    if (newNode && parent && INSTANCEOF(parent, mAbstractList)) {
        mAbstractList *list = (mAbstractList*)parent;
        if (_c(list)->addNode)
            ret = _c(list)->addNode(list, newNode);
    }
    else if (newNode && parent && INSTANCEOF(parent, mNode)) {
        mNode *list = (mNode *)parent;
        if (_c(list)->addNode)
            ret = _c(list)->addNode(list, newNode);
    }

    if (ret < 0) {
        DELETE(newNode);
        return NULL;
    }

    return newNode;
}
