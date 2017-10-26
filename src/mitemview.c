/* 
 ** $Id: mitemview.c 1681 2017-10-26 06:46:31Z weiym $
 **
 ** The implementation of ItemView.
 **
 ** Copyright (C) 2009 Feynman Software.
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

#include "mgncsconfig.h"
#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#include "mwidget.h"
#include "mrdr.h"
#include "mscroll_widget.h"

#include "mdblist.h"
#include "mitem.h"
#include "mitem_manager.h"
#include "mitemview.h"

#define mglist_for_each(pos, itemlist) \
    list_for_each(pos, &((mItemManager *)itemlist)->queue)

#define mglist_entry(ptr) \
    list_entry(ptr, mItem, list)

static int mItemView_getRect (mItemView *self, 
        HITEM hItem, RECT *rcItem, BOOL bConv)
{
    if (!self || !rcItem || !hItem)
        return -1;

    rcItem->top = _c(pItemList)->getItemYPos(pItemList, hItem);
    if (rcItem->top == -1)
        return -1;

    if (bConv)
        _c(self)->contentToWindow(self, NULL, &rcItem->top);

    rcItem->bottom = rcItem->top + _c(self)->getItemHeight(self, hItem);
    rcItem->left = 0;
    if (bConv)
        _c(self)->contentToWindow(self, &rcItem->left, NULL);
    if (self->realContWidth)
        rcItem->right = rcItem->left + self->realContWidth;
    else
        rcItem->right = rcItem->left + self->contWidth;

    return 0;
}

static int mItemView_inItem (mItemView *self, int mouseX, int mouseY, 
                     HITEM *pRet, POINT *pt)
{
    return _c(pItemList)->inItem(pItemList, mouseX, mouseY, pRet, pt);
}

static void mItemView_construct (mItemView *self, DWORD addData)
{
	Class(mScrollWidget).construct((mScrollWidget*)self, addData);

    self->defItemHeight = GetWindowFont(self->hwnd)->size;
    memset (&self->itemOps, 0, sizeof(self->itemOps));

    pItemList = (mItemManager*)NEW(mItemManager);
    pItemList->obj = (mWidget*)self;
}

static HITEM mItemView_createItem (mItemView *self, HITEM prev, HITEM next, 
        int index, int height, const char* text, DWORD addData, int *pos, BOOL adjust)
{
    int     ret = -1;
    mItem*  newItem = (mItem*)NEW(mItem);

    if (!newItem)
        return -1;

    if (height < 0)
        newItem->height = self->defItemHeight;
    else
        newItem->height = height;

    newItem->addData = addData;

    if (text && strlen(text) >= 1) {
        _c(newItem)->setItemString(newItem, text);
    }

    ret = _c(self)->insertItem(self, (HITEM)newItem, prev, next, index, pos);

    if (ret < 0) {
       DELETE(newItem); 
       return 0;
    }

    if (adjust)
        _c(self)->adjustItemsHeight(self, newItem->height);

    return (HITEM)newItem;
}

static void mItemView_destroy(mItemView* self)
{
    _c(self)->freeze(self, TRUE);
    _c(self)->removeAll(self);
    if (pItemList) 
		DELETE((mItem*)pItemList);

    Class(mScrollWidget).destroy((mScrollWidget*)self);
}

static void mItemView_freeze(mItemView *self, BOOL lock)
{
    _c(pItemList)->freeze(pItemList, lock);

    if (!lock) {
        _c(self)->setProperty(self, 
                NCSP_SWGT_CONTHEIGHT, _c(self)->getTotalHeight(self));
        InvalidateRect(self->hwnd, NULL, TRUE);
    }
}

static BOOL mItemView_isFrozen(mItemView *self)
{
    return _c(pItemList)->isFrozen(pItemList);
}
 
static void mItemView_refreshRect (mItemView* self, const RECT *rc)
{
    if (_c(pItemList)->isFrozen(pItemList))
        return;

    Class(mScrollWidget).refreshRect((mScrollWidget*)self, rc);
}

static int mItemView_adjustItemsHeight(mItemView* self, int diff)
{
    int height;
    _c(pItemList)->adjustItemsHeight(pItemList, diff);

    if (_c(pItemList)->isFrozen(pItemList))
        return 0;

    height = _c(self)->getTotalHeight(self);

    if (diff) {
        _c(self)->setProperty(self, NCSP_SWGT_CONTHEIGHT, height + 1);
    }
    _c(self)->refreshRect(self, NULL);
    return height;
}

static NCS_CB_DRAWITEM mItemView_setItemDraw(mItemView *self, NCS_CB_DRAWITEM func) 
{
    NCS_CB_DRAWITEM oldfn;

    oldfn = self->itemOps.drawItem;
    self->itemOps.drawItem = func;

    _c(self)->refreshRect(self, NULL);
    return oldfn;
}

static NCS_CB_INITITEM mItemView_setItemInit(mItemView *self, NCS_CB_INITITEM func) 
{
    NCS_CB_INITITEM oldfn;

    oldfn = self->itemOps.initItem;
    self->itemOps.initItem = func;

    return oldfn;
}

static NCS_CB_DSTRITEM mItemView_setItemDestroy(mItemView *self, NCS_CB_DSTRITEM func) 
{
    NCS_CB_DSTRITEM oldfn;

    oldfn = self->itemOps.destroyItem;
    self->itemOps.destroyItem = func;

    return oldfn;
}

static DWORD mItemView_getAddData(mItemView *self, HITEM hItem)   
{
    return _c(pItemList)->getAddData(pItemList, hItem);
}

static void mItemView_setAddData(mItemView *self, 
        HITEM hItem, DWORD addData)   
{
    _c(pItemList)->setAddData(pItemList, hItem, addData);
}

static void mItemView_refreshItem(mItemView *self, 
        HITEM hItem, const RECT *rcInv)
{
    RECT rcItem, rcTmp;

    if (_c(pItemList)->isFrozen(pItemList))
        return;

    if (hItem) {
        _c(self)->getRect (self, hItem, &rcItem, TRUE);

        if (rcInv) {
            rcTmp = *rcInv;
            OffsetRect (&rcTmp, rcItem.left, rcItem.top);

            if (IntersectRect(&rcTmp, &rcItem, &rcTmp))
                InvalidateRect (self->hwnd, &rcTmp, TRUE);
        }
        else {
            InvalidateRect(self->hwnd, &rcItem, TRUE);
        }
    }

}

static int mItemView_getCurSel(mItemView *self) 
{
    HITEM hItem;
    if (_c(pItemList)->getSelection(pItemList, &hItem, 1))
        return _c(pItemList)->indexOf(pItemList, hItem);

    return -1;
}

static int mItemView_setCurSel(mItemView *self, int newSel) 
{
    HITEM hItem;

    hItem = (HITEM) _c(pItemList)->getItem(pItemList, newSel);

    if (hItem) {
        _c(self)->hilight(self, hItem);
        return 0;
    }
    return -1;
}

static BOOL mItemView_showItemByIdx(mItemView *self, int index)
{
    HITEM hItem = _c(self)->getItem(self, index);
    return _c(self)->showItem(self, hItem);
}

static BOOL mItemView_showItem(mItemView *self, HITEM hItem) 
{
    RECT rcItem;
    int pos_y;

    if (!hItem) return FALSE;

    if (_c(self)->getRect (self, hItem, &rcItem, FALSE) < 0)
        return FALSE;

    if (rcItem.top >= self->contY && rcItem.bottom <= self->contY + self->visHeight)
    {
        _c(self)->contentToWindow(self, &rcItem.left, &rcItem.top);
        _c(self)->contentToWindow(self, &rcItem.right, &rcItem.bottom);
        InvalidateRect(self->hwnd, &rcItem, TRUE);
        return TRUE;
    }

    pos_y = rcItem.top;
    if (rcItem.bottom <= self->contY) {
        pos_y = rcItem.top;
    }
    else if (rcItem.top > self->contY) {
        pos_y = rcItem.bottom;
    }
    return _c(self)->makePosVisible(self, self->contX, pos_y); 
}

static BOOL mItemView_deselect(mItemView *self, HITEM hItem) 
{
    if (_c(pItemList)->select(pItemList, hItem, FALSE)) {
        _c(self)->refreshItem(self, hItem, NULL);
        return TRUE;
    }

    return FALSE;
}

static BOOL mItemView_select(mItemView *self, HITEM hItem) 
{
    HITEM hilight = _c(self)->getHilight(self);

    if (hilight != hItem)
        _c(pItemList)->select(pItemList, hilight, FALSE);

    if (_c(pItemList)->select(pItemList, hItem, TRUE)) {
        _c(self)->refreshItem(self, hItem, NULL);
        return TRUE;
    }

    return FALSE;
}

static NCS_CB_CMPITEM mItemView_setItemCmpFunc(mItemView *self, NCS_CB_CMPITEM func) 
{
    return _c(pItemList)->setItemCmpFunc(pItemList, func);
}

static void mItemView_sortItems(mItemView *self, NCS_CB_CMPITEM func) 
{
    if (func) {
        _c(self)->freeze(self, TRUE);
        _c(pItemList)->sortItems(pItemList, func);
        _c(self)->freeze(self, FALSE);
    }
}

static int mItemView_getItemCount(mItemView *self)
{
    return _c(pItemList)->getItemCount(pItemList);
}

static int mItemView_insertItem(mItemView* self, HITEM hItem, 
        HITEM prev, HITEM next, int index, int *pos)
{
    if (GetWindowStyle(self->hwnd) & NCSS_ITEMV_AUTOSORT) {
        _c(self)->setAutoSortItem(self, TRUE);
    }

    if (self->itemOps.initItem) {
        if (self->itemOps.initItem (self, hItem) <0) {
            return -1;
        }
    }

    return _c(pItemList)->insertItem(pItemList, hItem, prev, next, index, pos);
}

static int mItemView_removeItem(mItemView* self, HITEM hItem)
{
    int ret;
    int delH;

    if (!hItem)
        return -1;

    delH = _c(self)->getItemHeight(self, hItem);

    if (self->itemOps.destroyItem) {
        self->itemOps.destroyItem (self, hItem);
    }

    ret = _c(pItemList)->removeItem(pItemList, hItem);
    //if  remove success
    if (ret == 0) {
        //DELETE((mItem*)hItem); 
        _c(self)->adjustItemsHeight(self, -delH);
    }
    return ret;
}

static BOOL mItemView_removeAll(mItemView* self)
{
    BOOL isFrozen = _c(self)->isFrozen(self);

    if (!isFrozen) {
        _c(self)->freeze(self, TRUE);
    }
    _c(pItemList)->removeAll(pItemList);

    if (!isFrozen) {
        _c(self)->freeze(self, FALSE);
    }
    return TRUE;
}

static int mItemView_indexOf(mItemView* self, HITEM hItem)
{
    return _c(pItemList)->indexOf(pItemList, hItem);
}

static HITEM mItemView_getItem(mItemView* self, int index)
{
    return _c(pItemList)->getItem(pItemList, index);
}

static BOOL mItemView_isSelected(mItemView* self, HITEM hItem)
{
    return _c(pItemList)->isSelected(pItemList, hItem);
}

static BOOL mItemView_isEnabled(mItemView* self, HITEM hItem)
{
    return _c(pItemList)->isEnabled(pItemList, hItem);
}

static BOOL mItemView_enable(mItemView* self, HITEM hItem, BOOL enable)
{
    return _c(pItemList)->enable(pItemList, hItem, enable);
}

static void mItemView_selectAll(mItemView* self)
{
    if (!(GetWindowStyle(self->hwnd) & NCSS_ITEMV_MULTIPLE))
        return;

    _c(pItemList)->selectAll(pItemList);
    InvalidateRect (self->hwnd, NULL, TRUE);
}

static void mItemView_deselectAll(mItemView* self)
{
    _c(pItemList)->deselectAll(pItemList);
    InvalidateRect (self->hwnd, NULL, TRUE);
}

static BOOL _has_visible(mItemView *self, HITEM hItem) 
{
    RECT rcItem;
    if (!hItem) return FALSE;

    if (_c(self)->getRect (self, hItem, &rcItem, FALSE) < 0)
        return FALSE;

    if (rcItem.top >= self->contY && rcItem.bottom <= self->contY + self->visHeight) {
        return TRUE;
    }

    return FALSE;
}

static int mItemView_hilight(mItemView* self, HITEM hItem)
{
    if (hItem) {
        HITEM oldHilight = _c(self)->getHilight(self);

        if (hItem == oldHilight) {
            return 0;
        }

        ncsNotifyParentEx((mWidget*)self, NCSN_ITEMV_SELCHANGING, (DWORD)oldHilight);

        if (oldHilight && !(GetWindowStyle(self->hwnd) & NCSS_ITEMV_MULTIPLE))
            _c(pItemList)->select(pItemList, oldHilight, FALSE);

        _c(pItemList)->hilight(pItemList, hItem);

        ncsNotifyParentEx((mWidget*)self, NCSN_ITEMV_SELCHANGED, (DWORD)hItem);

        if (_has_visible(self, hItem)) {
            //refresh old and new hilight item.
            if (oldHilight) _c(self)->refreshItem(self, oldHilight, NULL);
            _c(self)->refreshItem(self, hItem, NULL);
        }
        else {
            //refresh all
            _c(self)->showItem(self, hItem);
        }
    }
    return 0;
}

static HITEM mItemView_getHilight(mItemView* self)
{
    return _c(pItemList)->getHilight(pItemList);
}

static BOOL mItemView_isHilight(mItemView* self, HITEM hItem)
{
    return _c(pItemList)->isHilight(pItemList, hItem);
}

static BOOL mItemView_setTextByIdx(mItemView* self, int index, const char* string)
{
    HITEM hItem;

    if (!string)
        return FALSE;

    hItem = _c(self)->getItem(self, index);
    return _c(self)->setText(self, hItem, string);
}
 
extern int _im_strcmp_item (mItemManager* self, HITEM hItem1, HITEM hItem2);
static BOOL mItemView_setText(mItemView* self, HITEM hItem, const char* string)
{
    if (!string)
        return FALSE;

    if (_c(pItemList)->setText(pItemList, hItem, string)) {

        if (GetWindowStyle(self->hwnd) & NCSS_ITEMV_AUTOSORT) {
            NCS_CB_CMPITEM defCmp = pItemList->itemCmp;
            _c(self)->sortItems(self, defCmp ? defCmp : _im_strcmp_item);
        }
        else
            _c(self)->refreshItem(self, hItem, NULL);
        return TRUE;
    }
    return FALSE;
}

static const char* mItemView_getTextByIdx(mItemView* self, int index)
{
    HITEM hItem = _c(self)->getItem(self, index);
    return _c(pItemList)->getText(pItemList, hItem);
}

static const char* mItemView_getText(mItemView* self, HITEM hItem)
{
    return _c(pItemList)->getText(pItemList, hItem);
}

static int mItemView_getTextLen(mItemView* self, HITEM hItem)
{
    return _c(pItemList)->getTextLen(pItemList, hItem);
}

static BOOL mItemView_getSelection(mItemView* self, HITEM *pRet, int count)
{
    return _c(pItemList)->getSelection(pItemList, pRet, count);
}

static int mItemView_getSelectionCount(mItemView* self)
{
    return _c(pItemList)->getSelectionCount(pItemList);
}

static void mItemView_setAutoSortItem (mItemView *self, BOOL sort)
{
    _c(pItemList)->setAutoSortItem(pItemList, sort);
}

static HITEM mItemView_getNext(mItemView* self, HITEM hItem)
{
    return _c(pItemList)->getNext(pItemList, hItem);
}

static HITEM mItemView_getPrev(mItemView* self, HITEM hItem)
{
    return _c(pItemList)->getPrev(pItemList, hItem);
}

static NCS_CB_CMPSTR mItemView_setStrCmpFunc(mItemView* self, NCS_CB_CMPSTR func)
{
    if (_c(self)->getItemCount(self) == 0 && func) {
        return _c(pItemList)->setStrCmpFunc(pItemList, func);
    }

    return NULL;
}

static NCS_CB_CMPSTR mItemView_getStrCmpFunc(mItemView* self)
{
    return _c(pItemList)->getStrCmpFunc(pItemList);
}

static int mItemView_getTotalHeight(mItemView *self)
{
    return _c(pItemList)->getTotalHeight(pItemList);
}

static int procBtnDown(mItemView* self, int x, int y, DWORD keyFlags)
{
    int nItem;
    RECT rcVis;
    HITEM hItem, hiliteItem;
    int mouseX = x, mouseY = y;

    if ( !_c(self)->inItem )
        return 1;

    _c(self)->getVisRect(self, &rcVis);

    /* not in the visible area */
    if (!PtInRect (&rcVis, mouseX, mouseY))
        return 1;

    _c(self)->windowToContent(self, &mouseX, &mouseY);

    if ( (nItem = _c(self)->inItem(self, mouseX, mouseY, &hItem, NULL)) >= 0 ) {
        ncsNotifyParentEx((mWidget*)self, NCSN_ITEMV_CLICKED, (DWORD)hItem);

        hiliteItem = _c(self)->getHilight(self);
        if (hItem != hiliteItem) {
            _c(self)->hilight(self, hItem);
        }
    }

    return 1;
}

