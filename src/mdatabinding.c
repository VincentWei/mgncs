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

#include "mdatabinding.h"
#include "hashtable.h"
#include "mem-slab.h"

static inline int set_bind_prop(mBindProp *psrc, mBindProp *pdest)
{
	if(!psrc->read )
		return SBPE_READERROR;

	if(!pdest->write)
		return SBPE_WRITEERROR;

	return ncsTransType(_c(psrc)->get(psrc), psrc->vtype, pdest->vtype, (NCS_SET_VALUE)(_c(pdest)->set), pdest);
}


static HashTable * ht_bind_prop_pool = NULL;
#define MAX_BINPROP  37

typedef struct _mBindPropEntry{
	mBindProp * prop_head;
}mBindPropEntry;

static void free_from_listener(mBindProp* plstn, mBindProp* psend)
{
	mBindPropListNode* ln;
	mBindPropListNode* prev = NULL;
	if(!plstn)
		return;

	ln = plstn->listens; 
	while(ln && ln->p != psend)
	{
		prev = ln;
		ln = ln->next;
	}

	if(ln)
	{
	 	psend->ref --;
		if(prev)
			prev->next = ln->next;
		else
			plstn->listens = ln->next;
		_SLAB_FREE(ln);
	}

}

static void free_bin_props_obj(mBindPropEntry* prop_entry)
{
	mBindProp *props = NULL;
	if(!prop_entry)
		return;

	props = prop_entry->prop_head;
	while(props)
	{
		mBindProp * p = props;
		mBindPropListNode * ln = props->listens;
		props->read = 0;
		props->write = 0;
		props = props->next;
		//remove from listners
		for( ; ln ; ln = ln->next)
		{
			free_from_listener(ln->p, p);
		}
		ncsBindPropRelease(p);
	}
	_SLAB_FREE(prop_entry);
}


mBindProp* ncsRegisterBindProp(mObject* obj,
		NCS_TEST_BINDPROP test_exist,
		NCS_NEW_BINDPROP  new_prop,
		void *param)
{
	mBindProp * p, *prev;
	mBindPropEntry * prop_entry = NULL;
	if(test_exist == NULL || obj == NULL || new_prop == NULL)
		return NULL;

	if(ht_bind_prop_pool == NULL)
	{
		ht_bind_prop_pool = hash_new(MAX_BINPROP, (freeObj)free_bin_props_obj);
	}

	if(!ht_bind_prop_pool)
		return FALSE;

	prop_entry = (mBindPropEntry*)hash_get(ht_bind_prop_pool, (ht_key_t)obj);

	if(!prop_entry)
	{
		prop_entry = _SLAB_ALLOC(sizeof(mBindPropEntry));
		hash_insert(ht_bind_prop_pool, (ht_key_t)obj,(void*)prop_entry);
		prop_entry->prop_head = new_prop(param);
		prop_entry->prop_head->next = NULL;
		return prop_entry->prop_head;
	}
	else
	{
		prev = prop_entry->prop_head;
		for(p=prop_entry->prop_head; p; p = p->next)
		{
			prev = p;
			if(test_exist(p, param))
				return p;
		}

		if(prev){
			mBindProp * prop = new_prop(param);
			if(prop)
			{
				prev->next = prop;
				prop->next = NULL;
			}
			return prop;
		}
	}

	return NULL;
}

#if 0
static BOOL _add_obj_test_exist(mBindProp* p, void *param){
	return p == (mBindProp*)param;
}
static mBindProp * _add_obj_new_prop(void *param){
	return (mBindProp*)param;
}
#endif

BOOL ncsAutoReflectObjectBindProps(mObject * obj)
{
	mBindPropEntry  *prop_entry = NULL;
	mBindProp *p;
	if(!obj)
		return FALSE;

	prop_entry = (mBindPropEntry*)hash_get(ht_bind_prop_pool, (ht_key_t)obj);
	if(!prop_entry)
		return FALSE;

	for(p = prop_entry->prop_head; p; p = p->next)
	{
		ncsBindPropAutoRelfect(p);
	}
	return TRUE;
}

BOOL ncsRemoveObjectBindProps(mObject *obj)
{
	return hash_delete(ht_bind_prop_pool, (ht_key_t)obj) == HTE_OK;
}

///////////////////////////////////////////////////////////////
void ncsBindPropRasieChanged(mBindProp *bprop)
{
	mBindPropListNode *ln;
	if(!bprop)
		return;

	for(ln = bprop->listens; ln; ln = ln->next)
	{
		if(ln->p)
			set_bind_prop(bprop, ln->p);
	}
}

