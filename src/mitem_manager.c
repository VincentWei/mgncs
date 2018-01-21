/*
 ** $Id: mitem_manager.c 1116 2010-12-02 04:03:35Z dongjunjie $
 **
 ** The implementation of mItemManager class.
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
#include "mscroll_widget.h"
#include "mdblist.h"
#include "mitem.h"
#include "mitem_manager.h"
#include "mitemview.h"

#define mglist_for_each(pos, itemlist) \
    list_for_each(pos, &((mItemManager *)itemlist)->queue)

#define mglist_entry(ptr) \
    list_entry(ptr, mItem, list)

static HITEM mItemManager_getListEntry(list_t* entry)
{
    return (HITEM) list_entry(entry, mItem, list);
}

static list_t* mItemManager_getQueue(mItemManager* self)
{
    return &(self->queue);
}

static int mItemManager_isEmpty(mItemManager *self)
{
    return list_empty(&(self)->queue);
}

static HITEM mItemManager_getFirstItem(mItemManager *self)
{
    return (HITEM)list_entry((self)->queue.next, mItem, list);
}

static NCS_CB_CMPSTR mItemManager_setStrCmpFunc(mItemManager* self, NCS_CB_CMPSTR func)
{
    NCS_CB_CMPSTR oldfn;
    oldfn = self->strCmp;
    self->strCmp = func;

    return oldfn;
}

static NCS_CB_CMPSTR mItemManager_getStrCmpFunc(mItemManager* self)
{
    return self->strCmp;
}

static NCS_CB_CMPITEM
mItemManager_setItemCmpFunc(mItemManager* self, NCS_CB_CMPITEM func)
{
    NCS_CB_CMPITEM oldfn;
    oldfn = self->itemCmp;
    self->itemCmp = func;

    return oldfn;
}

GET_ENTRY_BY_INDEX(mlGetItemByIndex, mItem, list)
GET_ENTRY_INDEX(mlGetItemIndex, mItem, list)

static HITEM mItemManager_getItem(mItemManager* self, int index)
{
    return (HITEM)mlGetItemByIndex(&self->queue, index);
}

static int mItemManager_indexOf(mItemManager* self, HITEM hItem)
{
    return mlGetItemIndex(&self->queue, (mItem*)hItem);
}

static HITEM mItemManager_getNext(mItemManager* self, HITEM hItem)
{
    list_t *me = hItem ? ((mItem*)hItem)->list.next : self->queue.next;
    return (HITEM)( (me == &self->queue) ? 0 : mglist_entry(me) );
}

static HITEM mItemManager_getPrev(mItemManager* self, HITEM hItem)
{
    list_t *me = hItem ? ((mItem*)hItem)->list.prev : self->queue.prev;
    return (HITEM)( (me == &self->queue) ? 0 : mglist_entry(me) );
}

static DWORD mItemManager_getAddData(mItemManager* self, HITEM hItem)
{
    return hItem ? _c((mItem*)hItem)->getItemAddData((mItem*)hItem) : 0;
}

static void mItemManager_setImage(mItemManager* self, HITEM hItem, DWORD image)
{
    if (hItem)
        _c((mItem*)hItem)->setItemImage((mItem*)hItem, image);
}

static DWORD mItemManager_getImage(mItemManager* self, HITEM hItem)
{
    return hItem ? _c((mItem*)hItem)->getItemImage((mItem*)hItem) : 0;
}

static void mItemManager_setAddData(mItemManager* self, HITEM hItem, DWORD addData)
{
    mItem* item = (mItem*)hItem;

    if (!item)
        return;

    _c(item)->setItemAddData(item, addData);
}

static BOOL mItemManager_isSelected(mItemManager* self, HITEM hItem)
{
    return hItem ? _c((mItem*)hItem)->isSelectedItem((mItem*)hItem) : FALSE;
}

static BOOL mItemManager_isEnabled(mItemManager* self, HITEM hItem)
{
    return hItem ? _c((mItem*)hItem)->isEnabledItem((mItem*)hItem) : FALSE;
}

static BOOL mItemManager_setText (mItemManager* self,
        HITEM hItem, const char* string)
{
    if (string == NULL || !hItem)
        return FALSE;

    return _c((mItem*)hItem)->setItemString((mItem*)hItem, string);
}

static const char* mItemManager_getText (mItemManager* self, HITEM hItem)
{
    return hItem ? _c((mItem*)hItem)->getItemString((mItem*)hItem) : NULL;
}

static int mItemManager_getTextLen (mItemManager* self, HITEM hItem)
{
    return hItem ? strlen(_c((mItem*)hItem)->getItemString((mItem*)hItem)) : 0;
}

void mItemManager_freeze (mItemManager *self, BOOL lock)
{
    if (lock) {
        self->flags |= NCSF_ITMMNG_FROZEN;
    }
    else {
        self->flags &= ~NCSF_ITMMNG_FROZEN;
    }
}

static int mItemManager_removeItem(mItemManager* self, HITEM hItem)
{
    if (!hItem)
        return -1;

    list_del (&((mItem*)hItem)->list);
    self->count--;

    if (hItem == self->pItemHilighted)
        self->pItemHilighted = 0;

    if (_c(self)->isSelected(self, hItem))
        list_del (&((mItem*)hItem)->selList);

	DELETE(hItem);
    return 0;
}

static int
mglist_move_item (mItemManager *self, mItem* curItem, mItem* prevItem)
{
    list_del (&curItem->list);

    if (prevItem)
        list_add_tail(&curItem->list, &prevItem->list);
    else
        list_add_tail(&curItem->list, &self->queue);

    return 0;
}

static int mItemManager_moveItem (mItemManager *self,
        mItem *curItem, int count, mItem* prevItem)
{
    list_t *first_me, *last_me;
    list_t *prev_me, *next_me;
    mItem *last;

    if (count <= 0 || !curItem)
        return -1;

    if (count == 1)
        return mglist_move_item(self, curItem, prevItem);

    //del list
    first_me = &curItem->list;

    last = curItem;
    while (--count) {
        last = (mItem*)_c(self)->getNext(self, (HITEM)last);
    }

    last_me = &last->list;

    prev_me = first_me->prev;
    next_me = last_me->next;

    prev_me->next = next_me;
    next_me->prev = prev_me;

    if (prevItem) {
        next_me = &prevItem->list;
        prev_me = next_me->prev;
    }
    else {
        next_me = &self->queue;
        prev_me = next_me->prev;
    }

    prev_me->next = first_me;
    first_me->prev = prev_me;
    next_me->prev = last_me;
    last_me->next = next_me;

    return 0;
}

static int mItemManager_sortItems(mItemManager *self, NCS_CB_CMPITEM pfn)
{
    mItem *pci, *pci2;
    list_t *me, *me2;
    int ret;

    if ( !pfn )
        return -1;

    mglist_for_each (me, self) {
        pci = mglist_entry (me);
        pci2 = NULL;
        ret = 0;
        mglist_for_each (me2, self) {
            if (me == me2)
                break;
            pci2 = mglist_entry (me2);
                ret = pfn (self, (HITEM)pci, (HITEM)pci2);
            if (ret < 0)
                break;
        }
        if (ret < 0) {
            me = me->prev;
            mglist_move_item (self, pci, pci2);
        }
    }

    return 0;
}

static int mItemManager_isHilight (mItemManager *self, HITEM hItem)
{
    return self->pItemHilighted == hItem;
}

static HITEM mItemManager_getHilight (mItemManager *self)
{
    return self->pItemHilighted;
}

static inline BOOL mglist_select_item (mItemManager *itemlist, mItem* pci, BOOL bSel)
{
    if (bSel) {
        if (!(pci->flags & NCSF_ITEM_SELECTED)) {
            pci->flags |= NCSF_ITEM_SELECTED;
            list_add_tail (&pci->selList, &itemlist->selQueue);
            return TRUE;
        }
    }
    else {
        if (pci->flags & NCSF_ITEM_SELECTED) {
            pci->flags &= ~NCSF_ITEM_SELECTED;
            list_del (&pci->selList);
            return TRUE;
        }
    }
    return FALSE;
}

static BOOL mItemManager_select (mItemManager *self, HITEM hItem, BOOL sel)
{
    if (hItem)
        return mglist_select_item(self, (mItem*)hItem, sel);

    return FALSE;
}

static BOOL mItemManager_enable(mItemManager* self,
        HITEM hItem, BOOL enable)
{
    mItem* item = (mItem*)hItem;

    if (item) {
        if (enable && (item->flags & NCSF_ITEM_DISABLED)) {
            item->flags &= ~NCSF_ITEM_DISABLED;
            return TRUE;
        }
        if (!enable && !(item->flags & NCSF_ITEM_DISABLED)) {
            item->flags |= NCSF_ITEM_DISABLED;
            return TRUE;
        }
    }
    return FALSE;
}

static void mItemManager_selectAll (mItemManager *self)
{
    mItem* item;
    list_t *me;

    list_for_each (me, &self->queue) {
        item = list_entry (me, mItem, list);
        mglist_select_item (self, item, TRUE);
    }
}

static void mItemManager_deselectAll (mItemManager *self)
{
    mItem* pci;
    volatile struct list_head *queue;
//   struct list_head *queue;

    queue = &self->selQueue;

    while (!list_empty ((struct list_head *)queue)) {
        pci = list_entry (queue->next, mItem, selList);
        if (pci->flags & NCSF_ITEM_SELECTED) {
            pci->flags &= ~NCSF_ITEM_SELECTED;
            list_del (queue->next);
        }
    }
}

static int mItemManager_getItemCount(mItemManager *self)
{
    return self->count;
}

static HITEM mItemManager_hilight(mItemManager* self, HITEM hItem)
{
    if (hItem) {
        HITEM oldHilight = self->pItemHilighted;
        self->pItemHilighted = hItem;
        mglist_select_item (self, (mItem*)hItem, TRUE);
        return oldHilight;
    }
    return 0;
}

static inline BOOL mItemManager_isFrozen (mItemManager *self)
{
    return self->flags & NCSF_ITMMNG_FROZEN;
}

int _im_strcmp_item (mItemManager* self, HITEM hItem1, HITEM hItem2)
{
    char* str1 = _c((mItem*)hItem1)->getItemString((mItem*)hItem1);
    char* str2 = _c((mItem*)hItem2)->getItemString((mItem*)hItem2);

    return self->strCmp(str1, str2, MIN(strlen(str1), strlen(str2)));
}

/*
 * mglist_get_sort_index : Get sort index of an item
 */