static int mItemView_onLButtonDown(mItemView* self, int x, int y, DWORD keyFlags)
{
    return procBtnDown(self, x, y, keyFlags);
}

static int mItemView_onRButtonDown(mItemView* self, int x, int y, DWORD keyFlags)
{
    return procBtnDown(self, x, y, keyFlags);
}

static int mItemView_getItemHeight(mItemView *self, HITEM hItem)
{
    return _c(pItemList)->getHeight(pItemList, hItem);
}

static void mItemView_resetContent(mItemView *self)
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
    else
        InvalidateRect (self->hwnd, NULL, TRUE);

    _c(self)->setScrollInfo(self, TRUE);
}

static int mItemView_setItemHeight (mItemView *self, HITEM hItem, int height)
{
    return _c(pItemList)->setHeight(pItemList, hItem, height);
}

static int mItemView_wndProc(mItemView* self, 
            int message, WPARAM wParam, LPARAM lParam)
{	
    switch(message){
        case MSG_RBUTTONDOWN:
            if (_c(self)->onRButtonDown 
                    && _c(self)->onRButtonDown(self, LOSWORD(lParam), HISWORD(lParam), wParam))
                break;

            return 0;

        case MSG_RBUTTONUP:
            if (_c(self)->onRButtonUp
                && _c(self)->onRButtonUp(self, LOSWORD(lParam), HISWORD(lParam), wParam))
                break;

            return 0;

        case MSG_LBUTTONDBLCLK:
            if (_c(self)->onLButtonDBClk
                && _c(self)->onLButtonDBClk(self, LOSWORD(lParam), HISWORD(lParam), wParam))
                    break;

            return 0;

        case MSG_SETFOCUS:
            ncsNotifyParent ((mWidget*)self, NCSN_ITEMV_SETFOCUS);
            break;

        case MSG_KILLFOCUS:
            ncsNotifyParent ((mWidget*)self, NCSN_ITEMV_KILLFOCUS);
            break;

        default:
            break;
    }
	
	return Class(mScrollWidget).wndProc((mScrollWidget*)self, message, wParam, lParam);
}

