

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSDB_SQLITE

#include <sqlite3.h>
#include "mdatabinding.h"
#include "mdatasource.h"
#include "comm_parser.h"
#include "msqlds.h"

#include "mem-slab.h"


typedef struct _sqlite_ds_create_info {
	const char* file_name;
	DWORD flags;
}sqlite_ds_create_info_t;

typedef struct _sqlite_rs_create_info {
	mSQLiteDataSource *source;
	sqlite3_stmt *stmt;
	int flags;
}sqlite_rs_create_info_t;

static void mSQLiteDataSource_construct(mSQLiteDataSource* self, DWORD param)
{
	sqlite_ds_create_info_t* cinfo = (sqlite_ds_create_info_t*)param;

	Class(mDataSource).construct((mDataSource*)self, 0);

	if(cinfo && cinfo->flags != 0)
	{
		int ret = sqlite3_open_v2(cinfo->file_name,
			&(self->db), cinfo->flags, NULL);

		if(ret == SQLITE_OK)
		{
			self->fileName = strdup(cinfo->file_name);
		}
		else
			fprintf(stderr, "sql open err: %d\n", ret);

		self->flags = cinfo->flags;
	}
}

static void mSQLiteDataSource_destroy(mSQLiteDataSource* self)
{
	while(self->sets)
	{
		mSQLiteRecordSet* s = self->sets;
		self->sets = s->next;

		_c(s)->release(s);
	}

	if(self->db)
	{
		int ret = sqlite3_close(self->db);

		if (ret != SQLITE_OK)
			fprintf(stderr, "sql close err: %d\n", ret);
	}

	if(self->fileName)
		free(self->fileName);

	Class(mDataSource).destroy((mDataSource*)self);
}

static mSQLiteRecordSet * mSQLiteDataSource_selectRecordSet(mSQLiteDataSource* self, const char* select, int select_type)
{
	sqlite_rs_create_info_t cinfo;
	mSQLiteRecordSet* rs ;
	sqlite3_stmt* stmt;

	sqlite3_prepare_v2(self->db, select, strlen(select), &stmt, 0);

	cinfo.source = self;
	cinfo.stmt  = stmt;
	cinfo.flags = select_type;

	rs = NEWEX(mSQLiteRecordSet, (DWORD)&cinfo);

	if(rs)
	{
		rs->next = self->sets;
		self->sets = rs;
	}

	return rs;
}

static void mSQLiteDataSource_execute(mSQLiteDataSource *self, const char *mql)
{
	int ret;

	ret = sqlite3_exec(self->db, mql, 0, 0, 0);

	if (ret != SQLITE_OK)
		fprintf(stderr, "sql exec err: %d\n", ret);
}

static DWORD mSQLiteDataSource_getValue(mSQLiteDataSource *self, const char *sql, BOOL *bok)
{
	int ret;

	char *str = NULL;

	sqlite3_stmt* stmt;

	ret = sqlite3_prepare_v2(self->db, sql, strlen(sql), &stmt, 0);

	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "sql prepare err: %d\n", ret);
		return 0;
	}

	ret = sqlite3_step(stmt);

	if (ret != SQLITE_ROW)
		goto out;

	str = strdup((char *)sqlite3_column_text(stmt, 0));

out:
	ret = sqlite3_finalize(stmt);

	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "sql finalize err: %d\n", ret);
		return 0;
	}

	return (DWORD)str;
}

static BOOL mSQLiteDataSource_setValue(mSQLiteDataSource *self, const char *sql, DWORD value)
{
	int ret;

	ret = sqlite3_exec(self->db, sql, 0, 0, 0);

	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "sql exec err: %d\n", ret);
		return FALSE;
	}

	return TRUE;
}

BEGIN_MINI_CLASS(mSQLiteDataSource, mDataSource)
	CLASS_METHOD_MAP(mSQLiteDataSource, construct)
	CLASS_METHOD_MAP(mSQLiteDataSource, destroy)
	CLASS_METHOD_MAP(mSQLiteDataSource, selectRecordSet)
	CLASS_METHOD_MAP(mSQLiteDataSource, execute)
	CLASS_METHOD_MAP(mSQLiteDataSource, getValue)
	CLASS_METHOD_MAP(mSQLiteDataSource, setValue)
END_MINI_CLASS


static void mSQLiteRecordSet_construct(mSQLiteRecordSet* self, DWORD param)
{
	sqlite_rs_create_info_t *pinfo = (sqlite_rs_create_info_t*)param;
	Class(mRecordSet).construct((mRecordSet*)self, param);

	if(pinfo)
	{
		self->source = pinfo->source;
		self->stmt = pinfo->stmt;
		self->flags = pinfo->flags;
	}
	self->cur_cursor = 0;
	self->count = -1;
	self->is_end = FALSE;
}

static void mSQLiteRecordSet_destroy(mSQLiteRecordSet* self)
{
	int	ret = sqlite3_finalize(self->stmt);

	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "sql finalize err: %d\n", ret);
	}

	if(self->source)
	{
		mSQLiteRecordSet* set = self->source->sets;
		if(set == self)
		{
			self->source->sets = self->next;
		}
		else
		{
			if(set)
			{
				while(set->next && set->next != self) set = set->next;
				if(set->next)
				{
					set->next = self->next;
				}
			}
		}
	}
	Class(mRecordSet).destroy((mRecordSet*)self);
}

static int mSQLiteRecordSet_getFieldCount(mSQLiteRecordSet* self)
{
	int nCol = sqlite3_column_count(self->stmt);

	return nCol;
}

