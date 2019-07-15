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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/fixedmath.h>

#include "mgncs.h"

#ifdef _MGNCS_RDR_SKIN

///////////////////////////////////
//common fucntion
////////////////////////////////////////////
// button box
static void skin_buttonbox_paint(mButtonBoxPiece *self, HDC hdc, mWidget * owner, DWORD add_data)
{
	RECT rc;
    DWORD key;
    DRAWINFO di;
	int state = add_data&NCS_PIECE_PAINT_STATE_MASK;
	int check_state = NCS_PIECE_PAINT_GET_CHECK(add_data);

	if(!_c(self)->getRect(self, &rc))
		return ;

	key = ncsGetElement(owner, NCS_IMAGE_BUTTON);

    di.bmp = GetBitmapFromRes (key);

	if(di.bmp == NULL)
		return;

    di.nr_line  = 5;
    di.nr_col   = 1;
    di.idx_col  = 0;
    di.idx_line = 0;
    di.margin1  = 6;
    di.margin2  = 6;
    di.direct   = FALSE;
    di.flip     = FALSE;
    di.style    = DI_FILL_TILE;

    if (state == NCS_ABP_DISABLED) {
        di.idx_line = 3;
    }else if (state == NCS_ABP_PUSHED){
        di.idx_line = 2;
    }else if (check_state == NCS_ABP_CHECKED){
            di.idx_line = 2;
	}else if (check_state == NCS_ABP_HALFCHECKED){
            di.idx_line = 4;
    } else if (state == NCS_ABP_NORMAL){
        di.idx_line = 0;
    }else if (state == NCS_ABP_HILIGHT){
        di.idx_line = 1;
    }

    ncsSkinDraw (hdc, &rc, &di);

}

//////////////////////////////////////////////////
// thumbbox
static void skin_draw_thumbbox(mHotPiece *self, HDC hdc, mWidget *owner, DWORD add_data, int id)
{
	//BOOL bIsVert = add_data&0x00010000;
	RECT rc;
	DWORD key;
	//int x, y, w, h;
	DRAWINFO di;

	if(!_c(self)->getRect(self, &rc))
		return ;

	memset(&di, 0, sizeof(di));
	key = ncsGetElement(owner, id);
	if(!(di.bmp = GetBitmapFromRes(key)))
		return ;
	di.nr_line   = 4;
	di.nr_col    = 1;
	di.idx_col   = 0;
	di.direct    = FALSE;
	di.flip      = FALSE;
	di.style     = DI_FILL_STRETCH;

	switch(add_data&NCS_PIECE_PAINT_STATE_MASK)
	{
	case NCS_ABP_NORMAL:
		di.idx_line = 0;
		break;
	case NCS_ABP_HILIGHT:
		di.idx_line = 1;
		break;
	case NCS_ABP_PUSHED:
		di.idx_line = 2;
		break;
	case NCS_ABP_DISABLED:
		di.idx_line = 3;
		break;
	}
	ncsSkinDraw(hdc, &rc, &di);

/*	x = rc.left;
	y = rc.top;
	w = RECTW(rc);
	h = RECTH(rc);

	if (bIsVert) {
        MoveTo (hdc, x + (w>>1) - 4, y + (h>>1));
        LineTo (hdc, x + (w>>1) + 2, y + (h>>1));
        MoveTo (hdc, x + (w>>1) - 3, y + (h>>1) - 2);
        LineTo (hdc, x + (w>>1) + 1, y + (h>>1) - 2);
        MoveTo (hdc, x + (w>>1) - 3, y + (h>>1) + 2);
        LineTo (hdc, x + (w>>1) + 1, y + (h>>1) + 2);
    } else {
        MoveTo (hdc, x + (w>>1), y + (h>>1) - 4);
        LineTo (hdc, x + (w>>1), y + (h>>1) + 2);
        MoveTo (hdc, x + (w>>1) - 2, y + (h>>1) - 3);
        LineTo (hdc, x + (w>>1) - 2, y + (h>>1) + 1);
        MoveTo (hdc, x + (w>>1) + 2, y + (h>>1) - 3);
        LineTo (hdc, x + (w>>1) + 2, y + (h>>1) + 1);
    }*/
}

