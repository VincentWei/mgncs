/*
 ** $Id: $
 **
 ** The implementation of mList class.
 **
 ** Copyright (C) 2009~2010 Feynman Software.
 **
 ** All rights reserved by Feynman Software.
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

//NCSLIST_xxx for internal used macro.
#define NCSLIST_FLAG_FOCUS     0x0010
#define pListLayout  (self->layout)

#define NCSLIST_LAYOUT_DEFHVAL(layout_ops)     ((layout_ops)->itemHeight)
#define NCSLIST_LAYOUT_DEFWVAL(layout_ops)     ((layout_ops)->itemWidth)

BOOL mList_adjustNodesHeight(mList *self, int diff)
{
    if (self && diff) {
        self->nodesHeight += diff;
        return _M(self, setProperty, NCSP_SWGT_CONTHEIGHT, self->nodesHeight);
    }
    return FALSE;
}

BOOL mList_adjustNodesWidth(mList *self, int diff)
{
    if (self && diff) {
        self->nodesWidth += diff;
        return _M(self, setProperty, NCSP_SWGT_CONTWIDTH, self->nodesWidth);
    }
    return FALSE;
}

void mList_drawFocusFrame(mList *self, mNode *node, HDC hdc, RECT *rcNode)
{
    if (self->flags & NCSLIST_FLAG_FOCUS && node == self->hiliteNode) {
        RECT rcFocus;
        CopyRect(&rcFocus, rcNode);
        InflateRect(&rcFocus, -1, -1);
        self->renderer->drawFocusFrame(self, hdc, &rcFocus);
    }
}
/************************* mList methods implementation **********************/
static void mList_construct (mList *self, DWORD addData)
{
	g_stmAbstractListCls.construct((mAbstractList*)self, addData);
    _M(self, initMargins, 1, 1, 1, 1);

    //reserved, selList for multiple selection
	INIT_LIST_HEAD(&self->selList);

    self->nodesHeight = 0;

    //default layout is mListLayout
    self->layout = NEW(mListLayout);
    _M(pListLayout, init,(mAbstractList*)self, GetWindowFont(self->hwnd)->size + 2, 0);

    self->textIndent = -1;
    self->imageIndent = -1;
}

static void mList_destroy(mList* self)
{
	_M(self, freeze, TRUE);
    _M(self, resetContent);
    _M(self, freeze, FALSE);
    _M(pListLayout, release);
    DELETE(pListLayout);

	Class(mAbstractList).destroy((mAbstractList *)self);
}

static void mList_resetContent(mList *self)
{
    _c(self)->removeAll(self);

    if (self->drawMode == NCSID_SWGT_ALWAYS) {
        ShowScrollBar (self->hwnd, SB_HORZ, TRUE);
        ShowScrollBar (self->hwnd, SB_VERT, TRUE);
    }

    //reset viewport
    self->contX = 0;
    self->contY = 0;
    self->contWidth = self->visWidth;
    self->contHeight = self->visHeight;

    if (_c(self)->moveContent)
        _c(self)->moveContent(self);

    _c(self)->setScrollInfo(self, TRUE);

    InvalidateRect (self->hwnd, NULL, TRUE);
}

static int mList_getRect(mList *self, mNode *node, RECT *rcNode, BOOL bConv)
{
	int ret;
    if (!self || !node || !rcNode)
        return -1;

    ret = _c(pListLayout)->getNodeRect(pListLayout, node, rcNode);
    if (ret == 0 && bConv) {
        _c(self)->contentToWindow(self, &rcNode->left, &rcNode->top);
        _c(self)->contentToWindow(self, &rcNode->right, &rcNode->bottom);
    }
    return ret;
}

static void mList_onPaint(mList *self, HDC hdc, const PCLIPRGN pinv_clip)
{
	RECT rcVis, rcCont;
    if (_M(self, isFrozen))
        return;

    _c(self)->getContRect(self, &rcCont);
    if (self->visWidth > self->realContWidth)
        rcCont.right = rcCont.left + self->visWidth;
    else
        rcCont.right = rcCont.left + self->contWidth;

    _c(self)->getVisRect(self, &rcVis);
    ClipRectIntersect (hdc, &rcVis);

    _M(pListLayout, update, hdc, &rcVis, &rcCont);
}

