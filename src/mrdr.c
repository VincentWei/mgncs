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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/fixedmath.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"
#include "hashtable.h"
#include "uthash.h"

static inline char upper(char ch)
{
	return ch >= 'a' && ch <= 'z' ? ch - 'a' + 'A' : ch;
}

static ht_key_t str2key(const char* str)
{
	char szKeyStr[128];
	int i;
	for(i=0; str[i]; i++)
	{
		szKeyStr[i] = upper(str[i]);
	}
	szKeyStr[i] = 0;

	return Str2Key(szKeyStr);
}
#define RENDER_SIZE  17

static HashTable * rendererFactroy = NULL;

static HashTable * _basic_renderer = NULL;
static char _basic_renderer_name[16] = "classic";
static HashTable * _sys_renderer = NULL;
static char _sys_renderer_name[16] ="classic";

BOOL ncsSetBasicRenderer(const char* name)
{

	HashTable * htRenderer;

	if(name == NULL)
		return FALSE;

	strncpy(_basic_renderer_name, name, sizeof(_basic_renderer_name)-1);
	_basic_renderer_name[sizeof(_basic_renderer_name)-1] = '\0';

	if(rendererFactroy == NULL)
		return FALSE;

	htRenderer = (HashTable*)hash_get(rendererFactroy,str2key(name));

	if(htRenderer == NULL)
		return FALSE;

	_basic_renderer = htRenderer;
	return TRUE;
}

BOOL ncsSetSystemRenderer(const char* name)
{
	HashTable * htRenderer;

	if(name == NULL)
		return FALSE;

	strncpy(_sys_renderer_name, name, sizeof(_sys_renderer_name)-1);
	_sys_renderer_name[sizeof(_sys_renderer_name)-1] = '\0';

	if(rendererFactroy == NULL)
		return FALSE;

	htRenderer = (HashTable*)hash_get(rendererFactroy,str2key(name));

	if(htRenderer == NULL)
		return FALSE;

	_sys_renderer = htRenderer;
	
	return TRUE;

}

const char* ncsGetSystemRenderer(void)
{
	return _sys_renderer_name;
}

mWidgetRenderer *ncsRetriveCtrlRDR (const char *rendererName, const char *className)
{
	HashTable * htRenderer;
	mWidgetRenderer* rdr;
    ht_key_t key;


	if(rendererFactroy == NULL)
		return NULL;

	if(className == NULL)
		return NULL;

	if(rendererName == NULL )
		htRenderer = _sys_renderer;
	else 
		htRenderer = (HashTable*) hash_get(rendererFactroy, str2key(rendererName));

	if(htRenderer == NULL)
		htRenderer = _basic_renderer;

	if(htRenderer == NULL)
		return NULL;

    key = str2key(className);
	rdr =  (mWidgetRenderer*) hash_get(htRenderer,key);
    if(rdr == NULL && _basic_renderer)
    {
        rdr = (mWidgetRenderer*) hash_get(_basic_renderer,  key);
    }

    if(rdr == NULL && _sys_renderer)
    {
        rdr = (mWidgetRenderer*) hash_get(_sys_renderer, key);
    }

    if(rdr == NULL)
    {
        htRenderer = (HashTable*) hash_get(rendererFactroy, str2key("classic"));
        if(htRenderer)
            rdr = (mWidgetRenderer*) hash_get(htRenderer, key);
    }

	
	return rdr;
}


BOOL ncsRegisterCtrlRDR (const char *rendererName, const char *className,
			  mWidgetRenderer *renderer)
{
	HashTable * htRenderer;
	ht_key_t key ;

	if(rendererName == NULL || className == NULL || renderer == NULL)
		return FALSE;

	if(rendererFactroy == NULL)
	{
		rendererFactroy = (HashTable*)hash_new(5, (void(*)(void*))hash_free);
	}
	
	key = str2key(rendererName);
	htRenderer = (HashTable*) hash_get(rendererFactroy, key);

	if(htRenderer == NULL)
	{
		htRenderer = hash_new(RENDER_SIZE, NULL);
		hash_insert(rendererFactroy, key, htRenderer);
		if(_basic_renderer == NULL && 
			strncmp(_basic_renderer_name, rendererName, sizeof(_basic_renderer_name)-1) == 0)
			_basic_renderer = htRenderer;

		if(_sys_renderer == NULL &&
			strncmp(_sys_renderer_name, rendererName, sizeof(_sys_renderer_name)-1) == 0)
			_sys_renderer = htRenderer;
	}

	return hash_insert(htRenderer, str2key(className), renderer) == HTE_OK;
}

BOOL ncsRegisterCtrlRDRs (const char *rendererName, NCS_RDR_ENTRY * entries,
			   int count)
{
	HashTable * htRenderer;
	ht_key_t key ;
	int i;

	if(rendererName == NULL ||  entries == NULL || count <= 0)
		return FALSE;

	if(rendererFactroy == NULL)
		rendererFactroy = (HashTable*)hash_new(5,(void(*)(void*)) hash_free);
	
	key = str2key(rendererName);
	htRenderer = (HashTable*) hash_get(rendererFactroy, key);

	if(htRenderer == NULL)
	{
		htRenderer = hash_new(RENDER_SIZE, NULL);
		hash_insert(rendererFactroy, key, htRenderer);
		if(_basic_renderer == NULL && 
			strncmp(_basic_renderer_name, rendererName, sizeof(_basic_renderer_name)-1) == 0)
			_basic_renderer = htRenderer;

		if(_sys_renderer == NULL &&
			strncmp(_sys_renderer_name, rendererName, sizeof(_sys_renderer_name)-1) == 0)
			_sys_renderer = htRenderer;
	}

	for(i=0; i<count; i++){
		hash_insert(htRenderer, str2key(entries[i].className), entries[i].renderer);
	}

	return TRUE;
}

void ncsUnregisterCtrlRDRs(const char* rdrName, const char* clssName)
{
	HashTable * htRenderer;

	if(rendererFactroy == NULL)
		return ;

	if(rdrName == NULL) //unregister ALL
	{
		hash_free(rendererFactroy);
		rendererFactroy = NULL;
		_basic_renderer = NULL;
		_sys_renderer = NULL;

		return ;
	}

	//find the speical rdr
	htRenderer = (HashTable*)hash_get(rendererFactroy, str2key(rdrName));

	if(!htRenderer)
		return ;

	if(clssName == NULL)
	{
		if(htRenderer == _basic_renderer)
			_basic_renderer = NULL;
		if(htRenderer == _sys_renderer)
			_sys_renderer = _basic_renderer;

		hash_delete(rendererFactroy, str2key(rdrName));
		return ;
	}

	hash_delete(htRenderer, str2key(clssName));
}

//////////////////////////////////////////

void ncsCommRDRDrawFocusFrame(HWND hWnd, HDC hdc, const RECT *rc)
{
    mWidget *self = ncsObjFromHandle(hWnd);
    DWORD color = ncsGetElement(self, NCS_FGC_3DBODY);

	gal_pixel old = SetPenColor(hdc, ncsColor2Pixel(hdc, color));
	DrawHDotLine(hdc, rc->left, rc->top, RECTWP(rc));
	DrawHDotLine(hdc, rc->left, rc->bottom, RECTWP(rc));
	DrawVDotLine(hdc, rc->left, rc->top, RECTHP(rc));
	DrawVDotLine(hdc, rc->right, rc->top, RECTHP(rc));
	SetPenColor(hdc, old);
}

