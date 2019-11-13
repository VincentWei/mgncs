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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */

#ifndef FASHION_COMMON_H
#define FASHION_COMMON_H

#if defined(WIN32) && defined(_MGRM_STANDALONE)
#include <pthread.h>
#endif

typedef struct _CacheSlot{
	void ** slots;
	int max;
	int free_cursor;
#ifdef _MGRM_THREAD
	pthread_mutex_t mutex;
#endif
	
	void * (*newObject)(void);
	void (*deleteObject)(void*);
	void (*cleanObject)(void*);
}CacheSlot;

CacheSlot * createCacheSlot(int max, 
	void*(*newObject)(void),
	void(*deleteObject)(void*), 
	void(*cleanObject)( void*));

void * getFreeObject(CacheSlot* slots);

void returnObject(CacheSlot* slots, void *obj);

void freeCacheSlot(CacheSlot* slots);

BOOL init_fashion_common(void);

void uninit_fashion_common(void);

#define MAX_BRUSH_SLOT  8

HBRUSH fashion_getBrush(void);
void fashion_retBrsuh(HBRUSH hbrush);

#define MAX_GRAPHICS_WIDTH  600
#define MAX_GRAPHICS_HEIGHT 600
HGRAPHICS fashion_getGraphics(int width, int height);
void fashion_retGraphics(HGRAPHICS hgraphics);

#define MPMakeARGB(r, g, b, a)    (((DWORD)((BYTE)(b))) | ((DWORD)((BYTE)(g)) << 8) \
	| ((DWORD)((BYTE)(r)) << 16) | ((DWORD)((BYTE)(a)) << 24))

static inline ARGB toARGB(DWORD color)
{
	return MPMakeARGB(GetRValue(color), GetGValue(color), GetBValue(color), GetAValue(color));
}

BOOL fashion_initLinerGrandientBrush(HBRUSH, DWORD color1, DWORD color2, int mode);

BOOL fashion_fillRoundRect(HDC hdc, DWORD color1, DWORD color2, int mode, 
	const RECT * rt, int rx, int ry);

BOOL fashion_fillTriangle(HDC hdc, DWORD color1, DWORD color2,
	int mode, const RECT *rtTrangle, int direction);

BOOL fashion_drawArrow(HDC hdc, 
		DWORD *shellColors, int shellMode, 
		DWORD* arrowColors, int arrowMode, 
		const RECT *rtTrangle, int rx, int ry, int direction);

BOOL fashion_getTriangleFromRect(const RECT *prc, RECT *prcTrangle, int arrow);


static inline void fashion_regular_rect(RECT *rt, BOOL bkeep_center)
{
	int w = RECTWP(rt);
	int h = RECTHP(rt);
	if(w > h)
	{
		if(bkeep_center)
			rt->left += (w-h)/2;

		rt->right = rt->left + h;
	}
	else if( w < h)
	{
		if(bkeep_center)
			rt->top += (h-w)/2;

		rt->bottom = rt->top + w;
	}
}

static inline void fashion_scale_rect(RECT *rt, int xscale, int yscale)
{
	rt->left += xscale;
	rt->right -= xscale;
	rt->top += yscale;
	rt->bottom -= yscale;
}

static inline void fashion_drawRoundRect(HDC hdc, const RECT *pRect, int rx, int ry, DWORD color)
{
	gal_pixel old = SetPenColor(hdc, ncsColor2Pixel(hdc, color));
	//rectangle
	if(rx > 0 && ry > 0)
		RoundRect(hdc, pRect->left, pRect->top, 
				pRect->right, pRect->bottom,
				rx, ry);
	else
		Rectangle(hdc, pRect->left, pRect->top,
			pRect->right, pRect->bottom);
	SetPenColor(hdc, old);
}

void fashion_draw_middle_gradient_rect(HGRAPHICS hbraphics, 
	HPATH hpath, 
	HBRUSH hbrush, 
	const RECT *rt, int rx, int ry,
	DWORD color1, DWORD color2, int mode);


BOOL fashion_fillEllipse(HDC hdc, DWORD color1, DWORD color2, int mode, const PRECT rt);

#define INIT_MGPLUGS_ENV(hdc, color1, color2, mode, rt) { \
	HBRUSH hbrush = 0; \
	HGRAPHICS hgraphics = 0; \
	HPATH hpath = 0; \
	hbrush = fashion_getBrush(); \
	hgraphics = fashion_getGraphics(RECTWP(rt), RECTHP(rt)); \
	hpath = MGPlusPathCreate (MP_PATH_FILL_MODE_WINDING); \
	if(!hbrush || !hgraphics || !hpath) \
		goto FAILED; \
	do{ RECT __rtmp ={ 0, 0, RECTWP(rt), RECTHP(rt)}; \
	MGPlusSetLinearGradientBrushRect (hbrush, (RECT*)(&__rtmp));}while(0); \
	/*filled color*/ \
	if(!fashion_initLinerGrandientBrush(hbrush, color1, color2, mode)) \
		goto FAILED; \
	MGPlusGraphicCopyFromDC(hgraphics, hdc,  \
		rt->left, rt->top, RECTWP(rt), RECTHP(rt), \
		0, 0);

void fashion_retBrush(HBRUSH hbrush);
#define END_MGPLUGS_ENV_NO_FILL_PATH(hdc, rt) \
	/*save path*/ \
	MGPlusGraphicSave(hgraphics, hdc, 0, 0, RECTWP(rt), RECTHP(rt), rt->left, rt->top); \
FAILED: \
	if(hbrush) \
		fashion_retBrush(hbrush); \
	if(hgraphics) \
		fashion_retGraphics(hgraphics); \
	if(hpath) \
		MGPlusPathDelete(hpath); }


#define END_MGPLUGS_ENV(hdc, rt) \
	MGPlusFillPath(hgraphics, hbrush, hpath); \
	END_MGPLUGS_ENV_NO_FILL_PATH(hdc, rt)

static inline void fashion_set_rect_path(HPATH hpath, const RECT *rt, int rx, int ry)
{
	MGPlusPathReset(hpath);
	if(rx > 0 && ry > 0)
		MGPlusPathAddRoundRectEx(hpath,
			rt->left, rt->top,
			RECTWP(rt), RECTHP(rt),
			rx, ry);
	else
		MGPlusPathAddRectangleI(hpath,
			rt->left, rt->top,
			RECTWP(rt), RECTHP(rt));
}

static inline void fashion_fill_round_rect(HGRAPHICS hgraphics, HPATH hpath, HBRUSH hbrush, 
	DWORD color1, DWORD color2, int mode,
	const RECT *rt, int rx, int ry)
{
	MGPlusSetLinearGradientBrushRect(hbrush, (RECT*)rt);
	fashion_initLinerGrandientBrush(hbrush, color1, color2, mode);
	fashion_set_rect_path(hpath, rt, rx, ry);
	MGPlusFillPath(hgraphics, hbrush, hpath);
}


BOOL fashion_fillHalfRoundRect(HDC hdc, DWORD color1, DWORD color2, int mode, 
	const RECT * rt, int rx, int ry, DWORD round_corners);


BOOL fashion_fillMiddleGradientRountRect(HDC hdc, 
	DWORD color_middle, 
	DWORD color_top, 
	DWORD color_bottom, 
	int mode, const RECT *prc, int round_x, int round_y, DWORD flags);

#endif