static void skin_thumbbox_paint(mThumbBoxPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	skin_draw_thumbbox((mHotPiece*)self, hdc, owner, add_data,
		add_data&NCS_PIECE_PAINT_VERT?NCS_IMAGE_TRKBAR_VTHUMB:NCS_IMAGE_TRKBAR_HTHUMB);
}

/////////////////////////////////////////////////////
//draw trackbar
static void skin_trackbar_paint(mTrackBarPiece* self, HDC hdc, mWidget *owner, DWORD add_data)
{
    /*
     * the data struct of add_data
     *
     * bits 0x0000FFFF  : the tick
     * bits 0x00FF0000  : horz or vert
     * bits 0xFF000000  : enable or disable
     */

	RECT  rc;
	DWORD key;
	fixed gap, tick;
	DRAWINFO di;
	int tick_count = add_data & 0xFFFF;

	if(!_c(self)->getRect(self, &rc))
		return;

	key = ncsGetElement(owner, NCS_PIECE_PAINT_VERT&add_data?NCS_IMAGE_TRKBAR_VSLIDER:NCS_IMAGE_TRKBAR_HSLIDER);
	memset(&di, 0, sizeof(di));
	if(!(di.bmp = GetBitmapFromRes(key)))
		return;

	di.nr_line = 1;
	di.nr_col  = 1;
	di.margin1 = 2;
	di.margin2 = 2;
	di.direct  = NCS_PIECE_PAINT_VERT&add_data?TRUE:FALSE;
	di.style   = DI_FILL_TILE;

	if(!(NCS_PIECE_PAINT_VERT&add_data)) //horz
	{
		rc.top = (rc.top + rc.bottom)/2 - 3;
		rc.bottom = rc.top + 6;
		ncsSkinDraw(hdc, &rc, &di);
		if(tick_count > 0)
		{
			gap = fixdiv(itofix(RECTW(rc)), itofix(tick_count));

			if((int)(fixtof(gap)) <= 0)
				return ;

			for(tick = itofix(rc.left);
				(int)(fixtof(tick)) <= rc.right;
				tick = fixadd(tick, gap))
			{
				MoveTo(hdc, (int)(fixtof(tick)), rc.bottom);
				LineTo(hdc, (int)(fixtof(tick)), rc.bottom + 4);
			}

			if((int)(fixtof(fixadd(fixsub(tick,gap), ftofix(0.9)))) < rc.bottom){
				MoveTo (hdc, rc.right, rc.bottom);
				LineTo (hdc, rc.right, rc.bottom + 4);
			}

		}
	}
	else //vert
	{
		rc.left = (rc.left + rc.right)/2 -3;
		rc.right = rc.left + 6;
		ncsSkinDraw(hdc, &rc, &di);

		if(tick_count > 0)
		{
			gap = fixdiv(itofix(RECTH(rc)), itofix(tick_count));

			if((int)(fixtof(gap)) <= 0)
				return ;

			for(tick = itofix(rc.top);
				(int)(fixtof(tick)) <= rc.bottom;
				tick = fixadd(tick, gap))
			{
				MoveTo(hdc, rc.right, (int)(fixtof(tick)));
				LineTo(hdc, rc.right + 4, (int)(fixtof(tick)));
			}

			if((int)(fixtof(fixadd(fixsub(tick,gap), ftofix(0.9)))) < rc.bottom){
				MoveTo (hdc, rc.right, rc.bottom);
				LineTo (hdc, rc.right + 4, rc.bottom);
			}


		}
	}
}
////////////////////////////////////
//common check
static void skin_draw_check(HDC hdc, const RECT *rc, int flag ,int state, int check_state, mWidget *owner, int eid)
{
    int w, h;
    int box_l, box_t;
    const BITMAP* check_bmp;
	DWORD key;
	int index = state;
	int style = 0;

	key = ncsGetElement(owner, eid);
	check_bmp = GetBitmapFromRes(key);
	if(!check_bmp)
		return;

	if(check_state)
		index += 4;

	w = rc->right - rc->left;
    h = rc->bottom - rc->top;

	style = check_bmp->bmHeight / 8;

	if( w >= style && h >= style){
		box_l = rc->left + (w>>1)-6;
		box_t = rc->top + (h>>1)-6;

		/*parity check*/
		if (w & 0x1)
			box_l += 1;

		if (h & 0x1)
			box_t += 1;

		FillBoxWithBitmapPart (hdc, box_l, box_t, style, style, 0, 0, check_bmp, 0, index * check_bmp->bmHeight / 8);
	}
	else {
		FillBitmapPartInBox (hdc, rc->left, rc->top, w, h, check_bmp, 0 ,index * check_bmp->bmHeight / 8, check_bmp->bmWidth, style);
	}
    return;
}