static int mglist_get_sort_index (mItemManager *self, mItem *item, BOOL bAdd)
{
    list_t *me;
    mItem *tmp = NULL;
    int i = 0;

    mglist_for_each (me, self) {
        tmp = mglist_entry (me);
        if ((self->itemCmp && self->itemCmp (self, (HITEM)item, (HITEM)tmp) < 0)
                || (self->strCmp && _im_strcmp_item(self, (HITEM)item, (HITEM)tmp) < 0)) {
            if (bAdd)
                list_add_tail (&item->list, &tmp->list);
            return i;
        }
        i++;
    }

    if (bAdd)
        list_add_tail (&item->list, &self->queue);
    return i;
}

static void mItemManager_construct (mItemManager *self, DWORD addData)
{
	Class(mItem).construct((mItem*)self, addData);

    INIT_LIST_HEAD(&self->queue);
    INIT_LIST_HEAD(&self->selQueue);

    self->count = 0;
    self->pItemHilighted = 0;
    self->totalHeight = 0;

    self->itemCmp = NULL;
    self->strCmp = strncasecmp;
    self->flags = 0;
    self->obj = NULL;
}

static void mItemManager_destroy(mItemManager* self)
{
    _c(self)->removeAll(self);
}

static void mItemManager_setAutoSortItem (mItemManager *self, BOOL sort)
{
    if (sort && !(self->flags & NCSF_ITMMNG_AUTOSORT))
        self->flags |= NCSF_ITMMNG_AUTOSORT;
    else if (!sort && (self->flags & NCSF_ITMMNG_AUTOSORT))
        self->flags &= ~NCSF_ITMMNG_AUTOSORT;
}

