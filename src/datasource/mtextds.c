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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSDB_TEXT

#include "mdatabinding.h"
#include "mdatasource.h"
#include "comm_parser.h"
#include "mtextds.h"

#include "mem-slab.h"

//static BOOL textds_get_fields(mTextDataSource* self, mTextRecordNode* rn, BOOL bfocus);

static BOOL textds_prepare_to(mTextDataSource* self, int max);

static void textds_update_recordsets(mTextDataSource* self, mTextRecordNode* node);

static void textds_update_bindprops(mTextDataSource* self, mTextRecordNode* node);

static void textds_update_del_record(mTextDataSource* self, mTextRecordNode* node);


#define TSBP_UPF_DEL_MARK 0
#define TSBP_UPF_DEL      1
#define TSBP_UPF_DEL_RECOVER 2
static void textds_update_bindprops_del_node(mTextDataSource* self, mTextRecordNode* node, int flags);

static long save_field(mTextDataSource* self, mTextRecordNode* rn);

static BOOL textds_update_fields(mTextDataSource* self);

static void textds_revert_new(mTextDataSource* self, mTextRecordNode* node);

typedef struct _text_rs_create_info {
	mTextDataSource* source;
	int flags;
	unsigned char* field_list;
	mSymbol ** tester;
}text_rs_create_info_t;

typedef struct _text_data_value_bind_prop_create_info {
	mDataSource* source;
	mTextRecordNode* node;
	int idx;
	int type;
}text_data_value_bind_prop_create_info_t;

///////////////////////////////////////////////////
//mTextDataSource

typedef struct _text_ds_create_info {
	const char* file_name;
	int   fs;
	DWORD flags;
}text_ds_create_info_t;


static void mTextDataSource_construct(mTextDataSource* self, DWORD param)
{
	text_ds_create_info_t* cinfo = (text_ds_create_info_t*)param;

	Class(mDataSource).construct((mDataSource*)self, 0);
	if(cinfo && cinfo->flags != 0)
	{

		self->fp = fopen(cinfo->file_name,
			(cinfo->flags & NCS_DS_SELECT_WRITE)?"a+":"rt");

		if(self->fp)
		{
			fseek(self->fp, 0, SEEK_SET);
			self->fileName = strdup(cinfo->file_name);
		}

		self->flags = (unsigned char)cinfo->flags;
		self->fs = cinfo->fs;
	}
}

static void delete_record_node(mTextRecordNode* rn, int n)
{
	if(rn && rn->fields)
	{
		int i;
		for(i=0; i<n ; i++)
			if(rn->fields[i])
				free(rn->fields[i]);
		free(rn->fields);
	}
	_SLAB_FREE(rn);
}

static void mTextDataSource_destroy(mTextDataSource* self)
{
	mTextDataBindProp* prop;
	if(self->fp)
	{
		fclose(self->fp);
	}
	if(self->fileName)
		free(self->fileName);

	//remove sets
	while(self->head)
	{
		mTextRecordNode* rn = self->head;
		self->head = rn->next;
		delete_record_node(rn,self->field_count);
	}

	//remove sets
	while(self->sets)
	{
		mTextRecordSet* s = self->sets;
		self->sets = s->next;
		_c(s)->release(s);
	}

	//remove props
	while(self->props)
	{
		prop = self->props;
		self->props = (mTextDataBindProp*)prop->next;
		prop->read = prop->write = 0;
		ncsBindPropRelease((mBindProp*)prop);
	}

	Class(mDataSource).destroy((mDataSource*)self);
}

static int get_writeable_rs_count(mTextDataSource* self)
{
	int i;
	mTextRecordSet* sets = self->sets;

	for(i=0; sets; sets = sets->next){
		if(sets->flags&NCS_DS_SELECT_WRITE)
			i++;
	}

	return i;
}

