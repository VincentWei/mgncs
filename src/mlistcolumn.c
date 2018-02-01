/* 
    The implementation of mListColumn class.

    This file is part of mGNCS, a component for MiniGUI.

    Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.
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