static HITEM mItemManager_createItem(mItemManager* self,
        HITEM prev, HITEM next, int index, int *pos)
{
    HITEM newItem = (HITEM)NEW(mItem);

    if (newItem) {
        if (_c(self)->insertItem(self, newItem, prev, next, index, pos) < 0) {
            DELETE((mItem*)newItem);
            return 0;
        }
    }

    return newItem;
}

static int mItemManager_insertItem(mItemManager* self,
        HITEM hItem, HITEM prev, HITEM next, int index, int *pos)
{
    int ret;
    mItem* item = (mItem*)hItem;

    if (!item)
        return -1;

    if (self->flags & NCSF_ITMMNG_AUTOSORT && (self->itemCmp || self->strCmp)) {
        ret = mglist_get_sort_index (self, item, TRUE);
    }
    else {
        if (prev) {
            list_add (&item->list, &((mItem*)prev)->list);
            ret = _c(self)->indexOf(self, prev) + 1;
        }
        else if (next) {
            list_add_tail (&item->list, &((mItem*)next)->list);
            ret = _c(self)->indexOf(self, next) - 1;
        }
        else {
            ret = list_add_by_index (&item->list, &self->queue, index);
            ret = index;
        }
    }

    self->count ++;

    if (pos)
        *pos = ( ret<0 ? self->count-1 : ret);

    return 0;
}