static mRecordSet * mTextDataSource_selectRecordSet(mTextDataSource* self, const char* select, int select_type)
{
	char szSelect[1024];
	char * selects[2] = {NULL, NULL};
	mSymbol** tester = NULL;
	unsigned char* field_list = NULL;
	char *str;
	text_rs_create_info_t cinfo;
	mTextRecordSet* rs ;
	if(select == NULL)
		return NULL;

	if(select_type & NCS_DS_SELECT_WRITE &&
			!((self->flags & NCS_DS_SELECT_WRITE)
				|| get_writeable_rs_count(self) > 0))
		return NULL;

	strcpy(szSelect, select);
	if(ncsStrSpliter(szSelect, selects, ':') <= 0)
	{
		selects[0] = szSelect;
		selects[1] = NULL;
	}

	//get test node
	str = selects[0];
	/*if(!str || *str == '*')*/ /* don't support expression now*/
		tester = NULL;
	/*else
		tester = ncsParserTestExpression(&str);*/

	if(selects[1]){
		str = selects[1];
		field_list = ncsGetFieldList(&str, NULL);
	}

	cinfo.source = self;
	cinfo.flags = select_type;
	cinfo.field_list = field_list;
	cinfo.tester = tester;
	rs = NEWEX(mTextRecordSet, (DWORD)&cinfo);
	if(rs)
	{
		rs->next = self->sets;
		self->sets = rs;
	}
	textds_prepare_to(self, -1);
	return (mRecordSet*)rs;
}

typedef struct _textds_rt{
	mTextDataSource* source;
	mTextRecordNode* node;
	int cur_idx;
}textds_rt_t;
//static DWORD textds_getValue(textds_rt_t* self, mSymbol* symb)
static DWORD textds_getValue(void* self_org, mSymbol* symb)
{
	textds_rt_t* self = (textds_rt_t*)self_org;
	if(!self || !self->source || !self->node)
		return 0L;

	if(symb->type == NCS_BT_IDX)
	{
		if(symb->data.ival == '@')
			return self->cur_idx;
		else
			return (DWORD) self->node->fields[symb->data.ival];
	}
	return 0;
}

static mTextExpRuntime _TextDSRT={
	textds_getValue
};

static mBindProp* mTextDataSource_getBindProp(mTextDataSource* self, const char* select, int bp_type)
{
	char szSelect[1024];
	char * selects[2] = {NULL, NULL};
	mSymbol** tester = NULL;
	mTextRecordNode* node ;
	mTextDataBindProp* prop = NULL;
	int field_idx = 0;
	char *str;
	if(select == NULL)
		return NULL;

	if((bp_type & NCS_PROP_FLAG_WRITE) &&
			!((self->flags & NCS_DS_SELECT_WRITE)))
		return NULL;

	if(!textds_prepare_to(self, 1))
		return NULL;

	strcpy(szSelect, select);
	if(ncsStrSpliter(szSelect, selects, ':') <= 0)
	{
		selects[0] = szSelect;
		selects[1] = NULL;
	}

	//get test node
	str = selects[0];
	tester = ncsParserTestExpression(&str);

	str = selects[1];
	if(str)
	{
		str = ncsStrGetSurround(str, '{', '}');
		if(str && ncsStrSpliter(str, NULL, ',')> 0)
		{
			str = ncsGetVarName(str);
			if(str){
				field_idx = strtol(str, NULL, 0);
				field_idx --;
			}
		}
		if(field_idx < 0)
			field_idx = 0;
		if(field_idx >= self->field_count)
			field_idx = self->field_count - 1;
	}

	node = self->head;

	if(tester)
	{
		textds_rt_t tdsrt = {
			self,
			node,
			0
		};
		while(node)
		{
			tdsrt.node = node;
			//textds_get_fields(self, node, FALSE);
			if(ncsExecTestExpression(tester, &_TextDSRT, (void*)&tdsrt))
				break;
			node = node->next;
		}
	}

	if(node)
	{
		//test if exits?
		prop = self->props;
		while(prop)
		{
			if(prop->node == node && prop->revert == field_idx)
			{
				ncsBindPropAddRef((mBindProp*)prop);
				break;
			}
			prop = (mTextDataBindProp*)prop->next;
		}

		if(!prop)
		{
			text_data_value_bind_prop_create_info_t cinfo = {
				(mDataSource *)self,
				node,
				field_idx,
				bp_type
			};
			prop = NEWEX(mTextDataBindProp, (DWORD)&cinfo);
			prop->next = (mBindProp*) self->props;
			self->props = prop;
		}
	}

	if(tester)
		ncsFreeTestExpression(tester);

	return (mBindProp*)prop;
}


BEGIN_MINI_CLASS(mTextDataSource, mDataSource)
	CLASS_METHOD_MAP(mTextDataSource, construct)
	CLASS_METHOD_MAP(mTextDataSource, destroy)
	CLASS_METHOD_MAP(mTextDataSource, selectRecordSet)
	CLASS_METHOD_MAP(mTextDataSource, getBindProp)