static void mItemView_onPaint(mItemView *self, 
        HDC hdc, const PCLIPRGN pinv_clip)
{
    list_t *me;
    HITEM hItem;
    RECT rcDraw;
    int h = 0, itemH = 0;
    RECT rcVis;

    rcDraw.left = 0;
    rcDraw.top = 0;

    if (self->realContWidth)
        rcDraw.right = self->realContWidth;
    else
        rcDraw.right = self->contWidth;

    if (self->realContHeight)
        rcDraw.bottom = self->realContHeight;
    else
        rcDraw.bottom = self->contHeight;

    _c(self)->contentToWindow(self, &rcDraw.left, &rcDraw.top);
    _c(self)->contentToWindow(self, &rcDraw.right, &rcDraw.bottom);
    _c(self)->getVisRect(self, &rcVis);

    ClipRectIntersect (hdc, &rcVis);

    mglist_for_each (me, pItemList) {
        hItem = (HITEM)mglist_entry (me);
        rcDraw.top += h;
        itemH = _c(self)->getItemHeight(self, hItem);
        rcDraw.bottom = rcDraw.top + itemH;

        if (rcDraw.bottom < rcVis.top) {
            h = itemH; 
            continue;
        }

        if (rcDraw.top > rcVis.bottom)
            break;

        if (self->itemOps.drawItem && itemH > 0) {
            self->itemOps.drawItem (self, hItem, hdc, &rcDraw);
        }
        h = itemH; 
    }
}