BOOL ncsCommRDRDrawBkgnd(HWND hWnd, HDC hdc, const RECT* inv_rc)
{
	PBITMAP pbkimg=NULL;
	HDC hdct;
	RECT rcClient;
	DWORD bkcolor;
	gal_pixel old;
	int bkimgmode;
	DWORD pbmpKey;
    mWidget *self = ncsObjFromHandle(hWnd);

	if(hdc == (HDC)0)
		hdct = GetClientDC(hWnd);
	else
		hdct = hdc;

	GetClientRect(hWnd, &rcClient);

	bkcolor = GetWindowBkColor(hWnd);

	old = SetBrushColor(hdct, bkcolor/*ncsColor2Pixel(hdct, bkcolor)*/);
	FillBox(hdct, rcClient.left, rcClient.top, RECTW(rcClient), RECTH(rcClient));


	pbmpKey = ncsGetElement(self, NCS_IMAGE_BKGND);

	if(pbmpKey != 0 && pbmpKey != INVALID_KEY)
	{
		pbkimg = GetResource(pbmpKey);
	}

	if(pbkimg)
	{
		bkimgmode = ncsGetElement(self, NCS_MODE_BKIMAGE);
		if(NCS_DM_NORMAL == bkimgmode)
		{
			int x = (RECTW(rcClient) - pbkimg->bmWidth) / 2;
			int y = (RECTH(rcClient) - pbkimg->bmHeight) / 2;
			FillBoxWithBitmap(hdct, x, y, -1, -1, pbkimg);
		}
		else if(NCS_DM_TILED == bkimgmode)
		{
			int x, y;
			for(x = rcClient.left; x < rcClient.right; x += pbkimg->bmWidth)
			{
				for(y = rcClient.top; y < rcClient.bottom; y += pbkimg->bmHeight)
				{
					FillBoxWithBitmap(hdct, x, y, -1, -1, pbkimg);
				}
			}
		}
		else
		{
			FillBoxWithBitmap(hdct, rcClient.left, rcClient.top, RECTW(rcClient), RECTH(rcClient), pbkimg);
		}
	}

	SetBrushColor(hdct, old);

	if(hdc != hdct)
		ReleaseDC(hdct);

	return TRUE;
}


static int inline calc_pen_width(const RECT *rc)
{
	int pen_width = RECTWP(rc);
	if(pen_width > RECTHP(rc))
		pen_width = RECTHP(rc);
	
	pen_width /= 10; //
	if(pen_width <= 0)
		return 1;
	
	return pen_width;
}


static void 
draw_one_frame (HDC hdc, const RECT* rc, int rx, int ry, DWORD lt_color, DWORD rb_color, DWORD part_flag)
{
	int ltx = 0, lty = 0; //left - top corners x, y
	int lbx = 0, lby = 0; //left - bottom corners x, y
	int rtx = 0, rty = 0; //right - top corners x, y
	int rbx = 0, rby = 0; //right - bottom corners, x, y

	if(rx > RECTWP(rc)/2)
		rx = RECTWP(rc)/2;
	if(ry > RECTHP(rc)/2)
		ry = RECTHP(rc)/2;
	
	if(rx > 0 && ry > 0)
	{
		if(part_flag & NCS_ROUND_CORNER_LEFT_TOP)
		{
			ltx = rx;
			lty = ry;
		}
		if(part_flag & NCS_ROUND_CORNER_LEFT_BOTTOM)
		{
			lbx = rx;
			lby = ry;
		}
		if(part_flag & NCS_ROUND_CORNER_RIGHT_TOP)
		{
			rtx = rx;
			rty = ry;
		}
		if(part_flag & NCS_ROUND_CORNER_RIGHT_BOTTOM)
		{
			rbx = rx;
			rby = ry;
		}
	}

    SetPenColor (hdc, ncsColor2Pixel(hdc, lt_color));
	
	if(part_flag & NCS_BORDER_LEFT){
	    MoveTo(hdc, rc->left, rc->bottom - lby - 1);
	   	LineTo(hdc, rc->left, rc->top + lty);
	}
	
	if((part_flag & (NCS_BORDER_LEFT|NCS_BORDER_TOP)) && ltx > 0 && lty > 0)
		ArcEx(hdc, rc->left, rc->top, rx*2, ry*2, 90*64, 90*64);
	
	if(part_flag & NCS_BORDER_TOP)
	{
		MoveTo(hdc, rc->left + ltx, rc->top);
   	 	LineTo(hdc, rc->right - rtx - 1, rc->top);
	}

	if((part_flag& (NCS_BORDER_LEFT|NCS_BORDER_BOTTOM)) && lbx > 0 && lby > 0)
		ArcEx(hdc, rc->left, rc->bottom - ry*2 - 1, rx*2, ry*2, 180*64, 90*64);


    SetPenColor(hdc, ncsColor2Pixel(hdc, rb_color));
	
	if(part_flag & NCS_BORDER_BOTTOM)
	{
   		 MoveTo (hdc, rc->left+(lbx>0?lbx:1), rc->bottom-1);
   		 LineTo (hdc, rc->right-1 - rbx, rc->bottom-1);
	}
	
	if(part_flag & NCS_BORDER_RIGHT)
	{
		MoveTo (hdc, rc->right-1, rc->bottom-1-rby);
   	 	LineTo (hdc, rc->right-1, rc->top + rty);
	}

	if((part_flag & (NCS_BORDER_RIGHT | NCS_BORDER_BOTTOM)) && rbx > 0 && rby > 0)
		ArcEx(hdc, rc->right - 1 -rx*2, rc->bottom - 1 - ry*2,rx*2, ry*2, 270*64, 90*64);

	if((part_flag & (NCS_BORDER_RIGHT|NCS_BORDER_TOP)) && rtx > 0 && rty > 0)
		ArcEx(hdc, rc->right - 1 -rx*2, rc->top, rx*2, ry*2,0, 90*64);
}

void ncsCommRDRDrawHalfRoundRect(HDC hdc, const RECT *rc, int rx, int ry, DWORD color, DWORD part_flag)
{
	gal_pixel old = GetPenColor(hdc);
	draw_one_frame(hdc, rc, rx, ry, color, color, part_flag);
	SetPenColor(hdc, old);
}

void ncsCommRDRFillHalfRoundRect(HDC hdc, const RECT *rc, int rx, int ry, DWORD color, DWORD part_flag)
{
	gal_pixel old_brush_color = SetBrushColor(hdc, ncsColor2Pixel(hdc, color));

	if(rx > 0 && ry > 0 && part_flag != 0)
	{
		FillBox(hdc, rc->left, rc->top +  ry, RECTWP(rc) , RECTHP(rc) - ry * 2);
		FillBox(hdc, rc->left + rx , rc->top , RECTWP(rc)- rx * 2 , ry);
		FillBox(hdc, rc->left + rx, rc->bottom - ry, RECTWP(rc)- rx * 2, ry);

		//FillArcs
		//left - top corner
		if(part_flag & NCS_ROUND_CORNER_LEFT_TOP)
			FillArcEx(hdc, rc->left, rc->top, rx*2, ry*2, 90*64, 90*64);
		else
			FillBox(hdc, rc->left, rc->top, rx, ry);
		//right - top corner
		if(part_flag & NCS_ROUND_CORNER_RIGHT_TOP)
			FillArcEx(hdc, rc->right - rx*2 - 1, rc->top, rx*2, ry*2, 0, 90*64);
		else
			FillBox(hdc, rc->right - rx, rc->top, rx, ry);
		//right - bottom corner
		if(part_flag & NCS_ROUND_CORNER_RIGHT_BOTTOM)
			FillArcEx(hdc, rc->right - rx*2 - 1, rc->bottom - ry*2 - 1, rx*2, ry*2, 270*64, 90*64);
		else
			FillBox(hdc, rc->right - rx , rc->bottom - ry, rx, ry);
		//left - bottom corner
		if(part_flag & NCS_ROUND_CORNER_LEFT_BOTTOM)
			FillArcEx(hdc, rc->left , rc->bottom - ry*2, rx*2, ry*2,180*64, 90*64);
		else
			FillBox(hdc, rc->left, rc->bottom - ry, rx, ry);
	}
	else
	{
		FillBox(hdc, rc->left, rc->top, RECTWP(rc), RECTHP(rc));
	}
	SetBrushColor(hdc, old_brush_color);

}


void ncsCommRDRDraw3DRoundBox(HDC hdc, const RECT *rc, int rx, int ry, DWORD color, DWORD flag)
{
	ncsCommRDRDraw3DHalfRoundBox(hdc, rc, rx, ry, color, flag, NCS_ROUND_CORNER_ALL|NCS_BORDER_ALL);
}

