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
    <http://www.minigui.com/blog/minigui-licensing-policy/>.
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

#if _MGNCSDB_DATASOURCE

#include "mtype.h"
#include "mdatabinding.h"
#include "mdatasource.h"

static mRecordSet * mDataSource_selectRecordSet(mDataSource* self, const char* select, int select_type)
{
	return NULL;
}

static DWORD mDataSource_getValue(mDataSource* self, const char* mql, BOOL *bok)
{
	if(bok)
		*bok = FALSE;
	return 0;
}

static BOOL mDataSource_setValue(mDataSource* self, const char* mql, DWORD value)
{
	return FALSE;
}

static mBindProp * mDataSource_getBindProp(mDataSource* self, const char* mql, int bp_type)
{
	return NULL;
}

static BOOL mDataSource_createValue(mDataSource* self, const char* ql, DWORD init_value)
{
	return FALSE;
}

static BOOL mDataSource_removeValue(mDataSource* self, const char* ql)
{
	return FALSE;
}

static BOOL mDataSource_execute(mDataSource* self, const char* msql)
{
	return FALSE;
}


BEGIN_MINI_CLASS(mDataSource, mObject)
	CLASS_METHOD_MAP(mDataSource, selectRecordSet)
	CLASS_METHOD_MAP(mDataSource, getValue)
	CLASS_METHOD_MAP(mDataSource, setValue)
	CLASS_METHOD_MAP(mDataSource, getBindProp)
	CLASS_METHOD_MAP(mDataSource, createValue)
	CLASS_METHOD_MAP(mDataSource, removeValue)
	CLASS_METHOD_MAP(mDataSource, execute)
END_MINI_CLASS

/////////////////////////////////////////////
static DWORD mRecordSet_getCursor(mRecordSet* self)
{
	return (DWORD)-1;
}

static BOOL mRecordSet_setCursor(mRecordSet* self, DWORD cursor)
{
	return FALSE;
}


static DWORD mRecordSet_seekCursor(mRecordSet* self, int seek, int offset)
{
	return (DWORD)-1;
}

static DWORD mRecordSet_getField(mRecordSet *self, int idx)
{
	return 0;
}

static BOOL mRecordSet_setField(mRecordSet *self, int idx, DWORD value)
{
	return FALSE;
}

static BOOL mRecordSet_newRecord(mRecordSet* self, int insert_type)
{
	return FALSE;
}

static BOOL mRecordSet_deleteRecord(mRecordSet* self)
{
	return FALSE;
}

static BOOL mRecordSet_update(mRecordSet *self)
{
	return FALSE;
}

static BOOL mRecordSet_cancel(mRecordSet *self)
{
	return FALSE;
}

static mBindProp * mRecordSet_getFieldBind(mRecordSet *self, int field_idx, int event_filter, int bp_type)
{
	return NULL;
}

static mBindProp * mRecordSet_getCursorBind(mRecordSet *self, int bp_type)
{
	return NULL;
}

static int mRecordSet_getFieldCount(mRecordSet* self)
{
	return 0;
}

static int mRecordSet_getRecordCount(mRecordSet* self)
{
	return 0;
}

static int mRecordSet_getFieldType(mRecordSet* self, int idx)
{
	return -1;
}

static BOOL mRecordSet_isEnd(mRecordSet* self)
{
	return TRUE;
}

BEGIN_MINI_CLASS(mRecordSet, mObject)
	CLASS_METHOD_MAP(mRecordSet, getFieldCount)
	CLASS_METHOD_MAP(mRecordSet, getRecordCount)
	CLASS_METHOD_MAP(mRecordSet, getFieldType)
	CLASS_METHOD_MAP(mRecordSet, getCursor)
	CLASS_METHOD_MAP(mRecordSet, setCursor)
	CLASS_METHOD_MAP(mRecordSet, seekCursor)
	CLASS_METHOD_MAP(mRecordSet, getField)
	CLASS_METHOD_MAP(mRecordSet, setField)
	CLASS_METHOD_MAP(mRecordSet, newRecord)
	CLASS_METHOD_MAP(mRecordSet, deleteRecord)
	CLASS_METHOD_MAP(mRecordSet, update)
	CLASS_METHOD_MAP(mRecordSet, cancel)
	CLASS_METHOD_MAP(mRecordSet, getFieldBind)
	CLASS_METHOD_MAP(mRecordSet, getCursorBind)
	CLASS_METHOD_MAP(mRecordSet, isEnd)