static int mItemView_isEmpty(mItemView *self)
{
    return _c(pItemList)->isEmpty(pItemList);
}

static HITEM mItemView_getFirstItem(mItemView *self)
{
    return _c(pItemList)->getFirstItem(pItemList);
}

static DWORD mItemView_getImage(mItemView* self, HITEM hItem)
{
    return _c(pItemList)->getImage(pItemList, hItem);
}

static void mItemView_setImage(mItemView* self, HITEM hItem, DWORD image)
{
    _c(pItemList)->setImage(pItemList, hItem, image);
}

static DWORD mItemView_getFlags(mItemView* self, HITEM hItem)
{
    return _c(pItemList)->getFlags(pItemList, hItem);
}

static void mItemView_setFlags(mItemView* self, HITEM hItem, DWORD image)
{
    _c(pItemList)->setFlags(pItemList, hItem, image);
}

static HITEM mItemView_getListEntry(mItemView* self, list_t* entry)
{
    return _c(pItemList)->getListEntry(entry);
}

static list_t* mItemView_getQueue(mItemView* self)
{
    return _c(pItemList)->getQueue(pItemList);
}

static int mItemView_getFirstVisItem(mItemView *self)
{
    int i = 0;
    int top = 0, bottom = 0, h = 0; 
    list_t *me;
    HITEM hItem;

    mglist_for_each(me, pItemList) {
        hItem = (HITEM)mglist_entry (me);
        top += h; 
        h = _c(self)->getItemHeight(self, hItem);
        bottom = top + h;
        _c(self)->contentToViewport(self, &top, &bottom);

        if (bottom >= 0)
            break;

        i++;
    }

    return i;
}

