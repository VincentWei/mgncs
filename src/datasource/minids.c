
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSDB_INI

#include "mdatabinding.h"
#include "mdatasource.h"
#include "minids.h"
#include "mem-slab.h"

#include "comm_parser.h"

#define ALLOC_STEP  4

#define STR_DUP(str)   FixStrDup((char*)(str))
#define STR_FREE(str)  FreeFixStr((char*)(str))

#ifdef WIN32
void win_truncate_file(FILE *fp, int size);
#elif !defined(HAVE_FTRUNCATE)
extern int ftruncate(int fd, off_t length);
#else
#include <unistd.h>
#include <sys/types.h>
#endif

/////////////////////////////////
// Creating structs
typedef struct _inids_create_info {
	const char* etcFile;
	ETC_S  * etc;
	DWORD  rw_flags;
}inids_create_info_t;

typedef struct _inirs_create_info{
	mIniDataSource* source;
	int             sec_idx;
	DWORD           flags;
}inirs_create_info_t;

typedef struct _inirs_arr_create_info{
	mIniDataSource* source;
	int             sec_idx;
	DWORD           flags;
	char*           count_key;
	char**          key_list;
	int             field_count;
}inirs_arr_create_info_t;

typedef struct _iniprop_create_info{
	mIniDataSource * source;
	int sec_idx;
	int key_idx;
	DWORD flags;
}iniprop_create_info_t;

///////////////////////////////
//
///////////////////////////
// common functions and structs
//

static void inids_rs_update_section(mIniDataSource* self, int sect, int key, mObject* sender);

static void inids_rs_update_del(mIniDataSource* self, int sect, int key, mObject* sender);

static void inids_rs_update_new(mIniDataSource* self, int sect, int key, mObject* sender);

static BOOL inids_save_section(mIniDataSource* self, PETCSECTION psec);

static BOOL inids_save_key(mIniDataSource* self, PETCSECTION psec, const char* key, const char* value);

static BOOL inids_save_key_byidx(mIniDataSource* self, PETCSECTION psec, int key_idx, const char* value);

///////////////////////////
// mIniDataSource


static void mIniDataSource_construct(mIniDataSource* self, DWORD param)
{
	inids_create_info_t *pinfo = (inids_create_info_t*)param;
	Class(mDataSource).construct((mDataSource*)self, 0);

	if(pinfo && pinfo->rw_flags != 0)
	{
		if(pinfo->etcFile)
		{
			if(!pinfo->etc)
				self->etc = (ETC_S*)LoadEtcFile(pinfo->etcFile);
			else
				self->etc = pinfo->etc;
			if(self->etc)
			{
				if(pinfo->rw_flags & NCS_DS_SELECT_WRITE)
					self->fp = fopen(pinfo->etcFile,"r+");
			}
		}
		else
		{
			self->etc = pinfo->etc;
			self->flags |= NCS_IDSF_HANDLE;
		}

		self->flags |= (pinfo->rw_flags);
	}
}

static void mIniDataSource_destroy(mIniDataSource* self)
{
	if(self->etc && !(self->flags & NCS_IDSF_HANDLE))
	{
		//UnloadEtcFile((GHANDLE)self->etc);
		int i,j;
		for(i = 0; i < self->etc->sect_nr_alloc; i++)
		{
			PETCSECTION psec = &self->etc->sections[i];
			if(!psec->name  && ! psec->keys && !psec->values)
				continue;

			for (j = 0; j < psec->key_nr_alloc; j++)
			{
				if(psec->keys[j])
					free(psec->keys[j]);
				if(psec->values[j])
					free(psec->values[j]);
			}
			if(psec->name)
				free(psec->name);
			free(psec->keys);
			free(psec->values);
		}
		if(self->etc->sections)
			free(self->etc->sections);
		free(self->etc);
		self->etc = NULL;
	}

	if(self->fp)
		fclose(self->fp);

	//remove sets
	while(self->sets)
	{
		mIniRecordSet* t = self->sets;
		self->sets = t->next;
		_c(t)->release(t);
	}

	//remove bind prop
	while(self->props)
	{
		mBindProp * tp = self->props;
		self->props = tp->next;
		tp->read = tp->write = 0;
		ncsBindPropRelease((mBindProp*)tp);
	}

	Class(mDataSource).destroy((mDataSource*)self);
}

typedef struct _select_r{
	mIniDataSource* self;
	int sec_idx;
	int key_idx;
	int at_key; //support '@'
	int field_count;
	union {
		unsigned char* field_list;
		char** key_list;
	}fl;
}select_r_t;

static int get_key_idx(PETCSECTION psec, char* key)
{
	int i;
	for(i = 0; i<psec->key_nr; i++)
		if(psec->keys[i] && strcmp(psec->keys[i], key) == 0)
			return i;

	return -1;
}

static inline const char* get_key_value(PETCSECTION psec, char* key)
{
	int idx = get_key_idx(psec, key);
	if(idx == -1)
		return NULL;
	return psec->values[idx];
}

static int get_section_idx(ETC_S* etc, char* sec_name)
{
	int i;
	for(i = 0; i<etc->section_nr; i++)
		if(strcmp(etc->sections[i].name, sec_name) == 0)
			return i;
	return -1;
}

//static int on_find_node_name(select_r_t * pr, char* name)
static int on_find_node_name(void * pr_org, const char* name_org)
{
	select_r_t *pr = (select_r_t *)pr_org;
	char* name = (char *)name_org;

	mIniDataSource* self = pr->self;
	char *str_count_key = NULL;

	if(name == NULL || *name == '\0')
		return 1;

	str_count_key = (char*)strchr(name,'@');
	if(str_count_key){
		*str_count_key = 0;
		str_count_key ++;
	}

	//whe not find the section, find it
	if(pr->sec_idx == -1)
	{
		pr->sec_idx = get_section_idx(self->etc, name);

		pr->key_idx = -1;
	}
	//when find the section, but not find the key
	else if(pr->key_idx == -1)
	{
		//get key value
		pr->key_idx = get_key_idx(&self->etc->sections[pr->sec_idx], name);
	}
	else
	{
		//try to get value, and make value as a section
		char* value = self->etc->sections[pr->sec_idx].values[pr->key_idx];
		int sec;
		if(!value)
			return 0;

		sec = get_section_idx(self->etc, value);
		if(!sec)
			return 0;
		pr->sec_idx = sec;
		//try to get key
		pr->key_idx = get_key_idx(&self->etc->sections[sec], name);
	}

	if(str_count_key)
	{
		pr->at_key = get_key_idx(&self->etc->sections[pr->sec_idx], str_count_key);
		return 0; //stop the spliter
	}

	return 1;//continue
}