int ncsBindPropAddRef(mBindProp *bprop)
{
	if(!bprop)
		return -1;
	return ++ (bprop->ref );
}

int ncsBindPropRelease(mBindProp *bprop)
{
	if(!bprop)
		return -1;

	if((--(bprop->ref)) <= 0)
	{
		_c(bprop)->destroy(bprop);
		_SLAB_FREE(bprop);
		return 0;
	}
	return bprop->ref;
}

void ncsBindPropAutoRelfect(mBindProp *bprop)
{
	mBindPropListNode * ln;
	if(!bprop || !bprop->write)
		return ;

	for(ln = bprop->listens; ln; ln = ln->next)
	{
		if(ln->p && set_bind_prop(ln->p, bprop) == SBPE_OK)
			return ;
	}
}

static void add_listener(mBindProp *psource, mBindProp* plistener)
{
	mBindPropListNode * ln;
	ln = (mBindPropListNode*)_SLAB_ALLOC(sizeof(mBindPropListNode));
	ln->p = plistener;
	ln->next = psource->listens;
	psource->listens = ln;
	ncsBindPropAddRef(plistener);
}

BOOL ncsConnectBindProps(mBindProp * psource, mBindProp* plistener, int type)
{
	if(!psource || !plistener)
		return FALSE;

	add_listener(psource, plistener);
	if(type == NCS_BPT_DBL)
		add_listener(plistener, psource);
	return TRUE;
}

//////////////////////////////////////
//mBindProp base
static void mBindProp_construct(mBindProp *self, DWORD param)
{
	Class(mObject).construct((mObject*)self,0);
	self->read = param&NCS_PROP_FLAG_READ?1:0;
	self->write = param&NCS_PROP_FLAG_WRITE?1:0;
	self->vtype = param>>16;
	self->ref = 1;
	self->listens = NULL;
	self->next = NULL;
}

static void mBindProp_destroy(mBindProp *self)
{
	mBindPropListNode * ln;
	ln = self->listens ;
	while(ln)
	{
		mBindPropListNode * lntmp = ln;
		ncsBindPropRelease(lntmp->p);
		ln = ln->next;
		_SLAB_FREE(lntmp);
	}
	Class(mObject).destroy((mObject*)self);
}

BEGIN_MINI_CLASS(mBindProp, mObject)
	CLASS_METHOD_MAP(mBindProp, construct)
	CLASS_METHOD_MAP(mBindProp, destroy)
END_MINI_CLASS


void ncsRaiseObjectBindProps(mObject *obj, NCS_RAISE_TEST raise_test, DWORD param)
{
	mBindPropEntry  *prop_entry = NULL;
	mBindProp *p;
	if(!obj)
		return ;

	prop_entry = (mBindPropEntry*)hash_get(ht_bind_prop_pool, (ht_key_t)obj);
	if(!prop_entry)
		return ;

	for(p = prop_entry->prop_head; p; p = p->next)
	{
		if(!raise_test || ((*raise_test)(p, param)))
		{
			ncsBindPropRasieChanged(p);
		}
	}
	return;

}


/////////////////////////////////////
//mSimplValueProp


///////////////////////////////////////
//mComponentProp
struct _comp_bin_prop_register_info {
	mComponent * comp;
	int          event_id;
	int          prop_id;
	int          data_type;
	DWORD        flags;
};

void mCompBindProp_construct(mCompBindProp *self, DWORD param)
{
	struct _comp_bin_prop_register_info * pinfo = (struct _comp_bin_prop_register_info*)param;
	param = (pinfo->data_type<<16)|pinfo->flags;
	Class(mBindProp).construct((mBindProp*)self,param);

	self->event_id = pinfo->event_id;
	self->prop_id  = pinfo->prop_id;
	self->host     = pinfo->comp;
}


BOOL mCompBindProp_set(mCompBindProp *self, DWORD value)
{
	if(self->host)
		return _c(self->host)->setProperty(self->host, self->prop_id, value);
	return FALSE;
}

DWORD mCompBindProp_get(mCompBindProp *self)
{
	if(self->host)
		return _c(self->host)->getProperty(self->host, self->prop_id);
	return 0;
}

BEGIN_MINI_CLASS(mCompBindProp, mBindProp)
	CLASS_METHOD_MAP(mCompBindProp, construct)
	CLASS_METHOD_MAP(mCompBindProp, set)
	CLASS_METHOD_MAP(mCompBindProp, get)
END_MINI_CLASS


