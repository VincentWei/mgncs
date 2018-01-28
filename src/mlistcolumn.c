/* 
 ** $Id: mlistcolumn.c 641 2009-11-10 01:48:49Z xwyan $
 **
 ** The implementation of mListColumn class.
 **
 ** Copyright (C) 2009 Feynman Software.
 **
 ** All rights reserved by Feynman Software.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

static void mListColumn_construct(mListColumn *self, DWORD addData)
{
	Class(mItem).construct((mItem*)self, addData);

    self->startX = 0;
    self->width = 62;
    self->sort = NCSID_LSTCLM_NOTSORTED;
    self->pfnCmp = NULL;
}

static BOOL mListColumn_setProperty(mListColumn* self, int id, DWORD value)
{
	if( id >= NCSP_LSTCLM_MAX)
		return FALSE;

	switch(id)
	{
        case NCSP_LSTCLM_POSX:
            self->startX = value;
            return TRUE;

        case NCSP_LSTCLM_WIDTH:
            self->width = value;
            return TRUE;

        case NCSP_LSTCLM_SORTTYPE:
            if (value >= NCSID_LSTCLM_NOTSORTED && value <= NCSID_LSTCLM_LOSORTED) {
                self->sort = value;
                return TRUE;
            }
            else 
                return FALSE;

        case NCSP_LSTCLM_CMPFUNC:
            self->pfnCmp = (NCS_CB_LISTV_CMPCLM)value;
            return TRUE;
    }

	return Class(mItem).setProperty((mItem*)self, id, value);
}

static DWORD mListColumn_getProperty(mListColumn* self, int id)
{
	if( id >= NCSP_LSTCLM_MAX)
		return -1;

	switch(id)
	{
        case NCSP_LSTCLM_POSX:
            return self->startX;

        case NCSP_LSTCLM_WIDTH:
            return self->width;

        case NCSP_LSTCLM_SORTTYPE:
            return self->sort;

        case NCSP_LSTCLM_CMPFUNC:
            return (DWORD) self->pfnCmp;
    }

	return Class(mItem).getProperty((mItem*)self, id);
}

BEGIN_MINI_CLASS(mListColumn, mItem)
	CLASS_METHOD_MAP(mListColumn, construct)
	CLASS_METHOD_MAP(mListColumn, setProperty)
	CLASS_METHOD_MAP(mListColumn, getProperty)
END_MINI_CLASS
