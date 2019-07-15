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
#include <stdarg.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "mem-slab.h"

static void mComponent_construct(mComponent* self, DWORD addData)
{
	g_stmObjectCls.construct((mObject*)self, addData);
	self->event_handlers = NULL;
}

static void mComponent_destroy(mComponent* self)
{
	if(self)
	{
		mSpecificDataNode * sn;
		NCS_EVENT_HANDLER_NODE *handlers = self->event_handlers;

		//remove the event
//		ncsRemoveEventListener((mObject*)self);
		//remove the source
//		ncsRemoveEventSource((mObject*)self);

		//remove the handler
		while(handlers){
			NCS_EVENT_HANDLER_NODE* tmp = handlers;
			handlers = handlers->next;
			free(tmp);
		}
		//delete special
		while(self->specificHead)
		{
			sn = self->specificHead;
			self->specificHead = sn->next;
			if(sn->freeSpecific)
				(*(sn->freeSpecific))(sn->key, sn->value);
			_SLAB_FREE(sn);
		}

		Class(mObject).destroy((mObject*)self);
	}
}

static mSpecificDataNode* find_special_node(mComponent*self, DWORD key)
{
	mSpecificDataNode* sn = self->specificHead;
	while(sn)
	{
		if(sn->key == key)
			return sn;
		sn = sn->next;
	}
	return NULL;
}

static BOOL mComponent_setSpecificData(mComponent*self, DWORD key, DWORD value, PFreeSpecificData freeSpecific)
{
	mSpecificDataNode* sn = find_special_node(self, key);

	if(sn)
	{
		if(sn->freeSpecific)
			(*(sn->freeSpecific))(key,sn->value);
		if(freeSpecific)
			sn->freeSpecific = freeSpecific;
		sn->value = value;
	}
	else
	{
		sn = _SLAB_ALLOC(sizeof(mSpecificDataNode));
		sn->key = key;
		sn->value = value;
		sn->freeSpecific = freeSpecific;
		sn->next = self->specificHead;
		self->specificHead = sn;
	}
	return TRUE;

}

static DWORD mComponent_getSpecificData(mComponent* self, DWORD key, BOOL *pok)
{
	mSpecificDataNode *sn = find_special_node(self, key);
	if(sn)
	{
		if(pok)
			*pok = TRUE;
		return sn->value;
	}

	if(pok)
		*pok = FALSE;
	return 0;
}



static PUBLIC DWORD mComponent_getPropertyByPath(mComponent* self, int *id_path)
{
	mComponent* cmp;
	if(!ISVALID_IDPATH(id_path))
		return 0;

	while(IDPATH_ISOBJ(id_path)) {
        if(id_path[0] == IDPATH_ME) {
            id_path ++;
            continue;
        } else if(id_path[0] == IDPATH_PARENT)
            cmp = _c(self)->getReleated(self, NCS_CMPT_PARENT);
        else if(id_path[0] == IDPATH_MAINPARENT)
            cmp = ncsCompGetMainParent((mComponent*)self);
        else if(id_path[0] == IDPATH_ROOT)
            cmp = ncsCompGetRoot((mComponent*)self);
        else
            cmp = _c(self)->getChild(self, id_path[0]);
        if(!cmp)
            return 0;
        return _c(cmp)->getPropertyByPath(cmp,  ++ id_path);
    }

    return _c(self)->getProperty(self, id_path[0]);
}