void ncsCommRDRDraw3DHalfRoundBox(HDC hdc, const RECT *rc, int rx, int ry, DWORD color, DWORD flag, DWORD part_flag)
{
	DWORD light_color;
	DWORD dark_color;
	BOOL has_set_light_color = FALSE;
	gal_pixel old_pen_color;
	int inner ;
	int pen_width;
	
	if(rc == NULL || RECTWP(rc) < 2 || RECTHP(rc) < 2)
		return;

	old_pen_color = GetPenColor(hdc);
	pen_width = 1;//calc_pen_width(rc);
	//old_pen_width = SetPenWidth(hdc, pen_width);
	inner = pen_width;
	
	//draw outer frame
	light_color = ncsCommRDRCalc3dboxColor(color, NCSR_COLOR_LIGHTEST);
	dark_color = ncsCommRDRCalc3dboxColor(color, NCSR_COLOR_DARKEST);

	if(flag & NCSRF_FILL)
	{
		DWORD fill_color;		
		RECT  rctmp = {rc->left + inner, rc->top + inner, rc->right - inner, rc->bottom - inner};
		if ((flag & 0xFFFF) == NCSRS_HIGHLIGHT)
		{
			light_color = ncsCommRDRCalc3dboxColor(color, NCSR_COLOR_LIGHTER);
			has_set_light_color = TRUE;

			fill_color =  light_color;
		}
		else{
			fill_color = color;
		}
		
		ncsCommRDRFillHalfRoundRect(hdc, &rctmp, rx, ry, fill_color, part_flag);
	}


	if((flag & 0xFFFF) == NCSRS_SIGNIFICANT)
		draw_one_frame(hdc, rc, rx, ry, dark_color, light_color, part_flag);
	else {
		if(flag & NCSRF_INNERFRAME)
			draw_one_frame(hdc, rc, rx, ry, color, dark_color, part_flag);
		else
			draw_one_frame(hdc, rc, rx, ry, light_color, dark_color, part_flag);
	}

	//draw inner frame
	if((flag & NCSRF_INNERFRAME) 
		&& (RECTWP(rc) > 6 && (RECTHP(rc) > 6)))
	{
		RECT rc_tmp = {
			rc->left + pen_width,
			rc->top + pen_width,
			rc->right - pen_width,
			rc->bottom - pen_width
		};
		if(!has_set_light_color)
			light_color = ncsCommRDRCalc3dboxColor(color, NCSR_COLOR_LIGHTER);
		dark_color = ncsCommRDRCalc3dboxColor(color, NCSR_COLOR_DARKER);

		inner += pen_width;
		
		if((flag & 0xFFFF) == NCSRS_SIGNIFICANT)
			draw_one_frame(hdc, &rc_tmp, rx, ry, dark_color, light_color, part_flag);
		else 
			draw_one_frame(hdc, &rc_tmp, rx, ry, light_color, dark_color, part_flag);

	}


	SetPenColor(hdc, old_pen_color);
	//SetPenWidth(hdc, old_pen_width);
}

void ncsCommRDRDraw3dbox(HDC hdc, const RECT* rc, DWORD color, DWORD flag)
{
	ncsCommRDRDraw3DRoundBox(hdc, rc, 0, 0, color, flag);
}


void ncsCommRDRDrawSmallHalfRoundBox(HDC hdc, const RECT *rc,  DWORD flags, BOOL bFill)
{
	if(flags == 0)
	{
		Rectangle(hdc, rc->left, rc->top, rc->right - 1, rc->bottom - 1);
		if(bFill)
		{
			FillBox(hdc, rc->left + 1, rc->top + 1, RECTWP(rc) - 2, RECTHP(rc) - 2);
		}
	}
	else if( flags == NCS_BORDER_ALL)
	{
		int radius = 2;
		if(RECTWP(rc) <= 14 || RECTHP(rc) <= 14)
			radius = 1;

		MoveTo(hdc, rc->left + radius, rc->top);
		LineTo(hdc, rc->right - radius - 1, rc->top);
		LineTo(hdc, rc->right - 1, rc->top +  radius);
		LineTo(hdc, rc->right - 1, rc->bottom - 1 - radius);
		LineTo(hdc, rc->right - radius - 1, rc->bottom - 1);
		LineTo(hdc, rc->left + radius, rc->bottom - 1);
		LineTo(hdc, rc->left, rc->bottom - radius - 1);
		LineTo(hdc, rc->left, rc->top + radius);
		LineTo(hdc, rc->left + radius, rc->top);
		if(bFill)
		{
			FillBox(hdc, rc->left + radius, rc->top + 1, RECTWP(rc) - radius * 2 , 1);
			FillBox(hdc, rc->left + 1, rc->top + 2, RECTWP(rc) - 2, RECTHP(rc) - 4);
			FillBox(hdc, rc->left + radius, rc->bottom - 2, RECTWP(rc) - radius * 2, 1);
		}
	}
	/*else
	{
#define LINE(hdc, a,b)  do{MoveTo(hdc, a.x, a.y); LineTo(hdc, b.x, b.y); }while(0)
		//left-top corner
		POINT A,B,C,D,E,F,G,H;
		if(flags & NCS_ROUND_CORNER_LEFT_TOP)
		{
			A.x = rc->left + 1;
			A.y = rc->top;
			H.x = rc->left;
			H.y = rc->top + 1;
			LINE(hdc, A,H);
		}
		else{
			A.x = H.x = rc->left;
			A.y = H.y = rc->top;
		}

		if(flags & NCS_ROUND_CORNER_RIGHT_TOP)
		{
			B.x = rc->right - 1;
			B.y = rc->top;
			C.x = rc->right;
			C.y = rc->top + 1;
			LINE(hdc, B,C);
		}
		else {
			B.x = C.x = rc->right;
			B.y = C.y = rc->top;
		}

		if(flags & NCS_ROUND_CORNER_RIGHT_BOTTOM)
		{
			D.x = rc->right;
			D.y = rc->bottom - 1;
			E.x = rc->right - 1;
			E.y = rc->bottom;
			LINE(hdc, D,E);
		}
		else {
			D.x = E.x = rc->right;
			D.y = E.y = rc->bottom;
		}

		if(flags & NCS_ROUND_CORNER_LEFT_BOTTOM)
		{
			F.x = rc->left + 1;
			F.y = rc->bottom;
			G.x = rc->left;
			G.y = rc->bottom - 1;
		}
		else {
			F.x = G.x = rc->left;
			F.y = F.y = rc->bottom;
		}

		LINE(hdc, A,B);
		LINE(hdc, C,D);
		LINE(hdc, E,F);
		LINE(hdc, G,H);

		if(bFill)
		{
			FillBox(hdc, A.x, rc->top + 1, B.x - A.x, 1);
			FillBox(hdc, rc->left + 1, rc->top + 2, RECTWP(rc) - 1, RECTHP(rc) - 3);
			FillBox(hdc, F.x, rc->bottom - 2, E.x - F.x, 1);
		}

#undef LINE
	}*/

}

/* draw a check */
void ncsCommRDRDrawCheckbox(HDC hdc, const RECT* rc, DWORD fgcolor, DWORD bgcolor, DWORD discolor, DWORD flag)
{
	int pen_width;
	int old_pen_width;

	if(rc == NULL)
		return ;

	pen_width = calc_pen_width(rc);
	old_pen_width = SetPenWidth(hdc, pen_width);

	//draw check box 
	//draw filled if it have
	if(flag & NCSRF_FILL) {
		//fill RECT
		gal_pixel old_brush = SetBrushColor(hdc, 
			DWORD2Pixel(hdc, bgcolor));
		FillBox(hdc, rc->left+1, rc->top+1, RECTWP(rc)-1, RECTHP(rc)-1);
		SetBrushColor(hdc, old_brush);
	}

	//draw frame if it have
	if(flag & NCSRF_FRAME)
	{
		gal_pixel old_pen = SetPenColor(hdc,
			ncsColor2Pixel(hdc,fgcolor));
		Rectangle(hdc, rc->left, rc->top, rc->right, rc->bottom);
		SetPenColor(hdc, old_pen);
	}

	//draw status
	if(NCSR_STATUS(flag) 
        & (NCSRS_SELECTED | NCSRS_HIGHLIGHT | NCSRS_SIGNIFICANT))
	{
		int x, y;
		gal_pixel old_pen = SetPenColor(hdc,
			ncsColor2Pixel(hdc, 
				flag&NCSRS_DISABLE?fgcolor:discolor));
		//calc start pos
		x = rc->left + pen_width*2;
		y = (rc->top + rc->bottom) / 2;
		MoveTo(hdc, x, y);
		x = rc->left + RECTWP(rc) / 3; 
		y = rc->bottom - pen_width*2;
		LineTo(hdc, x, y);
		x = rc->right - pen_width*2;
		y = rc->top + pen_width * 2;
		LineTo(hdc, x, y);

		SetPenColor(hdc, old_pen);

	}

	SetPenWidth(hdc, old_pen_width);
}

