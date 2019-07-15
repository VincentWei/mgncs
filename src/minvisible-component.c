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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

static void mInvsbComp_construct(mInvsbComp* self, DWORD addData)
{
    NCS_INVSB_CREATE_INFO *create_info =
        (NCS_INVSB_CREATE_INFO*)addData;

    self->id = -1;

    if(create_info)
    {
        self->parent = create_info->parent;
        self->id = create_info->id;
        //set propertype
        if(create_info->props)
        {
            NCS_PROP_ENTRY * props = create_info->props;
            int i;
            for(i = 0; props[i].id > 0; i++)
            {
                _c(self)->setProperty(self, props[i].id,props[i].value);
            }
        }

        //set event
        ncsSetComponentHandlers((mComponent*)self, create_info->handlers, -1);

    }
}

static void mInvsbComp_destroy(mInvsbComp *self)
{
    Class(mComponent).destroy((mComponent*)self);
}

static mInvsbComp * mInvsbComp_createComponent(mComponent* parent, mInvsbCompClass *_class, DWORD addValue)
{
    NCS_INVSB_CREATE_INFO invsb_create_info;
    mInvsbComp * self;
    NCS_WND_TEMPLATE * tmpl;

    if(_class == NULL || parent == NULL)
        return NULL;

    self = (mInvsbComp*)newObject((mObjectClass*)_class);

    if(self == NULL)
        return self;

    tmpl = (NCS_WND_TEMPLATE*)addValue;

    invsb_create_info.parent = parent;
    invsb_create_info.id = tmpl->id;
    invsb_create_info.props = tmpl->props;
    invsb_create_info.handlers = tmpl->handlers;
    invsb_create_info.user_data = tmpl->user_data;
    _c(self)->construct(self, (DWORD)&invsb_create_info);

    if(tmpl->notify_info && tmpl->notify_info->onCreated)
        tmpl->notify_info->onCreated(tmpl->notify_info, (mComponent*)self, tmpl->special_id);

    return self;
}

static mComponent * mInvsbComp_getReleated(mInvsbComp* self, int releated)
{
    switch(releated)
    {
    case NCS_CMPT_PARENT:
        return self->parent;
    case NCS_CMPT_PREV:
        return self->prev;
    case NCS_CMPT_NEXT:
        return self->next;
    }

    return NULL;
}

static mComponent* mInvsbComp_setReleated(mInvsbComp* self, mComponent* comp, int releated)
{
    mComponent *old = NULL;
    if(releated == NCS_CMPT_PARENT && comp == NULL)
        return NULL;

    switch(releated)
    {
    case NCS_CMPT_PARENT:
        old = self->parent;
        self->parent = comp;
        break;
    case NCS_CMPT_NEXT:
        old = self->next;
        self->next = comp;
        if(comp)
            _c(comp)->setReleated(comp, (mComponent*)self, NCS_CMPT_PREV);
        break;
    case NCS_CMPT_PREV:
        old = self->prev;
        self->prev = comp;
        //if(comp)
        //    _c(comp)->setReleated(comp, (mComponent*)self, NCS_CMPT_NEXT);
        break;
    }

    return old;
}

static BOOL mInvsbComp_setProperty(mInvsbComp *self, int id, DWORD addData)
{
    return FALSE;
}

static DWORD mInvsbComp_getProperty(mInvsbComp* self, int id)
{
    return (DWORD)-1;
}

static mComponent* mInvsbComp_getChild(mInvsbComp* self, int id)
{
    return NULL;
}

static int mInvsbComp_getId(mInvsbComp* self)
{
    return self->id;
}

static int mInvsbComp_setId(mInvsbComp* self, int id)
{
    int old_id = id;

    self->id = id;

    return old_id;
}

BEGIN_CMPT_CLASS(mInvsbComp, mComponent)
    CLASS_METHOD_MAP(mInvsbComp, construct)
    CLASS_METHOD_MAP(mInvsbComp, destroy)
    CLASS_METHOD_MAP(mInvsbComp, setReleated)
    CLASS_METHOD_MAP(mInvsbComp, getReleated)
    CLASS_METHOD_MAP(mInvsbComp, createComponent)
    CLASS_METHOD_MAP(mInvsbComp, setProperty)
    CLASS_METHOD_MAP(mInvsbComp, getProperty)
    CLASS_METHOD_MAP(mInvsbComp, getChild)
    CLASS_METHOD_MAP(mInvsbComp, getId)
    CLASS_METHOD_MAP(mInvsbComp, setId)
END_CMPT_CLASS

/////////////////////////////////////////////////////
mInvsbComp * ncsCreateInvsbCompIndirect(const char* class_name, NCS_INVSB_CREATE_INFO *create_info)
{
    mInvsbCompClass * invsCompCls;
    mInvsbComp * invsComp;

    if(create_info == NULL || create_info->parent == NULL)
        return NULL;

    //get class
    invsCompCls = SAFE_CAST_CLASS(ncsGetComponentClass(class_name, TRUE),mInvsbComp);

    if(invsCompCls == NULL)
        return NULL;

    if(invsCompCls == NULL )
        return NULL;

    invsComp = (mInvsbComp*)newObject((mObjectClass*)invsCompCls);

    if(invsComp == NULL)
        return NULL;

    _c(invsComp)->construct(invsComp, (DWORD)create_info);

    return invsComp;

}


mInvsbComp * ncsCreateInvsbComp(const char* class_name, mComponent* parent, int id, NCS_PROP_ENTRY *props, NCS_EVENT_HANDLER * handlers, DWORD user_data)
{
    NCS_INVSB_CREATE_INFO create_info;

    if(class_name == NULL || parent == NULL)
        return NULL;

    create_info.parent = parent;
    create_info.id = id;
    create_info.props = props;
    create_info.handlers = handlers;
    create_info.user_data = user_data;

    return ncsCreateInvsbCompIndirect(class_name, &create_info);
}