PUBLIC static BOOL mComponent_setPropertyByPath(mComponent* self, int *id_path, DWORD value)
{
	mComponent* cmp;
	if(!ISVALID_IDPATH(id_path))
		return FALSE;
	
	while(IDPATH_ISOBJ(id_path))
	{
        if(id_path[0] == IDPATH_ME) {
            id_path ++;
            continue;
        } else if(id_path[0] == IDPATH_PARENT)
            cmp = _c(self)->getReleated(self, NCS_CMPT_PARENT);
        else if(id_path[0] == IDPATH_MAINPARENT)
            cmp = ncsCompGetMainParent((mComponent*)self);
        else if(id_path[0] == IDPATH_ROOT)
            cmp = ncsCompGetRoot((mComponent*)self);
        else
            cmp = _c(self)->getChild(self, id_path[0]);
        if(!cmp)
            return FALSE;

        return _c(cmp)->setPropertyByPath(cmp, ++id_path, value);
    }

    return _c(self)->setProperty(self, id_path[0], value);

}

/*#define addEventListener(self, id, listener, on_event) do{ \
	ncsAddEventListener((mObject*)(self), (mObject*)listener, on_event, id); \
	return TRUE;                                                             \
}while(0)*/

PUBLIC static BOOL mComponent_addEventListenerByPath(mComponent* self, int *id_path, mObject* listener, NCS_CB_ONOBJEVENT on_event)
{
	mComponent* cmp;
	if(!ISVALID_IDPATH(id_path))
		return 0;

	while(IDPATH_ISOBJ(id_path)) {
        if(id_path[0] == IDPATH_ME) {
            id_path ++;
            continue;
        } else if(id_path[0] == IDPATH_PARENT)
            cmp = _c(self)->getReleated(self, NCS_CMPT_PARENT);
        else if(id_path[0] == IDPATH_MAINPARENT)
            cmp = ncsCompGetMainParent((mComponent*)self);
        else if(id_path[0] == IDPATH_ROOT)
            cmp = ncsCompGetRoot((mComponent*)self);
        else
            cmp = _c(self)->getChild(self, id_path[0]);
        if(!cmp)
            return 0;
        return _c(cmp)->addEventListenerByPath(self, ++id_path, listener, on_event);
    }

	return ncsAddEventListener((mObject*)self, (mObject*)listener, on_event, id_path[0]);
    
}

static inline BOOL addBindProp(mComponent* self, int prop_id, int event_id, int prop_type, int flags, mBindProp* prop, int bind_type)
{
	mBindProp* mybindprop =  NCS_CMPT_PROP(self, event_id, prop_id, prop_type, flags);
	if(!mybindprop)
		return FALSE;
	return ncsConnectBindProps(mybindprop, prop, bind_type);
}

PUBLIC BOOL mComponent_addBindPropByPath(mComponent* self, int *id_path, int event_id, int prop_type, int flags, mBindProp* prop, int bind_type)
{
	mComponent* cmp;
	if(!ISVALID_IDPATH(id_path))
		return 0;

	while(IDPATH_ISOBJ(id_path)) {
        if(id_path[0] == IDPATH_ME) {
            id_path ++;
            continue;
        } else if(id_path[0] == IDPATH_PARENT)
            cmp = _c(self)->getReleated(self, NCS_CMPT_PARENT);
        else if(id_path[0] == IDPATH_MAINPARENT)
            cmp = ncsCompGetMainParent((mComponent*)self);
        else if(id_path[0] == IDPATH_ROOT)
            cmp = ncsCompGetRoot((mComponent*)self);
        else
            cmp = _c(self)->getChild(self, id_path[0]);
        if(!cmp)
            return 0;
        return _c(cmp)->addBindPropByPath(cmp, ++id_path, event_id, prop_type, flags, prop, bind_type);
    }

	return addBindProp(self, id_path[0], event_id, prop_type, flags, prop, bind_type);
    
}


PUBLIC static BOOL mComponent_queryByPath(mComponent* self, int *id_path, DWORD *pret, ...)
{
	va_list va;
	BOOL bret;

	va_start(va, pret);
	bret = _c(self)->vqueryByPath(self, id_path, pret, va);
	va_end(va);

	return bret;
}