static BOOL select_node_result(mIniDataSource* self, const char* select, select_r_t *psr)
{
	char szSelect[1024];
	char *selects[2]={NULL, NULL};

	if(select == NULL)
		return FALSE;

	strcpy(szSelect, select);
	if(ncsStrSpliter(szSelect, selects, ':')<=0)
	{
		selects[0] = szSelect;
		selects[1] = NULL;
	}

	memset(psr, 0, sizeof(select_r_t));
	psr->sec_idx = -1;
	psr->key_idx = -1;
	psr->at_key  = -1;
	psr->self = self;
	ncsStrSpliterCallback(selects[0], on_find_node_name, '/', psr);

	if(psr->sec_idx == -1)
		return FALSE;

	if(selects[1])
	{
		if(psr->at_key!=-1) //get mIniArrRecordSet
		{
			psr->fl.key_list = ncsGetNamedFieldList(&selects[1], &psr->field_count);
		}
		else
		{
			psr->fl.field_list = ncsGetFieldList(&selects[1], &psr->field_count);
		}
	}

	return TRUE;
}

static mRecordSet* mIniDataSource_selectRecordSet(mIniDataSource* self, const char* select, int select_type)
{
	select_r_t  sr;
	mIniRecordSet* rs;
 	if((select_type & self->flags) != select_type)
		return NULL;

	if(select_node_result(self, select, &sr))
	{
		if(sr.at_key!= -1)
		{
			inirs_arr_create_info_t rsinfo;
			if(sr.fl.key_list == NULL)
				return NULL;
			rsinfo.source = self;
			rsinfo.sec_idx = sr.sec_idx;
			rsinfo.flags = select_type;
			rsinfo.count_key = self->etc->sections[sr.sec_idx].keys[sr.at_key];
			rsinfo.key_list = sr.fl.key_list;
			rsinfo.field_count = sr.field_count;
			rs = (mIniRecordSet*)NEWEX(mIniArrRecordSet, (DWORD)&rsinfo);
		}
		else
		{
			//create an mSectionRecordSet
			int sec;
			inirs_create_info_t rsinfo;
			sec = sr.sec_idx;
			if(sr.key_idx != -1) //find a sec
			{
				char* value = self->etc->sections[sec].values[sr.key_idx];
				if(value)
				{
					int tsec = get_section_idx(self->etc, value);
					if(tsec != -1)
						sec = tsec;
				}
			}
			rsinfo.source = self;
			rsinfo.sec_idx = sec;
			rsinfo.flags = select_type;
			if(sr.fl.field_list)
			{
				int i;
				for(i=0; i<sr.field_count; i++)
				{
					if(sr.fl.field_list[i] == 1)
						rsinfo.flags |= NCS_ISRS_KEY_FIELD;
					else if(sr.fl.field_list[i] == 2)
						rsinfo.flags |= NCS_ISRS_VALUE_FIELD;
				}
				free(sr.fl.field_list);
			}

			rs = (mIniRecordSet*)NEWEX(mIniSectionRecordSet, (DWORD)&rsinfo);
		}

		//insert
		rs->next = self->sets;
		self->sets = rs;
		return (mRecordSet*)rs;
	}
	return NULL;
}

static PETCSECTION select_value_node(mIniDataSource* self, const char* select, int *pkey_idx, BOOL *is_key)
{
	select_r_t  sr;

	if(is_key)
		*is_key = FALSE;

	if(select_node_result(self, select, &sr))
	{
		if(sr.at_key == -1)
		{
			int sec = sr.sec_idx;
			int key = sr.key_idx;
			PETCSECTION psec;
			if(key == -1)
				key = 0;

			if(sr.fl.field_list){
				if(is_key)
				{
					if(sr.fl.field_list[0] == 1)
						*is_key = TRUE;
				}
				free(sr.fl.field_list);
			}

			psec = &self->etc->sections[sec];
			if(!psec->values)
				return NULL;

			if(pkey_idx)
				*pkey_idx = key;

			return psec;
		}
		else
		{
			int sec = sr.sec_idx;
			int key;
			int i;
			PETCSECTION psec;
			char szKey[1024];
			if(!sr.fl.key_list)
				return NULL;

			psec = &self->etc->sections[sec];
			sprintf(szKey,"%s0",sr.fl.key_list[0]);
			key = get_key_idx(psec, szKey);

			for(i=0; i<sr.field_count; i++)
				if(sr.fl.key_list[i])free(sr.fl.key_list[i]);
			free(sr.fl.key_list);

			if(key == -1)
				return NULL;

			if(pkey_idx)
				*pkey_idx = key;
			return psec;
		}
	}

	return NULL;


}

static DWORD mIniDataSource_getValue(mIniDataSource* self, const char* select, BOOL *bok)
{
	int key = 0;
	BOOL is_key;
	PETCSECTION psec = select_value_node(self, select, &key, &is_key);

	if(!psec)
	{
		if(bok)
			*bok = FALSE;
		return 0;
	}

	if(bok)
		*bok = TRUE;

	return (DWORD)(is_key?psec->keys[key]:psec->values[key]);

}

static BOOL mIniDataSource_setValue(mIniDataSource* self, const char* select, DWORD value)
{
	int key = 0;
	BOOL is_key;
	PETCSECTION psec ;

	if(value == 0)
		return FALSE;

	if(self->flags & NCS_DS_SELECT_WRITE)
		return FALSE;

	psec = select_value_node(self, select, &key, &is_key);

	if(!psec)
		return FALSE;

	if(is_key)
	{
		if(psec->keys[key])
			STR_FREE(psec->keys[key]);
		psec->keys[key] = STR_DUP((char*)value);
	}
	else
	{
		if(psec->values[key])
			STR_FREE(psec->values[key]);
		psec->values[key] = STR_DUP((char*)value);
	}

	inids_rs_update_section(self, psec - self->etc->sections, key, (mObject*)self);

	return TRUE;
}

static mBindProp* mIniDataSource_getBindProp(mIniDataSource* self, const char* select, int bp_type)
{
	PETCSECTION psec;
	iniprop_create_info_t cinfo;
	int key;
	int sec;
	mIniValueBindProp* prop;
	if((bp_type & NCS_PROP_FLAG_WRITE))
	{
		if(!(self->flags&NCS_DS_SELECT_WRITE))
			return NULL;
	}

	psec = select_value_node(self, select, &key, NULL);

	if(!psec)
		return NULL;

	if(key == -1)
		key = 0;

	sec = psec - self->etc->sections;

	for(prop = (mIniValueBindProp*)self->props; prop; prop = (mIniValueBindProp*)prop->next)
	{
		if(prop->sec_idx == sec && prop->key_idx == key)
		{
			ncsBindPropAddRef((mBindProp*)prop);
			return (mBindProp*)prop;
		}
	}

	cinfo.source = self;
	cinfo.sec_idx = sec;
	cinfo.key_idx = key;
	cinfo.flags = bp_type;

	prop = NEWEX(mIniValueBindProp, (DWORD)&cinfo);

	prop->next = (mBindProp*)self->props;
	self->props = (mBindProp*)prop;
	return (mBindProp*)prop;
}

/*
static BOOL mIniDataSource_createValue(mIniDataSource* self, const char* select, DWORD ini_value)
{
}

static BOOL mIniDataSource_removeValue(mIniDataSource* self, const char* ql)
{
}*/

BEGIN_MINI_CLASS(mIniDataSource, mDataSource)
	CLASS_METHOD_MAP(mIniDataSource, construct)
	CLASS_METHOD_MAP(mIniDataSource, destroy)
	CLASS_METHOD_MAP(mIniDataSource, selectRecordSet)
	CLASS_METHOD_MAP(mIniDataSource, getValue)
	CLASS_METHOD_MAP(mIniDataSource, setValue)
	CLASS_METHOD_MAP(mIniDataSource, getBindProp)