END_MINI_CLASS

//////////////////////////////////

#define mDataBindPropInfoHeader \
	int event_filter; \
	int type; \
	int flags;

struct mDataBindPropInfo {
	mDataBindPropInfoHeader
};

static void mDataBindProp_construct(mDataBindProp *self, DWORD param)
{
	struct mDataBindPropInfo * pinfo = (struct mDataBindPropInfo*)param;
	if(pinfo)
	{
		param = (pinfo->type<<16)|pinfo->flags;
		Class(mBindProp).construct((mBindProp*)self,param);

		self->event_filter = (unsigned short)pinfo->event_filter;
	}
}

BEGIN_MINI_CLASS(mDataBindProp, mBindProp)
	CLASS_METHOD_MAP(mDataBindProp, construct)
END_MINI_CLASS

struct _data_value_bind_prop_info{
	mDataBindPropInfoHeader
	DWORD *pvalue;
};

static void mDataValueBindProp_construct(mDataValueBindProp* self, DWORD param)
{
	struct _data_value_bind_prop_info * pinfo = (struct _data_value_bind_prop_info*)param;
	Class(mDataBindProp).construct((mDataBindProp*)self, pinfo->event_filter);
	if(pinfo)
	{
		pinfo->pvalue = pinfo->pvalue;
	}
}