static inline BOOL vquery(mComponent* self, int cmd_id, DWORD *pret, va_list va)
{
	//DWORD ret;
	switch(cmd_id)
	{
	case QPC_SET_PROP:
		/*ret = */_c(self)->setProperty(self, va_arg(va,int), va_arg(va, DWORD));
		break;
	case QPC_GET_PROP:
		/*ret = */_c(self)->getProperty(self, va_arg(va,int));
		break;
	case QPC_ADD_EVENT:
		ncsAddEventListener((mObject*)self,va_arg(va,mObject*),va_arg(va, NCS_CB_ONOBJEVENT),va_arg(va,int));
		break;
	/*case QPC_ADD_BIND:
		ret = _c(self)->addBindPropByPath(self, id_path, va_arg(va, int), va_arg(va, int), va_arg(va, int), va_arg(va, mBindProp*), va_arg(va, int));
		break;*/
	default:
		return FALSE;
	}

	return TRUE;
}

PUBLIC static BOOL mComponent_vqueryByPath(mComponent* self, int *id_path, DWORD *pret, va_list va)
{
	mComponent* cmp;
	if(!ISVALID_IDPATH(id_path))
		return 0;

	while(IDPATH_ISOBJ(id_path)) {
        if(id_path[0] == IDPATH_ME) {
            id_path ++;
            continue;
        } else if(id_path[0] == IDPATH_PARENT)
            cmp = _c(self)->getReleated(self, NCS_CMPT_PARENT);
        else if(id_path[0] == IDPATH_MAINPARENT)
            cmp = ncsCompGetMainParent((mComponent*)self);
        else if(id_path[0] == IDPATH_ROOT)
            cmp = ncsCompGetRoot((mComponent*)self);
        else
            cmp = _c(self)->getChild(self, id_path[0]);
        if(!cmp)
            return 0;
        return _c(cmp)->vqueryByPath(cmp, ++id_path, pret, va);
    }
    
    return vquery(self, id_path[0], pret, va);
}

/*
{
	mComponent *parent;
	DWORD ret;

	if(!ISVALID_IDPATH(id_path))
		return FALSE;

	parent = _c(self)->getReleated(self, NCS_CMPT_PARENT);

	if(parent && parent != self) //send to parent
	{
		return _c(parent)->vqueryByPath(parent, cmd_id, id_path, pret, va);
	}

	switch(cmd_id)
	{
	case QPC_SET_PROP:
		ret = _c(self)->setPropertyByPath(self, id_path, va_arg(va, DWORD));
		break;
	case QPC_GET_PROP:
		ret = _c(self)->getPropertyByPath(self, id_path);
		break;
	case QPC_ADD_EVENT:
		ret = _c(self)->addEventListenerByPath(self, id_path, va_arg(va, mObject*), va_arg(va, NCS_CB_ONOBJEVENT));
		break;
	case QPC_ADD_BIND:
		ret = _c(self)->addBindPropByPath(self, id_path, va_arg(va, int), va_arg(va, int), va_arg(va, int), va_arg(va, mBindProp*), va_arg(va, int));
		break;
	default:
		return FALSE;
	}

	if(pret)
		*pret = ret;
	return TRUE;

}*/

static PUBLIC BOOL mComponent_isMainObj(mComponent* self){
	return FALSE;
}

static void mComponent_removeChild(mComponent* self, mComponent* cmp)
{
}

BEGIN_CMPT_CLASS(mComponent, mObject)
	CLASS_METHOD_MAP(mComponent, construct)
	CLASS_METHOD_MAP(mComponent, destroy)
	_class->_window_proc = NULL;
	CLASS_METHOD_MAP(mComponent, getPropertyByPath)
	CLASS_METHOD_MAP(mComponent, setPropertyByPath)
	CLASS_METHOD_MAP(mComponent, addEventListenerByPath)
	CLASS_METHOD_MAP(mComponent, addBindPropByPath)
	CLASS_METHOD_MAP(mComponent, queryByPath)
	CLASS_METHOD_MAP(mComponent, vqueryByPath)
	CLASS_METHOD_MAP(mComponent, isMainObj)
	CLASS_METHOD_MAP(mComponent, setSpecificData)
	CLASS_METHOD_MAP(mComponent, getSpecificData)
	CLASS_METHOD_MAP(mComponent, removeChild)
