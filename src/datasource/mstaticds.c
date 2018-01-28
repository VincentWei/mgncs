
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSDB_STATIC

#include "mdatabinding.h"
#include "mdatasource.h"
#include "mstaticds.h"

#include "mem-slab.h"

#include "comm_parser.h"

static mStaticDataSource gStaticDS;
mDataSource * g_pStaticDS = NULL;

static void mStaticDataSource_construct(mStaticDataSource* self, DWORD param)
{
	Class(mDataSource).construct((mDataSource*)self, param);
	self->head = NULL;
}

static void mStaticDataSource_destroy(mStaticDataSource* self)
{
	mStaticDataNode* node = self->head;
	while(node)
	{
		mStaticDataNode * pt = node;
		node = node->next;
		_SLAB_FREE(pt);
	}

	self->head = NULL;
	Class(mDataSource).destroy((mDataSource*)self);
}
#if 0
static mStaticDataNode * find_child_node(mStaticDataNode* pnode, const char* name)
{
	mStaticDataNode *pcur;
	if(!pnode || !name || pnode->type != NCS_SDT_TREE)
		return NULL;
	for(pcur = ((mStaticDataTreeNode*)pnode)->children; pcur; pcur = pcur->next)
	{
		if(strcmp(pcur->name, name) == 0)
		{
			return pcur;
		}
	}

	return NULL;
}
#endif
//static int on_find_node_name(mStaticDataNode** ppcur, char* name)
static int on_find_node_name(void* ppcur_org, const char* name_org)
{
	//const char* str;
	//int len;
	mStaticDataNode* pcur;

	mStaticDataNode **ppcur = (mStaticDataNode **)ppcur_org;
	char* name = (char *)name_org;

	if(!ppcur || !*ppcur || !name || (*ppcur)->type != NCS_SDT_TREE)
		return 0;

	name = ncsStrStrip(name);

#if 0
	str = strchr(name,'[');

	if(str)
		len = str - name;
	else
		len = strlen(name);
#endif

	pcur = *ppcur;

	pcur = ((mStaticDataTreeNode*)pcur)->children;

	while(pcur)
	{
		if(strcmp(pcur->name, name) == 0)
		{
			*ppcur = pcur;
			return 1;
		}
		pcur = pcur->next;
	}

	*ppcur = NULL;

	return 0;
}

static mRecordSet * mStaticDataSource_selectRecordSet(mStaticDataSource* self, const char* select, int select_type)
{
	char szSelect[1024];
	char * selects[2]={NULL, NULL};
	int field_count = 0;
	unsigned char * field_list = NULL;
	mStaticDataNode* pnode;
	mStaticDataTreeNode head;
	mStaticRecordSet *rs;

	if(select == NULL || select_type != NCS_DS_SELECT_READ)
		return NULL;

	strcpy(szSelect, select);
	if(ncsStrSpliter(szSelect, selects, ':') <= 0)
	{
		selects[0] = szSelect;
		selects[1] = NULL;
	}

	//get node
	head.type = NCS_SDT_TREE;
	head.children = self->head;
	pnode = (mStaticDataNode *)(void *)&head;
	ncsStrSpliterCallback(selects[0], on_find_node_name, '/', &pnode);
	if(pnode== (mStaticDataNode *)(void *)&head || pnode == NULL)
		return NULL;

	//get field list
	field_list = ncsGetFieldList(&selects[1], &field_count);

	//create mStaticRecordSet
	rs = NEWEX(mStaticRecordSet,select_type);
	rs->node = pnode;
	rs->cursor = pnode->type==NCS_SDT_TREE?(DWORD)((mStaticDataTreeNode*)pnode)->children:0;
	rs->field_list =field_list;

	return (mRecordSet *)rs;
}

DWORD mStaticDataSource_getValue(mStaticDataSource* self, const char* mql, BOOL *pok)
{
	mStaticDataNode * pnode;
	mStaticDataTreeNode head;
	char szText[1024];
	if(pok)
		*pok = FALSE;

	if(mql == NULL)
		return 0;

	head.children = self->head;
	head.type = NCS_SDT_TREE;
	pnode =  (mStaticDataNode *)(void *)&head;

	strcpy(szText, mql);

	ncsStrSpliterCallback(szText, on_find_node_name, '/', &pnode);

	if(pnode == (mStaticDataNode *)(void *)&head || pnode ==NULL)
		return 0;


	if(pok)
		*pok = TRUE;

	switch((pnode)->type)
	{
	case NCS_SDT_TREE:
		return (DWORD)((pnode)->name);
	case NCS_SDT_VALUE:
		return (DWORD)(((mStaticDataValueNode*)(pnode))->value);
	case NCS_SDT_TABLE:
		{
			int row,col;
			get_col_rows(szText, &row, &col);
			return *get_table_value((mStaticDataTableNode*)(pnode),row, col);
		}
	default:
		if(pok)
			*pok = FALSE;
	}

	return 0;

}