void ncsCommRDRDrawRadio(HDC hdc,const RECT* rc, DWORD fgcolor, DWORD bgcolor, DWORD discolor, DWORD flag)
{
	int pen_width;
	//int old_pen_width;

	int xo, yo;
	int r;

	if(rc == NULL)
		return ;

	//pen width
	pen_width = calc_pen_width(rc);
	/*old_pen_width = */SetPenWidth(hdc, pen_width);

	xo = (rc->left+rc->right)/2;
	yo = (rc->top+rc->bottom)/2;
	r = (RECTWP(rc)<RECTHP(rc)?RECTWP(rc):RECTHP(rc))/2;

	if(flag & NCSRF_FRAME)
	{
		//draw  frame
		gal_pixel old_pen = SetPenColor(hdc, 
			ncsColor2Pixel(hdc, fgcolor));
		Circle(hdc,xo, yo ,r);
		SetPenColor(hdc, old_pen);
	}

	if(flag & NCSRF_FILL)
	{
		//fill circl
		gal_pixel old_brush = SetBrushColor(hdc,
			ncsColor2Pixel(hdc, bgcolor));
		FillCircle(hdc, xo, yo, r - pen_width);
		SetBrushColor(hdc, old_brush);
	}

	if(NCSR_STATUS(flag) & (NCSRS_HIGHLIGHT|NCSRS_SIGNIFICANT))
	{
		//draw checked
		r -= r/4;
		if(r <= 0)
			r = 1;

		if(r <= 1)
			SetPixel(hdc, xo, yo, ncsColor2Pixel(hdc, fgcolor));
		else{
			gal_pixel old_brush = SetBrushColor(hdc, 
				ncsColor2Pixel(hdc, fgcolor));
			FillCircle(hdc, xo, yo, r);
			SetBrushColor(hdc, old_brush);
		}
	}
}

/*
 *    (top angle) ((x1+x2)/2, y2)
 * +-----*------+ (outer rectangle)
 * |            |
 * |            |
 * |            |
 * |            |
 * *------------*
 *(x1,y1)        (x2,y1)
 *
 *          (x1+x2), y2
 *
 *
 *  (x1,y1)             (x2, y1)
 */
static void draw_top_triangle(HDC hdc, int x1, int x2, int y1, int y2)
{
	int y;
	int xstart, xend;
	int delta ;
	int step = (y1<y2)?1:-1;
	int width;
	int xmid;
	
	if(y1 == y2){
		MoveTo(hdc, x1, y1);
		LineTo(hdc, x2, y1);
		return ;
	}

	if(x1 < x2){
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}

	delta = (((x1<x2?(x2-x1):(x1-x2))<<16)/2) / (y1<y2?(y2 - y1):(y1-y2));
	width = 0; //(((x2-x1)*step) << 16)/2;
	xmid = (x2+x1)/2;

	for(y=y1; y!=y2; y += step)
	{
		int w;
		w = width >>16;
		xstart = xmid - w;
		xend = xmid + w;
		MoveTo(hdc, xstart, y);
		LineTo(hdc, xend, y);
		width += delta;
	}
}

/* 
 *                        (x2, y1)
 *
 *
 *  (x1, (y1+y2)/2)      
 *
 *
 *                        (x2, y2)
 */

static void draw_left_triangle(HDC hdc, int x1, int x2, int y1, int y2)
{
	int x, ystart, yend;
	int height;
	int delta;
	int step;
	int ymid;

	if(x1 == x2)
	{
		MoveTo(hdc, x1, y1);
		LineTo(hdc, x1, y2);
		return ;
	}

	step = x1 < x2 ? 1 : -1;
	height = 1;
	delta = (((y1<y2?(y2-y1):(y1-y2))<<16)/2) / (x1<x2?(x2-x1):(x1-x2));
	ymid = (y1+y2)/2;

	for(x=x1; x!=x2; x+= step)
	{
		int h;
		h = height >> 16;
		ystart = ymid - h;
		yend = ymid + h;
		MoveTo(hdc, x, ystart);
		LineTo(hdc, x, yend);
		height += delta;
	}
}

#define DIV(a,b)  ((((a)<<16)/(b))>>16)
void ncsCommRDRDrawArrow(HDC hdc, const RECT* rc, int arrow, DWORD color, BOOL bFill)
{
	gal_pixel old;
	RECT rctmp;

	if(rc == NULL)
		return ;

/*	if(reduce >= 100)
		rctmp = *rc;
	else*/{
		int w = RECTWP(rc);
		int h = RECTHP(rc);
    	//int vdelta =((w>>2)) >(h/3)?(h/3):((w>>2));
    	//int hdelta =((h>>2)) >(w/3)?(w/3):((h>>2));
		int vdelta = DIV(w,3);
		int hdelta = DIV(h,3);
		if(vdelta > hdelta)
			vdelta = hdelta;
		hdelta = vdelta;
		switch(arrow)
		{
		case NCSR_ARROW_UP:
		case NCSR_ARROW_DOWN:
			vdelta *= 2;// vdelta * 3 / 2;
			break;
		case NCSR_ARROW_LEFT:
		case NCSR_ARROW_RIGHT:
			hdelta *= 2; //hdelta * 3 / 2;
			break;
		}

		rctmp.left = (rc->left + rc->right - vdelta) / 2;
		rctmp.top  = (rc->top + rc->bottom - hdelta) / 2;

		rctmp.right = rctmp.left + vdelta;
		rctmp.bottom = rctmp.top + hdelta;

/*		if(arrow == NCSR_ARROW_UP)
		{
			rctmp.left = rc->left + DIV(w,2) - vdelta;
			rctmp.top  = (rc->top + rc->bottom) / 2 - hdelta/2;
			rctmp.right = rc->left + DIV(w,2) + vdelta;
			rctmp.bottom = rctmp.top + hdelta;
		}
		else if(arrow == NCSR_ARROW_DOWN)
		{
			rctmp.left = rc->left + DIV(w,2) - vdelta;
			rctmp.top  = (rc->top + rc->bottom) / 2 - hdelta/2;
			rctmp.bottom = rctmp.top + hdelta;
			//rctmp.bottom = rc->bottom - DIV(h,3) - 1;
			rctmp.right = rc->left + DIV(w,2) + vdelta;
			//rctmp.top = rctmp.bottom - hdelta;
		}
		else if(arrow == NCSR_ARROW_LEFT)
		{
			rctmp.left = rc->left + DIV(w,3);
			rctmp.right = rctmp.left + hdelta;
			rctmp.top = rc->top + DIV(h,2) - hdelta;
			rctmp.bottom = rctmp.top + hdelta*2;
		}
		else
		{
			rctmp.right = rc->right - DIV(w,3) -1;
			rctmp.left = rctmp.right - hdelta;
			rctmp.top = rc->top + DIV(h,2) - hdelta;
			rctmp.bottom = rctmp.top + hdelta*2;
		}*/
	}

	old = SetPenColor(hdc, ncsColor2Pixel(hdc,color));

	switch(arrow)
	{
		case NCSR_ARROW_LEFT:
			if(bFill)
				draw_left_triangle(hdc, 
					rctmp.left, rctmp.right,
					rctmp.top, rctmp.bottom);
			else
			{
				MoveTo(hdc, rctmp.right, rctmp.top);
				LineTo(hdc, rctmp.left, (rctmp.top+rctmp.bottom)/2);
				LineTo(hdc, rctmp.right, rctmp.bottom);
				LineTo(hdc, rctmp.right, rctmp.top);
			}
			break;
		case NCSR_ARROW_RIGHT:
			if(bFill)
				draw_left_triangle(hdc,
					rctmp.right, rctmp.left,
					rctmp.top, rctmp.bottom);
			else
			{
				MoveTo(hdc, rctmp.left, rctmp.top);
				LineTo(hdc, rctmp.right, (rctmp.top+rctmp.bottom)/2);
				LineTo(hdc, rctmp.left, rctmp.bottom);
				LineTo(hdc, rctmp.left, rctmp.top);
			}
			break;
		case NCSR_ARROW_UP:
			if(bFill)
				draw_top_triangle(hdc,
					rctmp.left, rctmp.right,
					rctmp.top, rctmp.bottom);
			else
			{
				MoveTo(hdc, rctmp.left, rctmp.bottom);
				LineTo(hdc, (rctmp.right+rctmp.left)/2, rctmp.top);
				LineTo(hdc, rctmp.right, rctmp.bottom);
				LineTo(hdc, rctmp.left, rctmp.bottom);
			}
			break;
		case NCSR_ARROW_DOWN:
			if(bFill)
				draw_top_triangle(hdc,
					rctmp.left, rctmp.right,
					rctmp.bottom, rctmp.top);
			else
			{
				MoveTo(hdc, rctmp.left, rctmp.top);
				LineTo(hdc, (rctmp.left+rctmp.right)/2, rctmp.bottom);
				LineTo(hdc, rctmp.right, rctmp.top);
				LineTo(hdc, rctmp.left, rctmp.top);
			}
			break;
	}

	SetPenColor(hdc, old);
}