static BOOL mItemManager_removeAll(mItemManager* self)
{
    HITEM hItem;
    mItemView* itemView = (mItemView*)(self->obj);

    while (!_c(self)->isEmpty(self)) {
        hItem = _c(self)->getFirstItem(self);
        if (itemView)
            _c(itemView)->removeItem(itemView, hItem);
        else
            _c(self)->removeItem(self, hItem);
    }

    self->count = 0;
    return TRUE;
}

static int mItemManager_getSelectionCount(mItemManager* self)
{
    mItem* pci;
    int count = 0;

    while (!list_empty (&self->selQueue)) {
        pci = list_entry (self->selQueue.next, mItem, selList);
        if (pci->flags & NCSF_ITEM_SELECTED) {
            count ++;
        }
    }
    return count;
}


static BOOL mItemManager_getSelection(mItemManager* self,
        HITEM *pRet, int count)
{
    mItem* pci;
    int num = 0;

    while (!list_empty (&self->selQueue)) {
        pci = list_entry (self->selQueue.next, mItem, selList);
        if (pci->flags & NCSF_ITEM_SELECTED) {
            if (num >= count)
                break;
            pRet[num] = (HITEM)pci;
            num ++;
        }
    }

    return ((num > 0) ? TRUE : FALSE);
}

static int mItemManager_getTotalHeight(mItemManager *self)
{
    return self->totalHeight;
}

static int mItemManager_getHeight(mItemManager *self, HITEM hItem)
{
    if (self && hItem)
        return _c((mItem*)hItem)->getItemHeight ((mItem*)hItem);

    return -1;
}

static int mItemManager_getItemYPos(mItemManager *self, HITEM hItem)
{
    list_t *me;
    mItem  *tmp;
    int h = 0;

    mglist_for_each (me, self) {
        tmp = mglist_entry (me);
        if ((HITEM)tmp == hItem)
            return h;
        h += tmp->height;
    }
    return -1;
}

static int mItemManager_inItem (mItemManager *self, int mouseX, int mouseY,
                     HITEM *pRet, POINT *pt)
{
    int ret = 0;
    mItem *tmp = NULL;
    int h = 0;
    list_t *me;

    if (mouseY < 0 || mouseY >= self->totalHeight)
        return -1;

    mglist_for_each (me, self) {
        tmp = mglist_entry (me);
        h += tmp->height;
        if (h > mouseY)
            break;
        ret++;
    }

    if (pRet)
        *pRet = (HITEM)tmp;

    if (pt)
        pt->y = h - tmp->height;

    return ret;
}

static int mItemManager_setHeight (mItemManager *self, HITEM hItem, int height)
{
    int diff;
    mItem *item = (mItem*)hItem;
    mItemView* itemView = (mItemView*)(self->obj);

    if (!self || !item || height < 0 || height == item->height) return -1;

    diff = height - item->height;
    _c(item)->setItemHeight(item, height);

    if (itemView)
        _c(itemView)->adjustItemsHeight(itemView, diff);
    else
        _c(self)->adjustItemsHeight(self, diff);

    return height - diff;
}

static int mItemManager_adjustItemsHeight(mItemManager* self, int diff)
{
    if (diff != 0)
        self->totalHeight += diff;
    return self->totalHeight;
}

static void mItemManager_setFlags(mItemManager* self, HITEM hItem, DWORD flags)
{
    _c((mItem*)hItem)->setItemFlags((mItem*)hItem, flags);
}

static DWORD mItemManager_getFlags(mItemManager* self, HITEM hItem)
{
    return _c((mItem*)hItem)->getItemFlags((mItem*)hItem);
}

