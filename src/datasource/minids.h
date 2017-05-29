
#ifndef MGNCS_INI_DATASOURCE_H
#define MGNCS_INI_DATASOURCE_H

typedef struct _mIniDataSourceClass mIniDataSourceClass;
typedef struct _mIniDataSource mIniDataSource;

typedef struct _mIniRecordSetClass mIniRecordSetClass;
typedef struct _mIniRecordSet mIniRecordSet;

typedef struct _mIniSectionRecordSetClass mIniSectionRecordSetClass;
typedef struct _mIniSectionRecordSet mIniSectionRecordSet;

typedef struct _mIniArrRecordSetClass mIniArrRecordSetClass;
typedef struct _mIniArrRecordSet mIniArrRecordSet;

typedef struct _mIniValueBindPropClass mIniValueBindPropClass;
typedef struct _mIniValueBindProp mIniValueBindProp;

/////////////////////////////////////////////////////////

#define mIniDataSourceClassHeader(clss, superCls) \
	mDataSourceClassHeader(clss, superCls) 

struct _mIniDataSourceClass{
	mIniDataSourceClassHeader(mIniDataSource, mDataSource)
};

extern mIniDataSourceClass g_mIniDataSourceCls;

#define NCS_IDSF_HANDLE 0x10

#define mIniDataSourceHeader(clss) \
	mDataSourceHeader(clss) \
	DWORD flags; \
	FILE *fp; /*EtcFileHandler*/\
	ETC_S * etc; \
	mIniRecordSet* sets; \
	mBindProp * props; 

struct _mIniDataSource{
	mIniDataSourceHeader(mIniDataSource)
};


//////////////////////////////////////////

#define mIniRecordSetClassHeader(clss, superCls) \
	mRecordSetClassHeader(clss, superCls)  \
	BOOL (*isCurKey)(clss*, int key);

struct _mIniRecordSetClass{
	mIniRecordSetClassHeader(mIniRecordSet, mRecordSet)
};

#define IRSF_VALUE_CHANGED 0x10
#define IRSF_KEY_CHANGED   0x20
#define IRSF_KEY_DELETED   0x40
#define IRSF_KEY_CREATED   0x80

#define mIniRecordSetHeader(clss) \
	mRecordSetHeader(clss) \
	unsigned int flags:16; \
	unsigned int sec_idx:16; \
	mIniDataSource * source; \
	mIniRecordSet* next;

struct _mIniRecordSet{
	mIniRecordSetHeader(mIniRecordSet)
};

///////////////////////////////
#define mIniSectionRecordSetClassHeader(clss, superCls) \
	mIniRecordSetClassHeader(clss, superCls)

struct _mIniSectionRecordSetClass{
	mIniSectionRecordSetClassHeader(mIniSectionRecordSet, mIniRecordSet)
};

extern mIniSectionRecordSetClass g_stmIniSectionRecordSetCls;

#define NCS_ISRS_KEY_FIELD 0x100
#define NCS_ISRS_VALUE_FIELD 0x200
#define ISRS_FIELD_COUNT(self)  \
	(((((self)->flags&(NCS_ISRS_KEY_FIELD|NCS_ISRS_VALUE_FIELD))>>8)+1)/2)
#define mIniSectionRecordSetHeader(clss) \
	mIniRecordSetHeader(clss) \
	int cursor; 

struct _mIniSectionRecordSet{
	mIniSectionRecordSetHeader(mIniSectionRecordSet)
};

/////////////////////////
#define mIniArrRecordSetClassHeader(clss, superCls) \
	mIniRecordSetClassHeader(clss, superCls)

struct _mIniArrRecordSetClass{
	mIniArrRecordSetClassHeader(mIniArrRecordSet, mIniRecordSet)
};

extern mIniArrRecordSetClass g_stmIniArrRecordSetCls;

#define mIniArrRecordSetHeader(clss) \
	mIniRecordSetHeader(clss) \
	char* count_key; \
	int   cursor; \
	const char* cursor_key; \
	char** field_keys; \
	unsigned int count:16; \
	unsigned int field_count:16;

struct _mIniArrRecordSet{
	mIniArrRecordSetHeader(mIniArrRecordSet)
};

///////////////
#define mIniValueBindPropClassHeader(clss, superCls) \
	mBindPropClassHeader(clss, superCls) 

struct _mIniValueBindPropClass{
	mIniValueBindPropClassHeader(mIniValueBindProp, mBindProp)
};

extern mIniValueBindPropClass g_stmIniValueBindPropCls;

#define mIniValueBindPropHeader(clss) \
	mBindPropHeader(clss) \
	mIniDataSource *source; \
	unsigned int sec_idx:16; \
	unsigned int key_idx:16;

struct _mIniValueBindProp {
	mIniValueBindPropHeader(mIniValueBindProp)
};

#endif