void ncsCommRDRDrawItem(HDC hdc, const RECT* rc,DWORD color)
{
	gal_pixel old;
	if(rc == NULL)
		return ;
	
	old = SetBrushColor(hdc, ncsColor2Pixel(hdc, color));
	FillBox(hdc, rc->left, rc->top, RECTWP(rc), RECTHP(rc));
	SetBrushColor(hdc, old);
}

struct ThreeDEllipseDrawInfo {
	Uint8 *dc_mem;
	int byte_per_pixel;
	int pitch;
	int left;
	int top;
	int x0;     //center
	int y0; 
	gal_pixel up_color1;
	gal_pixel up_color2;
	gal_pixel down_color1;
	gal_pixel down_color2;
	void (*copy_pixel)(Uint8* pstart, gal_pixel color);
};

static void copy_8_pixel(Uint8* pstart, gal_pixel color)
{
	*pstart = (Uint8)color;
}

static void copy_16_pixel(Uint8* pstart, gal_pixel color)
{
#if 0
	*((Uint16*)pstart) = (Uint16)(color);
	//pstart[0] = (Uint8)((color&0xFF00)>>8);
	//pstart[1] = (Uint8)(color&0xFF);
#else
    memcpy(pstart, &color, sizeof(Uint16));
#endif
}

static void copy_24_pixel(Uint8* pstart, gal_pixel color)
{
	pstart[0] = (Uint8)((color&0xFF0000)>>16);
	pstart[1] = (Uint8)((color&0xFF00)>>8);
	pstart[2] = (Uint8)((color&0xFF));
}

static void copy_32_pixel(Uint8* pstart, gal_pixel color)
{
#if 0
    //*((Uint32*)pstart) = (Uint32)(color);
    
    pstart[0] = (Uint8)((color&0xFF000000)>>24);
    pstart[1] = (Uint8)((color&0xFF0000)>>16);
    pstart[2] = (Uint8)((color&0xFF00)>>8);
    pstart[3] = (Uint8)(color&0xFF);
#else   
    memcpy(pstart, &color, sizeof(Uint32));
#endif
}

static inline int int_abs(int a)
{
	return a>0?a:-a;
}

static inline Uint8* offset_pixel(Uint8* p, int x, int y, struct ThreeDEllipseDrawInfo * pdi)
{
	int xdiff, ydiff;
	xdiff = int_abs(x - pdi->x0);
	ydiff = int_abs(y - pdi->y0);
	if(xdiff < ydiff)
		return p + pdi->pitch;
	return p + pdi->byte_per_pixel;
}

static inline gal_pixel * get_color(int x, int y, int t, struct ThreeDEllipseDrawInfo* pdi)
{
	return (x+y)>t?(&pdi->up_color1):(&pdi->down_color1);
}

static void _3dbox_ellipse_set(void* context, int x1, int x2, int y)
{
	Uint8 *p1;
	Uint8 *p2;
	int t;
	gal_pixel *colors1, *colors2;
	struct ThreeDEllipseDrawInfo* pdi = (struct ThreeDEllipseDrawInfo*)context;
#if 0   
    if(x1 > pdi->x1 || y > pdi->y1)
        return ;
#endif
    p1 = pdi->dc_mem + ( y - pdi->top) * pdi->pitch + (x1 - pdi->left)* pdi->byte_per_pixel;
    p2 = p1 + (x2 - x1) * pdi->byte_per_pixel;
	
    t = pdi->x0 + pdi->y0;
	colors1 = get_color(x1, y, t, pdi);
	pdi->copy_pixel(p1, colors1[0]);
	p1 = offset_pixel(p1, x1, y, pdi);
	pdi->copy_pixel(p1, colors1[1]);
#if 0   
    if(x2 > pdi->x1)
        return;
#endif
    colors2 = get_color(x2, y, t, pdi);
	pdi->copy_pixel(p2, colors2[0]);
	p2 = offset_pixel(p2, x2, y, pdi);
	pdi->copy_pixel(p2, colors2[1]);
}

void ncsCommRDRDraw3DEllipse(HDC hdc, const RECT *rc, DWORD color1, DWORD color2, DWORD flag)
{
	HDC cpt_dc;
	int x0, y0;
	int rx, ry;
	DWORD dark_color;
	DWORD light_color;
	struct ThreeDEllipseDrawInfo drawInfo;

	if(rc == NULL)
		return ;
	
    drawInfo.byte_per_pixel = GetGDCapability(hdc,GDCAP_BPP);

    cpt_dc = CreateCompatibleDCEx(hdc, RECTWP(rc)+2, RECTHP(rc)+2);
    BitBlt (hdc, rc->left, rc->top, RECTWP(rc)+2, RECTHP(rc)+2, cpt_dc, 0, 0, 0);
	drawInfo.dc_mem = LockDC(cpt_dc, NULL, NULL, NULL, &drawInfo.pitch);
    if(drawInfo.dc_mem == NULL) {
        DeleteCompatibleDC(cpt_dc);
        return ;
    }
	
    x0 = (rc->left+rc->right)/2;
	y0 = (rc->top+rc->bottom)/2;
	rx = RECTWP(rc)/2;
	ry = RECTHP(rc)/2;

	drawInfo.up_color1 = ncsColor2Pixel(hdc, color1);
	drawInfo.down_color1 = ncsColor2Pixel(hdc, color2);

	if((flag & 0xFFFF) == NCSRS_SIGNIFICANT)
	{
		dark_color = ncsCommRDRCalc3dboxColor(color1, NCSR_COLOR_DARKER);
		light_color = ncsCommRDRCalc3dboxColor(color2, NCSR_COLOR_LIGHTEST);
		drawInfo.up_color2 = ncsColor2Pixel(hdc, dark_color);
		drawInfo.down_color2 = ncsColor2Pixel(hdc, light_color);
	}
	else
	{
		light_color = ncsCommRDRCalc3dboxColor(color1, NCSR_COLOR_LIGHTEST);
		dark_color = ncsCommRDRCalc3dboxColor(color2, NCSR_COLOR_DARKER);
		drawInfo.up_color2 = ncsColor2Pixel(hdc, light_color);
		drawInfo.down_color2 = ncsColor2Pixel(hdc, dark_color);
	}

	drawInfo.left = rc->left;
	drawInfo.top = rc->top;
	drawInfo.x0 = x0;
	drawInfo.y0 = y0;

	switch(drawInfo.byte_per_pixel)
	{
	case 2:
		drawInfo.copy_pixel = copy_16_pixel;
		break;
	case 3:
		drawInfo.copy_pixel = copy_24_pixel;
		break;
	case 4:
		drawInfo.copy_pixel = copy_32_pixel;
		break;
	default:
		drawInfo.copy_pixel = copy_8_pixel;
		break;
	}
    
	EllipseGenerator((void*)&drawInfo, x0, y0, rx, ry,(CB_ELLIPSE)_3dbox_ellipse_set);

	UnlockDC(cpt_dc);
    BitBlt (cpt_dc, 0, 0, 0, 0, hdc, rc->left, rc->top, 0);
    DeleteCompatibleDC(cpt_dc);
}