//	CLASS_METHOD_MAP(mIniDataSource, createValue)
//	CLASS_METHOD_MAP(mIniDataSource, removeValue)
END_MINI_CLASS


///////////////////////////

static inline PETCSECTION inirs_get_section(mIniRecordSet* self)
{
	if(!self->source
		|| !self->source->etc
		|| !self->source->etc->sections)
		return NULL;

	if(self->sec_idx >= self->source->etc->section_nr)
		return NULL;

	return &(self->source->etc->sections[self->sec_idx]);
}

static inline char* inirs_get_key(mIniRecordSet* self, int key_idx)
{
	PETCSECTION psec = inirs_get_section(self);
	if(!psec)
		return NULL;
	if(key_idx < 0 || key_idx >= psec->key_nr)
		return NULL;

	return psec->keys[key_idx];
}

static inline char* inirs_get_value(mIniRecordSet* self, int key_idx)
{
	PETCSECTION psec = inirs_get_section(self);
	if(!psec)
		return NULL;
	if(key_idx < 0 || key_idx >= psec->key_nr)
		return NULL;

	return psec->values[key_idx];
}

static BOOL section_resize(PETCSECTION psec, int new_size)
{
	if(!psec)
		return FALSE;

	if(new_size <= psec->key_nr)
		return TRUE;

	if(new_size < psec->key_nr_alloc && new_size >= psec->key_nr)
	{
		int i = psec->key_nr_alloc-1;
		while(i >= psec->key_nr && psec->keys[i])
			i --;
		if(i > new_size)
		{
			return TRUE;
		}
		new_size = psec->key_nr_alloc+1;
	}

	//new alloc
	new_size = ((new_size+ALLOC_STEP)/ALLOC_STEP)*ALLOC_STEP;

	psec->keys = (char**)realloc(psec->keys, new_size*sizeof(char*));
	psec->values = (char**)realloc(psec->values, new_size*sizeof(char*));
	memset(psec->keys + psec->key_nr_alloc, 0, sizeof(char*)*(new_size - psec->key_nr_alloc));
	memset(psec->values + psec->key_nr_alloc, 0, sizeof(char*)*(new_size - psec->key_nr_alloc));

	//remove the deleted keys;
	{
		int i,j;
		for(i=1; i<= psec->key_nr_alloc - psec->key_nr; i++)
		{
			if(!psec->keys[psec->key_nr_alloc-i])
				break;
			psec->keys[new_size-i] = psec->keys[psec->key_nr_alloc-i];
			psec->values[new_size-i] = psec->values[psec->key_nr_alloc-i];
		}

		//set as NULL, to indicate that the space can be used
		for(j=psec->key_nr; j<=new_size - i; j++)
			psec->keys[j] = NULL;

		psec->key_nr_alloc = new_size;
	}
	return TRUE;
}

#if 0
static BOOL etc_resize(ETC_S *petc, int new_size)
{
	if(!petc)
		return FALSE;

	if(new_size <= petc->sect_nr_alloc)
		return TRUE;


	petc->sections = (PETCSECTION)realloc(petc->sections, new_size*sizeof(ETCSECTION));
	petc->sect_nr_alloc = new_size;
	return TRUE;
}
#endif

///delete the section's key
static BOOL section_remove(PETCSECTION psec, int idx)
{
	int i,j;
	char *key_del, *val_del;
	if(!psec || idx < 0 || idx >= psec->key_nr)
		return FALSE;

	//get a delete  place
	for(i = psec->key_nr_alloc - 1; i>=psec->key_nr
		&& psec->keys[i]; i--);

	key_del = psec->keys[idx];
	val_del = psec->values[idx];

	//move array
	for(j = idx; j< psec->key_nr-1; j++)
	{
		psec->keys[j] = psec->keys[j+1];
		psec->values[j] = psec->values[j+1];
	}

	//clear the data
	psec->keys[j] = NULL;
	psec->values[j] = NULL;

	psec->key_nr --;

	//put the deleted key-value into remove place
	psec->keys[i] = key_del;
	psec->values[i] = val_del;

	return TRUE;
}

//

static void mIniRecordSet_construct(mIniRecordSet* self, DWORD param)
{
	inirs_create_info_t* pinfo = (inirs_create_info_t*)param;
	Class(mRecordSet).construct((mRecordSet*)self, param);

	if(pinfo)
	{
		self->source  = pinfo->source;
		self->flags   = pinfo->flags;
		self->sec_idx = pinfo->sec_idx;
	}
}

static void mIniRecordSet_destroy(mIniRecordSet* self)
{
	if(self->source)
	{
		mIniRecordSet* rs , *prev = NULL;
		rs = self->source->sets;
		while(rs && rs != self)
		{
			prev = rs;
			rs = rs->next;
		}

		if(prev == NULL)
			self->source->sets = self->next;
		else
			prev->next = self->next;
	}
}

static int mIniRecordSet_getFieldType(mIniRecordSet* self)
{
	return NCS_BT_STR;
}

static mBindProp * mIniRecordSet_getFieldBind(mIniRecordSet* self, int field_idx, int event_filter, int bp_type)
{
	return DATA_FIELD_PROP(self, field_idx, bp_type, event_filter);
}

BEGIN_MINI_CLASS(mIniRecordSet, mRecordSet)
	CLASS_METHOD_MAP(mIniRecordSet, construct)
	CLASS_METHOD_MAP(mIniRecordSet, destroy)
	CLASS_METHOD_MAP(mIniRecordSet, getFieldType)
	CLASS_METHOD_MAP(mIniRecordSet, getFieldBind)
END_MINI_CLASS

//////////////////////////

static int mIniSectionRecordSet_getFieldCount(mIniSectionRecordSet* self)
{
	return ISRS_FIELD_COUNT(self);
}

static int mIniSectionRecordSet_getRecordCount(mIniSectionRecordSet* self)
{
	PETCSECTION psec = inirs_get_section((mIniRecordSet*)self);

	if(!psec)
		return 0;

	return psec->key_nr;
}

static DWORD mIniSectionRecordSet_getCursor(mIniSectionRecordSet* self)
{
	PETCSECTION petc = inirs_get_section((mIniRecordSet*)self);
	if(!petc)
		return (DWORD)0;

	return (DWORD)(petc->keys+self->cursor);
}

static BOOL mIniSectionRecordSet_setCursor(mIniSectionRecordSet* self, DWORD cursor)
{
	PETCSECTION petc = inirs_get_section((mIniRecordSet*)self);
	int c;
	if(!petc || cursor == 0 || cursor == (DWORD)-1)
		return FALSE;

	c = ((char**)cursor) - petc->keys;
	if( c != self->cursor)
	{
		self->cursor = c;
		ncsRaiseDataSourceBindProps((mObject*)self, NCS_RSE_CURSOR_UPDATE);
	}

	return TRUE;
}

