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
/*
 *   This file is part of mGNCS, a component for MiniGUI.
 * 
 *   Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "mhotpiece.h"
#include "mstaticpiece.h"
#include "mrenderablepiece.h"
#include "mgridboxpiece.h"
#include "mdaygridpiece.h"

static BOOL mDayGridPiece_setProperty(mDayGridPiece* self, int id, int value)
{
	switch(id)
	{
	case NCSP_DAYGRIDPIECE_MONTH_DAYS:
		if(value <= 0 || value > 31)
			return FALSE;

		self->month_days = value&0xFF;
		return TRUE;
	case NCSP_DAYGRIDPIECE_LASTMONTH_DAYS:
		if(value <= 0 || value > 31)
			return FALSE;

		self->last_month_days = value&0xFF;
		return TRUE;
	case NCSP_DAYGRIDPIECE_WEEKDAY_OF_FIRSTDAY:
		if(value < Sunday || value > Saturday)
			return FALSE;
		self->weekday_of_first_day = value & 0xFF;
		return TRUE;
	}

	return Class(mGridBoxPiece).setProperty((mGridBoxPiece*)self, id, value);
}

static DWORD mDayGridPiece_getProperty(mDayGridPiece* self, int id)
{
	switch(id)
	{
	case NCSP_DAYGRIDPIECE_MONTH_DAYS:
		return self->month_days;
	case NCSP_DAYGRIDPIECE_LASTMONTH_DAYS:
		return self->last_month_days;
	case NCSP_DAYGRIDPIECE_WEEKDAY_OF_FIRSTDAY:
		return self->weekday_of_first_day;
	}

	return Class(mGridBoxPiece).getProperty((mGridBoxPiece*)self, id);
}

static CELL_POINT mDayGridPiece_day2Cell(mDayGridPiece *self, int day)
{
	CELL_POINT cp = NULL_CELL_POINT;
	int idx;
	if(day<=0) //last month
	{
		idx = self->weekday_of_first_day + day - 1;	
	}
/*	else if(day > self->month_days)
	{
		idx = day + self->weekday_of_first_day - 1;
	}*/
	else
	{
		if(self->weekday_of_first_day == 0) //sunday
			idx = 7 + day - 2;
		else
			idx = self->weekday_of_first_day + day - 2;
	}

	if(idx < 0 || idx >= (self->rows*self->cols))
		return cp;

	cp.row = idx / self->cols;
	cp.col = idx % self->cols;
	return cp;
}

static int mDayGridPiece_cell2Day(mDayGridPiece  *self, int row, int col)
{
	int idx = row * self->cols + col + 1;
	int cur_day_from_wday = 1;
	
	if(self->weekday_of_first_day == Sunday)
		cur_day_from_wday = 7;
	else
		cur_day_from_wday = self->weekday_of_first_day ;
	
	return idx - cur_day_from_wday + 1;

}


BEGIN_MINI_CLASS(mDayGridPiece, mGridBoxPiece)
	CLASS_METHOD_MAP(mDayGridPiece, setProperty);
	CLASS_METHOD_MAP(mDayGridPiece, getProperty);
	CLASS_METHOD_MAP(mDayGridPiece, day2Cell);
	CLASS_METHOD_MAP(mDayGridPiece, cell2Day);
END_MINI_CLASS