////////////////////////////////////
//checkbox
static void skin_checkbox_paint(mCheckBoxPiece* self, HDC hdc, mWidget *owner, DWORD add_data)
{
	RECT rc;

	if(!_c(self)->getRect(self, &rc))
		return ;

	skin_draw_check(hdc, &rc, NCSRF_FILL, add_data&NCS_PIECE_PAINT_STATE_MASK, NCS_PIECE_PAINT_GET_CHECK(add_data), owner, NCS_IMAGE_CHKBTN);
}
#if 1
//////////////////////////////////
//radiobox
static void skin_radiobox_paint(mCheckBoxPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	RECT rc;

	if(!_c(self)->getRect(self, &rc))
		return ;

	skin_draw_check(hdc, &rc, NCSRF_FILL, add_data&NCS_PIECE_PAINT_STATE_MASK, NCS_PIECE_PAINT_GET_CHECK(add_data), owner, NCS_IMAGE_RDOBTN);
}
#endif

#ifdef _MGNCSCTRL_SCROLLBAR
//////////////////////
//scrollbar

static void skin_scrollthumbbox_paint(mScrollThumbBoxPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	skin_draw_thumbbox((mHotPiece*)self, hdc, owner, add_data,
		add_data&NCS_PIECE_PAINT_VERT?WE_LFSKIN_SCROLLBAR_VTHUMB:WE_LFSKIN_SCROLLBAR_HTHUMB);
}


static void skin_scrollbar_paint(mScrollBarPiece* self, HDC hdc, mWidget *owner, DWORD add_data)
{
	DWORD key;
	RECT rc;
	DRAWINFO di;

	if(!_c(self)->getRect(self, &rc))
		return ;

	key = ncsGetElement(owner, add_data&NCS_PIECE_PAINT_VERT?WE_LFSKIN_SCROLLBAR_VSHAFT:WE_LFSKIN_SCROLLBAR_HSHAFT);

	memset(&di, 0, sizeof(di));

	if(!(di.bmp = GetBitmapFromRes(key)))
	{
		DWORD barcolor = ncsGetElement(owner, NCS_BGC_3DBODY);
		gal_pixel old_brush = SetBrushColor(hdc, ncsColor2Pixel(hdc, barcolor));
		FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
		SetBrushColor(hdc, old_brush);
	}
	else
	{
		di.nr_line = 4;
		di.nr_col  = 1;
		di.direct  = add_data&NCS_PIECE_PAINT_VERT?TRUE:FALSE;
		di.style   = DI_FILL_TILE;
		ncsSkinDraw(hdc, &rc, &di);
	}
}

