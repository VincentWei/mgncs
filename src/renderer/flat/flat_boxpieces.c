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

#ifdef _MGNCS_RDR_FLAT

///////////////////////////////////
//common fucntion
gal_pixel flat_set_crosss_brush(HDC hdc, gal_pixel color)
{
	static STIPPLE cross_stipple = {
		8, 8, 1, (size_t)8, (const unsigned char*)"\xAA\x55\xAA\x55\xAA\x55\xAA\x55"
	};

	gal_pixel old = SetBrushColor(hdc, color);
	SetBrushType(hdc, BT_STIPPLED);
	SetBrushInfo(hdc, NULL, &cross_stipple);

	return old;
}

void flat_draw_3dbox(HDC hdc, const RECT *rc, int flag ,int state, int check_state, mWidget *owner)
{
	DWORD bgcolor;
	DWORD fgcolor;
	gal_pixel old_brush;
	gal_pixel old_pen;
	int old_width;
    int border_width;
    int border_radius;

	bgcolor = ncsGetElement(owner, NCS_BGC_3DBODY);
	fgcolor = ncsGetElement(owner, NCS_FGC_3DBODY);

	border_radius = ncsGetElement (owner, NCS_METRICS_BORDER_RADIUS);
	border_width = ncsGetElement (owner, NCS_METRICS_BORDER_WIDTH);

	if (border_width < 1)
		border_width = 1;
	if (border_radius < 0)
		border_radius = 0;

    SetPenType (hdc, PT_SOLID);
    SetPenJoinStyle (hdc, PT_JOIN_ROUND);
    SetBrushType (hdc, BT_SOLID);
	old_brush = SetBrushColor (hdc, ncsColor2Pixel(hdc, bgcolor));
	old_pen   = SetPenColor (hdc, ncsColor2Pixel(hdc, fgcolor));
	old_width = SetPenWidth (hdc, border_width);

	//fill back with bgcolor
	if (border_radius > 0) {
		if (flag & NCSRF_FILL) {
			RoundRect (hdc, rc->left + (border_width >> 1), rc->top + (border_width >> 1),
                    rc->right - (border_width >> 1), rc->bottom - (border_width >> 1), border_radius, border_radius);
        }
		else
			ncsCommRDRDrawHalfRoundRect (hdc, rc, border_radius, border_radius, fgcolor,
				NCS_ROUND_CORNER_ALL|NCS_BORDER_ALL);
	}
	else {
		if (flag & NCSRF_FILL)
			FillBox (hdc, rc->left+1, rc->top+1, RECTWP(rc) - 2, RECTHP(rc) -2);
		Rectangle (hdc, rc->left, rc->top, rc->right-1, rc->bottom-1);
	}

	if(check_state == NCS_ABP_CHECKED) {
		if (border_radius > 0) {
			ArcEx (hdc, rc->left +  2, rc->top + 2, border_radius*2, border_radius*2,
				90*64, 90*64);
		}

		MoveTo (hdc, rc->left + border_radius + 2 , rc->top + 2);
		LineTo (hdc, (rc->right - border_radius )*85/100, rc->top+2);

		MoveTo (hdc, rc->left + 2 , rc->top + border_radius + 2);
		LineTo (hdc, rc->left + 2, (rc->bottom - border_radius)*85/100);
	}
	else if (check_state == NCS_ABP_HALFCHECKED) {
		if (border_radius > 0) {
			ArcEx (hdc, rc->right - border_radius * 2 - 3, 
				rc->bottom - border_radius*2  - 3, border_radius*2, border_radius*2,
				270*64, 90*64);
		}

		MoveTo(hdc, rc->right - 3 , (rc->top + border_radius)*100/85);
		LineTo(hdc, rc->right - 3, rc->bottom - border_radius - 3);

		MoveTo(hdc, rc->right - border_radius - 3 , rc->bottom - 3);
		LineTo(hdc, (rc->left + border_radius) * 100 / 85, rc->bottom - 3);
	}

	SetBrushColor (hdc, old_brush);
	SetPenColor (hdc, old_pen);
	SetPenWidth (hdc, old_width);
}


