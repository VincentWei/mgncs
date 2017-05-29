#ifndef MGNCS_XML_DATASOURCE_H
#define MGNCS_XML_DATASOURCE_H

typedef struct _mXMLDataSourceClass mXMLDataSourceClass;
typedef struct _mXMLDataSource mXMLDataSource;

typedef struct _mXMLRecordSetClass mXMLRecordSetClass;
typedef struct _mXMLRecordSet mXMLRecordSet;

#define mXMLDataSourceClassHeader(clss, superCls) \
	mDataSourceClassHeader(clss, superCls)

struct _mXMLDataSourceClass{
	mXMLDataSourceClassHeader(mXMLDataSource, mDataSource)
};

extern mXMLDataSourceClass g_mXMLDataSourceCls;

#define mXMLDataSourceHeader(clss) \
	mDataSourceHeader(clss) \
    xmlDocPtr pxmldoc; \
	char *fileName; \
	unsigned int flags;

struct _mXMLDataSource
{
	mXMLDataSourceHeader(mXMLDataSource)
};

/////////////////////////////////////////////////
//
#define mXMLRecordSetClassHeader(clss, superCls) \
	mRecordSetClassHeader(clss, superCls) 

struct _mXMLRecordSetClass
{
	mXMLRecordSetClassHeader(mXMLRecordSet, mRecordSet)
};

extern mXMLRecordSetClass g_stmXMLRecordSetCls;

#define mXMLRecordSetHeader(clss) \
	mRecordSetHeader(clss) \
    xmlXPathObjectPtr pxpathobj; \
	mXMLDataSource* source; \
    int field_count; \
	int flags; \
    XFIELD* fields; \
	int cur_cursor;	\
	BOOL is_end;

struct _mXMLRecordSet
{
	mXMLRecordSetHeader(mXMLRecordSet)
};

#endif