////////////////////////
//draw arrow
//
static void skin_arrow_paint(mScrollBarPiece *self, HDC hdc, mWidget * owner, DWORD add_data)
{
	DRAWINFO di;
	RECT rc;
	int arrow_type_idx;
	int arrow_state_idx;
	PBITMAP bmp_arrow;
	RECT rc_arrow;

	if(!_c(self)->getRect(self, &rc))
		return ;

	if(!(bmp_arrow = GetBitmapFromRes(ncsGetElement(owner, NCS_IMAGE_ARROWS))))
		return ;

	rc_arrow.left   = (rc.left + rc.right - bmp_arrow->bmWidth)/2;
	rc_arrow.right  = rc_arrow.left + bmp_arrow->bmWidth;
	rc_arrow.top    = (rc.top + rc.bottom - bmp_arrow->bmHeight/16)/2;
	rc_arrow.bottom = rc_arrow.top + bmp_arrow->bmHeight/16;

	if(rc_arrow.left < rc.left)
	{
		rc_arrow.left  = rc.left;
		rc_arrow.right = rc.right;
	}

	if(rc_arrow.top < rc.top)
	{
		rc_arrow.top = rc.top;
		rc_arrow.bottom = rc.bottom;
	}

	if(add_data & NCS_PIECE_PAINT_ARROW_SHELL)
	{
		memset(&di, 0, sizeof(di));
		if((di.bmp = GetBitmapFromRes(ncsGetElement(owner, NCS_IMAGE_ARROWSHELL))))
		{
			di.nr_line = 4;
			di.nr_col  = 1;
			di.idx_col = 0;
			di.margin1 = 0;
			di.margin2 = 0;
			di.direct  = FALSE;
			di.flip    = FALSE;
			di.style   = DI_FILL_STRETCH;

			switch(add_data & NCS_PIECE_PAINT_STATE_MASK)
			{
			case NCS_ABP_NORMAL:
				di.idx_line = 0;
				break;
			case NCS_ABP_HILIGHT:
				di.idx_line = 1;
				break;
			case NCS_ABP_PUSHED:
				di.idx_line = 2;
				break;
			case NCS_ABP_DISABLED:
				di.idx_line = 3;
				break;
			default:
				di.idx_line = 0;
				break;
			}
			ncsSkinDraw(hdc, &rc, &di);
		}
	}

	switch(add_data&NCS_PIECE_PAINT_STATE_MASK){
	case PIECE_STATE_DISABLE:
		arrow_state_idx = 3;
		break;
	case PIECE_STATE_HILIGHT:
		arrow_state_idx = 1;
		break;
	case PIECE_STATE_PUSHED:
		arrow_state_idx = 2;
		break;
	case PIECE_STATE_NORMAL:
	default:
		arrow_state_idx = 0;
		break;
	}

	switch(add_data&NCS_PIECE_PAINT_ARROW_MASK)
	{
	case NCS_ARROWPIECE_RIGHT:
		arrow_type_idx = 3;
		break;
	case NCS_ARROWPIECE_UP:
		arrow_type_idx = 0;
		break;
	case NCS_ARROWPIECE_DOWN:
		arrow_type_idx = 1;
		break;
	case NCS_ARROWPIECE_LEFT:
	default:
		arrow_type_idx = 2;
		break;
	}

	di.bmp      = bmp_arrow;
	di.nr_line  = 16;
	di.nr_col   = 1;
	di.idx_col  = 0;
	di.idx_line = arrow_type_idx * 4 + arrow_state_idx;
	di.margin1  = 0;
	di.margin2  = 0;
	di.direct   = FALSE;
	di.flip     = FALSE;
	di.style    = DI_FILL_STRETCH;

	//draw arrow
	ncsSkinDraw(hdc, &rc_arrow, &di);
}
#endif //_MGNCSCTRL_SCROLLBAR

///////////////////////////////////////////
//grid box
// day grid
//

typedef void (*PGridBoxPaintContent)(mGridBoxPiece *self, HDC hdc, int row, int col, const RECT *c, mWidget *owner, void *paintData);

