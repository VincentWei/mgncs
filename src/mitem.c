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
    The implementation of mItem class.

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

static void mItem_setItemEditor(mItem* self, hEditor editor)
{
    self->editor = editor;
}

static hEditor mItem_getItemEditor(mItem* self)
{
    return self->editor;
}

static void mItem_setItemAddData(mItem* self, DWORD addData)
{
    self->addData = addData;
}

static DWORD mItem_getItemAddData(mItem* self)
{
    return self->addData;
}

static void mItem_setItemImage(mItem* self, DWORD image)
{
    self->image = image;
}

static DWORD mItem_getItemImage(mItem* self)
{
    return self->image;
}

static BOOL mItem_setItemHeight (mItem* self, int height)
{
    if (self && height >= 0) {
        self->height = height;
        return TRUE;
    }

    return FALSE;
}

static int mItem_getItemHeight (mItem* self)
{
    return self ? self->height : -1;
}

static BOOL mItem_setItemString(mItem* self, const char* string)
{
    if (!string)
        return FALSE;

    if (self->string) {
        FreeFixStr (self->string);
    }

    self->string = FixStrAlloc (strlen(string) + 1);
    if (!self->string)
        return FALSE;

    strcpy (self->string, string);
    return TRUE;
}

static char* mItem_getItemString(mItem* self)
{
    return self->string ? self->string : "";
}

static void mItem_construct(mItem *self, DWORD addData)
{
	Class(mObject).construct((mObject*)self, addData);
	INIT_LIST_HEAD(&self->list);
	INIT_LIST_HEAD(&self->selList);
    self->editor = HWND_NULL;
    self->string = NULL;
    self->image = 0;
    self->flags = NCSF_ITEM_NORMAL;
}

static void mItem_destroy(mItem *self)
{
    if (self->string) {
        FreeFixStr (self->string);
    }
	Class(mObject).destroy((mObject*)self);
}

static void mItem_setItemFlags(mItem* self, DWORD flags)
{
    self->flags = flags;
}

static DWORD mItem_getItemFlags(mItem* self)
{
    return self->flags;
}

static BOOL mItem_isEnabledItem(mItem* self)
{
    return !(self->flags & NCSF_ITEM_DISABLED);
}


static BOOL mItem_isSelectedItem(mItem* self)
{
    return (self->flags & NCSF_ITEM_SELECTED);
}

static BOOL mItem_setProperty(mItem* self, int id, DWORD value)
{
    if (id >= NCSP_ITEM_MAX)
        return FALSE;

    switch (id) {
        case NCSP_ITEM_HEIGHT:
            return _c(self)->setItemHeight(self, value);

        case NCSP_ITEM_FLAGS:
            _c(self)->setItemFlags(self, value);
            return TRUE;
    } 

    return FALSE;
}

static DWORD mItem_getProperty(mItem* self, int id)
{
    if (id >= NCSP_ITEM_MAX)
        return -1;

    switch (id) {
        case NCSP_ITEM_HEIGHT:
            return _c(self)->getItemHeight(self);

        case NCSP_ITEM_FLAGS:
            return _c(self)->getItemFlags(self);
    } 

    return -1;
}

BEGIN_MINI_CLASS(mItem, mObject)
	CLASS_METHOD_MAP(mItem, construct)
	CLASS_METHOD_MAP(mItem, destroy)
	CLASS_METHOD_MAP(mItem, setProperty)
	CLASS_METHOD_MAP(mItem, getProperty)
    CLASS_METHOD_MAP(mItem, setItemAddData)
    CLASS_METHOD_MAP(mItem, getItemAddData)
    CLASS_METHOD_MAP(mItem, setItemImage)
    CLASS_METHOD_MAP(mItem, getItemImage)
    CLASS_METHOD_MAP(mItem, setItemString)
    CLASS_METHOD_MAP(mItem, getItemString)
    CLASS_METHOD_MAP(mItem, setItemEditor)
    CLASS_METHOD_MAP(mItem, getItemEditor)
    CLASS_METHOD_MAP(mItem, isSelectedItem)
    CLASS_METHOD_MAP(mItem, isEnabledItem)
    CLASS_METHOD_MAP(mItem, setItemHeight)
    CLASS_METHOD_MAP(mItem, getItemHeight)
    CLASS_METHOD_MAP(mItem, getItemFlags)
    CLASS_METHOD_MAP(mItem, setItemFlags)
END_MINI_CLASS