END_CMPT_CLASS

mComponent *ncsCompGetMainParent(mComponent* self)
{
	mComponent *mainobj = self;
	if(!self)
		return NULL;
	
	while((mainobj = _c(mainobj)->getReleated(mainobj, NCS_CMPT_PARENT)) && !_c(mainobj)->isMainObj(mainobj));

	return mainobj;
}

mComponent *ncsCompGetRoot(mComponent* self)
{
	mComponent *root = self;
	while(root)
	{
		mComponent* parent = _c(root)->getReleated(root, NCS_CMPT_PARENT);
		if(!parent)
			return root;
		root = parent;
	}
	return root;
}

/////////////////////////////////////
void * ncsSetComponentHandler(mComponent* comp, UINT message, void *handler)
{
	NCS_EVENT_HANDLER_NODE * handlers;
	if(comp == NULL || handler == NULL)
		return NULL;

	handlers = comp->event_handlers;
	//to check
	while(handlers)
	{
		if(handlers->message == message)
		{
			void *oldHandler = handlers->handler;
			handlers->handler = handler;
			return oldHandler;
		}
		handlers = handlers->next;
	}

	handlers = (NCS_EVENT_HANDLER_NODE*)malloc(sizeof(NCS_EVENT_HANDLER_NODE));

	if(handlers)
	{
		handlers->message = message;
		handlers->handler = handler;
		handlers->next = comp->event_handlers;
		comp->event_handlers = handlers;
	}

	return NULL;
}

void ncsSetComponentHandlers(mComponent* comp, NCS_EVENT_HANDLER* handlers, int count)
{
	int i;
	NCS_EVENT_HANDLER_NODE * hnode = NULL, *header = NULL;
	if(comp == NULL || handlers == NULL)
		return ;

	//donot check it
	for(i = 0; i != count && handlers[i].handler; ++i)
	{
		if(hnode == NULL)
		{
			hnode = (NCS_EVENT_HANDLER_NODE*)malloc(sizeof(NCS_EVENT_HANDLER_NODE));
			header = hnode;
		}
		else{
			hnode->next = (NCS_EVENT_HANDLER_NODE*)malloc(sizeof(NCS_EVENT_HANDLER_NODE));
			hnode = hnode->next;
		}

		if(hnode == NULL)
			break;

		hnode->message = handlers[i].message;
		hnode->handler = handlers[i].handler;
	}

	if(header == NULL)
		return ;

	hnode->next = comp->event_handlers;
	comp->event_handlers = header;

}

void* ncsGetComponentHandler(mComponent* comp, UINT message)
{
	NCS_EVENT_HANDLER_NODE * hnode ;
	if(comp == NULL)
		return NULL;

	hnode = comp->event_handlers;
	while(hnode && message != hnode->message)
		hnode = hnode->next;

	return hnode?hnode->handler:NULL;
}

BOOL ncsIsChildClass(const char* child_clsname, const char* parent_clsname)
{
	mComponentClass *parentCls;
	
    if(child_clsname == NULL || parent_clsname == NULL)
		return FALSE;

	parentCls = ncsGetComponentClass(parent_clsname, TRUE);
	if(parentCls == NULL)
		return FALSE;
	
	return ncsIsClass(child_clsname, parentCls);
}

BOOL ncsIsClass(const char* class_name, mComponentClass* clss)
{
	mComponentClass* myclss;
	
	if(clss == NULL)
		return FALSE;

	myclss = ncsGetComponentClass(class_name, TRUE);
	
	while(myclss){
		if(myclss == clss)
			return TRUE;
		myclss = (mComponentClass*)myclss->super;
	}
	
	return FALSE;
}