static int mSQLiteRecordSet_getFieldType(mSQLiteRecordSet* self, int field_idx)
{
	return NCS_BT_STR;
}

static DWORD mSQLiteRecordSet_getField(mSQLiteRecordSet* self, int idx)
{
	if (idx <= 0 || idx > _c(self)->getFieldCount(self))
		return -1;

	char *str;

	str = (char *)sqlite3_column_text(self->stmt, idx - 1);

	return (DWORD)str;
}

static DWORD mSQLiteRecordSet_getCursor(mSQLiteRecordSet* self)
{
	return (DWORD)self->cur_cursor;
}

static BOOL mSQLiteRecordSet_setCursor(mSQLiteRecordSet* self, DWORD index)
{
	int offset, cnt = 0;
	int ret;

	if (index < 0)
		return FALSE;

	if (index > self->cur_cursor)
	{
		offset = index - self->cur_cursor;

		for (; offset > 0; offset--)
		{
			ret = sqlite3_step(self->stmt);

			if (ret == SQLITE_ROW)
				cnt++;
			else if (ret == SQLITE_DONE)
			{
				self->count = self->cur_cursor + cnt + 1;

				self->cur_cursor = self->count - 1;
				self->is_end = TRUE;
				return TRUE;
			}
			else
			{
				fprintf(stderr, "sql err: %d\n", ret);
				return FALSE;
			}
		}
	}
	else if (index < self->cur_cursor)
	{
		sqlite3_reset(self->stmt);

		for(; index > 0; index--)
		{
			ret = sqlite3_step(self->stmt);

			if (ret == SQLITE_ROW)
				return TRUE;
			else
			{
				fprintf(stderr, "sql err: %d\n", ret);
				return FALSE;
			}
		}
	}

	if(index == 0)
	{
		sqlite3_reset(self->stmt);

		ret = sqlite3_step(self->stmt);

		if (ret == SQLITE_ROW)
			return TRUE;
		else
		{
			fprintf(stderr, "sql err: %d\n", ret);
			return FALSE;
		}
	}

	self->cur_cursor = index;

	return TRUE;
}

static int mSQLiteRecordSet_getRecordCount(mSQLiteRecordSet* self)
{
	int ret, cnt = 0;

	if (self->count == -1)
	{
		while(1)
		{
			ret = sqlite3_step(self->stmt);

			if (ret == SQLITE_ROW)
				cnt++;
			else if (ret == SQLITE_DONE)
			{
				int index = self->cur_cursor;

				self->count = self->cur_cursor + cnt + 1;
				self->cur_cursor = self->count - 1;

				_c(self)->setCursor(self, index);

				break;
			}
			else
			{
				fprintf(stderr, "sql err: %d\n", ret);
				return -1;
			}
		}
	}

	return self->count;
}

static DWORD mSQLiteRecordSet_seekCursor(mSQLiteRecordSet* self, int seek, int offset)
{
	if(!self || !self->source)
		return -1;

	switch(seek)
	{
	case NCS_RS_CURSOR_BEGIN:
		_c(self)->setCursor(self, offset);
		break;
	case NCS_RS_CURSOR_END:
	{
		int pos;

		if (self->count == -1)
			_c(self)->getRecordCount(self);

		pos = self->count + offset - 1;

		if (pos < 0)
			return -1;

		_c(self)->setCursor(self, pos);
	}
		break;
	case NCS_RS_CURSOR_CUR:
		_c(self)->setCursor(self, self->cur_cursor + offset);
		break;
	default:
		fprintf(stderr, "should get here\n");
		break;
	}

	return 0;
}

static BOOL mSQLiteRecordSet_isEnd(mSQLiteRecordSet* self)
{
	return self->is_end;
}

BEGIN_MINI_CLASS(mSQLiteRecordSet, mRecordSet)
	CLASS_METHOD_MAP(mSQLiteRecordSet, construct)
	CLASS_METHOD_MAP(mSQLiteRecordSet, destroy)
	CLASS_METHOD_MAP(mSQLiteRecordSet, getCursor)
	CLASS_METHOD_MAP(mSQLiteRecordSet, setCursor)
	CLASS_METHOD_MAP(mSQLiteRecordSet, seekCursor)
	CLASS_METHOD_MAP(mSQLiteRecordSet, getFieldCount)
	CLASS_METHOD_MAP(mSQLiteRecordSet, getFieldType)
	CLASS_METHOD_MAP(mSQLiteRecordSet, getField)
	CLASS_METHOD_MAP(mSQLiteRecordSet, getRecordCount)
	CLASS_METHOD_MAP(mSQLiteRecordSet, isEnd)
END_MINI_CLASS

/////////////////////////////////////////////////////////////////////////////
// mode:
//		MSQLITE_READONLY
//		MSQLITE_READWRITE
//		MSQLITE_CREATE
//
///

mDataSource* ncsCreateSQLiteDataSource(const char* source_file, int mode)
{
	sqlite_ds_create_info_t cinfo;
	if(source_file == NULL)
		return NULL;

	cinfo.file_name = source_file;
	cinfo.flags = mode;

	return (mDataSource*)NEWEX(mSQLiteDataSource, (DWORD)&cinfo);
}

//////////////////////

void ncsInitSQLiteDataSource(void)
{
	MGNCS_INIT_CLASS(mSQLiteDataSource);
	MGNCS_INIT_CLASS(mSQLiteRecordSet);
}

#endif
//#endif