static int
mItemView_onKeyDown(mItemView* self, int scancode, int state)
{
    HITEM hItem = 0, curHilighted;
    DWORD style;

    if (!self)
        return 1;

    curHilighted = _c(self)->getHilight(self);
    style = GetWindowStyle(self->hwnd);

    switch (scancode) {
        case SCANCODE_CURSORBLOCKRIGHT:
        case SCANCODE_CURSORBLOCKDOWN:
            hItem = _c(self)->getNext(self, curHilighted);
            if (style & NCSS_ITEMV_LOOP && !hItem) {
                hItem = _c(self)->getNext(self, 0);
            }
            break;

        case SCANCODE_CURSORBLOCKLEFT:
        case SCANCODE_CURSORBLOCKUP:
            hItem = _c(self)->getPrev(self, curHilighted);
            if (style & NCSS_ITEMV_LOOP && !hItem) {
                hItem = _c(self)->getPrev(self, 0);
            }
            break;
        case SCANCODE_HOME:
            hItem = _c(self)->getNext(self, 0);
            break;
        case SCANCODE_END:
            hItem = _c(self)->getPrev(self, 0);
            break;

        case SCANCODE_KEYPADENTER:
        case SCANCODE_ENTER:
            ncsNotifyParent ((mWidget*)self, NCSN_ITEMV_ENTER);
            return 0;

    }

    while (hItem && _c(self)->getItemHeight(self, hItem) <= 0) {
        hItem = (scancode == SCANCODE_CURSORBLOCKDOWN || scancode == SCANCODE_HOME) ?
            _c(self)->getNext(self, hItem) :_c(self)->getPrev(self, hItem);
    }
    if ((style & NCSS_ITEMV_LOOP) 
            && (scancode == SCANCODE_CURSORBLOCKDOWN) && !hItem)
        hItem = _c(self)->getNext(self, 0);

    if (hItem) {
        if (hItem != curHilighted) {
            _c(self)->hilight(self, hItem);
        }
    }
    return 0;
}