static BOOL _comp_bindprop_raise_test(mBindProp *p, DWORD event_id)
{
	mCompBindProp* cbp = SAFE_CAST(mCompBindProp, p);
	return (cbp && (event_id == 0 || cbp->event_id == (DWORD)event_id));
}

void ncsRaiseComponentBindProps(mComponent *comp, int event_id)
{
	ncsRaiseObjectBindProps((mObject*)comp, (NCS_RAISE_TEST)_comp_bindprop_raise_test, (DWORD)event_id);
}

static BOOL _comp_register_test_exist(mBindProp* p, struct _comp_bin_prop_register_info * pinfo)
{
	mCompBindProp* cbp = SAFE_CAST(mCompBindProp, p);

	return (cbp
			&& cbp->host == pinfo->comp
#if 0 /* VW: should be event_id */
			&& cbp->event_id == (int)pinfo->comp
#else
			&& cbp->event_id == pinfo->event_id
#endif
			&& cbp->prop_id == pinfo->prop_id
			&& (cbp->read?NCS_PROP_FLAG_READ:0) == (pinfo->flags&NCS_PROP_FLAG_READ)
			&& (cbp->write?NCS_PROP_FLAG_WRITE:0) == (pinfo->flags&NCS_PROP_FLAG_WRITE));

}

static mBindProp* _comp_register_new_prop(struct _comp_bin_prop_register_info * pinfo)
{
	return (mBindProp *)NEWEX(mCompBindProp, (DWORD)pinfo);
}

mBindProp * ncsRegisterComponentBindProp(mComponent* comp, int event_id, int prop_id, int data_type, DWORD flags)
{
	struct _comp_bin_prop_register_info info = {
		comp,
		event_id,
		prop_id,
		data_type,
		flags
	};
	if( comp== NULL || prop_id <= 0)
		return NULL;
	return ncsRegisterBindProp((mObject*)comp, (NCS_TEST_BINDPROP)_comp_register_test_exist, (NCS_NEW_BINDPROP)_comp_register_new_prop, (void*)&info);
}

/////////////////////////////////////////
//varaible
typedef struct _variable_register_info{
	void *pvalue;
	int  data_type;
	DWORD flags;
}variable_register_info;



static void mVariableBindProp_construct(mVariableBindProp* self, DWORD param)
{
	variable_register_info * pinfo = (variable_register_info*)param;
	param = (pinfo->data_type<<16)|pinfo->flags;
	Class(mBindProp).construct((mBindProp*)self, param);

	self->p = pinfo->pvalue;
}

static BOOL mVariableBindProp_set(mVariableBindProp* self, DWORD p)
{
	switch(self->vtype)
	{
	case NCS_BT_INT:
		_SET_VAR(int, self->p, p);
		break;
	case NCS_BT_STR:
		strcpy((char*)(self->p), (char*)p);
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

static DWORD  mVariableBindProp_get(mVariableBindProp* self)
{
	switch(self->vtype)
	{
	case NCS_BT_INT:
		return (DWORD)(*((int*)(self->p)));
	case NCS_BT_STR:
		return (DWORD)self->p;
	}
	return 0;
}

BEGIN_MINI_CLASS(mVariableBindProp, mBindProp)
	CLASS_METHOD_MAP(mVariableBindProp, construct)
	CLASS_METHOD_MAP(mVariableBindProp, set)
	CLASS_METHOD_MAP(mVariableBindProp, get)
END_MINI_CLASS

static BOOL _variable_register_test_exist(mBindProp *p, variable_register_info* pinfo)
{
	mVariableBindProp* vbp = SAFE_CAST(mVariableBindProp, p);
	return (vbp
			&& vbp->p == pinfo->pvalue);
}

static mBindProp* _variable_register_new_prop(variable_register_info* pinfo)
{
	if(pinfo->pvalue == NULL)
		return NULL;

	return (mBindProp *)NEWEX(mVariableBindProp, (DWORD)pinfo);
}

mBindProp * ncsRegisterVariableBindProp(void *pvalue, int data_type, DWORD flags)
{
	variable_register_info info = {
		pvalue,
		data_type,
		flags
	};

	if(!pvalue)
		return NULL;
	return ncsRegisterBindProp((mObject*)pvalue, (NCS_TEST_BINDPROP)_variable_register_test_exist, (NCS_NEW_BINDPROP)_variable_register_new_prop, (void*)&info);
}

////////////////////////////////////////////
BOOL ncsInitDataBinding()
{
    ht_bind_prop_pool = NULL;
	MGNCS_INIT_CLASS(mBindProp);
	MGNCS_INIT_CLASS(mVariableBindProp);
	MGNCS_INIT_CLASS(mCompBindProp);
	return TRUE;
}