static mBindProp* mStaticDataSource_getBindProp(mStaticDataSource* self, const char* ql, int bp_type)
{
	mStaticDataNode * pnode;
	mStaticDataTreeNode head;
	char szText[1024];

	if(bp_type != NCS_PROP_FLAG_READ)
		return NULL;

	if(ql == NULL)
		return 0;

	head.children = self->head;
	head.type = NCS_SDT_TREE;
	pnode =  (mStaticDataNode *)(void *)&head;

	strcpy(szText, ql);

	ncsStrSpliterCallback(szText, on_find_node_name, '/', &pnode);

	if(pnode == (mStaticDataNode *)(void *)&head || pnode ==NULL)
		return 0;


	switch((pnode)->type)
	{
	case NCS_SDT_VALUE:
	{
		mStaticDataValueNode* pvn = (mStaticDataValueNode*)pnode;
		return DATA_VAR_PROP(self, pvn->value,
				pvn->flags ,bp_type, NCS_RSE_VALUE_UPATED);
	}
	case NCS_SDT_TABLE:
		{
			int row,col;
			mStaticDataTableNode* ptn = (mStaticDataTableNode*)(pnode);
			get_col_rows(szText, &row, &col);
			return DATA_VAR_PROP(self, (*get_table_value(ptn,row, col)),
					ptn->field_types[col],bp_type, NCS_RSE_VALUE_UPATED);
		}
	}

	return NULL;
}



BEGIN_MINI_CLASS(mStaticDataSource, mDataSource)
	CLASS_METHOD_MAP(mStaticDataSource, construct)
	CLASS_METHOD_MAP(mStaticDataSource, destroy)
	CLASS_METHOD_MAP(mStaticDataSource, selectRecordSet)
	CLASS_METHOD_MAP(mStaticDataSource, getValue)
	CLASS_METHOD_MAP(mStaticDataSource, getBindProp)
END_MINI_CLASS

/////////////////////////////
static inline mStaticDataNode* create_tree_node(const char* name, mStaticDataNode* children)
{
	mStaticDataTreeNode* tn = (mStaticDataTreeNode*)calloc(sizeof(mStaticDataTreeNode),1);
	tn->name = strdup(name);
	tn->children = children;
	tn->type = NCS_SDT_TREE;
	return (mStaticDataNode *)tn;
}

static BOOL register_node(mStaticDataSource* self, const char* pvalue_path, mStaticDataNode* (*create_node)(void*,const char*), void* user)
{
	char szText[1024];
	char* str, *strHead;
	mStaticDataNode* node;
	mStaticDataNode* head = NULL, *parent;

	if(*pvalue_path != '/')
		return FALSE;

	strcpy(szText, pvalue_path);
	strHead = szText + 1;
	head = self->head;
	parent = NULL;

	while(strHead && head )
	{
		str = (char*)strchr(strHead, '/');
		while(head)
		{
			if((!str && strcmp(head->name, strHead) == 0) || (str && strncmp(head->name, strHead, str-strHead) == 0))
			{
				//find one
				parent = head;
				if(head->type != NCS_SDT_TREE)
				{
					return FALSE;
				}
				head = ((mStaticDataTreeNode*)head)->children;
				break;
			}
			head = head->next;
		}
		if(!head)
			break;

		if(str)
			strHead = str+1;
		else
			break;
	}

	if(strHead == NULL || *strHead == '\0')
		return FALSE;

	//create the table node
	str = (char*)strrchr(strHead, '/');
	if(!str)
		str = strHead;
	else{
		*str = 0;
		str ++;
	}

	node = create_node(user, str);

	while(str > strHead)
	{
		str = (char*) strrchr(strHead, '/');
		if(!str){
			str = strHead;
			if(!*str)
				break;
		}
		else{
			*str = 0;
			str ++;
		}
		node = create_tree_node(str, node);
	}

	if(parent == NULL)
	{
		node->next = self->head;
		self->head = node;
	}
	else
	{
		node->next = ((mStaticDataTreeNode*)parent)->children;
		((mStaticDataTreeNode*)parent)->children = node;
	}

	return TRUE;
}


struct _create_table_node_info {
	void ** array;
	int array_count;
	int field_count;
	int field_width;
	const unsigned char* field_types ;
};

//mStaticDataNode* _create_table_node(struct _create_table_node_info* pci, const char* name)
mStaticDataNode* _create_table_node(void* pci_org, const char* name)
{
	struct _create_table_node_info *pci = (struct _create_table_node_info *)pci_org;
	mStaticDataTableNode* tn = (mStaticDataTableNode*)calloc(sizeof(mStaticDataTableNode),1);

	tn->type = NCS_SDT_TABLE;
	tn->name = strdup(name);
	tn->field_count = pci->field_count;
	tn->field_width = pci->field_width;
	tn->record_count = pci->array_count;
	tn->field_types = pci->field_types;
	tn->table = pci->array;

	return (mStaticDataNode*)tn;
}