#define UNDEFINED_HUE   (itofix(-1)) 
static void RGB2HSV(fixed r, fixed g, fixed b, fixed* h, fixed* s, fixed* v)
{
    fixed min; 
    fixed max; 
    fixed delta;
    fixed tmp;

    /*change r g b to [0, 1]*/
    r = fixdiv (r, itofix(255));
    g = fixdiv (g, itofix(255));
    b = fixdiv (b, itofix(255));

    tmp = MIN(r, g);
    min = MIN(tmp, b);
    tmp = MAX(r, g);
    max = MAX(tmp, b);

    *v = max; // v
    delta = max - min;

    if (max != 0)
        *s = fixdiv(delta, max);
    else {
        *s = 0;
        *h = UNDEFINED_HUE;
        return;
    }

    if (fixtof(delta) == 0)
    {
        *h = 0;
        *s = 0;
        return;
    }

    if (r == max)
        /*between yellow & magenta*/
        *h = fixdiv(fixsub(g, b), delta);
    else if( g == max )
        /*between cyan & yellow*/
        *h = fixadd(itofix(2), fixdiv(fixsub(b, r), delta));
    else 
        /*magenta & cyan*/
        *h = fixadd(itofix(4), fixdiv(fixsub(r, g), delta));

    /*degrees*/
    *h = fixmul(*h, itofix(60)); 

    if (*h < itofix(0))
        *h = fixadd(*h, itofix(360));
}

/*
 * HSV2RGB:
 *      This function translate color in HSV space to in RGB space.
 * Author: XuguangWang
 * Date: 2007-11-22
 */
static void HSV2RGB(fixed h, fixed s, fixed v, fixed* r, fixed* g, fixed* b)
{
    int i;
    fixed f, p, q, t;

    if (s == 0) {
        *r = fixmul(v, itofix(255));
        *g = fixmul(v, itofix(255));
        *b = fixmul(v, itofix(255));
        return;
    }

    /*sector 0 to 5*/
    h = fixdiv(h, itofix(60));
    i = (h >> 16) % 6;
    /*factorial part of h*/
    f = fixsub(h, itofix(i));
    p = fixmul(v, fixsub(itofix(1), s));
    /*q = v * (1 - s*f)*/
    q = fixmul (v, fixsub (itofix(1), fixmul(s, f)));
   
    /*t = v * (1 - s*(1-f))*/
    t = fixmul (v, fixsub (itofix(1), fixmul(s, fixsub( itofix(1), f))));
    
    switch (i) {
        case 0: 
            *r = fixmul (v, itofix(255)); 
            *g = fixmul (t, itofix(255)); 
            *b = fixmul (p, itofix(255));
            break;
        case 1:
            *r = fixmul (q, itofix(255));
            *g = fixmul (v, itofix(255));
            *b = fixmul (p, itofix(255));
            break;
        case 2:
            *r = fixmul (p, itofix(255));
            *g = fixmul (v, itofix(255));
            *b = fixmul (t, itofix(255));
            break;
        case 3:
            *r = fixmul (p, itofix(255));
            *g = fixmul (q, itofix(255));
            *b = fixmul (v, itofix(255));
            break;
        case 4:
            *r = fixmul (t, itofix(255));
            *g = fixmul (p, itofix(255));
            *b = fixmul (v, itofix(255));
            break;
        case 5:
            *r = fixmul (v, itofix(255));
            *g = fixmul (p, itofix(255));
            *b = fixmul (q, itofix(255));
            break;
    }
}

DWORD ncsCommRDRCalc3dboxColor(DWORD color, int degree)
{
 	fixed h;
    fixed s;
    fixed v;
    fixed r = itofix(GetRValue(color));
    fixed g = itofix(GetGValue(color));
    fixed b = itofix(GetBValue(color));
    UINT  a = GetAValue(color);
	BOOL  darker = degree < 0; 
    /*RGB => HSV*/
    RGB2HSV(r, g, b, &h, &s, &v);

	if(degree < 0) degree = -degree;
	if(degree == 0)
		return color;
	else if(degree > 255)
		degree = 255;

	if(darker)
	{
		v = fixsub(v, degree * v / 255);
		if (v < 0)
			v = 0;
	}
	else
	{
		if( v > itofix(1))
			v = itofix(1);
		else
			v = fixadd(v, degree * v / 255);

		if( v >= itofix(1))
		{
			v = itofix(1);
			s = fixsub(s, degree * s/ 255);
			if (s < 0)
				s = 0;
		}
	}

    HSV2RGB(h, s, v, &r, &g, &b);

	return MakeRGBA(fixtoi(r), fixtoi(g), fixtoi(b), a);

}

/* 
 * disabled_text_out:
 *
 * This function outputs disabled text.
 *
 * Author: XiaoweiYan
 * Date: 2007-12-04
 */

void 
ncsDisabledTextOut(HWND hWnd, HDC hdc, const char* spText, PRECT rc, DWORD dt_fmt)
{
    DWORD mainc, color;
    mWidget *self;
    
    SetBkMode (hdc, BM_TRANSPARENT);
    SetBkColor (hdc, GetWindowBkColor (hWnd));

    rc->left+=1;
    rc->top+=1;
    rc->right+=1;
    rc->bottom+=1;

    self = ncsObjFromHandle(hWnd);
    mainc = ncsGetElement(self, NCS_BGC_3DBODY);

	color = ncsCommRDRCalc3dboxColor(mainc, LFRDR_3DBOX_COLOR_LIGHTER);
    SetTextColor (hdc,
            RGBA2Pixel (hdc, GetRValue (color), GetGValue (color), 
                GetBValue (color), GetAValue (color)));
    DrawText (hdc, spText, -1, rc, dt_fmt);

    rc->left-=1;
    rc->top-=1;
    rc->right-=1;
    rc->bottom-=1;

	color = ncsCommRDRCalc3dboxColor(mainc, LFRDR_3DBOX_COLOR_DARKER);
    SetTextColor (hdc,
            RGBA2Pixel (hdc, GetRValue (color), GetGValue (color), 
                GetBValue (color), GetAValue (color)));
    DrawText (hdc, spText, -1, rc, dt_fmt);
}

///////////////////////////////////////////////////////
//

#define LEN_CLASS_NAME 63 

/////////////////////////////////////////////////////////////////////////////
typedef struct {
    UT_hash_handle hh;
    char str[32];
    int id;
}we_attr_id_t;

/////////////
typedef struct {
    UT_hash_handle hh;
    int id;
    char clsname[LEN_CLASS_NAME + 1];
    DWORD value;
}ncs_hash_table_t;

typedef struct {
    int  id;
    char clsname[LEN_CLASS_NAME + 1];
}ncs_lookup_key_t;

typedef struct _ncsRdrHashTable ncsRdrHashTable;
struct _ncsRdrHashTable {
    char name[LEN_RENDERER_NAME + 1];
	DWORD name_key;
    ncs_hash_table_t *hash; 
    ncsRdrHashTable * next;   
};

static ncsRdrHashTable *rdr_hash_table = NULL;

#define KEYLEN(ptr) \
    (offsetof(ncs_hash_table_t, clsname) \
    + strlen(ptr.clsname) - offsetof(ncs_hash_table_t, id))

static GHANDLE hRdrEtcFile = 0;