static void skin_grid_box_paint_all(mGridBoxPiece *self, HDC hdc, mWidget *owner, void *pPaintData,
	PGridBoxPaintContent paint_content)
{
	RECT rc, rc_cell;
	int cell_width, cell_height;
	int row, col;
	if(!_c(self)->getRect(self, &rc))
		return ;

	if(self->rows <= 0 || self->cols <= 0)
		return ;

	cell_width  = RECTW(rc) / self->cols;
	cell_height = RECTH(rc) / self->rows;

	rc_cell.top  = rc.top;

	for(row = 0; row < self->rows; row ++)
	{
		rc_cell.left = rc.left;
		if(row == self->rows - 1)
			rc_cell.bottom = rc.bottom;
		else
			rc_cell.bottom = rc_cell.top + cell_height;
		for(col = 0; col < self->cols; col ++)
		{
			if(col == self->cols - 1)
				rc_cell.right = rc.right;
			else
				rc_cell.right = rc_cell.left + cell_width;
			paint_content(self, hdc, row, col, &rc_cell, owner, pPaintData);
			rc_cell.left = rc_cell.right;
		}
		rc_cell.top = rc_cell.bottom;
	}
}

/////
//day grid

#define DAY_PREV  0
#define DAY_CUR   1
#define DAY_NEXT  2
static inline int get_day_by_cell(mDayGridPiece* self, int row, int col, int *pday)
{
	int day = _c(self)->cell2Day(self, row, col);

	if(day <= 0)
	{
		*pday = day + self->last_month_days;
		return DAY_PREV;
	}
	else if(day > self->month_days)
	{
		*pday = day - self->month_days;
		return DAY_NEXT;
	}

	*pday = day;

	return DAY_CUR;
}

typedef struct mDayGridPiecePaintContentData{
	mGridBoxPiecePaintData * paint_data;
	gal_pixel bkcolor;
	gal_pixel fgcolor;
	gal_pixel other_month_bkcolor;
	gal_pixel other_month_fgcolor;
	gal_pixel hot_bkcolor;
	gal_pixel hot_fgcolor;
	gal_pixel sel_bkcolor;
	gal_pixel sel_fgcolor;
	int last_type;
}mDayGridPiecePaintContentData;