static DWORD mIniSectionRecordSet_seekCursor(mIniSectionRecordSet* self, int seek, int offset)
{
	PETCSECTION psec = inirs_get_section((mIniRecordSet*)self);
	int cursor = 0;
	if(!psec)
		return 0;

	switch(seek)
	{
	case NCS_RS_CURSOR_BEGIN:
		cursor = offset;
		break;
	case NCS_RS_CURSOR_CUR:
		cursor = self->cursor + offset;
		break;
	case NCS_RS_CURSOR_END:
		cursor = psec->key_nr + offset - 1;
	}

	if(cursor < 0)
		cursor = 0;
	else if(cursor > psec->key_nr)
		cursor = psec->key_nr;

	if(self->cursor != cursor)
	{
		self->cursor = cursor;
		ncsRaiseDataSourceBindProps((mObject*)self, NCS_RSE_CURSOR_UPDATE);
	}

	return (DWORD)(psec->keys + self->cursor);
}

static inline int idx_to_type(mIniSectionRecordSet* self, int idx)
{
	if(idx <= 0 || idx > ISRS_FIELD_COUNT(self))
		return -1;

	if(idx == 1)
	{
		if(self->flags&NCS_ISRS_KEY_FIELD)
			return 0; //get key
		else
			return 1; //get value
	}

	return 1; //get value
}

static DWORD mIniSectionRecordSet_getField(mIniSectionRecordSet* self, int idx)
{
	PETCSECTION petc = inirs_get_section((mIniRecordSet*)self);
	if(!petc)
		return 0;

	if(self->cursor >= petc->key_nr)
		return 0;

	idx = idx_to_type(self, idx);
	if(idx == -1)
		return 0;

	if(idx == 0)
		return (DWORD)petc->keys[self->cursor];
	return (DWORD)petc->values[self->cursor];
}

static BOOL mIniSectionRecordSet_setField(mIniSectionRecordSet* self, int idx, DWORD value)
{
	PETCSECTION petc = inirs_get_section((mIniRecordSet*)self);
	if(!(self->flags & NCS_DS_SELECT_WRITE))
		return FALSE;
	if(!petc || value == 0)
		return 0;

	if(self->cursor >= petc->key_nr)
		return 0;

	idx = idx_to_type(self, idx);
	if(idx == -1)
		return 0;

	if(idx == 0 ) //key
	{
		if(petc->keys[self->cursor] && strcmp(petc->keys[self->cursor],(char*)value) == 0)
			return TRUE;

		if(petc->keys[self->cursor])
			STR_FREE(petc->keys[self->cursor]);
		petc->keys[self->cursor] = STR_DUP((char*)value);
		self->flags |= IRSF_KEY_CHANGED;
	}
	else
	{
		if(petc->values[self->cursor])
			STR_FREE(petc->values[self->cursor]);

		petc->values[self->cursor] = STR_DUP((char*)value);
		self->flags |= IRSF_VALUE_CHANGED;
	}

	inids_rs_update_section(self->source, self->sec_idx, self->cursor,(mObject*)self);

	return TRUE;
}

static BOOL mIniSectionRecordSet_newRecord(mIniSectionRecordSet* self, int insert_type)
{
	PETCSECTION psec = inirs_get_section((mIniRecordSet*)self);
	int at;
	int i;
	if(!(self->flags & NCS_DS_SELECT_WRITE))
		return FALSE;

	if(!psec)
		return FALSE;

	//resize
	if(!section_resize(psec, psec->key_nr+1))
		return FALSE;

	switch(insert_type)
	{
	case NCS_RS_INSERT_BEFORE:
		at = self->cursor -1;
		if(at < 0)
			at = 0;
		break;
	case NCS_RS_INSERT_AFTER:
		at = self->cursor + 1;
		if(at > psec->key_nr)
			at = psec->key_nr;
		break;
	case NCS_RS_INSERT_APPEND:
	default:
		at = psec->key_nr;
		break;
	}

	if(self->cursor < 0)
		self->cursor = 0;
	else if(self->cursor >= psec->key_nr)
		self->cursor = psec->key_nr - 1;

	//move keys and values
	for(i = psec->key_nr-1; i>=at; i--)
	{
		psec->keys[i+1] = psec->keys[i];
		psec->values[i+1] = psec->values[i];
	}

	psec->keys[at] = NULL; //ready for add
	psec->values[at] = NULL; //ready for add
	psec->key_nr ++;

	self->flags |= IRSF_KEY_CREATED;

	inids_rs_update_new(self->source, self->sec_idx, at, (mObject*)self);

	if(at != self->cursor)
	{
		self->cursor = at;
		ncsRaiseDataSourceBindProps((mObject*)self, NCS_RSE_CURSOR_UPDATE);
	}

	return TRUE;
}

BOOL mIniSectionRecordSet_deleteRecord(mIniSectionRecordSet* self)
{
	if(!(self->flags & NCS_DS_SELECT_WRITE))
		return FALSE;

	if(section_remove(inirs_get_section((mIniRecordSet*)self),self->cursor))
	{
		self->flags |= IRSF_KEY_DELETED;
		inids_rs_update_del(self->source,self->sec_idx, self->cursor,(mObject*)self);
		return TRUE;
	}
	return FALSE;
}

static BOOL mIniSectionRecordSet_update(mIniSectionRecordSet* self)
{
	if(!(self->flags & NCS_DS_SELECT_WRITE))
		return FALSE;
	if(!(self->flags&(IRSF_KEY_DELETED|IRSF_VALUE_CHANGED|IRSF_KEY_CHANGED|IRSF_KEY_CREATED)))
		return TRUE;

	return inids_save_section(self->source, inirs_get_section((mIniRecordSet*)self));
}

static BOOL mIniSectionRecordSet_cancel(mIniSectionRecordSet* self)
{
	//only can canl the delete
	PETCSECTION psec;
	int i;
	int old_nr;
	if(!(self->flags & NCS_DS_SELECT_WRITE))
		return FALSE;

	if(!(self->flags&IRSF_KEY_DELETED))
		return FALSE;

	if((psec = inirs_get_section((mIniRecordSet*)self))== NULL)
		return FALSE;

	old_nr = psec->key_nr;

	for(i=0; i<psec->key_nr_alloc - psec->key_nr; i++)
	{
		if(!psec->keys[psec->key_nr_alloc-i-1])
			break;

		psec->keys[psec->key_nr + i] = psec->keys[psec->key_nr_alloc - i -1];
		psec->values[psec->key_nr + i] = psec->values[psec->key_nr_alloc - i -1];
		psec->keys[psec->key_nr_alloc - i -1] = NULL;
	}

	psec->key_nr = old_nr + i;
	if(self->cursor >= old_nr)
		ncsRaiseDataSourceBindProps((mObject*)self, NCS_RSE_VALUE_UPATED);

	return TRUE;
}

static BOOL mIniSectionRecordSet_isEnd(mIniSectionRecordSet* self)
{
	return self->cursor >= _c(self)->getRecordCount(self);
}


static BOOL mIniSectionRecordSet_isCurKey(mIniSectionRecordSet* self, int key)
{
	return self->cursor == key;
}



