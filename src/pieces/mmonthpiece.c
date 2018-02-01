/* 
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
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "mhotpiece.h"
#include "mcontainerpiece.h"
#include "mmonthpiece.h"

#include "mstaticpiece.h"
#include "mrenderablepiece.h"
#include "mgridboxpiece.h"
#include "mdaygridpiece.h"

#ifdef _MGNCSCTRL_MONTHCALENDAR

static void mMonthPiece_construct(mMonthPiece* self, DWORD add_data)
{
	Class(mContainerPiece).construct((mContainerPiece*)self, add_data);

	if(add_data && INSTANCEOF(add_data, mDayGridPiece))
		self->body  = (mHotPiece*)add_data;
	else
		self->body = (mHotPiece*)NEWPIECE(mDayGridPiece);

	if(self->body)
	{
		_c(self->body)->setProperty(self->body, NCSP_GRIDBOXPIECE_ROWS, 6);
		_c(self->body)->setProperty(self->body, NCSP_GRIDBOXPIECE_COLS, 7);
	}
}

static BOOL mMonthPiece_setProperty(mMonthPiece* self, int id, DWORD value)
{
	switch(id)
	{
	case NCSP_DAYGRIDPIECE_MONTH_DAYS:
		if(_c(self->body)->setProperty(self->body, id, value))
		{
			if(value < self->cur_day)
				self->cur_day = value;
			return TRUE;
		}
		return FALSE;
	case NCSP_MONTHPIECE_CURDAY:
		if(value <= 0 || value > 31 || value == self->cur_day)
			return FALSE;
		self->cur_day = value;
		return TRUE;
	}

	return _c(self->body)->setProperty(self->body, id, value);
}

static DWORD mMonthPiece_getProperty(mMonthPiece* self, int id)
{
	switch(id)
	{
	case NCSP_MONTHPIECE_CURDAY:
		return self->cur_day;
	}

	return _c(self->body)->getProperty(self->body,id);
}


static void set_new_day(mMonthPiece *self, int day, CELL_POINT cp, mWidget* owner)
{
	mDayGridPiece * daygrid = (mDayGridPiece*)(self->body);

	if(day == self->cur_day)
		return;

	if(day <= 0) //goto last month
	{
		RAISE_EVENT(self, NCSN_MONTHPIECE_PREVMONTH, 0);
		return ;
	}
	else if(day > daygrid->month_days)
	{
		RAISE_EVENT(self, NCSN_MONTHPIECE_NEXTMONTH, 0);
	}
	else
	{
		int old_day = self->cur_day;
		self->cur_day = day;
		_c(daygrid)->updateNewCell(daygrid,
			_c(daygrid)->day2Cell(daygrid, old_day),
			cp,
			NCS_GRIDBOXPIECE_PAINT_MASK_REDRAW,
			NCS_GRIDBOXPIECE_PAINT_MASK_DRAWSEL,
			owner);

		RAISE_EVENT(self, NCSN_MONTHPIECE_DAYCHANGED, day);
	}
}

static void set_new_hot_cell(mMonthPiece *self, mDayGridPiece * daygrid, int new_row, int new_col, mWidget *owner)
{

	CELL_POINT old_cp, new_cp;
	int day_old, day_new;

	if(new_col < 0)
	{
		new_col = daygrid->cols - 1;
		new_row --;
	}
	else if(new_col >= daygrid->cols)
	{
		new_col = 0;
		new_row ++;
	}

	if(new_row < 0 || new_row >= (int)daygrid->rows)
		return ;


	if(new_row == (int)self->hot_row && new_col == (int)self->hot_col)
		return;

	day_new = _c(daygrid)->cell2Day(daygrid, new_row, new_col);

	day_old = _c(daygrid)->cell2Day(daygrid, self->hot_row, self->hot_col);

	old_cp.row = self->hot_row;
	old_cp.col = self->hot_col;

	new_cp.row = new_row;
	new_cp.col = new_col;

	_c(daygrid)->updateNewCell(daygrid,
		old_cp,
		new_cp,
		day_old == self->cur_day?0:NCS_GRIDBOXPIECE_PAINT_MASK_REDRAW,
		day_new == self->cur_day?0:NCS_GRIDBOXPIECE_PAINT_MASK_HOT,
		owner);

	self->hot_row = new_row;
	self->hot_col = new_col;

}

static int mMonthPiece_processMessage(mMonthPiece* self, int message, WPARAM wParam, LPARAM lParam, mWidget * owner)
{
	mDayGridPiece * daygrid = (mDayGridPiece*)(self->body);
	if(!daygrid)
		return 0;

	switch(message)
	{
	case MSG_LBUTTONDOWN:
		{
			CELL_POINT cp = _c(daygrid)->hitCell(daygrid, LOSWORD(lParam), HISWORD(lParam));
			int day;
			if(CELL_POINT_ISNULL(cp))
				return 0;
			day = _c(daygrid)->cell2Day(daygrid, cp.row, cp.col);
			set_new_day(self, day, cp, owner);
			mWidget_setHoveringFocus(owner, (mObject*)self);
		}
		break;
	case MSG_MOUSEMOVE:
		{
			CELL_POINT cp = _c(daygrid)->hitCell(daygrid, LOSWORD(lParam), HISWORD(lParam));
			set_new_hot_cell(self, daygrid, cp.row, cp.col, owner);
			mWidget_setHoveringFocus(owner, (mObject*)self);
		}
		return 0;
	case MSG_KEYUP:
		switch(wParam)
		{
		case SCANCODE_CURSORBLOCKUP:
			set_new_hot_cell(self, daygrid, self->hot_row - 1, self->hot_col, owner);
			break;
		case SCANCODE_CURSORBLOCKDOWN:
			set_new_hot_cell(self, daygrid, self->hot_row + 1, self->hot_col, owner);
			break;
		case SCANCODE_CURSORBLOCKLEFT:
			set_new_hot_cell(self, daygrid, self->hot_row , self->hot_col - 1, owner);
			break;
		case SCANCODE_CURSORBLOCKRIGHT:
			set_new_hot_cell(self, daygrid, self->hot_row, self->hot_col + 1, owner);
			break;
		case SCANCODE_SPACE:
		case SCANCODE_ENTER:
			{
				CELL_POINT cp = {self->hot_row, self->hot_col};
				int day = _c(daygrid)->cell2Day(daygrid, self->hot_row, self->hot_col);
				set_new_day(self, day, cp, owner);
			}
			break;
		}
	}
	return 0;
}

static void mMonthPiece_paint(mMonthPiece* self, HDC hdc, mWidget *onwer, DWORD add_data)
{
	if(self->body)
	{
		mGridBoxPiecePaintData paint_data;
		paint_data.sel_cell = _c((mDayGridPiece*)(self->body))->day2Cell((mDayGridPiece*)(self->body), self->cur_day);
		paint_data.hot_cell.row = self->hot_row;
		paint_data.hot_cell.col = self->hot_col;
		paint_data.add_data = add_data;
		paint_data.mask = NCS_GRIDBOXPIECE_PAINT_MASK_ALL_CELLS
			| NCS_GRIDBOXPIECE_PAINT_MASK_DRAWSEL
			| NCS_GRIDBOXPIECE_PAINT_MASK_HOT;
		_c(self->body)->paint(self->body, hdc, (mObject*)onwer, (DWORD)&paint_data);
	}
}

BEGIN_MINI_CLASS(mMonthPiece, mContainerPiece)
	CLASS_METHOD_MAP(mMonthPiece, construct)
	CLASS_METHOD_MAP(mMonthPiece, processMessage)
	CLASS_METHOD_MAP(mMonthPiece, setProperty)
	CLASS_METHOD_MAP(mMonthPiece, getProperty)
	CLASS_METHOD_MAP(mMonthPiece, paint)
END_MINI_CLASS

#endif // _MGNCSCTRL_MONTHCALENDAR