BOOL ncsRegisterStaticData(const char* pvalue_path,
	void ** array,
	int array_count,
	int field_count,
	int field_width,
	const unsigned char* field_types)
{
	struct _create_table_node_info ci;

	if(pvalue_path == NULL
		|| array == NULL
		|| array_count <= 0
		|| field_count <= 0
		|| field_width <= 1)
		return FALSE;

	ci.array = array;
	ci.array_count = array_count;
	ci.field_count = field_count;
	ci.field_width = field_width;
	ci.field_types = field_types;

	return register_node((mStaticDataSource *)g_pStaticDS, pvalue_path, _create_table_node, &ci);
}

struct _create_value_node_info {
	DWORD value;
	int   type;
};

//static mStaticDataNode* _create_value_node(struct _create_value_node_info * pci, const char* name)
static mStaticDataNode* _create_value_node(void * pci_org, const char* name)
{
	struct _create_value_node_info *pci = (struct _create_value_node_info *)pci_org;
	mStaticDataValueNode * vn = (mStaticDataValueNode*)calloc(sizeof(mStaticDataValueNode),1);

	vn->type = NCS_SDT_VALUE;
	vn->name = strdup(name);
	vn->flags = (unsigned short)pci->type;
	vn->value = pci->value;
	return (mStaticDataNode*)vn;
}

BOOL ncsRegisterStaticValue(const char* pvalue_path, DWORD value, int type)
{
	if(!pvalue_path)
		return FALSE;

	{
		struct _create_value_node_info ci = {
			value,
			type
		};

		return register_node((mStaticDataSource *)g_pStaticDS, pvalue_path, _create_value_node, &ci);
	}
}


//////////////////////////////////////////////////////////

static void mStaticRecordSet_construct(mStaticRecordSet* self, DWORD param)
{
	Class(mRecordSet).construct((mRecordSet*)self, param);
}

static DWORD mStaticRecordSet_getCursor(mStaticRecordSet* self)
{
	return (DWORD)self->cursor;
}

static BOOL mStaticRecordSet_setCursor(mStaticRecordSet* self, DWORD cursor)
{
	int cur = (int)cursor;
	if(cur < 0 || cur >= _c(self)->getRecordCount(self))
		return FALSE;

	if(cur != self->cursor){
		self->cursor = cur;
		ncsRaiseDataSourceBindProps((mObject*)self, NCS_RSE_CURSOR_UPDATE);
	}

	return TRUE;
}

static int mStaticRecordSet_getRecordCount(mStaticRecordSet *self)
{
	if(!self->node)
		return 0;

	if(self->node->type == NCS_SDT_TABLE)
	{
		mStaticDataTableNode * tn = (mStaticDataTableNode*)self->node;
		return tn->record_count;
	}
	else if(self->node->type == NCS_SDT_TREE)
	{
		mStaticDataNode* node = ((mStaticDataTreeNode*)(self->node))->children;
		int i;
		for(i = 0; node; i++, node=node->next);
		return i;
	}
	else
	{
		return 1;
	}
}

static int mStaticRecordSet_getFieldCount(mStaticRecordSet* self)
{
	if(!self->node)
		return 0;

	if(self->field_list)
	{
		return strlen((char *)self->field_list);
	}

	if(self->node->type == NCS_SDT_TABLE)
	{
		mStaticDataTableNode * tn = (mStaticDataTableNode*)self->node;
		return tn->field_count;
	}
	else
	{
		return 1;
	}
}

static DWORD mStaticRecordSet_seekCursor(mStaticRecordSet* self, int seek, int offset)
{
	DWORD cursor = 0;
	int count ;
	if(self->node == NULL)
		return (DWORD)-1;


	if(self->node->type == NCS_SDT_TREE)
	{
		mStaticDataNode* node = (mStaticDataNode*)(self->cursor);
		if(node == NULL)
		{
			node = ((mStaticDataTreeNode*)(self->node))->children;
		}

		if(!node)
			return 0;

		if(seek == NCS_RS_CURSOR_CUR && offset > 0)
		{
			for(;node && offset> 0 && node; node = node->next , offset --);
		}
		else
		{
			int i = 0;
			if(seek == NCS_RS_CURSOR_END)
				count = _c(self)->getRecordCount(self);
			else if(seek == NCS_RS_CURSOR_CUR)
			{
				mStaticDataNode* tempn = ((mStaticDataTreeNode*)(self->node))->children;
				for(count = 0; tempn && tempn != node; count ++, tempn = tempn->next);
			}
			else
			{
				count = offset;
			}
			for(node = ((mStaticDataTreeNode*)(self->node))->children;
				node && i < count; node = node->next, i++);
		}
		cursor = (DWORD)node;
	}
	else
	{
		count = _c(self)->getRecordCount(self);
		switch(seek)
		{
		case NCS_RS_CURSOR_BEGIN:
			cursor = offset;
			break;
		case NCS_RS_CURSOR_CUR:
			cursor = self->cursor + offset;
			break;
		case NCS_RS_CURSOR_END:
			cursor = count + offset;
		}

		if(cursor < 0)
			cursor = 0;
		else if(cursor >= count)
			cursor = count;

	}

	if( cursor != self->cursor)
	{
		//raise
		ncsRaiseDataSourceBindProps((mObject*)self, NCS_RSE_CURSOR_UPDATE);
	}

	self->cursor = cursor;

	return (DWORD)cursor;
}