/////////////////////////////////////

static mComponentClass** _comp_class_table = NULL;
static int _comp_count = 0;
#define COMP_MAGIC_MASK  0xFFFF0000

void free_comp_class(void)
{
    if (_comp_class_table)
        free(_comp_class_table);
    _comp_class_table = NULL;
    _comp_count = 0;
}

static DWORD _add_comp_class(mComponentClass* compCls)
{
	DWORD magic_num;

	if(compCls == NULL)
		return 0xFFFFFFFF;
	if(!_comp_class_table)
	{
		_comp_class_table = (mComponentClass**)malloc(sizeof(mComponentClass*));
	}
	else 
	{
		_comp_class_table = (mComponentClass**)realloc(_comp_class_table, (_comp_count+1)*sizeof(mComponentClass*));
	}

	if(_comp_class_table == NULL)
		return 0xFFFFFFFF;

	_comp_class_table[_comp_count] = compCls;
	
	magic_num = COMP_MAGIC_MASK | ((WORD)_comp_count);

	_comp_count ++;

	return magic_num;
}

mComponentClass * ncsClassFromMagicNum(DWORD magic_num, BOOL check, const char* class_name)
{
	int idx;
	if((magic_num & COMP_MAGIC_MASK) != COMP_MAGIC_MASK)
		return NULL;
	
	idx = magic_num & 0xFFFF;

	if(idx < 0 || idx >= _comp_count)
		return NULL;

	if(_comp_class_table[idx] == NULL)
		return NULL;

	if(check)
	{
		if(strcasecmp(_comp_class_table[idx]->className, class_name) != 0)
			return NULL;
	}

	return _comp_class_table[idx];

}

mComponentClass * ncsGetComponentClass(const char* class_name, BOOL check)
{
	WNDCLASS WndClass;
	
	if(class_name == NULL)
		return NULL;

	WndClass.spClassName = (char*)class_name;
	WndClass.opMask = COP_ADDDATA;

	if(!GetWindowClassInfo(&WndClass)){
		return NULL;
	}

	return ncsClassFromMagicNum(WndClass.dwAddData, check, class_name);
}

BOOL ncsRegisterComponent(mComponentClass *compCls, DWORD dwStyle, 
	DWORD dwExStyle, int idCursor, int idBkColor)
{
    WNDCLASS WndClass;
	DWORD magic_num = _add_comp_class(compCls);

	if(magic_num == 0xFFFFFFFF)
		return FALSE;

    WndClass.spClassName = (char*)compCls->className;
    WndClass.dwStyle     = dwStyle;
    WndClass.dwExStyle   = dwExStyle;
    WndClass.hCursor     = GetSystemCursor (idCursor);
    WndClass.iBkColor    = ncsColor2Pixel(HDC_SCREEN, ncsGetElement(NULL, idBkColor));
    WndClass.WinProc     = compCls->_window_proc;
    WndClass.dwAddData   = magic_num;

    return RegisterWindowClass(&WndClass);
}

BOOL ncsComponentConnectEvents(mComponent* comps, NCS_EVENT_CONNECT_INFO *connects, int counts/*=-1*/)
{
	int i = 0;
	int comps_id  ;
	if(!comps || !connects)
		return FALSE;

	comps_id = _c(comps)->getId(comps);

	while(i!= counts && connects[i].event)
	{
		mComponent *sender = comps_id == connects[i].id_sender?comps:_c(comps)->getChild(comps, connects[i].id_sender);
		mComponent *listener = comps_id == connects[i].id_listener?comps:_c(comps)->getChild(comps, connects[i].id_listener);
		if(!sender || !listener) {
			i ++;
			continue;
		}

		ncsAddEventListener((mObject*)sender, (mObject*)listener, connects[i].event, connects[i].event_id);
        i ++;
	}

	return TRUE;
}