static void mList_removeAll(mList *self)
{
    _c(self->root)->removeAll(self->root);
    self->nodesHeight = 0;
    self->nodesWidth = 0;

    if (!_M(self, isFrozen)) {
        _c(self)->setProperty(self, NCSP_SWGT_CONTHEIGHT, self->nodesHeight);
        _c(self)->setProperty(self, NCSP_SWGT_CONTWIDTH, self->nodesWidth);
    }
}

extern BOOL mNode_changeSelectState(mNode *self, BOOL select);

static BOOL mList_selectNode(mList *self, mNode *node, BOOL select)
{
    BOOL ret;
    mNode *oldHilite = self->hiliteNode;
    if (!self || !node || (select && self->hiliteNode == node))
        return FALSE;

    ret = mNode_changeSelectState(node, select);

    if (node != self->hiliteNode && (select && ret) && self->hiliteNode) {
        mNode_changeSelectState(self->hiliteNode, FALSE);
        _c(self)->refreshNode(self, self->hiliteNode, NULL);
        self->hiliteNode = NULL;
    }

    if (ret) {
        if (select)
            self->hiliteNode = node;
        _c(self)->refreshNode(self, node, NULL);
    }

    if (oldHilite != self->hiliteNode) {
        ncsNotifyParentEx((mWidget*)self, NCSN_LIST_SELCHANGED, (DWORD)self->hiliteNode);
    }

    return ret;
}

static mNode* mList_getCurSel(mList *self)
{
    if (self)
        return self->hiliteNode;

    return NULL;
}

static BOOL mList_setCurSel(mList *self, int index)
{
    mNode *selNode = _c(self)->getNode(self, index);
    if (_c(self)->selectNode(self, selNode, TRUE)) {
        _c(self)->showNode(self, selNode);
        return TRUE;
    }
    return FALSE;
}

static mNode* mList_setRoot(mList *self, mNode *newRoot)
{
    mNode *oldRoot;
	mNode *node;

    if (!self || !newRoot || self->root == newRoot)
        return NULL;

    //parent information
    if ((INSTANCEOF(newRoot->parent, mList) && newRoot->parent != (mObject*)self)
        || (INSTANCEOF(newRoot->parent, mNode)
            && mAbstractList_getControl(newRoot) != (mAbstractList*)self))
        return NULL;

    _c(self)->freeze(self, TRUE);
    oldRoot = self->root;

    _c(pListLayout)->nodeEvent(pListLayout, NCSE_LIST_ROOTCHANGING, (DWORD)newRoot);

    //init
    self->contX = 0;
    self->contY = 0;
    self->root = newRoot;
    self->hiliteNode = NULL;

    //get hiliteNode
    node = _c(self)->getNode(self, 0);
    while(node) {
        if (!self->hiliteNode && _c(node)->isSelected(node)) {
            self->hiliteNode = node;
            break;
        }
        node = _c(node)->getNext(node);
    }

    //update layout
    _c(pListLayout)->nodeEvent(pListLayout, NCSE_LIST_ROOTCHANGED, (DWORD)newRoot);

    _c(self)->freeze(self, FALSE);

    //update hilite node
    if (!self->hiliteNode)
        _c(self)->setCurSel(self, 0);
    else
        _c(self)->showNode(self, self->hiliteNode);

    ncsNotifyParentEx ((mWidget*)self, NCSN_LIST_ROOTNODECHANGED, (DWORD)newRoot);
    return oldRoot;
}

static void mList_enterSubList(mList *self, mNode *selNode)
{
    if (!self || !selNode)
        return;

    if (selNode && _c(selNode)->canAsRoot(selNode)) {
        ncsNotifyParentEx ((mWidget*)self, NCSN_LIST_ENTERSUBLIST, (DWORD)selNode);
        mList_setRoot(self, selNode);
    }
}

static void mList_backUpList(mList *self, mNode *selNode)
{
	mNode *newRoot;
    if (!self)
        return;

	if(!selNode)
		newRoot = (mNode*)_c(self)->getProperty(self, NCSP_ASTLST_ROOTNODE);
	else
		newRoot = (mNode*)_c(selNode)->getProperty(selNode, NCSP_NODE_PARENTNODE);

	if (newRoot && INSTANCEOF(newRoot->parent, mNode)) {
        newRoot = (mNode*)newRoot->parent;

        while (!_c(newRoot)->canAsRoot(newRoot)) {
            if (INSTANCEOF(newRoot->parent, mNode)) {
                newRoot = (mNode*)newRoot->parent;
            }
        }
        ncsNotifyParentEx ((mWidget*)self, NCSN_LIST_BACKUPLIST, (DWORD)selNode);
        mList_setRoot(self, newRoot);
    }
}