static int _map_recordset_field_idx(mStaticRecordSet *self, int idx)
{
	if(idx <= 0 || idx > _c(self)->getFieldCount(self))
		return -1;

	if(self->field_list)
	{
		idx = self->field_list[idx-1]-1;
		if(self->node->type == NCS_SDT_TABLE)
		{
			mStaticDataTableNode * tn = (mStaticDataTableNode*)self->node;
			if(idx >= tn->field_count)
				return -1;
		}
		else
		{
			if(idx >= 1)
				return -1;
		}
		return idx;
	}
	return idx-1;
}

static DWORD mStaticRecordSet_getField(mStaticRecordSet * self, int idx)
{
	idx = _map_recordset_field_idx(self,idx);
	if(idx < 0)
		return 0;

	switch(self->node->type)
	{
	case NCS_SDT_TABLE:
		{
			mStaticDataTableNode* tn = (mStaticDataTableNode*)self->node;
			return *get_table_value(tn, (int)self->cursor, idx);
		}
	case NCS_SDT_VALUE:
		{
			mStaticDataValueNode* vn = (mStaticDataValueNode*)self->node;
			return vn->value;
		}
	case NCS_SDT_TREE:
		{
			mStaticDataNode* node = (mStaticDataNode*)self->cursor;
			if(!node)
				return 0;
			return (DWORD)(node->name);
		}
	}
	return 0;
}

static BOOL mStaticRecordSet_isEnd(mStaticRecordSet* self)
{
	if(!self->node)
		return TRUE;

	if(self->node->type == NCS_SDT_TREE)
		return self->cursor == 0;
	else
		return self->cursor >= _c(self)->getRecordCount(self);
}

static mBindProp * mStaticRecordSet_getFieldBind(mStaticRecordSet* self, int field_idx, int event_filter, int bp_type)
{
	return  DATA_FIELD_PROP(self, field_idx, bp_type, event_filter);
}

static int mStaticRecordSet_getFieldType(mStaticRecordSet* self, int field_idx)
{
	field_idx = _map_recordset_field_idx(self,field_idx);

	if(field_idx < 0)
		return -1;

	switch(self->node->type)
	{
	case NCS_SDT_TABLE:
		{
			mStaticDataTableNode* tn = (mStaticDataTableNode*)(self->node);
			return tn->field_types?tn->field_types[field_idx]:NCS_BT_STR;
		}
	case NCS_SDT_TREE:
		return NCS_BT_STR;
	case NCS_SDT_VALUE:
		return self->node->flags;
	}
	return -1;
}

BEGIN_MINI_CLASS(mStaticRecordSet, mRecordSet)
	CLASS_METHOD_MAP(mStaticRecordSet, construct)
	CLASS_METHOD_MAP(mStaticRecordSet, getCursor)
	CLASS_METHOD_MAP(mStaticRecordSet, setCursor)
	CLASS_METHOD_MAP(mStaticRecordSet, seekCursor)
	CLASS_METHOD_MAP(mStaticRecordSet, getFieldCount)
	CLASS_METHOD_MAP(mStaticRecordSet, getFieldType)
	CLASS_METHOD_MAP(mStaticRecordSet, getRecordCount)
	CLASS_METHOD_MAP(mStaticRecordSet, getField)
	CLASS_METHOD_MAP(mStaticRecordSet, getFieldBind)
	CLASS_METHOD_MAP(mStaticRecordSet, isEnd)
END_MINI_CLASS

/////////////////////////////////////
void ncsInitStaticDataSource()
{
	MGNCS_INIT_CLASS(mStaticDataSource);
	MGNCS_INIT_CLASS(mStaticRecordSet);
	gStaticDS._class = &Class(mStaticDataSource);
	Class(mStaticDataSource).construct(&gStaticDS,0);
	g_pStaticDS = (mDataSource*)(void *)(&gStaticDS);
}

#endif //_MGNCSDB_STATIC
//#endif