static BOOL mItemView_setProperty(mItemView* self, int id, DWORD value)
{
	if( id >= NCSP_ITEMV_MAX)
		return FALSE;

	switch(id)
	{
        case NCSP_ITEMV_DEFITEMHEIGHT:
            if (value > 0) {
                self->defItemHeight = value;
                return TRUE;
            }
            return FALSE;

        case NCSP_ITEMV_ITEMCOUNT:
            return FALSE;
    }

	return Class(mScrollWidget).setProperty((mScrollWidget*)self, id, value);
}

static DWORD mItemView_getProperty(mItemView* self, int id)
{
	if( id >= NCSP_ITEMV_MAX)
		return -1;

	switch(id)
	{
        case NCSP_ITEMV_DEFITEMHEIGHT:
            return self->defItemHeight;

        case NCSP_ITEMV_ITEMCOUNT:
            return _c(self)->getItemCount(self);
    }

	return Class(mScrollWidget).getProperty((mScrollWidget*)self, id);
}

BEGIN_CMPT_CLASS(mItemView, mScrollWidget)
	CLASS_METHOD_MAP(mItemView, construct)
	CLASS_METHOD_MAP(mItemView, destroy)
	CLASS_METHOD_MAP(mItemView, setProperty)
	CLASS_METHOD_MAP(mItemView, getProperty)
	CLASS_METHOD_MAP(mItemView, wndProc)
	CLASS_METHOD_MAP(mItemView, onPaint)
	CLASS_METHOD_MAP(mItemView, onKeyDown)
	CLASS_METHOD_MAP(mItemView, onLButtonDown)
	CLASS_METHOD_MAP(mItemView, onRButtonDown)
	CLASS_METHOD_MAP(mItemView, setStrCmpFunc)
	CLASS_METHOD_MAP(mItemView, getStrCmpFunc)
	CLASS_METHOD_MAP(mItemView, freeze)
	CLASS_METHOD_MAP(mItemView, setItemDraw)
	CLASS_METHOD_MAP(mItemView, setItemInit)
	CLASS_METHOD_MAP(mItemView, setItemDestroy)
	CLASS_METHOD_MAP(mItemView, getAddData)
	CLASS_METHOD_MAP(mItemView, setAddData)
	CLASS_METHOD_MAP(mItemView, refreshItem)
	CLASS_METHOD_MAP(mItemView, getCurSel)
	CLASS_METHOD_MAP(mItemView, setCurSel)
	CLASS_METHOD_MAP(mItemView, showItem)
	CLASS_METHOD_MAP(mItemView, showItemByIdx)
	CLASS_METHOD_MAP(mItemView, setItemCmpFunc)
	CLASS_METHOD_MAP(mItemView, sortItems)
	CLASS_METHOD_MAP(mItemView, getItemCount)
	CLASS_METHOD_MAP(mItemView, getItemHeight)
	CLASS_METHOD_MAP(mItemView, insertItem)
	CLASS_METHOD_MAP(mItemView, createItem)
	CLASS_METHOD_MAP(mItemView, removeItem)
	CLASS_METHOD_MAP(mItemView, removeAll)
	CLASS_METHOD_MAP(mItemView, indexOf)
	CLASS_METHOD_MAP(mItemView, getItem)
	CLASS_METHOD_MAP(mItemView, isSelected)
	CLASS_METHOD_MAP(mItemView, isEnabled)
	CLASS_METHOD_MAP(mItemView, enable)
	CLASS_METHOD_MAP(mItemView, select)
	CLASS_METHOD_MAP(mItemView, deselect)
	CLASS_METHOD_MAP(mItemView, selectAll)
	CLASS_METHOD_MAP(mItemView, deselectAll)
	CLASS_METHOD_MAP(mItemView, hilight)
	CLASS_METHOD_MAP(mItemView, getHilight)
	CLASS_METHOD_MAP(mItemView, isHilight)
	CLASS_METHOD_MAP(mItemView, setText)
	CLASS_METHOD_MAP(mItemView, setTextByIdx)
	CLASS_METHOD_MAP(mItemView, getText)
	CLASS_METHOD_MAP(mItemView, getTextByIdx)
	CLASS_METHOD_MAP(mItemView, getTextLen)
	CLASS_METHOD_MAP(mItemView, getSelection)
	CLASS_METHOD_MAP(mItemView, getSelectionCount)
	CLASS_METHOD_MAP(mItemView, adjustItemsHeight)
	CLASS_METHOD_MAP(mItemView, setAutoSortItem)
	CLASS_METHOD_MAP(mItemView, getNext)
	CLASS_METHOD_MAP(mItemView, getPrev)
	CLASS_METHOD_MAP(mItemView, getRect)
	CLASS_METHOD_MAP(mItemView, inItem)
	CLASS_METHOD_MAP(mItemView, getTotalHeight)
	CLASS_METHOD_MAP(mItemView, refreshRect)
	CLASS_METHOD_MAP(mItemView, resetContent)
	CLASS_METHOD_MAP(mItemView, setItemHeight)
	CLASS_METHOD_MAP(mItemView, isEmpty)
	CLASS_METHOD_MAP(mItemView, getFirstItem)
	CLASS_METHOD_MAP(mItemView, getFirstVisItem)
	CLASS_METHOD_MAP(mItemView, getQueue)
	CLASS_METHOD_MAP(mItemView, getListEntry)
	CLASS_METHOD_MAP(mItemView, setImage)
	CLASS_METHOD_MAP(mItemView, getImage)
	CLASS_METHOD_MAP(mItemView, setFlags)
	CLASS_METHOD_MAP(mItemView, getFlags)
	CLASS_METHOD_MAP(mItemView, isFrozen)
END_CMPT_CLASS