static inline BOOL _is_visible_node(mList *self, mNode *node)
{
    if (self && (mObject*)(self->root) == node->parent)
        return TRUE;

    return FALSE;
}

static void mList_notifyEvent(mList* self, int eventId, DWORD eventInfo)
{
    switch (eventId) {
        case NCSE_NODE_REFRESH:
            if (_is_visible_node(self, (mNode*)eventInfo))
                _c(self)->refreshNode(self, (mNode*)eventInfo, NULL);
            break;

        case NCSE_NODE_NODEADDED:
            //mAbstractList process sorted
            Class(mAbstractList).notifyEvent((mAbstractList*)self, eventId, eventInfo);

            if (_is_visible_node(self, (mNode*)eventInfo))
                _c(pListLayout)->nodeEvent(pListLayout, eventId, eventInfo);
            break;

        case NCSE_NODE_NODEDELETED:
            //reset hilite node
            if (!_is_visible_node(self, (mNode*)eventInfo))
                return;

            if (self->hiliteNode && (mNode *)eventInfo == self->hiliteNode) {
                mNode *next = _c((mNode*)eventInfo)->getNext((mNode*)eventInfo);
				self->hiliteNode = NULL;
                if (!next)
                    next = _c((mNode*)eventInfo)->getPrev((mNode*)eventInfo);

                if (next)
                    _c(self)->selectNode(self, next, TRUE);
            }

            _c(pListLayout)->nodeEvent(pListLayout, eventId, eventInfo);
            break;

        default:
            if (_is_visible_node(self, (mNode*)eventInfo))
                _c(pListLayout)->nodeEvent(pListLayout, eventId, eventInfo);
            break;
    }
}

static BOOL mList_setNodeDefSize(mList *self, int defW, int defH)
{
    if (self) {
        int flags = 0;
        if (defW > 0 && defW != NCSLIST_LAYOUT_DEFWVAL(pListLayout)) {
            NCSLIST_LAYOUT_DEFWVAL(pListLayout) = defW;
            flags |= NCSF_LIST_LAYOUT_ITEMWCHANGED;
            self->hStepVal = defW;
        }

        if (defH > 0 && defH != NCSLIST_LAYOUT_DEFHVAL(pListLayout)) {
            NCSLIST_LAYOUT_DEFHVAL(pListLayout) = defH;
            flags |= NCSF_LIST_LAYOUT_ITEMHCHANGED;
            self->vStepVal = defH;
        }

        if (flags) {
            if (!_c(pListLayout)->recalcContSize(pListLayout, flags)) {
                InvalidateRect(self->hwnd, NULL, TRUE);
            }
            _c(self)->showNode(self, self->hiliteNode);
            return TRUE;
        }
    }
    return FALSE;
}

static BOOL mList_setProperty(mList* self, int id, DWORD value)
{
	if( id >= NCSP_LIST_MAX || !self)
		return FALSE;

	switch (id)
    {
        case NCSP_ASTLST_CMPNODEFUNC:
            if (INSTANCEOF(pListLayout, mLGroupLayout)) {
                self->nodeCmp = _M((mLGroupLayout*)pListLayout, decorateNodeCmp, (NCS_CB_CMPNODE)value);
                _c(self)->sortNodes(self, self->nodeCmp, NULL);
                return TRUE;
            }
            else
                return Class(mAbstractList).setProperty((mAbstractList*)self, id, value);

        case NCSP_SWGT_CONTHEIGHT:
            if (value < 0) {
                //value invalid
                return FALSE;
            }
            self->nodesHeight = value;
            if (_M(self, isFrozen))
                return FALSE;

            break;
        case NCSP_SWGT_CONTWIDTH:
            if (value < 0) {
                //value invalid
                return FALSE;
            }
            self->nodesWidth = value;
            if (_M(self, isFrozen))
                return FALSE;

            break;

        case NCSP_ASTLST_DEFNODEHEIGHT:
        case NCSP_LIST_DEFNODEHEIGHT:
            return _c(self)->setNodeDefSize(self, NCSLIST_LAYOUT_DEFWVAL(pListLayout), value);

        case NCSP_LIST_DEFNODEWIDTH:
            return _c(self)->setNodeDefSize(self, value, NCSLIST_LAYOUT_DEFHVAL(pListLayout));

        case NCSP_LIST_LAYOUT:
        {
            if (value >= 0 && value < NCS_LIST_MAX) {
                void *layout = NULL;
                int height = 0;

                switch(value) {
                    case NCS_LIST_LLIST:
                        layout = NEW(mListLayout);
                        height = GetWindowFont(self->hwnd)->size + 4;
                        break;
                    case NCS_LIST_LICONV:
                        layout = NEW(mLVIconLayout);
                        break;
                    case NCS_LIST_LICONH:
                        layout = NEW(mLHIconLayout);
                        break;
                    case NCS_LIST_LCBOXH:
                        layout = NEW(mLHCenterBoxLayout);
                        break;
                    case NCS_LIST_LGROUP:
                        if (_c(self)->getProperty(self, NCSP_ASTLST_AUTOSORT))
                            layout = NEW(mLGroupLayout);
                        break;
                }
                return _c(self)->setLayout(self, layout, 0, height);
            }
            return FALSE;
        }
    }

	return Class(mAbstractList).setProperty((mAbstractList*)self, id, value);
}

