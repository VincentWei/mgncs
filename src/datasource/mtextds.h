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
#ifdef _MGNCSDB_TEXT

#ifndef MGNCS_TEXT_DATASOURCE_H
#define MGNCS_TEXT_DATASOURCE_H

typedef struct _mTextDataSourceClass mTextDataSourceClass;
typedef struct _mTextDataSource mTextDataSource;

typedef struct _mTextRecordSetClass mTextRecordSetClass;
typedef struct _mTextRecordSet mTextRecordSet;

typedef struct _mTextDataBindPropClass mTextDataBindPropClass;
typedef struct _mTextDataBindProp mTextDataBindProp;


#define mTextDataSourceClassHeader(clss, superCls) \
	mDataSourceClassHeader(clss, superCls)

struct _mTextDataSourceClass{
	mTextDataSourceClassHeader(mTextDataSource, mDataSource)
};


extern mTextDataSourceClass g_mTextDataSourceCls;

#define NCS_TRNF_CHANGED 0x1
#define NCS_TRNF_DELETED 0x2
#define NCS_TRNF_NEW     0x4
typedef struct _mTextRecordNode {
	DWORD pos; //the pos of line
	DWORD flags; // write flags
	char  ** fields;
	struct _mTextRecordNode * next, *prev;
}mTextRecordNode;


#define mTextDataSourceHeader(clss) \
	mDataSourceHeader(clss) \
	FILE *fp; \
	char* fileName;        \
	unsigned int  flags:8; \
	unsigned int  field_count:8; \
	unsigned int  record_count:16; \
	mTextRecordNode * head; \
	mTextRecordNode * tail; \
	mTextRecordSet * sets; \
	mTextDataBindProp * props; \
	int fs;


struct _mTextDataSource
{
	mTextDataSourceHeader(mTextDataSource)
};

/////////////////////////////////////////////////
//
#define mTextRecordSetClassHeader(clss, superCls) \
	mRecordSetClassHeader(clss, superCls)

struct _mTextRecordSetClass
{
	mTextRecordSetClassHeader(mTextRecordSet, mRecordSet)
};

extern mTextRecordSetClass g_stmTextRecordSetCls;

#define mTextRecordSetHeader(clss) \
	mRecordSetHeader(clss) \
	mTextDataSource* source; \
	unsigned char* field_list; \
	mTextRecordNode* cur_node; \
	/*int             cursor;*/ \
	mTextRecordSet* next; \
	unsigned int flags:16; \
	unsigned int record_count:16; \
	mSymbol ** tester;

struct _mTextRecordSet
{
	mTextRecordSetHeader(mTextRecordSet)
};

///////////////////////////////////////////////////

#define mTextDataBindPropClassHeader(clss, superCls) \
	mDataBindPropClassHeader(clss, superCls)

struct _mTextDataBindPropClass{
	mTextDataBindPropClassHeader(mTextDataBindProp, mDataBindProp)
};

extern mTextDataBindPropClass g_stmTextDataBindPropCls;

#define mTextDataBindPropHeader(clss) \
	mDataBindPropHeader(clss) \
	mTextRecordNode * node;  \
	mTextDataSource * source;

struct _mTextDataBindProp {
	mTextDataBindPropHeader(mTextDataBindProp)
};


#endif
#endif //_MGNCSDB_TEXT