END_MINI_CLASS

static char* get_field(FILE* fp, int fs, BOOL *pFieldEnd/*, int* ppos, BOOL getCountOnly*/)
{
	char buff[1024];
	char* str = NULL;
	int idx = 0;
	int buf_idx = 0;
	//int pos = 0;
	BOOL fieldEnd = FALSE;

	while(!feof(fp))
	{
		int ch = fgetc(fp);
		if(ch == '\n' || ch == '\r')
		{
			fieldEnd = TRUE;
			break;
		}


		if(fs == 0 && (ch == ' ' || ch == '\t'))
		{
			while(!feof(fp) && (ch == ' ' || ch == '\t')) ch = fgetc(fp);
			ungetc(ch, fp);
			break;
		}
		else if(fs == ch)
			break;

		/*if(!getCountOnly)*/
			buff[buf_idx] = ch;
		buf_idx ++;
		if(buf_idx >= sizeof(buff))
		{
			/*if(!getCountOnly)*/
			{
				str = (char*)realloc(str, buf_idx+idx);
				strncpy(str+idx, buff, buf_idx);
			}
			idx += buf_idx;
			buf_idx = 0;
		}
	}

	if(feof(fp))
	{
		fieldEnd = TRUE;
	}

/*	if(ppos)
		*ppos = pos;
*/

	if(pFieldEnd)
		*pFieldEnd = fieldEnd;

/*	if(!getCountOnly)*/
	{
		if(buf_idx > 0)
		{
			str = (char*)realloc(str, buf_idx+idx);
			//strncpy(str+idx, buff, buf_idx);
			memcpy(str+idx, buff, buf_idx); // VW: avoid warning
		}
		if(str)
			str[idx+buf_idx] = 0;
		return str;
	}
//	return (char*)(idx+buf_idx);
}

/*
static BOOL textds_get_fields(mTextDataSource* self, mTextRecordNode* rn, BOOL bfocus)
{
	long pos;
	int i;
	if(!self || !self->fp || !rn || self->field_count <= 0 || rn->pos == (DWORD)-1)
		return FALSE;

	if(bfocus && rn->fields)
	{
		//delete
		for(i = 0; i<self->field_count; i++)
		{
			if(rn->fields[i])
			{
				free(rn->fields[i]);
				rn->fields[i] = NULL;
			}
		}
	}
	if(rn->fields)
	{
		rn->fields = (char**)calloc(self->field_count, sizeof(char*));
	}

	pos = ftell(self->fp);
	fseek(self->fp, rn->pos, SEEK_SET);

	//get fileds
	for(i = 0; i<self->field_count; i++)
	{
		rn->fields[i] = get_field(self->fp, self->fs,NULL, FALSE);
		if(rn->fields[i])
			break;
	}

	fseek(self->fp,pos, SEEK_SET);

	return TRUE;
}*/

static int parse_field_count(mTextDataSource* self)
{
	long pos;
	int  count = 0;
	int  ch = 0;
	if(!self || !self->fp)
		return 0;

	pos = fseek(self->fp, SEEK_SET,0);
	//find the first not null line
	while(!feof(self->fp))
	{
		ch = fgetc(self->fp);
		if(ch != '\n' && ch != '\t' && ch != '\r' && ch != ' ')
		{
			break;
		}
	}

	if(feof(self->fp))
		return 0;

	ungetc(ch, self->fp);

	pos = ftell(self->fp);

	while(!feof(self->fp))
	{
		ch = fgetc(self->fp);
REDETECT:
		if(ch == '\n' || ch == '\r') //
		{
			count ++;
			break;
		}

		if( (self->fs == 0 && (ch == ' ' || ch == '\t'))
			|| self->fs == ch)
		{
			//break;
			count ++;

			if(self->fs == 0){
				while(!feof(self->fp) && (ch == ' ' || ch == '\t') && ch != '\n' && ch != '\r') ch = fgetc(self->fp);
				goto REDETECT;
			}
		}
	}

	fseek(self->fp, pos, SEEK_SET);
	return count ;

}