static DWORD mList_getProperty(mList* self, int id)
{
	if( id >= NCSP_LIST_MAX || !self)
		return -1;

    switch (id)
    {
        case NCSP_ASTLST_CMPNODEFUNC:
            if (INSTANCEOF(pListLayout, mLGroupLayout)) {
                return _c(pListLayout)->getProperty(pListLayout, NCSP_LGROUP_LAYOUT_NODECMPFUNC);
            }
            break;

        case NCSP_LIST_LAYOUT:
            if (INSTANCEOF(pListLayout, mLGroupLayout))
                return NCS_LIST_LGROUP;
            else if (INSTANCEOF(pListLayout, mLVIconLayout))
                return NCS_LIST_LICONV;
            else if (INSTANCEOF(pListLayout, mLHCenterBoxLayout))
                return NCS_LIST_LCBOXH;
            else if (INSTANCEOF(pListLayout, mLHIconLayout))
                return NCS_LIST_LICONH;
            else if (INSTANCEOF(pListLayout, mListLayout))
                return NCS_LIST_LLIST;
            else
                return -1;

        case NCSP_ASTLST_DEFNODEHEIGHT:
        case NCSP_LIST_DEFNODEHEIGHT:
            return NCSLIST_LAYOUT_DEFHVAL(pListLayout);

        case NCSP_LIST_DEFNODEWIDTH:
            return NCSLIST_LAYOUT_DEFWVAL(pListLayout);
    }

	return Class(mAbstractList).getProperty((mAbstractList*)self, id);
}

static void mList_freeze(mList *self, BOOL lock)
{
    Class(mAbstractList).freeze((mAbstractList*)self, lock);

    if (!lock) {
        _c(self)->setProperty(self, NCSP_SWGT_CONTHEIGHT, self->nodesHeight);
        _c(self)->setProperty(self, NCSP_SWGT_CONTWIDTH, self->nodesWidth);
        InvalidateRect(self->hwnd, NULL, TRUE);
    }
}

static int mList_onSizeChanged (mList* self, RECT* rcClient)
{
    int ret = Class(mAbstractList).onSizeChanged((mAbstractList*)self, rcClient);
    _c(pListLayout)->nodeEvent(pListLayout, NCSE_LIST_SIZECHANGED, 0);

    return ret;
}

