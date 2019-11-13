/* 
    The implementation of mListItem class.

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

#define NCSF_ITEM_SETBKGND      (0x01<<NCSF_ITEM_SHIFT)
#define NCSF_ITEM_SETFGGND      (0x02<<NCSF_ITEM_SHIFT)
#define NCSF_ITEM_SETADDDATA    (0x04<<NCSF_ITEM_SHIFT)

static void mListItem_construct (mListItem *self, DWORD addData)
{
	Class(mItemManager).construct((mItemManager*)self, addData);
    self->nrChild = 0;
    self->parent = 0;
    self->depth = -1;
}

static mListItem* mListItem_getParent(mListItem *self)
{
    return self->parent;
}

static int mListItem_getChildCount(mListItem *self)
{
    return self->nrChild;
}

static int mListItem_getDepth(mListItem *self)
{
    return self->depth;
}

static BOOL mListItem_delChild(mListItem *self, mListItem *child)
{
    mListItem *tmp;

    if (!child || !self || child->parent != self)
        return FALSE;

    tmp = self;
    while (tmp) {
        tmp->nrChild -= child->nrChild + 1;
        tmp = tmp->parent;
    }

    return TRUE;
}

static BOOL mListItem_addChild(mListItem *self, mListItem *child)
{
    mListItem *tmp;

    if (!self || !child)
        return FALSE;

    //init child info
    child->parent = self;
    child->depth = self->depth + 1;

    tmp = self;
    while (tmp) {
        tmp->nrChild += child->nrChild + 1;
        tmp = tmp->parent;
    }

    return TRUE;
}

static BOOL mListItem_setFold(mListItem *self, BOOL fold)
{
    BOOL isFold;

    if (!self || self->nrChild <= 0)
        return FALSE;

    isFold = self->flags & NCSF_LSTITM_FOLD;

    if (fold && !isFold) 
        self->flags |= NCSF_LSTITM_FOLD;
    else if (!fold && isFold)
        self->flags &= ~NCSF_LSTITM_FOLD;
    else 
        return FALSE;

    return TRUE;

}

static BOOL mListItem_isFold(mListItem *self)
{
    return self->flags & NCSF_LSTITM_FOLD;
}

typedef struct _mCellItemInfo
{
    DWORD itemData;
    int flag;
    int bkColor;
    int fgColor;
}mCellItemInfo;

static void mListItem_setBackground(mListItem *self, int index, int *color)
{
    mCellItemInfo *info;
    mItem* item;

    item = (mItem*)_c(self)->getItem(self, index);

    if (item) {
        info = (mCellItemInfo*) item->addData;
        if (info) {
            if (color) {
                info->flag |= NCSF_ITEM_SETBKGND;
                info->bkColor = *color;
            }
            else {
                info->flag &= ~NCSF_ITEM_SETBKGND;
                info->bkColor = 0;
            }
        }
    }
    else {
        if (color) {
            self->flags |= NCSF_LSTITM_PRIVBKCOL;
            self->bkColor = *color;
        }
        else {
            self->flags &= ~NCSF_LSTITM_PRIVBKCOL;
            self->bkColor = 0;
        }
    }
}

static void mListItem_setForeground(mListItem *self, int index, int *color)
{
    mCellItemInfo *info;
    mItem* item;

    item = (mItem*)_c(self)->getItem(self, index);

    if (item) {
        info = (mCellItemInfo*) item->addData;
        if (info) {
            if (color) {
                info->flag |= NCSF_ITEM_SETFGGND;
                info->fgColor = *color;
            }
            else {
                info->flag &= ~NCSF_ITEM_SETFGGND;
                info->fgColor = 0;
            }
        }
    }
    else {
        if (color) {
            self->flags |= NCSF_LSTITM_PRIVFGCOL;
            self->fgColor = *color;
        }
        else {
            self->flags &= ~NCSF_LSTITM_PRIVFGCOL;
            self->fgColor = 0;
        }
    }
}

static int mListItem_getBackground(mListItem *self, int index, int *color)
{
    mCellItemInfo *info;
    mItem* item;

    if (!color)
        return -1;

    item = (mItem*)_c(self)->getItem(self, index);

    if (item) {
        info = (mCellItemInfo*) item->addData;

        if (info && (info->flag & NCSF_ITEM_SETBKGND)) {
            *color = info->bkColor;
            return 0;
        }
    }

    if (self->flags & NCSF_LSTITM_PRIVBKCOL) {
        *color = self->bkColor;
        return 0;
    }
    return -1;
}

static int mListItem_getForeground(mListItem *self, int index, int *color)
{
    mCellItemInfo *info;
    mItem* item;

    if (!color)
        return -1;

    item = (mItem*)_c(self)->getItem(self, index);

    if (item) {
        info = (mCellItemInfo*) item->addData;

        if (info && (info->flag & NCSF_ITEM_SETFGGND)) {
            *color = info->fgColor;
            return 0;
        }
    }

    if (self->flags & NCSF_LSTITM_PRIVFGCOL) {
        *color = self->fgColor;
        return 0;
    }

    return -1;
}

static HITEM mListItem_createItem(mListItem* self, 
        HITEM prev, HITEM next, int index, int *pos)
{
    mItem *newItem = 
        (mItem*) Class(mItemManager).createItem(
                (mItemManager*)self, prev, next, index, pos);

    if (newItem) {
        newItem->addData = (DWORD)calloc(1, sizeof(mCellItemInfo));
    }
    return (HITEM)newItem;
}

static int mListItem_removeItem(mListItem* self, HITEM hItem)
{
    if (hItem) {
        free((mCellItemInfo*)(((mItem*)hItem)->addData)); 
		return Class(mItemManager).removeItem((mItemManager*)self, hItem);
    }

    return -1;
}

static DWORD mListItem_setAddData(mListItem* self, HITEM hItem, DWORD addData)
{
    mCellItemInfo *info;
    DWORD old;
    mItem* item = (mItem*)hItem;

    if (!item)
        return 0;

    info = (mCellItemInfo*) _c(item)->getItemAddData(item);
    if (!info)
        return 0;

    old = info->itemData;
    info->itemData = addData;

    _c(item)->setItemAddData(item, (DWORD)info);

    return old;
}

static DWORD mListItem_getAddData(mListItem* self, HITEM hItem)
{
    mCellItemInfo *info = NULL;

    if (hItem) {
        info = (mCellItemInfo*) _c((mItem*)hItem)->getItemAddData((mItem*)hItem);

        if (info) {
            return info->itemData;
        }
    }

    return info? info->itemData : 0;
}


static BOOL mListItem_setProperty(mListItem* self, int id, DWORD value)
{
	if( id >= NCSP_LSTITM_MAX)
		return FALSE;

    if (id >= NCSP_LSTITM_NRCHILD)
        return FALSE;

	return Class(mItemManager).setProperty((mItemManager*)self, id, value);
}

static DWORD mListItem_getProperty(mListItem* self, int id)
{
	if( id >= NCSP_LSTITM_MAX)
		return -1;

	switch(id)
	{
        case NCSP_LSTITM_NRCHILD:
            return self->nrChild;

        case NCSP_LSTITM_DEPTH:
            return self->depth;

        case NCSP_LSTITM_RHEIGHT:
            return self->showHeight;
    }

	return Class(mItemManager).getProperty((mItemManager*)self, id);
}

BEGIN_MINI_CLASS(mListItem, mItemManager)
    CLASS_METHOD_MAP(mListItem, construct)
	CLASS_METHOD_MAP(mListItem, setProperty)
	CLASS_METHOD_MAP(mListItem, getProperty)
    CLASS_METHOD_MAP(mListItem, createItem)
    CLASS_METHOD_MAP(mListItem, removeItem)
    CLASS_METHOD_MAP(mListItem, setAddData)
    CLASS_METHOD_MAP(mListItem, getAddData)
    CLASS_METHOD_MAP(mListItem, getParent)
    CLASS_METHOD_MAP(mListItem, getChildCount)
    CLASS_METHOD_MAP(mListItem, getDepth)
    CLASS_METHOD_MAP(mListItem, addChild)
    CLASS_METHOD_MAP(mListItem, delChild)
    CLASS_METHOD_MAP(mListItem, isFold)
    CLASS_METHOD_MAP(mListItem, setFold)
    CLASS_METHOD_MAP(mListItem, setBackground)
    CLASS_METHOD_MAP(mListItem, setForeground)
    CLASS_METHOD_MAP(mListItem, getBackground)
    CLASS_METHOD_MAP(mListItem, getForeground)
END_MINI_CLASS