BEGIN_MINI_CLASS(mIniSectionRecordSet, mIniRecordSet)
	CLASS_METHOD_MAP(mIniSectionRecordSet, getCursor)
	CLASS_METHOD_MAP(mIniSectionRecordSet, setCursor)
	CLASS_METHOD_MAP(mIniSectionRecordSet, seekCursor)
	CLASS_METHOD_MAP(mIniSectionRecordSet, getFieldCount)
	CLASS_METHOD_MAP(mIniSectionRecordSet, getRecordCount)
	CLASS_METHOD_MAP(mIniSectionRecordSet, getField)
	CLASS_METHOD_MAP(mIniSectionRecordSet, setField)
	CLASS_METHOD_MAP(mIniSectionRecordSet, isEnd)
	CLASS_METHOD_MAP(mIniSectionRecordSet, newRecord)
	CLASS_METHOD_MAP(mIniSectionRecordSet, deleteRecord)
	CLASS_METHOD_MAP(mIniSectionRecordSet, update)
	CLASS_METHOD_MAP(mIniSectionRecordSet, cancel)
	CLASS_METHOD_MAP(mIniSectionRecordSet, isCurKey)
END_MINI_CLASS


//////////////////////////
//
static int field_idx_to_key_idx(mIniArrRecordSet* self, PETCSECTION psec, int field_idx);

static void inirs_arr_update_count(mIniArrRecordSet*self)
{
	PETCSECTION psec;
	psec = inirs_get_section((mIniRecordSet*)self);
	if(psec && self->count_key != NULL)
	{
		int key = get_key_idx(psec, self->count_key);
		if(key == -1)
			return ;
		self->count	= atoi(psec->values[key]);
	}
}

static void mIniArrRecordSet_construct(mIniArrRecordSet* self, DWORD param)
{
	inirs_arr_create_info_t *pinfo = (inirs_arr_create_info_t*)param;
	Class(mIniRecordSet).construct((mIniRecordSet*)self, param);

	if(pinfo)
	{
		self->count_key   = pinfo->count_key;
		self->field_keys  = pinfo->key_list;
		self->field_count = pinfo->field_count;
		inirs_arr_update_count(self);
	}
}

static void mIniArrRecordSet_destroy(mIniArrRecordSet* self)
{
	if(self->field_keys)
	{
		int i;
		for(i=0; i<self->field_count; i++)
			free(self->field_keys[i]);
		free(self->field_keys);
	}
	Class(mIniRecordSet).destroy((mIniRecordSet*)self);
}

static int mIniArrRecordSet_getFieldCount(mIniArrRecordSet* self)
{
	return self->field_count;
}

static int mIniArrRecordSet_getRecordCount(mIniArrRecordSet* self)
{
	return self->count;
}

static DWORD mIniArrRecordSet_getCursor(mIniArrRecordSet* self)
{
	return (DWORD)self->cursor_key;
}

static BOOL mIniArrRecordSet_setCursor(mIniArrRecordSet* self, DWORD cursor)
{
	char * cursor_key;
	char sz[32];
	int  i = sizeof(sz)-1;
	int  len;
	int    c;
	if(cursor == 0 || cursor == (DWORD)-1)
		return FALSE;

	cursor_key = (char*)cursor;
	sz[i] = 0;
	len = strlen(cursor_key)-1;
	while(len >= 0 && cursor_key[len]>='0' && cursor_key[len]<='9')
	{
		sz[--i] = cursor_key[len];
		len --;
	}

	c = atoi(sz+i);

	self->cursor_key = cursor_key;
	if(self->cursor != c)
	{
		self->cursor = c;
		ncsRaiseDataSourceBindProps((mObject*)self, NCS_RSE_CURSOR_UPDATE);
	}
	return TRUE;
}

static DWORD mIniArrRecordSet_seekCursor(mIniArrRecordSet* self, int seek, int offset)
{
	PETCSECTION psec;
	int cursor = 0;
	if(!(psec = inirs_get_section((mIniRecordSet*)self)))
		return 0;

	switch(seek)
	{
	case NCS_RS_CURSOR_BEGIN:
		cursor = offset;
		break;
	case NCS_RS_CURSOR_CUR:
		cursor = self->cursor + offset;
		break;
	case NCS_RS_CURSOR_END:
		cursor = self->count + offset-1;
		break;
	}

	if(cursor < 0)
		cursor = 0;
	else if(cursor > self->count)
		cursor = self->count;


	if(cursor != self->cursor)
	{
		int i;
		self->cursor = cursor;
		i = field_idx_to_key_idx(self, psec, 1);
		if(i < 0)
			self->cursor_key = NULL;
		else
			self->cursor_key = psec->keys[i];

		ncsRaiseDataSourceBindProps((mObject*)self, NCS_RSE_CURSOR_UPDATE);
	}
	else
	{
		if(self->cursor_key == NULL)
		{
			int i = field_idx_to_key_idx(self, psec,1);
			self->cursor_key = i<0?NULL:psec->keys[i];
		}
	}

	return (DWORD)self->cursor_key;
}

static const char* field_idx_to_key(mIniArrRecordSet* self, int field_idx, char* buff)
{
	const char* key;
	if(field_idx <= 0 || field_idx > self->field_count || self->field_keys == NULL)
		return NULL;

	if(self->cursor >= self->count)
		return NULL;

	key = self->field_keys[field_idx-1];
	if(!key)
		return NULL;

	sprintf(buff, "%s%d", key, self->cursor);
	return buff;
}

static int field_idx_to_key_idx(mIniArrRecordSet* self, PETCSECTION psec, int field_idx)
{
	char szkey[1024];
	int i;
	if(!psec)
		psec = inirs_get_section((mIniRecordSet*)self);

	if(!psec)
		return -1;

	if(!field_idx_to_key(self, field_idx, szkey))
		return -1;

	//find key
	for(i=0; i<psec->key_nr; i++)
	{
		if(psec->keys[i] && strcmp(psec->keys[i], szkey) == 0)
			return i;
	}

	return -1;
}

static DWORD mIniArrRecordSet_getField(mIniArrRecordSet* self, int field_idx)
{
	PETCSECTION psec;
	int idx;

	if(!(psec = inirs_get_section((mIniRecordSet*)self)))
		return 0;

	idx = field_idx_to_key_idx(self, psec, field_idx);

	if(idx == -1)
		return 0;

	return (DWORD)psec->values[idx];
}

static BOOL mIniArrRecordSet_setField(mIniArrRecordSet* self, int field_idx, DWORD value)
{
	PETCSECTION psec;
	int idx;

	if(value == 0 || !(self->flags&NCS_DS_SELECT_WRITE))
		return FALSE;

	if(!(psec = inirs_get_section((mIniRecordSet*)self)))
		return FALSE;

	idx = field_idx_to_key_idx(self, psec, field_idx);

	if(idx == -1)
		return FALSE;

	if(psec->values[idx])
		STR_FREE(psec->values[idx]);
	psec->values[idx] = STR_DUP((char*)value);
	self->flags |= IRSF_VALUE_CHANGED;
	inids_rs_update_section(self->source, self->sec_idx, idx,(mObject*)self);
	return TRUE;
}