static NCS_RDR_ID_NAME config_str[]=
{
    {"fgc_window",              NCS_FGC_WINDOW}, 
    {"bgc_window",              NCS_BGC_WINDOW}, 
    {"fgc_3dbody",              NCS_FGC_3DBODY}, 
    {"bgc_3dbody",              NCS_BGC_3DBODY}, 

    {"metrics_border_width",    NCS_METRICS_BORDER_WIDTH}, 
    {"metrics_border_radius",   NCS_METRICS_BORDER_RADIUS}, 
    {"metrics_3dbody_roundx",   NCS_METRICS_3DBODY_ROUNDX}, 
    {"metrics_3dbody_roundy",   NCS_METRICS_3DBODY_ROUNDY}, 

    {"fgc_selected_item",       NCS_FGC_SELECTED_ITEM}, 
    {"bgc_selected_item",       NCS_BGC_SELECTED_ITEM}, 

    {"fgc_hilight_item",        NCS_FGC_HILIGHT_ITEM}, 
    {"bgc_hilight_item",        NCS_BGC_HILIGHT_ITEM}, 

    {"fgc_disabled_item",       NCS_FGC_DISABLED_ITEM}, 
    {"bgc_disabled_item",       NCS_BGC_DISABLED_ITEM}, 
    
    {"bgc_selected_nofocus",    NCS_BGC_SELITEM_NOFOCUS}, 

    {"bgc_pb_chunk",            NCS_BGC_PRGBAR_CHUNK}, 

    {"bgc_tb_slider",           NCS_BGC_TRKBAR_SLIDER}, 

//    {"bgc_tb_thumb",            NCS_BGC_TRKBAR_THUMB}, 
    {"bgc_normal_tab",          NCS_BGC_PRPSHT_NMLTAB}, 

    {"mode_bgc",                NCS_MODE_BGC},
    {"mode_bkimage",            NCS_MODE_BKIMAGE},
    {"mode_fgc",                NCS_MODE_FGC},

	{"mode_item",               NCS_MODE_ITEM},

 /*   {"mode_selected_item",      NCS_MODE_SELECTED_ITEM},
    {"mode_hilight_item",       NCS_MODE_HILIGHT_ITEM},
    {"mode_disabled_item",      NCS_MODE_DISABLED_ITEM},
*/
    {"mode_useflat",            NCS_MODE_USEFLAT},

    {"image_bkgnd",             NCS_IMAGE_BKGND},
    {"image_arrows",            NCS_IMAGE_ARROWS},
    {"image_arrowshell",        NCS_IMAGE_ARROWSHELL},

    {"image_pushbutton",        NCS_IMAGE_BUTTON},
    {"image_checkbutton",       NCS_IMAGE_CHKBTN},
    {"image_radiobutton",       NCS_IMAGE_RDOBTN},

    {"image_tree",              NCS_IMAGE_LISTV_TREE},
    {"image_header",            NCS_IMAGE_LISTV_HDR},
    {"image_tab",               NCS_IMAGE_PRPSHT_TAB},

    {"image_pb_hchunk",         NCS_IMAGE_PRGBAR_HCHUNK},
    {"image_pb_vchunk",         NCS_IMAGE_PRGBAR_VCHUNK},
    {"image_pb_hslider",        NCS_IMAGE_PRGBAR_HSLIDER},
    {"image_pb_vslider",        NCS_IMAGE_PRGBAR_VSLIDER},

    {"image_tb_hslider",        NCS_IMAGE_TRKBAR_HSLIDER},
    {"image_tb_vslider",        NCS_IMAGE_TRKBAR_VSLIDER},
    {"image_tb_hthumb",         NCS_IMAGE_TRKBAR_HTHUMB},
    {"image_tb_vthumb",         NCS_IMAGE_TRKBAR_VTHUMB},
};

static we_attr_id_t  *macro_idhash = NULL;

static void _init_macro_idtable(void)
{
    we_attr_id_t *msg;
    int i;

    if (macro_idhash)
        return;

    for (i = 0; i < sizeof(config_str)/sizeof(NCS_RDR_ID_NAME); i++) {
        msg = (we_attr_id_t*)calloc(1, sizeof(we_attr_id_t));
        memcpy(msg->str, config_str[i].we_name, strlen(config_str[i].we_name) + 1);
        msg->id = (int)config_str[i].we_id;
        HASH_ADD_STR(macro_idhash, str, msg);
    }
}

static void _destroy_macro_idtable(void)
{
    if (macro_idhash) {
        we_attr_id_t *tmp, *del = macro_idhash;
        while (del) {
            tmp = del->hh.next;
            HASH_DEL(macro_idhash, del);
            free(del);
            del = tmp;
        }
    }
    macro_idhash = NULL;
}

static int _find_macro_id(const char* str)
{
	we_attr_id_t *msg;

    if (!str)
        return -1;

    HASH_FIND_STR(macro_idhash, str, msg);
    if (msg) {
        return msg->id;
    }
    return -1;
}

static ncs_hash_table_t* _find_element(ncs_hash_table_t *hash, 
        int id, const char* clsname)
{
    ncs_lookup_key_t lookup_key;
    ncs_hash_table_t *msg = NULL;
    
    if (!hash)
        return NULL;

    memset (&lookup_key, 0, sizeof(lookup_key));
    lookup_key.id = id;
    if (clsname)
        sprintf (lookup_key.clsname, "%s", clsname);

    HASH_FIND(hh, hash, &(lookup_key.id), KEYLEN(lookup_key), msg);
    return msg;
}

#ifdef _DEBUG
static void dump_hash_table (ncs_hash_table_t *table)
{
    ncs_hash_table_t *tmp, *msg;
    for (tmp = table; tmp != NULL; tmp = tmp->hh.next) {
        msg = _find_element(table, tmp->id, tmp->clsname);
        if (msg) {
            _DBG_PRINTF ("mGNCS>RDR: id: 0x%0x, clsname: %s, value: %lx \n", 
                msg->id, msg->clsname, msg->value);
        }
    }
}
#endif

static ncsRdrHashTable* _get_hash_table(const char* rdrname, BOOL bNew)
{
    ncsRdrHashTable * rdr_ht;
	DWORD key;

    if (!rdrname)
        return NULL;

	key = Str2Key(rdrname);

    for (rdr_ht = rdr_hash_table; rdr_ht; rdr_ht = rdr_ht->next) {
		if(rdr_ht->name_key == key){
            return rdr_ht;
        }
    }

    if(bNew){
        ncsRdrHashTable * ht = (ncsRdrHashTable*)calloc(1, sizeof(ncsRdrHashTable));
        if(rdr_hash_table) {
            rdr_ht = rdr_hash_table;
            while(rdr_ht->next)
                rdr_ht = rdr_ht->next;
            rdr_ht->next = ht;
        }
        else
            rdr_hash_table = ht;
        strncpy(ht->name, rdrname, LEN_RENDERER_NAME);
        ht->name_key = Str2Key(ht->name);
        return ht;
    }

    return NULL;
}

#define IS_VALID(value) (strncmp (value, "none", 5) != 0)
static int _register_image_res(const char* image_name)
{
    char bmpfile[MAX_NAME+1];

    if (!image_name)
        return -1;

    sprintf (bmpfile, "bmp/%s", image_name);

    if (IS_VALID(bmpfile) && !RegisterResFromFile(HDC_SCREEN, bmpfile))
        return -1;

    return Str2Key(bmpfile);
}

static void _load_rdr_hash_table (ETC_S *pEtc, const char* rdrname, NCS_RDR_ID_NAME * user_id_names)
{
    PETCSECTION psect;
    int i = 0, j = 0, id;
    char name[32];
    char *clsname;
    ncs_hash_table_t *msg;
    ncsRdrHashTable *table = NULL;

    sprintf (name, "%s.", rdrname);

    table = _get_hash_table(rdrname, TRUE);
    if (!table)
        return;

    psect = (PETCSECTION)FindSectionInEtc((GHANDLE)pEtc, rdrname, FALSE);
    if (psect == NULL)
        return;

    j = 0;
    while(psect != pEtc->sections + pEtc->section_nr) {
        if (j!=0 && strncmp(psect->name, name, strlen(name)))
            break;

        clsname = psect->name + strlen(name);

        //keys, values, name
        for (i = 0; i < psect->key_nr; i++) {
            id = _find_macro_id(psect->keys[i]);
            if (id == -1) {
                if(user_id_names)
                {
                    int j;
                    for(j = 0; user_id_names[j].we_name; j ++)
                    {
                        if(strcmp(psect->keys[i], user_id_names[j].we_name) == 0)
                            break;
                    }

                    if(user_id_names[j].we_id == -1)
                        continue;
                    id = user_id_names[j].we_id;
                }
                else
                {
                    continue;
                }
            }
            msg = (ncs_hash_table_t*)calloc(1, sizeof(ncs_hash_table_t));
            msg->id = id;

            if ((msg->id & NCS_TYPE_MASK) == NCS_TYPE_IMAGE) {
                msg->value = _register_image_res(psect->values[i]);
            }
            else
                msg->value = strtoul(psect->values[i], NULL, 0);


            if (msg->value == -1) {
                free(msg);
                continue;
            }

            if (j!=0)
                sprintf (msg->clsname, "%s%s", clsname, _MGNCS_CLASS_SUFFIX);

            HASH_ADD(hh, (table->hash), id, KEYLEN((*msg)), msg);
        }
        psect++;
        j++;
    }

#ifdef _DEBUG
    dump_hash_table(table->hash);
#endif
}