////////////////////////////////////////////
// button box
static void flat_buttonbox_paint(mButtonBoxPiece *self, HDC hdc, mWidget * owner, DWORD add_data)
{
	RECT rc;

	if(!_c(self)->getRect(self, &rc))
		return ;

	flat_draw_3dbox(hdc, &rc, NCSRF_FILL, add_data&NCS_PIECE_PAINT_STATE_MASK,
			NCS_PIECE_PAINT_GET_CHECK(add_data), owner);
}

//////////////////////////////////////////////////
// thumbbox
static void flat_thumbbox_paint(mThumbBoxPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	BOOL bIsVert = add_data&0x00010000;
	RECT rc;
	int x, y, w, h;

	if(!_c(self)->getRect(self, &rc))
		return ;


	flat_draw_3dbox(hdc, &rc, NCSRF_FILL, add_data&NCS_PIECE_PAINT_STATE_MASK,
	NCS_PIECE_PAINT_GET_CHECK(add_data), owner);

	x = rc.left;
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
    }



}

/////////////////////////////////////////////////////
//draw trackbar
static void flat_trackbar_paint(mTrackBarPiece* self, HDC hdc, mWidget *owner, DWORD add_data)
{
    /*
     * the data struct of add_data
     *
     * bits 0x0000FFFF  : the tick
     * bits 0x00FF0000  : horz or vert
     * bits 0xFF000000  : enable or disable
     */

	//DWORD bgcolor;
	RECT  rc;
	fixed gap, tick;
	int   tick_count = add_data & 0xFFFF;

	if(!_c(self)->getRect(self, &rc))
		return;

	//bgcolor = ncsGetElement(owner, NCS_BGC_3DBODY);

	if(!(NCS_PIECE_PAINT_VERT&add_data)) //horz
	{
		rc.top = (rc.top + rc.bottom)/2 - 3;
		rc.bottom = rc.top + 6;
		//ncsCommRDRDraw3dbox(hdc, &rc, bgcolor,NCSRF_FILL | NCSRS_SIGNIFICANT);
		Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
		rc.bottom += 3;
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
		//ncsCommRDRDraw3dbox(hdc, &rc, bgcolor,NCSRF_FILL | NCSRS_SIGNIFICANT);
		Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

		rc.right += 3;

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
#define MAX_CHECK_RADIO 14
static void flat_draw_check(HDC hdc, const RECT *rc, int state, int check_state, mWidget *owner,
	void (*frame)(HDC hdc, const RECT *prc),
	void (*fill)(HDC hdc, const RECT *prc),
	void (*draw_state)(HDC hdc, const RECT *prc, BOOL bhalfCheck))
{
	DWORD bgcolor;
	DWORD fgcolor;
	RECT  rctmp ;
	//gal_pixel old_brush;
	//gal_pixel old_pen;

	rctmp.left = (rc->left + rc->right - MAX_CHECK_RADIO) / 2;
	rctmp.top  = (rc->top + rc->bottom - MAX_CHECK_RADIO) / 2;
	rctmp.right = rctmp.left + MAX_CHECK_RADIO;
	rctmp.bottom = rctmp.top + MAX_CHECK_RADIO;

	bgcolor = ncsGetElement(owner, NCS_BGC_3DBODY);
	fgcolor = ncsGetElement(owner, NCS_FGC_3DBODY);

	/*old_pen   = */SetPenColor(hdc, ncsColor2Pixel(hdc, fgcolor));

	//frame rect
	frame(hdc, &rctmp);

	//set state info
	if(NCS_ABP_HILIGHT == state)
		/*old_brush = */flat_set_crosss_brush(hdc, ncsColor2Pixel(hdc, fgcolor));
	else
		/*old_brush = */SetBrushColor(hdc, ncsColor2Pixel(hdc, bgcolor));

	//fill state
	rctmp.left += 1;
	rctmp.top += 1;
	rctmp.right -= 1;
	rctmp.bottom -= 1;
	fill(hdc, &rctmp);

	if(check_state == NCS_ABP_CHECKED || check_state == NCS_ABP_HALFCHECKED)
	{
		rctmp.left += 2;
		rctmp.top  += 2;
		rctmp.right -= 2;
		rctmp.bottom -= 2;
		draw_state(hdc, &rctmp, check_state == NCS_ABP_HALFCHECKED);
	}
}


////////////////////////////////////
//checkbox

static void flat_checkbox_frame(HDC hdc, const RECT *prc)
{
	Rectangle(hdc, prc->left, prc->top, prc->right - 1, prc->bottom -1);
}

static void flat_checkbox_fill(HDC hdc, const RECT *prc)
{
	FillBox(hdc, prc->left, prc->top, RECTWP(prc), RECTHP(prc));
}

static void flat_checkbox_draw_state(HDC hdc, const RECT *prc, BOOL bhalf)
{
	if(bhalf)
	{
		SetBrushColor(hdc, GetPenColor(hdc));
		SetBrushType(hdc, BT_SOLID);
		FillBox(hdc, prc->left + 1, prc->top+1, RECTWP(prc)-2, RECTHP(prc)-2);
	}
	else
	{
		MoveTo(hdc, prc->left, prc->top);
		LineTo(hdc, prc->right-1, prc->bottom-1);
		MoveTo(hdc, prc->left+1, prc->top);
		LineTo(hdc, prc->right-1, prc->bottom-2);
		MoveTo(hdc, prc->left, prc->top+1);
		LineTo(hdc, prc->right-2, prc->bottom-1);

		MoveTo(hdc, prc->right-1, prc->top);
		LineTo(hdc, prc->left, prc->bottom-1);
		MoveTo(hdc, prc->right-1, prc->top+1);
		LineTo(hdc, prc->left+1, prc->bottom-1);
		MoveTo(hdc, prc->right-2, prc->top);
		LineTo(hdc, prc->left, prc->bottom-2);
	}
}

static void flat_checkbox_paint(mCheckBoxPiece* self, HDC hdc, mWidget *owner, DWORD add_data)
{
	RECT rc;

	if(!_c(self)->getRect(self, &rc))
		return ;

	flat_draw_check(hdc, &rc,
		add_data&NCS_PIECE_PAINT_STATE_MASK,
		NCS_PIECE_PAINT_GET_CHECK(add_data),
		owner,
		flat_checkbox_frame,
		flat_checkbox_fill,
		flat_checkbox_draw_state);
}
#if 1
//////////////////////////////////
//radiobox
static void flat_radiobox_frame(HDC hdc, const RECT *prc)
{
	Ellipse(hdc, (prc->left+prc->right)/2, (prc->top+prc->bottom)/2, RECTWP(prc)/2, RECTHP(prc)/2);
}

static void flat_radiobox_fill(HDC hdc, const RECT *prc)
{
	FillEllipse(hdc, (prc->left+prc->right)/2, (prc->top+prc->bottom)/2, RECTWP(prc)/2, RECTHP(prc)/2);
}

static void flat_radiobox_draw_state(HDC hdc, const RECT *prc, BOOL bhalf)
{
	gal_pixel pen_color = GetPenColor(hdc);
	SetBrushColor(hdc, pen_color);
	SetBrushType(hdc, BT_SOLID);

	//draw
	FillEllipse(hdc, (prc->left+prc->right)/2, (prc->top+prc->bottom)/2, RECTWP(prc)/2, RECTHP(prc)/2);
}

static void flat_radiobox_paint(mCheckBoxPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	RECT rc;

	if(!_c(self)->getRect(self, &rc))
		return ;

	flat_draw_check(hdc, &rc,
		add_data&NCS_PIECE_PAINT_STATE_MASK,
		NCS_PIECE_PAINT_GET_CHECK(add_data),
		owner,
		flat_radiobox_frame,
		flat_radiobox_fill,
		flat_radiobox_draw_state);
}
#endif

#ifdef _MGNCSCTRL_SCROLLBAR
//////////////////////
//scrollbar
static void flat_scrollbar_paint(mScrollBarPiece* self, HDC hdc, mWidget *owner, DWORD add_data)
{
	DWORD barcolor = ncsGetElement(owner, NCS_BGC_3DBODY);
	RECT rc;
	gal_pixel old_brush = SetBrushColor(hdc, ncsColor2Pixel(hdc, barcolor));

	if(!_c(self)->getRect(self, &rc))
		return ;

	FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
	SetBrushColor(hdc, old_brush);
}

////////////////////////
//draw arrow
//
static void flat_arrow_paint(mScrollBarPiece *self, HDC hdc, mWidget * owner, DWORD add_data)
{
	RECT rc;
	DWORD arrow_color;
	int arrow;
	if(!_c(self)->getRect(self, &rc))
		return ;

	if(add_data & NCS_PIECE_PAINT_ARROW_SHELL)
	{
		flat_draw_3dbox(hdc, &rc, NCSRF_FILL, add_data&NCS_PIECE_PAINT_STATE_MASK,
					NCS_PIECE_PAINT_GET_CHECK(add_data), owner);
	}

	arrow_color = ncsGetElement(owner, NCS_FGC_3DBODY);

	switch(add_data&NCS_PIECE_PAINT_ARROW_MASK)
	{
	case NCS_ARROWPIECE_RIGHT:
		arrow = NCSR_ARROW_RIGHT;
		break;
	case NCS_ARROWPIECE_UP:
		arrow = NCSR_ARROW_UP;
		break;
	case NCS_ARROWPIECE_DOWN:
		arrow = NCSR_ARROW_DOWN;
		break;
	case NCS_ARROWPIECE_LEFT:
	default:
		arrow = NCSR_ARROW_LEFT;
		break;
	}

	//draw arrow
	ncsCommRDRDrawArrow(hdc, &rc, arrow, arrow_color,
		((add_data&NCS_PIECE_PAINT_STATE_MASK) == PIECE_STATE_DISABLE)?0:NCSRF_FILL);
}
#endif

///////////////////////////////////////////
//grid box
// day grid
//

typedef void (*PGridBoxPaintContent)(mGridBoxPiece *self, HDC hdc, int row, int col, const RECT *c, mWidget *owner, void *paintData);

static void flat_grid_box_paint_all(mGridBoxPiece *self, HDC hdc, mWidget *owner, void *pPaintData,
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

static void flat_daygrid_paint_content(mDayGridPiece *self,
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

static void flat_daygrid_paint(mDayGridPiece *self, HDC hdc, mWidget * owner, DWORD add_data)
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

		flat_grid_box_paint_all((mGridBoxPiece*)self, hdc, owner, &paint_content_data, (PGridBoxPaintContent)flat_daygrid_paint_content);
	}
	else
	{
		RECT rc;
		if(paint_data->mask & NCS_GRIDBOXPIECE_PAINT_MASK_DRAWSEL)
		{
			paint_content_data.sel_bkcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_BGC_SELECTED_ITEM));
			paint_content_data.sel_fgcolor = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_FGC_SELECTED_ITEM));

			flat_daygrid_paint_content(self, hdc,
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
			flat_daygrid_paint_content(self, hdc,
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

			flat_daygrid_paint_content(self, hdc,
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
static void flat_weekhead_paint(mWeekHeadPiece *self, HDC hdc, mWidget * owner, DWORD add_data)
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
static void flat_progress_paint(mProgressPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
    gal_pixel old_color;
	RECT rc;
	RECT rc_prog;
	gal_pixel prog_color;
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
	}
	else
	{
		int width = RECTW(rc_prog) * (self->cur - self->min) / (self->max - self->min);
		rc_prog.right = rc_prog.left + width;
	}

	if(IsRectEmpty(&rc_prog))
		return ;

	prog_color = ncsColor2Pixel(hdc, ncsGetElement(owner, NCS_FGC_HILIGHT_ITEM));

    old_color = SetBrushColor (hdc, prog_color);

	if(add_data & NCS_PIECE_PAINT_BLOCK)
	{
		if(add_data & NCS_PIECE_PAINT_VERT)
		{
			int width = RECTW(rc_prog);
			i=rc_prog.bottom;
			while(i > rc_prog.top)
			{
				FillBox(hdc, rc_prog.left, i - NCSPB_BLOCK, width, MIN(NCSPB_BLOCK, (i - rc_prog.top)));
				i -= (NCSPB_BLOCK+NCSPB_INNER);
			}
		}
		else
		{
			int height = RECTH(rc_prog);
			i = rc_prog.left;
			while(i < rc_prog.right)
			{
				FillBox(hdc, i, rc_prog.top, MIN(NCSPB_BLOCK, (rc_prog.right - i)), height);
				i += (NCSPB_BLOCK+NCSPB_INNER);
			}
		}
	}
	else
	{
		FillBox(hdc, rc_prog.left, rc_prog.top, RECTW(rc_prog), RECTH(rc_prog));
	}

	if(add_data & NCS_PIECE_PAINT_PERCENT)
	{
		RECT      rc_text;
		RECT      rc_tmp;
		int       percent = (self->cur - self->min)*100 / (self->max - self->min);
		char      szText[100];
		gal_pixel old_text_color;
		int       old_bkmode;
		SIZE      size;

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
	}

	SetBrushColor(hdc, old_color);
}
#endif

//////////////////////////////////
//init boxpiece
void flat_init_boxpiece_renderer(void)
{
	NCS_RDR_ENTRY entries [] = {
		{Class(mButtonBoxPiece).typeName, (mWidgetRenderer*)(void*)flat_buttonbox_paint},
		{Class(mThumbBoxPiece).typeName, (mWidgetRenderer*)(void*)flat_thumbbox_paint},
#ifdef _MGNCSCTRL_SCROLLBAR
		{Class(mScrollThumbBoxPiece).typeName, (mWidgetRenderer*)(void*)flat_thumbbox_paint},
#endif
		{Class(mTrackBarPiece).typeName, (mWidgetRenderer*)(void*)flat_trackbar_paint},
		{Class(mCheckBoxPiece).typeName, (mWidgetRenderer*)(void*)flat_checkbox_paint},
		{Class(mRadioBoxPiece).typeName, (mWidgetRenderer*)(void*)flat_radiobox_paint},
#ifdef _MGNCSCTRL_SCROLLBAR
		{Class(mScrollBarPiece).typeName, (mWidgetRenderer*)(void*)flat_scrollbar_paint},
		{Class(mLeftArrowPiece).typeName, (mWidgetRenderer*)(void*)flat_arrow_paint},
		{Class(mRightArrowPiece).typeName, (mWidgetRenderer*)(void*)flat_arrow_paint},
		{Class(mUpArrowPiece).typeName, (mWidgetRenderer*)(void*)flat_arrow_paint},
		{Class(mDownArrowPiece).typeName, (mWidgetRenderer*)(void*)flat_arrow_paint},
#endif
		{Class(mDayGridPiece).typeName, (mWidgetRenderer*)(void*)flat_daygrid_paint},
		{Class(mWeekHeadPiece).typeName, (mWidgetRenderer*)(void*)flat_weekhead_paint},
#ifdef _MGNCSCTRL_PROGRESSBAR
		{Class(mProgressPiece).typeName, (mWidgetRenderer*)(void*)flat_progress_paint}
#endif
	};

	ncsRegisterCtrlRDRs("flat",
		entries,
		sizeof(entries)/sizeof(NCS_RDR_ENTRY));

}

#endif

