#ifdef _MGNCSDB_SQLITE

#ifndef MGNCS_SQLITE_DATASOURCE_H
#define MGNCS_SQLITE_DATASOURCE_H

typedef struct _mSQLiteDataSourceClass mSQLiteDataSourceClass;
typedef struct _mSQLiteDataSource mSQLiteDataSource;

typedef struct _mSQLiteRecordSetClass mSQLiteRecordSetClass;
typedef struct _mSQLiteRecordSet mSQLiteRecordSet;

#define mSQLiteDataSourceClassHeader(clss, superCls) \
	mDataSourceClassHeader(clss, superCls)

struct _mSQLiteDataSourceClass{
	mSQLiteDataSourceClassHeader(mSQLiteDataSource, mDataSource)
};

extern mSQLiteDataSourceClass g_mSQLiteDataSourceCls;

#define mSQLiteDataSourceHeader(clss) \
	mDataSourceHeader(clss) \
	sqlite3 *db; \
	char *fileName; \
	unsigned int flags;	\
	mSQLiteRecordSet * sets;

struct _mSQLiteDataSource
{
	mSQLiteDataSourceHeader(mSQLiteDataSource)
};

/////////////////////////////////////////////////
//
#define mSQLiteRecordSetClassHeader(clss, superCls) \
	mRecordSetClassHeader(clss, superCls)

struct _mSQLiteRecordSetClass
{
	mSQLiteRecordSetClassHeader(mSQLiteRecordSet, mRecordSet)
};

extern mSQLiteRecordSetClass g_stmSQLiteRecordSetCls;

#define mSQLiteRecordSetHeader(clss) \
	mRecordSetHeader(clss) \
	mSQLiteDataSource* source; \
	int flags; \
	sqlite3_stmt *stmt;	\
	int count;	\
	int cur_cursor;	\
	mSQLiteRecordSet* next; \
	BOOL is_end;

struct _mSQLiteRecordSet
{
	mSQLiteRecordSetHeader(mSQLiteRecordSet)
};

#endif
#endif // _MGNCSDB_SQLITE