static int mList_onKeyDown(mList* self, int scancode, DWORD key_flags)
{
	mNode *node;
    int direct = 0;
    if (!self)
        return 1;

    if (self->hiliteNode
        && _M(self->hiliteNode, msgHandler, MSG_KEYDOWN, scancode, key_flags, (mObject*)self) != 1)
        return 0;

    node = self->hiliteNode;

    switch(scancode) {
        case SCANCODE_CURSORBLOCKDOWN:
            direct = NCSF_LIST_DOWN;
            break;
        case SCANCODE_CURSORBLOCKRIGHT:
            direct = NCSF_LIST_RIGHT;
            break;
        case SCANCODE_CURSORBLOCKUP:
            direct = NCSF_LIST_UP;
            break;
        case SCANCODE_CURSORBLOCKLEFT:
            direct = NCSF_LIST_LEFT;
            break;
        case SCANCODE_HOME:
            direct = NCSF_LIST_HOME;
            break;

        case SCANCODE_END:
            direct = NCSF_LIST_END;
            break;

        case SCANCODE_BACKSPACE:
        {
            ncsNotifyParentEx((mWidget*)self, NCSN_LIST_BACKSPACE, (DWORD)_M(self, getCurSel));
            mList_backUpList(self, _M(self, getCurSel));
            return 0;
        }

        case SCANCODE_KEYPADENTER:
        case SCANCODE_ENTER:
        {
            ncsNotifyParentEx((mWidget*)self, NCSN_LIST_ENTER, (DWORD)_M(self, getCurSel));
            mList_enterSubList(self, _M(self, getCurSel));
            return 0;
        }

        case SCANCODE_PAGEDOWN:
        case SCANCODE_PAGEUP:
            return _c(self)->super->onKeyDown((void*)self, scancode, key_flags);
    }

    if (direct && (node || (direct == NCSF_LIST_HOME || direct == NCSF_LIST_END))) {
        int loop = 0;

        if (direct != NCSF_LIST_HOME && direct != NCSF_LIST_END)
            loop = (GetWindowStyle(self->hwnd) & NCSS_LIST_LOOP) ? NCSF_LIST_LOOP : 0;

        node = _M(pListLayout, onDirKey, node, loop | direct);

        if (direct == NCSF_LIST_HOME) {
            direct = NCSF_LIST_RIGHT;
        }
        else if (direct == NCSF_LIST_END) {
            direct = NCSF_LIST_LEFT;
        }

        while (node &&  node != self->hiliteNode && !_M(node, getProperty, NCSP_NODE_ENABLED)) {
            node = _M(pListLayout, onDirKey, node, loop | direct);
        }

        if (node &&  node != self->hiliteNode) {
            _M(self, selectNode, node, TRUE);
            _M(self, showNode, node);
        }
    }

    return 1;
}

static int mList_onLButtonDown(mList *self, int x, int y, DWORD key_flags)
{
    RECT rcVis;
    mNode *node = NULL;
    int mouseX = x, mouseY = y;

    _M(self, getVisRect, &rcVis);
    if (!PtInRect (&rcVis, mouseX, mouseY))
        return 1;

    _M(self, windowToContent, &mouseX, &mouseY);

    node = _M(pListLayout, onMouseHit, mouseX, mouseY);
    if (node){
        ncsNotifyParentEx((mWidget*)self, NCSN_WIDGET_CLICKED, (DWORD)node);

        if (!(GetWindowStyle(self->hwnd) & NCSS_LIST_MULTIPLE)) {
            if (node != self->hiliteNode) {
                _M(self, selectNode, node, TRUE);
                _M(self, showNode, node);
            } else if (node){
                _M(node, msgHandler, MSG_LBUTTONDOWN, MAKELONG(x, y), key_flags, (mObject*)self);
            }
        }
    }

    return 1;
}

static int mList_onLButtonDBClk(mList *self, int x, int y, DWORD key_flags)
{
    RECT rcVis;
    mNode *node = NULL;
    int mouseX = x, mouseY = y;

    _M(self, getVisRect, &rcVis);
    if (!PtInRect (&rcVis, mouseX, mouseY))
        return 1;

    _M(self, windowToContent, &mouseX, &mouseY);
    node = _M(pListLayout, onMouseHit, mouseX, mouseY);

    if (node)
        mList_enterSubList(self, node);
    return 1;
}

static int mList_onChar(mList* self, int asciiCode, DWORD keyFlags)
{
	mNode* node;
	int start = 0;
	unsigned char head[2];

	if (HIBYTE (asciiCode))
        return 1;

    head [0] = LOBYTE (asciiCode);
    head [1] = '\0';

    if (head[0] == ' ') {
        if (!self->hiliteNode) {
            return 1;
        }

        if (!_M(self->hiliteNode, isEnabled))
            return 1;

        //TODO:for multiple, select or deselect
        return 1;
    }

    if (self->hiliteNode)
        start = _M(self->hiliteNode, indexOf);

    node = _M(self, findNode, (DWORD)head, NCS_NODE_FTSTRING, FALSE, start);

    if (!node && start != 0) {
        node = _M(self, findNode, (DWORD)head, NCS_NODE_FTSTRING, FALSE, 0);
    }

    if (node) {
        if (_M(node, isEnabled)) {
            _M(self, selectNode, node, TRUE);
            _M(self, showNode, node);
        }
    }
    return 1;
}