static BOOL textds_prepare_to(mTextDataSource* self, int max)
{
	mTextRecordNode *rn = NULL, *prev = NULL;
	//int i;
	//int pos;
	if(!self || !self->fp)
		return FALSE;

	if(self->head && self->tail)
		return TRUE;

	self->field_count = parse_field_count(self);
	if(self->field_count <= 0)
		return FALSE;

	//get field datas;
	while(!feof(self->fp))
	{
		int j;
		BOOL bEndFiled = FALSE;
		//skip null line
		while(!feof(self->fp))
		{
			int ch = fgetc(self->fp);
			if(!(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'))
			{
				ungetc(ch, self->fp);
				break;
			}
		}
		if(feof(self->fp))
			break;

		rn = (mTextRecordNode*)_SLAB_ALLOC(sizeof(mTextRecordNode));
		if(!rn)
			break;
		if(self->head == NULL)
			self->head = rn;
		else{
			prev->next = rn;
			rn->prev = prev;
		}
		prev = rn;

		//get pos
		rn->pos = ftell(self->fp);

		rn->fields = (char**)calloc(sizeof(char*), self->field_count);
		for(j = 0; j < self->field_count && !bEndFiled; j++)
		{
			rn->fields[j] =  get_field(self->fp, self->fs, &bEndFiled);
			//printf("--- %d:%s\n", j, rn->fields[j]);
		}
	}

	self->tail = rn;


	return TRUE;
}


static void textds_update_recordsets(mTextDataSource* self, mTextRecordNode* node)
{
	mTextRecordSet* sets;
	if(!node)
		return ;

	sets = self->sets;
	while(sets)
	{
		if(sets->cur_node == node)
		{
			ncsRaiseDataSourceBindProps((mObject*)sets, NCS_RSE_VALUE_UPATED);
		}
		sets = sets->next;
	}
}


static void textds_update_bindprops(mTextDataSource* self, mTextRecordNode* node)
{
	mTextDataBindProp* prop;

	if(!node)
		return ;

	for(prop = self->props; prop; prop = (mTextDataBindProp*)prop->next)
	{
		if(prop->node == node)
		{
			if(prop->event_filter & NCS_RSE_VALUE_UPATED)
				ncsBindPropRasieChanged((mBindProp*)prop);
		//		ncsBindPropRasieChanged((mTextDataBindProp*)prop);
		}
	}
}

static void textds_update_bindprops_del_node(mTextDataSource* self, mTextRecordNode* node, int flags)
{
	mTextDataBindProp* props, *prev = NULL;

	if(!node )
		return ;

	for(props = self->props; props; props = (mTextDataBindProp*)props->next)
	{
		if(props->node == node)
		{
			if(flags == TSBP_UPF_DEL){
				mTextDataBindProp* tp = (mTextDataBindProp*)props->next;
				//remove from list
				if(prev)
					prev->next = (mBindProp*)tp;
				else
					self->props = tp;
				props->node = NULL;
				props->write = 0;
				if(ncsBindPropRelease((mBindProp*)props) > 0
					&& (props->event_filter & NCS_RSE_VALUE_UPATED))
				{
					ncsBindPropRasieChanged((mBindProp*)props);
				}
				props->read = 0;
				props = tp;
				continue;
			}
			else
			{
				if(props->event_filter & NCS_RSE_VALUE_UPATED)
					ncsBindPropRasieChanged((mBindProp*)props);
			}

		}

		prev = props;
		props = (mTextDataBindProp*)props->next;
	}

}


static void textds_update_del_record(mTextDataSource* self, mTextRecordNode* node)
{
	mTextRecordSet* sets;
	if(!node)
		return ;

	sets = self->sets;
	while(sets)
	{
		if(sets->cur_node == node)
		{
			//delete
			ncsRaiseDataSourceBindProps((mObject*)sets, NCS_RSE_VALUE_REMOVED);
			_c(sets)->seekCursor(sets, NCS_RS_CURSOR_CUR,1);
		}
		sets = sets->next;
	}

	textds_update_bindprops_del_node(self, node, TSBP_UPF_DEL_MARK);
}

static long save_field(mTextDataSource* self, mTextRecordNode* rn)
{
	int i;
	if(!self  || !rn || !self->fp )
		return 0;

	for(i=0; i<self->field_count; i++)
	{
		fprintf(self->fp,"%s%c",rn->fields[i]?rn->fields[i]:"", self->fs==0?' ':self->fs);
	}
	fprintf(self->fp,"\n");
	return ftell(self->fp);
}

static BOOL textds_update_fields(mTextDataSource* self)
{
	//BOOL must_save = FALSE;
	int  pos = 0;
	mTextRecordNode* node = self->head;
	mTextRecordNode* prev = NULL;

	self->fp = freopen(self->fileName, "wt", self->fp);

	while(node)
	{
		//if(!must_save)
		//	pos = node->pos;

		if(node->flags & NCS_TRNF_DELETED)
		{
			mTextRecordNode* tn = node;
			node = node->next;
			if(prev == NULL)
				self->head = node;
			else
				prev->next = node;
			textds_update_bindprops_del_node(self, tn, TSBP_UPF_DEL);
			delete_record_node(tn, self->field_count);
			//must_save = TRUE;
			continue;
		}

/*		if(node->flags & (NCS_TRNF_NEW|NCS_TRNF_CHANGED))
		{
	//		fseek(self->fp, pos, SEEK_SET);
			pos = save_field(self, node);
			must_save = TRUE;
			self->flags = 0;
			node->pos = pos;
		}
		else
		{*/
	//		if(must_save)
			{
	//			fseek(self->fp, pos, SEEK_SET);
				pos = save_field(self, node);
				node->pos = pos;
				node->flags = 0;
			}
	//	}

		prev  = node;
		node = node->next;
	}

	return TRUE;
}

static void textds_revert_new(mTextDataSource* self, mTextRecordNode* node)
{
	mTextRecordSet* sets;
	if(!self || !node || !(node->flags&NCS_TRNF_NEW))
		return ;

	sets = self->sets;
	while(sets)
	{
		if(sets->cur_node == node)
		{
			ncsRaiseDataSourceBindProps((mObject*)sets, NCS_RSE_VALUE_REMOVED);
			_c(sets)->seekCursor(sets, NCS_RS_CURSOR_CUR, 1);
		}
		sets = sets->next;
	}

	//delete this node
	if(self->head == node)
		self->head = node->next;

	if(node->prev)
		node->prev->next = node->next;
	if(node->next)
		node->next->prev = node->prev;


	textds_update_bindprops_del_node(self, node, TSBP_UPF_DEL_RECOVER);

	delete_record_node(node, self->field_count);
}



 ///////////////////////////////////////////////////
//mTextRecordSet

static void mTextRecordSet_construct(mTextRecordSet* self, DWORD param)
{
	text_rs_create_info_t *pinfo = (text_rs_create_info_t*)param;
	Class(mRecordSet).construct((mRecordSet*)self, param);

	if(pinfo)
	{
		self->source = pinfo->source;
		self->field_list = pinfo->field_list;
		self->flags = pinfo->flags;
		self->tester = pinfo->tester;
	}

}

static void mTextRecordSet_destroy(mTextRecordSet* self)
{
	if(self->tester)
		ncsFreeTestExpression(self->tester);
	if(self->field_list)
		free(self->field_list);
	//remove from source

	if(self->source)
	{
		mTextRecordSet* sets = self->source->sets;
		if(sets == self)
		{
			self->source->sets = self->next;
		}
		else
		{
			while(sets->next && sets->next != self) sets = sets->next;
			if(sets->next)
			{
				sets->next = self->next;
			}
		}
	}

	Class(mRecordSet).destroy((mRecordSet*)self);

}

static int mTextRecordSet_getFieldCount(mTextRecordSet* self)
{
	if(self->source == NULL)
		return 0;
	if(self->field_list)
		return strlen((char*)self->field_list);
	return self->source->field_count;
}

static int mTextRecordSet_getFieldType(mTextRecordSet* self, int field_idx)
{
	return NCS_BT_STR;
}

static DWORD mTextRecordSet_getCursor(mTextRecordSet* self)
{
	return (DWORD)self->cur_node;
}

static BOOL mTextRecordSet_setCursor(mTextRecordSet* self, DWORD cursor)
{
	mTextRecordNode* cur;
	if(cursor == (DWORD)-1)
		return FALSE;

	cur = (mTextRecordNode*)cursor;
	if(self->cur_node != cur)
	{
		self->cur_node = cur;
		ncsRaiseDataSourceBindProps((mObject*)self, NCS_RSE_CURSOR_UPDATE);
	}

	return TRUE;

}

//static DWORD getValue(mTextRecordSet* self, mSymbol* symb)
static DWORD getValue(void* self_org, mSymbol* symb)
{
	mTextRecordSet* self = (mTextRecordSet *)self_org;
	if(symb->type == NCS_BT_IDX)
	{
		int field_idx;
		//if(symb->data.ival == '@')
		//	return self->cursor;

		field_idx = symb->data.ival;
		return _c(self)->getField(self, field_idx);
	}

	return 0;
}

static mTextExpRuntime _TextRecordSetRT={
	getValue
};

static BOOL  test_record_node(mTextRecordSet* self, mTextRecordNode* rn)
{
//	if(!textds_get_fields(self->source, rn, FALSE))
//		return FALSE;

	if(!rn || (rn->flags & NCS_TRNF_DELETED))
		return FALSE;

	if(self->tester)
		return ncsExecTestExpression(self->tester, &_TextRecordSetRT, (void*)self);

	return TRUE;
}

static mTextRecordNode* get_next_tested_node(mTextRecordSet* self, mTextRecordNode* rn)
{
	if(!rn)
		return NULL;
	rn = rn->next;
	while(rn && !test_record_node(self, rn)) rn = rn->next;
	return rn;
}

static mTextRecordNode* get_prev_tested_node(mTextRecordSet* self, mTextRecordNode* rn)
{
	if(!rn)
		return NULL;
	rn = rn->prev;
	while(rn && !test_record_node(self, rn)) rn = rn->prev;
	return rn;
}

static int mTextRecordSet_getRecordCount(mTextRecordSet* self)
{
	if(self->record_count == 0)
	{
		int count = 0;
		mTextRecordNode* node;
		textds_prepare_to(self->source, -1);
		node = self->source->head;
		do
		{
			node = get_next_tested_node(self,node);
			if(!node)
				break;
			count ++;
		}while(1);
	}
	return self->record_count;
}

static DWORD mTextRecordSet_seekCursor(mTextRecordSet* self, int seek, int offset)
{
	mTextRecordNode* cur ;
	//int cursor ;
	if(!self || !self->source)
		return -1;

	switch(seek)
	{
	case NCS_RS_CURSOR_BEGIN:
		//cursor = 0;
		cur = self->source->head;
		if(!test_record_node(self, cur))
			cur  = get_next_tested_node(self, cur);
		if(offset <= 0)
			goto RETURN;
		break;
	case NCS_RS_CURSOR_END:
		//cursor = _c(self)->getRecordCount(self);
		cur = self->source->tail;
		if(!test_record_node(self, cur))
			cur  = get_prev_tested_node(self, cur);
		//cursor --;
		if(offset >= 0)
			goto RETURN;
		break;
	default:
		//cursor = self->cursor;
		cur = self->cur_node;
		break;
	}


	if(offset < 0)
	{
		while(cur && offset < 0)
		{
			mTextRecordNode*  node = get_prev_tested_node(self, cur);
			cur = node;
			if(!node)
				break;
			offset ++;
			//cursor --;
		}
	}
	else
	{
		while(cur && offset > 0)
		{
			mTextRecordNode*  node = get_next_tested_node(self, cur);
			cur = node;
			if(!node)
				break;
			offset --;
			//cursor ++;
		}
	}

RETURN:

	//if(self->cursor != cursor)
	if(self->cur_node != cur)
	{
		//self->cursor = cursor;
		self->cur_node = cur;
		ncsRaiseDataSourceBindProps((mObject*)self, NCS_RSE_CURSOR_UPDATE);
	}

//	return self->cursor;
	return (DWORD)self->cur_node;
}

static int _map_recordset_field_idx(mTextRecordSet *self, int idx)
{
	if(idx <= 0 || idx > _c(self)->getFieldCount(self))
		return -1;

	if(self->field_list)
		return self->field_list[idx-1]-1;
	return idx -1;
}

static DWORD mTextRecordSet_getField(mTextRecordSet* self, int idx)
{
	idx = _map_recordset_field_idx(self, idx);
	if(idx < 0 || self->cur_node == NULL)
		return 0;

	//textds_get_fields(self->source, self->cur_node, FALSE);
	return (DWORD)self->cur_node->fields[idx];

}

static BOOL mTextRecordSet_setField(mTextRecordSet* self, int idx, DWORD value)
{
	mTextRecordNode* cur_node;
	if(!(self->flags & NCS_DS_SELECT_WRITE) || value == 0)
		return FALSE;


	cur_node = self->cur_node;
	if(!cur_node)
		return FALSE;

	idx = _map_recordset_field_idx(self, idx);
	if(idx < 0)
		return FALSE;

	//textds_get_fields(self->source, cur_node, FALSE);
	if(cur_node->fields[idx])
	{
		free(cur_node->fields[idx]);
	}
	cur_node->flags |= NCS_TRNF_CHANGED;

	cur_node->fields[idx] = strdup((const char*)value);
	//TODO upate the node
	textds_update_recordsets(self->source, cur_node);
	textds_update_bindprops(self->source, cur_node);
	self->flags |= NCS_TRNF_CHANGED;
	return TRUE;
}

BOOL mTextRecordSet_newRecord(mTextRecordSet* self, int insert_type)
{
	mTextRecordNode * rn;

	if(!self || _c(self)->getFieldCount(self) <= 0)
		return FALSE;

	if(!(self->flags & NCS_DS_SELECT_WRITE) )
		return FALSE;


	switch(insert_type)
	{
	case NCS_RS_INSERT_BEFORE:
		if(self->cur_node == NULL) //seek end
			_c(self)->seekCursor(self, NCS_RS_CURSOR_END, 0);
		rn = _SLAB_ALLOC( sizeof(mTextRecordNode));
		rn->pos = -1;
		if(self->cur_node)
		{
			rn->prev = self->cur_node->prev;
			if(rn->prev)
				rn->prev->next = rn;
			rn->next = self->cur_node;
			self->cur_node->prev = rn;
		}
		if(self->source->head == NULL || self->cur_node == self->source->head)
			self->source->head = rn;
		self->cur_node = rn;
		break;
	case NCS_RS_INSERT_APPEND:
		_c(self)->seekCursor(self, NCS_RS_CURSOR_END, 0);
	case NCS_RS_INSERT_AFTER:
		rn = _SLAB_ALLOC( sizeof(mTextRecordNode));
		rn->pos = -1;
		if(self->cur_node == NULL)
			_c(self)->seekCursor(self, NCS_RS_CURSOR_END, 0);
		if(self->cur_node)
		{
			rn->prev = self->cur_node;
			rn->next = self->cur_node->next;
			self->cur_node->next = rn;
		}
		if(rn->next)
			rn->next->prev = rn;
		if(self->source->tail == NULL || self->source->tail == self->cur_node)
			self->source->tail = rn;
		self->cur_node = rn;
		//self->cursor ++;
		break;
	default:
		return FALSE;
	}

	rn->flags |= NCS_TRNF_NEW;
	if(rn->fields == NULL)
		rn->fields = (char**)calloc(sizeof(char*), self->source->field_count);

	self->record_count ++;

	self->flags |= NCS_TRNF_CHANGED;

	return TRUE;
}

static BOOL mTextRecordSet_deleteRecord(mTextRecordSet* self)
{
	mTextRecordNode* cur_node;

	if(!(self->flags & NCS_DS_SELECT_WRITE) )
		return FALSE;

	if(!self || !self->cur_node)
		return FALSE;

	cur_node = self->cur_node;
	cur_node->flags |= NCS_TRNF_DELETED;

	_c(self)->seekCursor(self, NCS_RS_CURSOR_CUR, 1);

	if(_c(self)->isEnd(self))
		_c(self)->seekCursor(self, NCS_RS_CURSOR_END, 0);

	textds_update_del_record(self->source, cur_node);


	self->record_count --;
	self->flags |= NCS_TRNF_CHANGED;

	return TRUE;
}

static BOOL mTextRecordSet_update(mTextRecordSet* self)
{
	if(!(self->flags & NCS_DS_SELECT_WRITE) )
		return FALSE;

	if(!(self->flags & NCS_TRNF_CHANGED))
		return TRUE;

	if(textds_update_fields(self->source))
	{
		self->flags &= ~NCS_TRNF_CHANGED;
		return TRUE;
	}
	return FALSE;
}

static BOOL mTextRecordSet_cancel(mTextRecordSet* self)
{
	mTextRecordNode* node;
	if(!(self->flags & NCS_DS_SELECT_WRITE) )
		return FALSE;

	if(!(self->flags&NCS_TRNF_CHANGED))
		return TRUE;

	node = self->source->head;

	if(!test_record_node(self, node))
		node = get_next_tested_node(self, node);

	while(node)
	{
		if(node->flags&NCS_TRNF_NEW){
			mTextRecordNode* tn = node;
			//remove
			if(self->source->head == node)
				self->source->head = node->next;
			else{
				if(node->prev)
					node->prev->next = node->next;
				if(node->next)
					node->next->prev = node->prev;
			}
			node = get_next_tested_node(self, node);
			textds_revert_new(self->source, tn);
			continue;
		}
		else if (node->flags & NCS_TRNF_DELETED)
		{
			node->flags &= ~NCS_TRNF_DELETED;
		}
		node = get_next_tested_node(self, node);
	}

	return TRUE;
}

static BOOL mTextRecordSet_isEnd(mTextRecordSet* self)
{
	return self->cur_node == NULL;
}

static mBindProp* mTextRecordSet_getFieldBind(mTextRecordSet* self, int field_idx, int event_filter, int bp_type)
{
	return DATA_FIELD_PROP(self, field_idx, bp_type, event_filter);
}

BEGIN_MINI_CLASS(mTextRecordSet, mRecordSet)
	CLASS_METHOD_MAP(mTextRecordSet, construct)
	CLASS_METHOD_MAP(mTextRecordSet, destroy)
	CLASS_METHOD_MAP(mTextRecordSet, getCursor)
	CLASS_METHOD_MAP(mTextRecordSet, setCursor)
	CLASS_METHOD_MAP(mTextRecordSet, seekCursor)
	CLASS_METHOD_MAP(mTextRecordSet, getFieldCount)
	CLASS_METHOD_MAP(mTextRecordSet, getFieldType)
	CLASS_METHOD_MAP(mTextRecordSet, getRecordCount)
	CLASS_METHOD_MAP(mTextRecordSet, getField)
	CLASS_METHOD_MAP(mTextRecordSet, getFieldBind)
	CLASS_METHOD_MAP(mTextRecordSet, setField)
	CLASS_METHOD_MAP(mTextRecordSet, isEnd)
	CLASS_METHOD_MAP(mTextRecordSet, newRecord)
	CLASS_METHOD_MAP(mTextRecordSet, deleteRecord)
	CLASS_METHOD_MAP(mTextRecordSet, update)
	CLASS_METHOD_MAP(mTextRecordSet, cancel)
END_MINI_CLASS

////////////////////////////////////////////////
//mTextDataBindProp

static void mTextDataBindProp_construct(mTextDataBindProp* self, DWORD param)
{
	text_data_value_bind_prop_create_info_t *pinfo =
		(text_data_value_bind_prop_create_info_t*)param;
	Class(mDataBindProp).construct((mDataBindProp*)self, pinfo->type);

	if(pinfo)
	{
		self->source = (mTextDataSource *)(pinfo->source);
		self->node = pinfo->node;
		self->revert = pinfo->idx;
	}
}

static DWORD mTextDataBindProp_get(mTextDataBindProp* self)
{
	return (DWORD)(self->node?self->node->fields[self->revert]:0);
}

static BOOL mTextDataBindProp_set(mTextDataBindProp* self, DWORD value)
{
	if(value == 0)
		return FALSE;

	if(self->node)
	{
		if(self->node->fields[self->revert])
			free(self->node->fields[self->revert]);
		self->node->fields[self->revert] = strdup((char*)value);
		textds_update_recordsets(self->source,self->node);
		return TRUE;
	}
	return FALSE;
}

BEGIN_MINI_CLASS(mTextDataBindProp, mDataBindProp)
	CLASS_METHOD_MAP(mTextDataBindProp, construct)
	CLASS_METHOD_MAP(mTextDataBindProp, set)
	CLASS_METHOD_MAP(mTextDataBindProp, get)
END_MINI_CLASS

/////////////////////////////////////////////////////////////////////////////

mDataSource* ncsCreateTextDataSource(const char* source_file, int spliter, int mode)
{
	text_ds_create_info_t cinfo;
	if(source_file == NULL)
		return NULL;

	cinfo.file_name = source_file;
	cinfo.fs = spliter;
	cinfo.flags = mode;

	return (mDataSource*)NEWEX(mTextDataSource, (DWORD)&cinfo);
}

//////////////////////

void ncsInitTextDataSource(void)
{
	MGNCS_INIT_CLASS(mTextDataSource);
	MGNCS_INIT_CLASS(mTextRecordSet);
	MGNCS_INIT_CLASS(mTextDataBindProp);
}

#endif //_MGNCSDB_TEXT
//#endif