static BOOL mItemManager_setProperty(mItemManager* self, int id, DWORD value)
{
    if (id >= NCSP_ITMMNG_MAX)
        return FALSE;

    switch (id) {
        case NCSP_ITMMNG_ITEMCOUNT:
            return FALSE;

        case NCSP_ITMMNG_TOTALHEIGHT:
            return FALSE;
    }

	return Class(mItem).setProperty((mItem*)self, id, value);
}

static DWORD mItemManager_getProperty(mItemManager* self, int id)
{
    if (id >= NCSP_ITMMNG_MAX)
        return -1;

    switch (id) {
        case NCSP_ITMMNG_ITEMCOUNT:
            return self->count;

        case NCSP_ITMMNG_TOTALHEIGHT:
            return self->totalHeight;
    }
	return Class(mItem).getProperty((mItem*)self, id);
}

BEGIN_MINI_CLASS(mItemManager, mItem)
    CLASS_METHOD_MAP(mItemManager, construct);
    CLASS_METHOD_MAP(mItemManager, destroy);
	CLASS_METHOD_MAP(mItemManager, setProperty)
	CLASS_METHOD_MAP(mItemManager, getProperty)
	CLASS_METHOD_MAP(mItemManager, createItem)
    CLASS_METHOD_MAP(mItemManager, insertItem);
    CLASS_METHOD_MAP(mItemManager, removeItem);
    CLASS_METHOD_MAP(mItemManager, getItem);
    CLASS_METHOD_MAP(mItemManager, indexOf);
    CLASS_METHOD_MAP(mItemManager, getNext);
    CLASS_METHOD_MAP(mItemManager, getPrev);
    CLASS_METHOD_MAP(mItemManager, setAddData);
    CLASS_METHOD_MAP(mItemManager, getAddData);
    CLASS_METHOD_MAP(mItemManager, isSelected);
    CLASS_METHOD_MAP(mItemManager, isEnabled);
    CLASS_METHOD_MAP(mItemManager, setText);
    CLASS_METHOD_MAP(mItemManager, getText);
    CLASS_METHOD_MAP(mItemManager, getTextLen);
    CLASS_METHOD_MAP(mItemManager, hilight);
    CLASS_METHOD_MAP(mItemManager, isHilight);
    CLASS_METHOD_MAP(mItemManager, getHilight);
	CLASS_METHOD_MAP(mItemManager, select);
	CLASS_METHOD_MAP(mItemManager, deselectAll);
	CLASS_METHOD_MAP(mItemManager, selectAll);
	CLASS_METHOD_MAP(mItemManager, getItemCount);
	CLASS_METHOD_MAP(mItemManager, getHeight);
	CLASS_METHOD_MAP(mItemManager, getItemYPos);
    CLASS_METHOD_MAP(mItemManager, enable);
    CLASS_METHOD_MAP(mItemManager, isFrozen);
    CLASS_METHOD_MAP(mItemManager, freeze);
    CLASS_METHOD_MAP(mItemManager, sortItems);
    CLASS_METHOD_MAP(mItemManager, setItemCmpFunc);
    CLASS_METHOD_MAP(mItemManager, setHeight);
    CLASS_METHOD_MAP(mItemManager, setStrCmpFunc);
	CLASS_METHOD_MAP(mItemManager, getStrCmpFunc);
    CLASS_METHOD_MAP(mItemManager, setAutoSortItem);
    CLASS_METHOD_MAP(mItemManager, removeAll);
    CLASS_METHOD_MAP(mItemManager, getSelection);
    CLASS_METHOD_MAP(mItemManager, getSelectionCount);
    CLASS_METHOD_MAP(mItemManager, getTotalHeight);
	CLASS_METHOD_MAP(mItemManager, inItem)
	CLASS_METHOD_MAP(mItemManager, adjustItemsHeight)
	CLASS_METHOD_MAP(mItemManager, isEmpty)
	CLASS_METHOD_MAP(mItemManager, getFirstItem)
	CLASS_METHOD_MAP(mItemManager, getImage)
	CLASS_METHOD_MAP(mItemManager, setImage)
	CLASS_METHOD_MAP(mItemManager, getFlags)
	CLASS_METHOD_MAP(mItemManager, setFlags)
	CLASS_METHOD_MAP(mItemManager, getListEntry)
	CLASS_METHOD_MAP(mItemManager, getQueue)
	CLASS_METHOD_MAP(mItemManager, moveItem)
END_MINI_CLASS