static inline int get_key_idx_by_cursor(mIniArrRecordSet* self, PETCSECTION psec, int cursor, int idx)
{
	char szkey[1024];
	int i;

	if(!self->field_keys[idx])
		return -1;

	sprintf(szkey,"%s%d", self->field_keys[idx],cursor);

	//try to find from cursor
	for(i=cursor; i<psec->key_nr; i++)
	{
		if(psec->keys[i] && strcmp(szkey, psec->keys[i])==0)
			return i;
	}

	for(i=cursor-1; i>=0; i--)
	{
		if(psec->keys[i] && strcmp(szkey, psec->keys[i]) == 0)
			return i;
	}

	return -1;
}

static BOOL mIniArrRecordSet_newRecord(mIniArrRecordSet* self, int insert_type)
{
	PETCSECTION psec;
	int cursor = 0;
	int i;
	int j;
	int old_key_nr;
	int old_count;
	if(!(self->flags&NCS_DS_SELECT_WRITE))
		return FALSE;

	if(self->field_keys == NULL)
		return FALSE;

	if(!(psec = inirs_get_section((mIniRecordSet*)self)))
		return FALSE;

	switch(insert_type)
	{
	case NCS_RS_INSERT_BEFORE:
		cursor = self->cursor - 1;
		if(cursor < 0)
			cursor = 0;
		break;
	case NCS_RS_INSERT_AFTER:
		cursor = self->cursor + 1;
		if(cursor > self->count)
			cursor = self->count;
		break;
	case NCS_RS_INSERT_APPEND:
		cursor = self->count;
		break;
	}

	//alloc the keys
	section_resize(psec, psec->key_nr + self->field_count);
	old_key_nr = psec->key_nr;
	psec->key_nr += self->field_count;
	old_count = self->count;
	self->count ++;

	j = old_key_nr;
	for(i=0; i<self->field_count; i++)
	{
		char szkey[512];
		sprintf(szkey, "%s%d", self->field_keys[i],old_count);
		psec->keys[j+i] = STR_DUP(szkey);
	}


	for(i= old_count; i>cursor; i--)
	{
		for(j=0; j<self->field_count; j++)
		{
			int idx = get_key_idx_by_cursor(self, psec, i, j);
			int ridx = get_key_idx_by_cursor(self, psec, i-1, j);
			if(idx == -1 || ridx == -1)
				continue;
			psec->values[idx] = psec->values[ridx];
			psec->values[ridx] = NULL;
			inids_rs_update_section(self->source, self->sec_idx, idx,(mObject*)self);
		}
	}

	for(j=old_key_nr; j<psec->key_nr; j++)
	{
		psec->values[j] = NULL;
		inids_rs_update_section(self->source, self->sec_idx, j,(mObject*)self);
	}

	return TRUE;
}

#if 0
static BOOL mIniArrRecordSet_deleteRecord(mIniArrRecordSet* self)
{

	return FALSE;
#if 0
	PETCSECTION psec;
	int i;
	if(value == 0 || !(self->flags&NCS_DS_SELECT_WRITE))
		return FALSE;

	if(self->field_keys == NULL)
		return FALSE;

	if(!(psec = inirs_get_section(self)))
		return FALSE;

#endif
}
#endif

static BOOL mIniArrRecordSet_update(mIniArrRecordSet* self)
{
	PETCSECTION psec;
	if(!(self->flags & NCS_DS_SELECT_WRITE))
		return FALSE;
	if(!(self->flags&(IRSF_KEY_DELETED|IRSF_VALUE_CHANGED|IRSF_KEY_CHANGED|IRSF_KEY_CREATED)))
		return TRUE;

	if(!(psec = inirs_get_section((mIniRecordSet*)self)))
		return FALSE;
	//save count
	{
		char szText[100];
		sprintf(szText,"%d",self->count);
		inids_save_key(self->source, psec, self->count_key,szText);
	}

	return inids_save_section(self->source, inirs_get_section((mIniRecordSet*)self));
}

static BOOL mIniArrRecordSet_cancel(mIniArrRecordSet* self)
{
	return FALSE;
}

static BOOL mIniArrRecordSet_isEnd(mIniArrRecordSet* self)
{
	return self->cursor >= self->count;
}

static BOOL mIniArrRecordSet_isCurKey(mIniArrRecordSet* self, int keyidx)
{
	PETCSECTION psec = inirs_get_section((mIniRecordSet*)self);
	const char *key;
	if(!psec)
		return FALSE;

	if(keyidx < 0 || keyidx >= psec->key_nr)
		return FALSE;

	key = psec->keys[keyidx];

	if(key)
	{
		int len ;
		const char* str;
		int cursor;

		if(key == self->count_key || strcmp(key, self->count_key) == 0)
		{
			inirs_arr_update_count(self);
			return FALSE;
		}

		len = strlen(key);
		str = key + len -1;
		while(str > key && (*str>='0' && *str<='9'))
			str --;
		if(str <= key || str == key+len-1)
			return FALSE;

		cursor = atoi(str);
		return (self->cursor == cursor);
	}

	return FALSE;
}


BEGIN_MINI_CLASS(mIniArrRecordSet, mIniRecordSet)
	CLASS_METHOD_MAP(mIniArrRecordSet, construct)
	CLASS_METHOD_MAP(mIniArrRecordSet, destroy)
	CLASS_METHOD_MAP(mIniArrRecordSet, getCursor)
	CLASS_METHOD_MAP(mIniArrRecordSet, setCursor)
	CLASS_METHOD_MAP(mIniArrRecordSet, seekCursor)
	CLASS_METHOD_MAP(mIniArrRecordSet, getFieldCount)
	CLASS_METHOD_MAP(mIniArrRecordSet, getRecordCount)
	CLASS_METHOD_MAP(mIniArrRecordSet, getField)
	CLASS_METHOD_MAP(mIniArrRecordSet, setField)
	CLASS_METHOD_MAP(mIniArrRecordSet, update)
	CLASS_METHOD_MAP(mIniArrRecordSet, cancel)
	CLASS_METHOD_MAP(mIniArrRecordSet, newRecord)
	CLASS_METHOD_MAP(mIniArrRecordSet, isEnd)
	CLASS_METHOD_MAP(mIniArrRecordSet, isCurKey)
END_MINI_CLASS

///////////////////////////////////

static void mIniValueBindProp_construct(mIniValueBindProp* self, DWORD param)
{
	iniprop_create_info_t *pinfo = (iniprop_create_info_t*)param;
	if(pinfo)
	{
		Class(mBindProp).construct((mBindProp*)self, pinfo->flags);
		self->sec_idx = pinfo->sec_idx;
		self->key_idx = pinfo->key_idx;
	}
	else
	{
		Class(mBindProp).construct((mBindProp*)self, 0);
	}

}

static void mIniValueBindProp_destroy(mIniValueBindProp* self)
{
	//remove from list
	if(self->source)
	{
		mBindProp * prop = self->source->props;
		mBindProp * prev = NULL;
		while(prop && prop != (mBindProp*)self)
		{
			prev = prop;
			prop = prop->next;
		}
		if(prev == NULL)
			self->source->props = self->next;
		else
		{
			prev->next = self->next;
		}
	}

	Class(mBindProp).destroy((mBindProp*)self);
}