DWORD ncsGetElementEx(mWidget* self, const char* rdrname, int id)
{
    char *clsname = NULL;
    ncs_hash_table_t *msg, *hash = NULL;
    const WINDOW_ELEMENT_RENDERER *we_rdr;
    DWORD value;
    ncsRdrHashTable *table;

    if ((id & 0xFF00) < NCS_TYPE_METRICS) {
        return GetWindowElementAttr(self? self->hwnd : HWND_NULL, id);
    }
    else {
        if (self) {
            value = GetWindowElementAttr(self->hwnd, id);

            if(value != INVALID_RDR_VALUE)
                return value;

            we_rdr = GetWindowInfo(self->hwnd)->we_rdr;
            //get clsname
            clsname = (char*)_c(self)->className;
        }
        else {
            if (rdrname)
                we_rdr = GetWindowRendererFromName(rdrname);
            else 
                we_rdr = GetDefaultWindowElementRenderer();
        }

        if (!we_rdr)
            return -1;

        table = _get_hash_table(we_rdr->name, FALSE);

        hash = table ? table->hash : NULL;
        if (!table || !hash)
            return -1;
		
        msg = _find_element(hash, id, clsname);
        if (msg) {
            return msg->value;
        }

        msg = _find_element(hash, id, NULL);
        if (msg) {
            return msg->value;
        }

        return -1;
    }
}

DWORD ncsSetElementEx(mWidget* self, const char* rdrname, int id, DWORD value)
{
    ncs_hash_table_t *msg, *hash = NULL;
    ncs_lookup_key_t lookup_key;
    const WINDOW_ELEMENT_RENDERER *we_rdr;
    ncsRdrHashTable *table;

    if (self) {
        return SetWindowElementAttr(self->hwnd, id, value);
    }

    if ((id & 0xFF00) < NCS_TYPE_METRICS)
        return SetWindowElementAttr(HWND_NULL, id, value);
    else {
        if (rdrname)
            we_rdr = GetWindowRendererFromName(rdrname);
        else
            we_rdr = GetDefaultWindowElementRenderer();

        if (!we_rdr)
            return -1;

        table = _get_hash_table(we_rdr->name, FALSE);
        hash = table ? table->hash : NULL;
        if (!table || !hash)
            return -1;

        memset (&lookup_key, 0, sizeof(lookup_key));
        lookup_key.id = id;
        HASH_FIND(hh, hash, &(lookup_key.id), KEYLEN(lookup_key), msg);
        if (msg) {
            msg->value = value;
        }

        return -1;
    }
}

#ifndef __NOUNIX__
#    include <unistd.h>
#    include <pwd.h>
#endif

#ifdef WIN32
#include <sys/stat.h>
#endif
static char ncsRdrEtcFile [MAX_PATH + 1];

static BOOL _lookup_etcfile (const char* filename)
{
    char etcfile [MAX_PATH + 1];
    FILE* fp = NULL;
	char * env_path;
	struct passwd *pwd;

	//try from NCS_CFG_PATH
	if((env_path = getenv("NCS_CFG_PATH"))) {
		sprintf(etcfile, "%s/%s", env_path,filename);
		fp = fopen(etcfile,"r");
		if(fp){
			fclose(fp);
			strcpy(ncsRdrEtcFile, etcfile);
			return TRUE;
		}
	}

    if (!getcwd (etcfile, MAX_PATH))
        return FALSE;
    strcat (etcfile, "/");
    strcat (etcfile, filename);

    fp = fopen(etcfile, "r");
    if (fp) {
        fclose (fp);
        strcpy (ncsRdrEtcFile, etcfile);
        return TRUE;
    }
#ifndef __NOUNIX__
    if ((pwd = getpwuid (geteuid ())) != NULL) {
        strcpy (etcfile, pwd->pw_dir);
        if (etcfile [strlen (etcfile) - 1] != '/')
            strcat (etcfile, "/");
        strcat (etcfile, ".");
        strcat (etcfile, filename);

        fp = fopen(etcfile, "r");
        if (fp) {
            fclose (fp);
            strcpy (ncsRdrEtcFile, etcfile);
            return TRUE;
        }
    }

    sprintf (etcfile,"/usr/local/etc/%s", filename);  

    fp = fopen(etcfile, "r");
    if (fp) {
        strcpy (ncsRdrEtcFile, etcfile);
        fclose (fp);
        return TRUE;
    }

    sprintf (etcfile,"/etc/%s", filename);  
    fp = fopen(etcfile, "r");
    if (fp) {
        strcpy (ncsRdrEtcFile, etcfile);
        fclose (fp);
        return TRUE;
    }

    return FALSE;

#elif defined(WIN32)
    fp = fopen(etcfile, "r");
    if (fp) {
        strcpy (ncsRdrEtcFile, etcfile);
        fclose (fp);
        return TRUE;
    }

	sprintf(etcfile, "c:\\%s", filename);
	 fp = fopen(etcfile, "r");
    if (fp) {
        strcpy (ncsRdrEtcFile, etcfile);
        fclose (fp);
        return TRUE;
    }
	return FALSE;
#endif
    
    if (fp) {
        strcpy (ncsRdrEtcFile, etcfile);
        fclose (fp);
        return TRUE;
    }

    return FALSE;
}

GHANDLE ncsLoadRdrEtcFile (const char* etcfile, const char** rdrname, int count)
{
    ETC_S *pEtc;
    int i = 0;

    if(!g_ncsEtcHandle)
    {
        if (!etcfile || !rdrname || count <=0)
            return 0;

        //init macro<-->id table
        _init_macro_idtable();

        if (hRdrEtcFile == 0) {
            if (_lookup_etcfile(etcfile)) {
                hRdrEtcFile = LoadEtcFile(ncsRdrEtcFile);
            }
        }
    }
    else
    {
        _init_macro_idtable();
        hRdrEtcFile = g_ncsEtcHandle;
    }

    if (hRdrEtcFile == 0)
        return 0;

    pEtc = (ETC_S*)hRdrEtcFile;

    for (i = 0; i < count; i++) {
        _load_rdr_hash_table(pEtc, rdrname[i], NULL);
    }

    return hRdrEtcFile;
}


int ncsUnloadRdrEtcFile(GHANDLE hEtcFile)
{
    ncs_hash_table_t *hash = NULL;
    ncsRdrHashTable * rdr_ht;

    //release all 
    rdr_ht = rdr_hash_table; 
    while(rdr_ht){
        ncsRdrHashTable * rdr_tmp = rdr_ht;
        hash = rdr_ht->hash;
        if (hash) {
            ncs_hash_table_t *del= hash, *tmp;
            while (del) {
                tmp = del->hh.next;
                HASH_DEL(hash, del);
                free(del);
                del = tmp;
            }
        }
        rdr_ht = rdr_ht->next;
        free(rdr_tmp);
    }
    rdr_hash_table = NULL;

    _destroy_macro_idtable();
    hRdrEtcFile = 0;

    if(hEtcFile)
        return UnloadEtcFile(hEtcFile);

    rdr_hash_table = NULL;
    return 0;
}

MGNCS_EXPORT BOOL ncsLoadRdrElementsFromEtcFile(const char* etcfile, 
        const char** rdrNames, int count, NCS_RDR_ID_NAME *id_names)
{
    GHANDLE hEtc;
    BOOL bRet;
    if(!etcfile || !rdrNames || count < 0)
        return FALSE;

    hEtc = LoadEtcFile(etcfile);
    if(!hEtc)
        return FALSE;
    
    bRet = ncsLoadRdrElementsFromEtcHandle(hEtc, rdrNames,count, id_names);
    UnloadEtcFile(hEtc);
    return bRet;
}

MGNCS_EXPORT BOOL ncsLoadRdrElementsFromEtcHandle(GHANDLE hEtc, const char** rdrNames, int count, NCS_RDR_ID_NAME *id_names)
{
    int i;
    if(!hEtc || !rdrNames || count <= 0)
        return FALSE;
    
    _init_macro_idtable();
    for(i = 0; i < count; i ++)
    {
        if(rdrNames[i])
            _load_rdr_hash_table((ETC_S*)hEtc, rdrNames[i], id_names);
    }
    return count > 0;
}