static void skin_daygrid_paint_content(mDayGridPiece *self,
		HDC hdc,
		int row ,int col,
		const RECT *rc,
		mWidget *owner,
		mDayGridPiecePaintContentData *data)
{
	char szText[8];
	int day, daytype;

	daytype = get_day_by_cell(self, row, col, &day);

	if((data->paint_data->mask & NCS_GRIDBOXPIECE_PAINT_MASK_HOT)
			&& data->paint_data->hot_cell.row == row
			&& data->paint_data->hot_cell.col == col)
	{
		SetBrushColor(hdc, data->hot_bkcolor);
		SetTextColor(hdc, data->hot_fgcolor);
		data->last_type = -1;
	}
	else if((data->paint_data->mask & NCS_GRIDBOXPIECE_PAINT_MASK_DRAWSEL)
			&& data->paint_data->sel_cell.row == row
			&& data->paint_data->sel_cell.col == col)
	{
		SetBrushColor(hdc, data->sel_bkcolor);
		SetTextColor(hdc, data->sel_fgcolor);
		data->last_type = -1;
	}
	else if(daytype != data->last_type)
	{
		switch(daytype)
		{
		case DAY_PREV:
		case DAY_NEXT:
			SetBrushColor(hdc, data->other_month_bkcolor);
			SetTextColor(hdc, data->other_month_fgcolor);
			break;
		case DAY_CUR:
			SetBrushColor(hdc, data->bkcolor);
			SetTextColor(hdc, data->fgcolor);
			break;
		}
		data->last_type = daytype;
	}

	//fill RECT
	FillBox(hdc, rc->left, rc->top, RECTWP(rc), RECTHP(rc));
	//draw Text

	if(day>=10)
	{
		sprintf(szText, "%d", day);
	}
	else
	{
		sprintf(szText," %d", day);
	}


	DrawText(hdc, szText, 2, (RECT*)rc, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
}

static void skin_daygrid_paint(mDayGridPiece *self, HDC hdc, mWidget * owner, DWORD add_data)
{
	mGridBoxPiecePaintData * paint_data = (mGridBoxPiecePaintData*)add_data;
	mDayGridPiecePaintContentData paint_content_data;
	gal_pixel old_bkcolor;
	gal_pixel old_fgcolor;
	DWORD bkcolor;
	int mode;
	if(!paint_data)
	{
		return ;
	}

	mode = SetBkMode(hdc, BM_TRANSPARENT);

	paint_content_data.paint_data = paint_data;

	paint_content_data.last_type = -1;


	old_bkcolor = GetBrushColor(hdc);
	old_fgcolor = GetTextColor(hdc);


	if(paint_data->mask & NCS_GRIDBOXPIECE_PAINT_MASK_ALL_CELLS)
	{
		paint_content_data.bkcolor = ncsColor2Pixel(hdc, (bkcolor=ncsGetElement(owner, NCS_BGC_WINDOW)));
		paint_content_data.fgcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_FGC_WINDOW));

		paint_content_data.other_month_bkcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_BGC_DISABLED_ITEM));
		paint_content_data.other_month_fgcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_FGC_DISABLED_ITEM));


		paint_content_data.hot_bkcolor = ncsColor2Pixel(hdc, ncsCommRDRCalc3dboxColor(bkcolor, NCSR_COLOR_DARKER));
		paint_content_data.hot_fgcolor = paint_content_data.fgcolor;

		paint_content_data.sel_bkcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_BGC_SELECTED_ITEM));
		paint_content_data.sel_fgcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_FGC_SELECTED_ITEM));

		skin_grid_box_paint_all((mGridBoxPiece*)self, hdc, owner, &paint_content_data, (PGridBoxPaintContent)skin_daygrid_paint_content);
	}
	else
	{
		RECT rc;
		if(paint_data->mask & NCS_GRIDBOXPIECE_PAINT_MASK_DRAWSEL)
		{
			paint_content_data.sel_bkcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_BGC_SELECTED_ITEM));
			paint_content_data.sel_fgcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_FGC_SELECTED_ITEM));

			skin_daygrid_paint_content(self, hdc,
				paint_data->sel_cell.row, paint_data->sel_cell.col,
				_c(self)->getCellRect( self, paint_data->sel_cell.row, paint_data->sel_cell.col, &rc),
				owner,
				&paint_content_data);
		}
		else if(paint_data->mask & NCS_GRIDBOXPIECE_PAINT_MASK_REDRAW)
		{
			paint_content_data.bkcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_BGC_WINDOW));
			paint_content_data.fgcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_FGC_WINDOW));
			paint_content_data.other_month_bkcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_BGC_DISABLED_ITEM));
			paint_content_data.other_month_fgcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_FGC_DISABLED_ITEM));
			skin_daygrid_paint_content(self, hdc,
				paint_data->sel_cell.row, paint_data->sel_cell.col,
				_c(self)->getCellRect( self, paint_data->sel_cell.row, paint_data->sel_cell.col, &rc),
				owner,
				&paint_content_data);
		}
		else if(paint_data->mask & NCS_GRIDBOXPIECE_PAINT_MASK_HOT)
		{
			paint_content_data.hot_bkcolor = ncsColor2Pixel(hdc,
				ncsCommRDRCalc3dboxColor(ncsGetElement(owner, NCS_BGC_WINDOW),NCSR_COLOR_DARKER));
			paint_content_data.hot_fgcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_FGC_WINDOW));

			skin_daygrid_paint_content(self, hdc,
				paint_data->hot_cell.row, paint_data->hot_cell.col,
				_c(self)->getCellRect( self, paint_data->hot_cell.row, paint_data->hot_cell.col, &rc),
				owner,
				&paint_content_data);
		}
		else
			return ;
	}

	SetBrushColor(hdc, old_bkcolor);
	SetTextColor(hdc, old_fgcolor);
	SetBkMode(hdc, mode);
}