static DWORD mIniValueBindProp_get(mIniValueBindProp* self)
{
	PETCSECTION psec;
	if(!self->source
		|| !self->source->etc
		|| !self->source->etc->sections)
		return 0;

	if(self->sec_idx >= self->source->etc->section_nr)
		return 0;

	psec = &(self->source->etc->sections[self->sec_idx]);

	if(!psec->keys || !psec->values
		|| self->key_idx >= psec->key_nr)
		return 0;

	return (DWORD)psec->values[self->key_idx];
}

static BOOL mIniValueBindProp_set(mIniValueBindProp* self, DWORD value)
{
	PETCSECTION psec;
	if(!self->source
		|| !self->source->etc
		|| !self->source->etc->sections)
		return FALSE;

	if(!(self->source->flags & NCS_DS_SELECT_WRITE))
		return FALSE;

	if(self->sec_idx >= self->source->etc->section_nr)
		return FALSE;

	psec = &(self->source->etc->sections[self->sec_idx]);

	if(!psec->keys || !psec->values
		|| self->key_idx >= psec->key_nr)
		return FALSE;

	if(inids_save_key_byidx(self->source, psec, self->key_idx,(const char*)value))
	{
		inids_rs_update_section(self->source, self->sec_idx, self->key_idx, (mObject*)self);
	}
	return TRUE;
}

BEGIN_MINI_CLASS(mIniValueBindProp, mBindProp)
	CLASS_METHOD_MAP(mIniValueBindProp, construct)
	CLASS_METHOD_MAP(mIniValueBindProp, destroy)
	CLASS_METHOD_MAP(mIniValueBindProp, set)
	CLASS_METHOD_MAP(mIniValueBindProp, get)
END_MINI_CLASS


////////////////////////////////////////////////////
//COMMON FUNCTIONS
static void inids_rs_update_section(mIniDataSource* self, int sect, int key, mObject* sender)
{
	mIniRecordSet* sets;
	mIniValueBindProp* prop;
	for(sets = self->sets; sets; sets = sets->next)
	{
		if(sender != (mObject*)sets && sets->sec_idx == sect && _c(sets)->isCurKey(sets, key))
		{
			ncsRaiseDataSourceBindProps((mObject*)sets, NCS_RSE_VALUE_UPATED);
		}
	}

	for(prop = (mIniValueBindProp*)self->props; prop; prop = (mIniValueBindProp*)prop->next)
	{
		if(sender != (mObject*)prop
			&& (int)prop->sec_idx == sect
			&& (int)prop->key_idx == key)
		{
			ncsBindPropRasieChanged((mBindProp*)prop);
		}
	}
}

static void inids_rs_update_del(mIniDataSource* self, int sect, int key, mObject* sender)
{
	mIniRecordSet* sets;
	mIniValueBindProp* prop;
	for(sets = self->sets; sets; sets = sets->next)
	{
		if(sender == (mObject*)sets && sets->sec_idx == sect && _c(sets)->isCurKey(sets, key))
		{
			_c(sets)->seekCursor(sets, NCS_RS_CURSOR_CUR, 1);
			ncsRaiseDataSourceBindProps((mObject*)sets, NCS_RSE_VALUE_REMOVED);
		}
	}

	for(prop = (mIniValueBindProp*)self->props; prop; prop = (mIniValueBindProp*)prop->next)
	{
		if(sender != (mObject*)prop
			&& (int)prop->sec_idx == sect
			&& (int)prop->key_idx == key)
		{
			ncsBindPropRasieChanged((mBindProp*)prop);
		}
	}
}

static void inids_rs_update_new(mIniDataSource* self, int sect, int key, mObject *sender)
{
	inids_rs_update_del(self, sect, key, sender);
}

static BOOL seek_section(FILE *fp, PETCSECTION psec)
{
	int idx = 0;
	int state = 0; //0 -- first line, 1 -- in_section 2 -- to get next line
	fseek(fp, 0, SEEK_SET);
	while(!feof(fp))
	{
		int ch = fgetc(fp);
		switch(state)
		{
		case 0: //first line
			if(ch == '[')
			{
				state = 1;
				idx = 0;
			}
			else if(ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n'){
				break;
			}
			else{
				state = 2; //get next line;
			}
			break;
		case 1: //in section
			if(ch == ']')
			{
				if(psec->name[idx] == 0) //find
					goto FOUND;

				state = 2; //get next line
			}
			else
			{
				if(psec->name[idx] == ch)
					idx ++;
				else
				{
					state = 2;
				}
			}
			break;
		case 2: //get next line
			if(ch == '\n')
			{
				state = 0;
			}
			break;
		}
	}

	fseek(fp, 0, SEEK_END);

	return FALSE; //not found

FOUND:
	while(!feof(fp))
	{
		int ch = fgetc(fp);
		if(ch == '\n')
			return TRUE;
	}

	fseek(fp, 0, SEEK_END);
	return TRUE;
}

static BOOL seek_key(FILE* fp, const char* key)
{
	int state = 0;
	int idx = 0;
	long line_pos = ftell(fp);
	while(!feof(fp))
	{
		int ch = fgetc(fp);
		switch(state)
		{
		case 0: //new line
			if(ch == ' ' || ch == '\t' || ch =='\r' || ch == '\n')
				break;
			else if(ch == '#')
				state = 1;
			else if(ch == '[') //next section
			{
				fseek(fp, line_pos, SEEK_SET);
				return FALSE;
			}
			else
				state = 2;
			break;
		case 1: //comment
			if(ch == '\n')
			{
				idx = 0;
				state = 0;
				line_pos = ftell(fp);
			}
			break;
		case 2: //inline test
			if((ch == '=' || ch==' ' || ch == '\t') && key[idx] == 0) //found
			{
				fseek(fp, line_pos, SEEK_SET);
				return TRUE;
			}

			if(key[idx] == ch)
				idx ++;
			else
				state = 1;
			break;
		}
	}

	fseek(fp, 0, SEEK_END);

	return FALSE;
}

static char* read_left(FILE *fp, int* plen)
{
	int len;
	long pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	len = ftell(fp) - pos;
	fseek(fp, pos, SEEK_SET);
	if(len > 0)
	{
		char* str = (char*)malloc(len+1);
		int readLen = fread(str, 1, len, fp);
		str[readLen] = 0;
		fseek(fp, pos, SEEK_SET);
		if(plen)
			*plen = readLen;
		return str;
	}

	return NULL;
}

static const char* strchrend(const char* str, int ch, int end)
{
	while(*str && *str != end && *str != ch)
		str ++;

	if(*str && *str == ch)
		return str;
	return NULL;
}

