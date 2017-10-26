#ifdef _MGNCSDB_STATIC

#ifndef MGNCS_STATIC_DATASOURCE_H
#define MGNCS_STATIC_DATASOURCE_H

typedef struct _mStaticDataSourceClass mStaticDataSourceClass;
typedef struct _mStaticDataSource mStaticDataSource;

#define mStaticDataSourceClassHeader(clss, superCls) \
	mDataSourceClassHeader(clss, superCls)

struct _mStaticDataSourceClass {
	mStaticDataSourceClassHeader(mStaticDataSource, mDataSource)
};

extern mStaticDataSourceClass g_stmStaticDataSourceCls;

#define mStaticDataNodeHeader \
	unsigned int type:16; \
	unsigned int flags:16; \
	char * name; \
	struct _mStaticDataNode* next;

typedef struct _mStaticDataNode {
	mStaticDataNodeHeader
}mStaticDataNode;


//define the tree node
typedef struct _mStaticDataTreeNode {
	mStaticDataNodeHeader
	mStaticDataNode *children;
}mStaticDataTreeNode;

//define the value node
typedef struct _mStaticDataValueNode{
	mStaticDataNodeHeader
	DWORD value;
}mStaticDataValueNode;

//define the table node
typedef struct _mStaticDataTableNode{
	mStaticDataNodeHeader
	unsigned int field_count:8;
	unsigned int field_width:8;
	unsigned int record_count:16;
	const unsigned char *field_types;
	void ** table;
}mStaticDataTableNode;

static inline DWORD*  get_table_value(mStaticDataTableNode* t, int row, int col)
{
	unsigned char *table;
	if(row < 0)
		row = 0;
	else if(row > t->record_count)
		row = t->record_count - 1;

	if(col < 0)
		col = 0;
	else if(col >= t->field_count)
		col = t->field_count - 1;

	table = (unsigned char*)(t->table);
	table = (table + t->field_count * t->field_width * row);

	return ((DWORD*)(table+col*t->field_width));

}

#define NCS_SDT_TREE  1
#define NCS_SDT_VALUE 2
#define NCS_SDT_TABLE 3

#define mStaticDataSourceHeader(clss) \
	mDataSourceHeader(clss) \
	mStaticDataNode *head;

struct _mStaticDataSource {
	mStaticDataSourceHeader(mStaticDataSource)
};


typedef struct _mStaticRecordSetClass mStaticRecordSetClass;
typedef struct _mStaticRecordSet mStaticRecordSet;

#define mStaticRecordSetClassHeader(clss, superCls) \
	mRecordSetClassHeader(clss, superCls)

struct _mStaticRecordSetClass {
	mStaticRecordSetClassHeader(mStaticRecordSet, mRecordSet)
};

extern mStaticRecordSetClass g_stmStaticRecordSetCls;

#define mStaticRecordSetHeader(clss) \
	mRecordSetHeader(clss) \
	DWORD cursor; \
	mStaticDataNode* node; \
	unsigned char* field_list;

struct _mStaticRecordSet{
	mStaticRecordSetHeader(mStaticRecordSet)
};


/////////////////////////////////////////

void ncsInitStaticDataSource(void);

#endif
#endif //_MGNCSDB_STATIC