static BOOL mDataValueBindProp_set(mDataValueBindProp* self, DWORD v)
{
	switch(self->vtype)
	{
	case NCS_BT_INT:
		_SET_VAR(int, self->pvalue, v);
		break;
	case NCS_BT_STR:
		strcpy((char*)(self->pvalue), (char*)v);
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

static DWORD mDataValueBindProp_get(mDataValueBindProp* self)
{
	switch(self->vtype)
	{
	case NCS_BT_INT:
		return (DWORD)(*((int*)(self->pvalue)));
	case NCS_BT_STR:
		return (DWORD)self->pvalue;
	}
	return 0;

}

BEGIN_MINI_CLASS(mDataValueBindProp, mDataBindProp)
	CLASS_METHOD_MAP(mDataValueBindProp, construct)
	CLASS_METHOD_MAP(mDataValueBindProp, get)
	CLASS_METHOD_MAP(mDataValueBindProp, set)
END_MINI_CLASS


static BOOL _test_bind_prop(mBindProp* p, struct _data_value_bind_prop_info *pinfo)
{
	mDataValueBindProp* dbp = SAFE_CAST(mDataValueBindProp, p);

	return (dbp
			&& dbp->pvalue == pinfo->pvalue
			&& dbp->event_filter == pinfo->event_filter);
}

static mBindProp* _new_bind_prop(struct _data_value_bind_prop_info *pinfo)
{
	return (mBindProp *)NEWEX(mDataBindProp, (DWORD)pinfo);
}

mBindProp * ncsRegisterDataValueBindProp(mDataSource *obj, DWORD *pvalue, int type, int flags, int event_filter)
{
	struct _data_value_bind_prop_info info = {
		event_filter,
		type,
		flags,
		pvalue
	};

	return ncsRegisterBindProp((mObject*)obj, (NCS_TEST_BINDPROP)_test_bind_prop, (NCS_NEW_BINDPROP)_new_bind_prop, (void *)&info);
}

//////
// mDataFieldBindProp

struct data_field_bind_prop_info {
	mDataBindPropInfoHeader
	mRecordSet *rs;
	int field_idx;
};

static void mDataFieldBindProp_construct(mDataFieldBindProp* self, DWORD param)
{
	struct data_field_bind_prop_info* pinfo = (struct data_field_bind_prop_info*) param;
	Class(mDataBindProp).construct((mDataBindProp*)self, param);
	if(pinfo)
	{
		self->recordSet = pinfo->rs;
		self->revert = (unsigned short)pinfo->field_idx;
	}
}

static BOOL mDataFieldBindProp_set(mDataFieldBindProp *self, DWORD v)
{
	if(!self->recordSet)
		return FALSE;

	return _c(self->recordSet)->setField(self->recordSet, self->revert, v);
}

static DWORD mDataFieldBindProp_get(mDataFieldBindProp *self)
{
	if(!self->recordSet)
		return 0;

	return _c(self->recordSet)->getField(self->recordSet, self->revert);
}

BEGIN_MINI_CLASS(mDataFieldBindProp, mDataBindProp)
	CLASS_METHOD_MAP(mDataFieldBindProp, construct)
	CLASS_METHOD_MAP(mDataFieldBindProp, get)
	CLASS_METHOD_MAP(mDataFieldBindProp, set)
END_MINI_CLASS

static BOOL _test_data_file_bind_prop(mBindProp *p, struct data_field_bind_prop_info *pinfo)
{
	mDataFieldBindProp *dfp = SAFE_CAST(mDataFieldBindProp,p);

	return (dfp
		&& dfp->recordSet == pinfo->rs
		&& dfp->revert == (unsigned short)pinfo->field_idx
		&& dfp->event_filter == (unsigned short)pinfo->event_filter);
}

static mBindProp * _new_data_field_bind_prop(struct data_field_bind_prop_info* pinfo)
{
	return (mBindProp *)NEWEX(mDataFieldBindProp, (DWORD)pinfo);
}

mBindProp * ncsRegisterDataFieldBindProp(mRecordSet * rs, int field_idx, int flags, int event_filter)
{

	if(!rs)
		return NULL;
	if(field_idx < 0 || field_idx > _c(rs)->getFieldCount(rs))
		return NULL;

	{
		struct data_field_bind_prop_info info = {
			event_filter,
			_c(rs)->getFieldType(rs, field_idx),
			flags,
			rs,
			field_idx
		};

		return ncsRegisterBindProp((mObject*)rs,
				(NCS_TEST_BINDPROP)_test_data_file_bind_prop,
				(NCS_NEW_BINDPROP)_new_data_field_bind_prop,
				(void *)&info);
	}
}

//////////
static BOOL _raise_test(mBindProp* p, DWORD event_filter)
{
	mDataBindProp* dbp = SAFE_CAST(mDataBindProp, p);
	return (dbp && (dbp->event_filter & event_filter));
}

void ncsRaiseDataSourceBindProps(mObject *obj, int event_filter)
{
	ncsRaiseObjectBindProps(obj, (NCS_RAISE_TEST)_raise_test, (DWORD)event_filter);
}

////////////////////////////////////
// init
extern void ncsInitStaticDataSource(void);

extern void ncsInitTextDataSource(void);

extern void ncsInitIniDataSource(void);

#ifdef _MGNCSDB_SQLITE
extern void ncsInitSQLiteDataSource(void);
#endif

#ifdef _MGNCSDB_XML
extern void ncsInitXMLDataSource(void);
#endif

extern void init_common_parser(void);


BOOL ncsInitDefaultDataSource(void)
{
	MGNCS_INIT_CLASS(mDataSource);
	MGNCS_INIT_CLASS(mRecordSet);
	MGNCS_INIT_CLASS(mDataBindProp);
	MGNCS_INIT_CLASS(mDataValueBindProp);
	MGNCS_INIT_CLASS(mDataFieldBindProp);

	init_common_parser();

#ifdef _MGNCSDB_STATIC
	ncsInitStaticDataSource();
#endif

#ifdef _MGNCSDB_TEXT
	ncsInitTextDataSource();
#endif

#ifdef _MGNCSDB_INI
	ncsInitIniDataSource();
#endif

#ifdef _MGNCSDB_SQLITE
	ncsInitSQLiteDataSource();
#endif

#ifdef _MGNCSDB_XML
	ncsInitXMLDataSource();
#endif
	return TRUE;
}


#endif