static BOOL inids_save_section(mIniDataSource* self, PETCSECTION psec)
{
	int len;
	char* buf;
	int i;
	int idx;
	int next_section_pos = 0;
	int left;
	char *write_mark = NULL;
	int total_len;
	//save to file
	if(!(self->flags & NCS_DS_SELECT_WRITE))
		return FALSE;

	if(!self->fp)
		return FALSE;

	//find the section
	if(!seek_section(self->fp, psec))
	{
		fprintf(self->fp,"[%s]\n", psec->name);
		for(i=0; i<psec->key_nr; i++)
			fprintf(self->fp, "%s=%s\n", psec->keys[i], psec->values[i]);
		return TRUE;
	}

	buf = read_left(self->fp, &len);

	if(buf == NULL)
	{
		for(i=0; i<psec->key_nr; i++)
			fprintf(self->fp, "%s=%s\n", psec->keys[i], psec->values[i]);
		return TRUE;
	}

	//get next section pos
	next_section_pos = 0;
	while(buf[next_section_pos])
	{
		//skip space
		idx = next_section_pos;
		while(buf[next_section_pos] &&
			( buf[next_section_pos] == ' '
			  || buf[next_section_pos] == '\t'
			  || buf[next_section_pos] == '\r'
			  || buf[next_section_pos] == '\n')) next_section_pos ++;

		if(buf[next_section_pos] == '[')
		{
			next_section_pos = idx;
			break;
		}
		else
			while(buf[next_section_pos] && buf[next_section_pos] != '\n') next_section_pos ++;
	}

	//save values
	i = 0;
	left = psec->key_nr;

	//set write mark
//	if(next_section_pos > i)
	{
		write_mark = (char*)ALLOCA(psec->key_nr+1);
		memset(write_mark, 0, psec->key_nr+1);
	}

	while(i<next_section_pos)
	{
		char* str;
		while(i<next_section_pos && buf[i] && (buf[i] == ' ' || buf[i] == '\t' || buf[i] == '\r' || buf[i] == '\n'))
		{
			fwrite(&buf[i],1,1,self->fp);
			i ++;
		}
		if(i>=next_section_pos)
			break;
		if(buf[i]== '#' || (str=(char*)strchrend(buf+i, '=', '\n'))==NULL) //comments
		{
			while(buf[i] && buf[i]!='\n'){
				fwrite(&buf[i], 1, 1, self->fp);
				i++;
			}
		}
		else
		{
			//find key value
			int j;
			for(j=0; j<psec->key_nr; j++)
			{
				if(!write_mark[j] && strncmp(psec->keys[j], buf+i, str - buf - i) == 0)
				{
					write_mark[j] = 1;
					break;
				}
			}
			if(j < psec->key_nr) //found
			{
				fprintf(self->fp, "%s=%s", psec->keys[j], psec->values[j]);
				left --;
			}
			while(i< next_section_pos && buf[i] != '\n' && buf[i] != '#') i++;
		}
	}

	//out the left key values
	if(left > 0)
	{
		int j;
		fprintf(self->fp, "\n");
		for(j = 0; j<psec->key_nr; j++)
		{
			if(!write_mark[j])
				fprintf(self->fp,"%s=%s\n",psec->keys[j], psec->values[j]);
		}
	}

	total_len = ftell(self->fp);
	//out the left buf
	if(next_section_pos < len)
	{
	//	fwrite(buf + next_section_pos, 1, len - next_section_pos, self->fp);
		fprintf(self->fp, "%s", buf + next_section_pos);
		total_len += strlen(buf + next_section_pos);
	}

#ifdef WIN32
	win_truncate_file(self->fp, total_len);
#else
	if(ftruncate(fileno(self->fp), total_len) < 0)
        fprintf(stderr, "ftruncate file failure\n");
#endif

	if(write_mark)
		FREEA(write_mark);
	if(buf)
		free(buf);

	fflush(self->fp);

	return TRUE;
}

static BOOL inids_save_key(mIniDataSource* self, PETCSECTION psec, const char* key, const char* value)
{
	int idx ;
	if(!(self->flags & NCS_DS_SELECT_WRITE))
		return FALSE;

	if(!self->fp)
		return FALSE;

	for(idx = 0; idx < psec->key_nr; idx ++)
	{
		if(psec->keys[idx] && strcmp(key, psec->keys[idx]) == 0)
			return inids_save_key_byidx(self, psec, idx, value);
	}

	return FALSE;
}

static BOOL inids_save_key_byidx(mIniDataSource* self, PETCSECTION psec, int key_idx, const char* value)
{
	char * str = NULL;
	int len;
	if(!(self->flags & NCS_DS_SELECT_WRITE))
		return FALSE;

	if(!psec->values[key_idx] || strcmp(psec->values[key_idx], value) != 0)
	{
		if(psec->values[key_idx])
			STR_FREE(psec->values[key_idx]);

		psec->values[key_idx] = STR_DUP(value);

		inids_rs_update_section(self, psec - self->etc->sections, key_idx,(mObject*)self);
	}

	if(!self->fp)
		return FALSE;


	if(!seek_section(self->fp,psec))
	{
		fprintf(self->fp, "\n[%s]\n", psec->name);
		fprintf(self->fp, "%s=%s\n", psec->keys[key_idx], value);
	}
	else if(seek_key(self->fp, psec->keys[key_idx]))
	{
		//get next line
		int write_pos = ftell(self->fp);
		while(!feof(self->fp))
		{
			if(fgetc(self->fp) != '\n');
		}

		str = read_left(self->fp, &len);
		fseek(self->fp, write_pos,SEEK_SET);
	}
	else
		str = read_left(self->fp, &len);

	fprintf(self->fp, "%s=%s\n", psec->keys[key_idx],value);

	if(str)
	{
		fwrite(str, 1, len, self->fp);
		free(str);
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////

mDataSource* ncsCreateEtcDataSource(const char* etc_file, int mode)
{
	inids_create_info_t cinfo;
	if(etc_file == NULL || mode == 0)
		return NULL;

	cinfo.etc = (ETC_S*)LoadEtcFile(etc_file);
	if(cinfo.etc == NULL)
		return NULL;
	cinfo.etcFile = etc_file;
	cinfo.rw_flags = mode;

	return (mDataSource*)NEWEX(mIniDataSource, (DWORD)&cinfo);
}

mDataSource* ncsCreateEtcDataSourceByHandle(GHANDLE hEtc, int mode)
{
	inids_create_info_t cinfo;
	if(hEtc == (GHANDLE)0 || mode == 0)
		return NULL;

	cinfo.etcFile = NULL;
	cinfo.etc = (ETC_S*)hEtc;
	cinfo.rw_flags = mode;

	return (mDataSource *)NEWEX(mDataSource, (DWORD)&cinfo);
}


/////////////////////////////////////
void ncsInitIniDataSource(void)
{
	MGNCS_INIT_CLASS(mIniDataSource);
	MGNCS_INIT_CLASS(mIniRecordSet);
	MGNCS_INIT_CLASS(mIniSectionRecordSet);
	MGNCS_INIT_CLASS(mIniArrRecordSet);
	MGNCS_INIT_CLASS(mIniValueBindProp);
}

////////////////////////////////////////////
//#if defined(__VXWORKS__) || !defined(__NOUNIX__)
#if !defined(HAVE_FTRUNCATE)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int ftruncate(int fd, off_t length)
{
    //set zero to the end of file
    struct stat statbuff;
    int ret;
    int i;
    if ( (ret = fstat(fd, &statbuff)) != 0)
        return ret;

    if(statbuff.st_size <= length)
        return 0;

    for (i = 0; i < statbuff.st_size - length; i ++)
    {
        char c = 0;
        if (write(fd, &c, 1) < 1) {
            return -1;
        }
    }


    return 0;
}

#endif

#endif //_MGNCSDB_INI