//////////////////////////////////////
//draw weekhead
static const char * default_week_names [] = {
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat",
	"Sun"
};
static void skin_weekhead_paint(mWeekHeadPiece *self, HDC hdc, mWidget * owner, DWORD add_data)
{
	RECT rc;
	int cell_size;
	int i;
	const char** weeknames = (const char**)self->week_names;
	DWORD bgcolor, fgcolor;
	gal_pixel old_bgcolor;
	gal_pixel old_fgcolor;
	if(weeknames == NULL)
		weeknames = default_week_names;

	if(!_c(self)->getRect(self, &rc))
		return ;

	cell_size = RECTW(rc)/7;

	bgcolor = ncsGetElement(owner, NCS_BGC_SELECTED_ITEM);
	fgcolor = ncsGetElement(owner, NCS_FGC_SELECTED_ITEM);

	old_bgcolor = SetBrushColor(hdc, ncsColor2Pixel(hdc, bgcolor));
	old_fgcolor = SetTextColor(hdc, ncsColor2Pixel(hdc, fgcolor));

	//Fill Box
	FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
	//draw Text
	for(i=0; i<7; i++)
	{
		rc.right = rc.left + cell_size;
		DrawText(hdc, weeknames[i], -1, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		rc.left = rc.right;
	}
	SetBrushColor(hdc, old_bgcolor);
	SetTextColor(hdc, old_fgcolor);
}


///////////////////////////////////////
//progresspiece
#ifdef _MGNCSCTRL_PROGRESSBAR
#define NCSPB_BLOCK 8
#define NCSPB_INNER 2
static void skin_progress_paint(mProgressPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	RECT rc;
	RECT rc_prog;
	PBITMAP bmp_prog;
	int i;

	if(self->max <= self->min)
		return;

	if(!_c(self)->getRect(self, &rc))
		return;

	rc_prog = rc;
	//calc rc_prog;
	if(add_data & NCS_PIECE_PAINT_VERT)
	{
		int height = RECTH(rc_prog) * (self->cur - self->min) / (self->max - self->min);
		rc_prog.top = rc_prog.bottom - height;
		bmp_prog = GetBitmapFromRes(ncsGetElement(owner, NCS_IMAGE_PRGBAR_VCHUNK));
	}
	else
	{
		int width = RECTW(rc_prog) * (self->cur - self->min) / (self->max - self->min);
		rc_prog.right = rc_prog.left + width;
		bmp_prog = GetBitmapFromRes(ncsGetElement(owner, NCS_IMAGE_PRGBAR_HCHUNK));
	}

	if(IsRectEmpty(&rc_prog))
		return ;

	if(!bmp_prog)
		return ;


	if(add_data & NCS_PIECE_PAINT_BLOCK)
	{
		if(add_data & NCS_PIECE_PAINT_VERT)
		{
			int width = RECTW(rc_prog);
			i=rc_prog.bottom;
			while(i > rc_prog.top)
			{
                int height = MIN(NCSPB_BLOCK, (i - rc_prog.top));
				FillBoxWithBitmap(hdc, rc_prog.left, i - height, width, height, bmp_prog);
				i -= (NCSPB_BLOCK+NCSPB_INNER);
			}
		}
		else
		{
			int height = RECTH(rc_prog);
			i = rc_prog.left;
			while(i < rc_prog.right)
			{
				FillBoxWithBitmap(hdc, i, rc_prog.top, MIN(NCSPB_BLOCK, (rc_prog.right - i)), height, bmp_prog);
				i += (NCSPB_BLOCK+NCSPB_INNER);
			}
		}
	}
	else
	{
		FillBoxWithBitmap(hdc, rc_prog.left, rc_prog.top, RECTW(rc_prog), RECTH(rc_prog), bmp_prog);
	}

	if(add_data & NCS_PIECE_PAINT_PERCENT)
	{
		RECT      rc_text;
		RECT      rc_tmp;
		int       percent = (self->cur - self->min)*100 / (self->max - self->min);
		char      szText[100];
		gal_pixel old_text_color;
		gal_pixel prog_color;
    	gal_pixel old_color;
		int       old_bkmode;
		SIZE      size;

		prog_color = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_BGC_HILIGHT_ITEM));

    	old_color = SetBrushColor (hdc, prog_color);

		sprintf(szText, "%d%%", percent);
		GetTextExtent(hdc, szText, -1, &size);
		rc_text = rc;

		old_bkmode = SetBkMode(hdc, BM_TRANSPARENT);
		old_text_color = GetTextColor(hdc);

		if(add_data & NCS_PIECE_PAINT_VERT)
		{
			rc_text.top = (rc.top + rc.bottom - size.cy) / 2;
			rc_text.bottom = rc_text.top + size.cy;
			rc.bottom = rc_prog.top;
		}
		else
		{
			rc_text.left = (rc.left + rc.right - size.cx) / 2;
			rc_text.right = rc_text.left + size.cx;
			rc.left = rc_prog.right;
		}
		// draw
		if(IntersectRect(&rc_tmp, &rc_text, &rc_prog))
		{
			SelectClipRect(hdc, &rc_tmp);
			SetTextColor(hdc, GetWindowBkColor(owner->hwnd));
			DrawText(hdc, szText, -1, &rc_text, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}

		if(IntersectRect(&rc_tmp, &rc_text, &rc))
		{
			SelectClipRect(hdc, &rc_tmp);
			SetTextColor(hdc, prog_color);
			DrawText(hdc, szText, -1, &rc_text, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}

		SetTextColor(hdc, old_text_color);
		SetBkMode(hdc, old_bkmode);
		SetBrushColor(hdc, old_color);
	}

}
#endif

//////////////////////////////////
//init boxpiece
void skin_init_boxpiece_renderer(void)
{
	NCS_RDR_ENTRY entries [] = {
		{Class(mButtonBoxPiece).typeName, (mWidgetRenderer*)(void*)skin_buttonbox_paint},
		{Class(mThumbBoxPiece).typeName, (mWidgetRenderer*)(void*)skin_thumbbox_paint},
#ifdef _MGNCSCTRL_SCROLLBAR
		{Class(mScrollThumbBoxPiece).typeName, (mWidgetRenderer*)(void*)skin_scrollthumbbox_paint},
#endif
		{Class(mTrackBarPiece).typeName, (mWidgetRenderer*)(void*)skin_trackbar_paint},
		{Class(mCheckBoxPiece).typeName, (mWidgetRenderer*)(void*)skin_checkbox_paint},
		{Class(mRadioBoxPiece).typeName, (mWidgetRenderer*)(void*)skin_radiobox_paint},
#ifdef _MGNCSCTRL_SCROLLBAR
		{Class(mScrollBarPiece).typeName, (mWidgetRenderer*)(void*)skin_scrollbar_paint},
		{Class(mLeftArrowPiece).typeName, (mWidgetRenderer*)(void*)skin_arrow_paint},
		{Class(mRightArrowPiece).typeName, (mWidgetRenderer*)(void*)skin_arrow_paint},
		{Class(mUpArrowPiece).typeName, (mWidgetRenderer*)(void*)skin_arrow_paint},
		{Class(mDownArrowPiece).typeName, (mWidgetRenderer*)(void*)skin_arrow_paint},
#endif
		{Class(mDayGridPiece).typeName, (mWidgetRenderer*)(void*)skin_daygrid_paint},
		{Class(mWeekHeadPiece).typeName, (mWidgetRenderer*)(void*)skin_weekhead_paint},
#ifdef _MGNCSCTRL_PROGRESSBAR
		{Class(mProgressPiece).typeName, (mWidgetRenderer*)(void*)skin_progress_paint}
#endif
	};

	ncsRegisterCtrlRDRs("skin",
		entries,
		sizeof(entries)/sizeof(NCS_RDR_ENTRY));

}

#endif