static int mList_wndProc(mList* self, int message, WPARAM wParam, LPARAM lParam)
{
    switch(message){
        case MSG_CHAR:
            mList_onChar(self, wParam, lParam);
            break;

        case MSG_SETFOCUS:
            if (!(self->flags & NCSLIST_FLAG_FOCUS)) {
                self->flags |= NCSLIST_FLAG_FOCUS;
                if (self->hiliteNode) {
                    _M(self, showNode, self->hiliteNode);
                }
            }
            break;
        case MSG_KILLFOCUS:
            self->flags &= ~NCSLIST_FLAG_FOCUS;
            break;

		case MSG_LBUTTONDBLCLK:
            return mList_onLButtonDBClk(self, LOSWORD (lParam), HISWORD(lParam), (DWORD)wParam);
    }

    return Class(mAbstractList).wndProc((mAbstractList *)self, message, wParam, lParam);
}

mListLayout* mList_getLayout(mList *self)
{
	return self != NULL ? pListLayout : NULL;
}

BOOL mList_setLayout(mList *self, mListLayout *layout,
        int defNodeW, int defNodeH)
{
    BOOL resort = FALSE, frozen;

    if (!self || !layout ||
            pListLayout == layout || !INSTANCEOF(layout, mListLayout))
        return FALSE;

    if (!(frozen = _M(self, isFrozen)))
        _M(self, freeze, TRUE);

    //release previous layout
    if (INSTANCEOF(pListLayout, mLGroupLayout)) {
        resort = TRUE;
        self->nodeCmp = (NCS_CB_CMPNODE)_M(self, getProperty, NCSP_ASTLST_CMPNODEFUNC);
    }

    _M(pListLayout, release);
	DELETE(pListLayout);

    //set new layout
    pListLayout = layout;
    _M(pListLayout, addRef);
    _M(pListLayout, init, (mAbstractList*)self, defNodeW, defNodeH);

    if (resort && !INSTANCEOF(layout, mLGroupLayout)) {
        NCS_CB_CMPNODE func = (NCS_CB_CMPNODE)_M(self, getProperty, NCSP_ASTLST_CMPNODEFUNC);
        //reset callback and update sorting
        _M(self, sortNodes, func, NULL);
    }
    /*
    else
        InvalidateRect(self->hwnd, NULL, TRUE);
    */

    if (self->hiliteNode)
		_M(self, showNode, self->hiliteNode);

    if (!frozen)
        _M(self, freeze, FALSE);

    return TRUE;
}

BEGIN_CMPT_CLASS(mList, mAbstractList)
	CLASS_METHOD_MAP(mList, construct)
	CLASS_METHOD_MAP(mList, destroy)
	CLASS_METHOD_MAP(mList, onPaint)
	CLASS_METHOD_MAP(mList, getProperty)
	CLASS_METHOD_MAP(mList, setProperty)
    CLASS_METHOD_MAP(mList, wndProc)
	CLASS_METHOD_MAP(mList, onSizeChanged)
	CLASS_METHOD_MAP(mList, onKeyDown)
	CLASS_METHOD_MAP(mList, onLButtonDown)
	CLASS_METHOD_MAP(mList, freeze)
	CLASS_METHOD_MAP(mList, getRect)
	CLASS_METHOD_MAP(mList, resetContent)
	CLASS_METHOD_MAP(mList, removeAll)
	CLASS_METHOD_MAP(mList, notifyEvent)
	CLASS_METHOD_MAP(mList, selectNode)
	CLASS_METHOD_MAP(mList, setCurSel)
	CLASS_METHOD_MAP(mList, getCurSel)
	CLASS_METHOD_MAP(mList, enterSubList)
	CLASS_METHOD_MAP(mList, backUpList)
	CLASS_METHOD_MAP(mList, setLayout)
	CLASS_METHOD_MAP(mList, getLayout)
	CLASS_METHOD_MAP(mList, setNodeDefSize)
	SET_DLGCODE(DLGC_WANTARROWS | DLGC_WANTCHARS | DLGC_WANTENTER);
END_CMPT_CLASS

static void foreach_node_children(mNode* node, void (*foreach)(mNode* node, void* param), void* param)
{
	if(!node)
		return;
	node = _M(node, getNode, 0);
	while(node)
	{
		foreach(node, param);
		foreach_node_children(node, foreach, param);
		node = _M(node, getNext);
	}
}

void mList_foreachNode(mList *list, void (*foreach)(mNode *node, void* param), void* param)
{
	mNode *node;
	if(!list || ! foreach)
		return ;

	node = (mNode*)_M(list, getProperty, NCSP_ASTLST_ROOTNODE);

	foreach_node_children(node, foreach, param);
}

#endif //_MGNCSCTRL_LIST
