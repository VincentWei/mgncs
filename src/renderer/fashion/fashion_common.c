#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgplus/mgplus.h>

#include "mgncsconfig.h"
#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#include "mwidget.h"
#include "mrdr.h"

#include "fashion_common.h"

#ifdef _MGNCS_RDR_FASHION

CacheSlot * createCacheSlot(int max, 
	void* (*newObject)(void),
	void(*deleteObject)(void*), 
	void(*cleanObject)( void*))
{
	CacheSlot * slots = NULL;

	if(max <= 0 
		|| newObject == NULL)
		return NULL;

	slots = (CacheSlot*)malloc(sizeof(CacheSlot));
	
	if(!slots)
		return NULL;

	slots->slots = (void**)calloc(max, sizeof(void*));
	slots->max = max;
	slots->newObject = newObject;
	slots->deleteObject = deleteObject;
	slots->cleanObject = cleanObject;
	slots->free_cursor = 0;
#ifdef _MGRM_THREAD
	pthread_mutex_init(&slots->mutex, NULL);
#endif
	return slots;
}

void * getFreeObject(CacheSlot* slots)
{
	void ** pptr;

	if(slots == NULL)
		return NULL;

	if(slots->free_cursor >= slots->max)
		return NULL;

#ifdef _MGRM_THREAD
	pthread_mutex_lock(&slots->mutex);
#endif
	pptr = slots->slots + slots->free_cursor;

	if(*pptr == NULL)
	{
		*pptr = (slots->newObject)();
	}

	slots->free_cursor ++;

#ifdef _MGRM_THREAD
	pthread_mutex_unlock(&slots->mutex);
#endif

	return *pptr;
}

void returnObject(CacheSlot* slots, void *obj)
{
	int i;
	int top;
	if(slots == NULL || obj == NULL)
		return ;

#ifdef _MGRM_THREAD
	pthread_mutex_lock(&slots->mutex);
#endif
	top = slots->free_cursor-1;
	for(i = top; i>=0; i--)
	{
		if(slots->slots[i] == obj){
			//swamp with head
			if(i != top)
			{
				void *tmp = slots->slots[i];
				slots->slots[i] = slots->slots[top];
				slots->slots[top] = tmp;
			}
			if(slots->cleanObject)
				(slots->cleanObject)(slots->slots[top]);
			slots->free_cursor --;
			break;
		}
	}
	if(i < 0 && slots->deleteObject)
		(*(slots->deleteObject))(obj);

#ifdef _MGRM_THREAD
	pthread_mutex_unlock(&slots->mutex);
#endif
}

void freeCacheSlot(CacheSlot* slots)
{
	int i;
	if(slots == NULL)
		return;

	if(slots->deleteObject){
		for(i = 0; i < slots->free_cursor; i++)
		{
			(slots->deleteObject)(slots->slots[i]);
		}
	}

	free(slots->slots);

#ifdef _MGRM_THREAD
	pthread_mutex_destroy(&slots->mutex);
#endif
	free(slots);
}

//////////////////////////////////////////////////////
//
static CacheSlot * _brush_cache_slots = NULL;

static void* brush_new(void)
{
	return (void*)MGPlusBrushCreate(MP_BRUSH_TYPE_LINEARGRADIENT);
}

static void brush_delete(void* obj)
{
	MGPlusBrushDelete((HBRUSH)obj);
}

HBRUSH fashion_getBrush()
{
	return (HBRUSH)getFreeObject(_brush_cache_slots);
}

void fashion_retBrush(HBRUSH hbrush)
{
	returnObject(_brush_cache_slots, (void*)hbrush);
}



////////////////////////////////////////

static HGRAPHICS _hgraphics = 0;
static int _graphics_max_width = MAX_GRAPHICS_WIDTH;
static int _graphics_max_height = MAX_GRAPHICS_HEIGHT;
#ifdef _MGRM_THREAD
static pthread_mutex_t _graphics_mutex;
#endif

static BOOL init_graphics(void)
{
    _hgraphics = 0;
	_graphics_max_width = 
		_graphics_max_width > RECTW(g_rcScr) ?
			RECTW(g_rcScr):_graphics_max_width;
	_graphics_max_height = 
		_graphics_max_height > RECTW(g_rcScr) ?
			RECTW(g_rcScr):_graphics_max_height;

#ifdef _MGRM_THREAD
	pthread_mutex_init(&_graphics_mutex, NULL);
#endif
	return TRUE;
}

static void unit_graphics(void)
{
	if(_hgraphics)
		MGPlusGraphicDelete(_hgraphics);

#ifdef _MGRM_THREAD
	pthread_mutex_destroy(&_graphics_mutex);
#endif
}

HGRAPHICS fashion_getGraphics(int width, int height)
{
	if(width <= _graphics_max_width && height <= _graphics_max_height)
	{
		if(!_hgraphics)
			_hgraphics = MGPlusGraphicCreate(_graphics_max_width, _graphics_max_height);
#ifdef _MGRM_THREAD
		pthread_mutex_lock(&_graphics_mutex);
#endif
		return _hgraphics;
	}
	
	return MGPlusGraphicCreate(width, height);
}

void fashion_retGraphics(HGRAPHICS hgraphics)
{
	if(hgraphics ==  _hgraphics)
	{
#ifdef _MGRM_THREAD
		pthread_mutex_unlock(&_graphics_mutex);
#endif
	}
	else
	{
		MGPlusGraphicDelete(hgraphics);
	}
}


///////////////////////////////////////////

BOOL init_fashion_common()
{
	_brush_cache_slots = createCacheSlot(MAX_BRUSH_SLOT,
		brush_new,
		brush_delete,
		NULL);

	if(!init_graphics())
		return FALSE;

	return TRUE;
		
}

void uninit_fashion_common()
{
	freeCacheSlot(_brush_cache_slots);

	unit_graphics();
}

////////////////////////////////

BOOL fashion_initLinerGrandientBrush(HBRUSH hbrush, DWORD color1, DWORD color2, int mode)
{
	ARGB colors[3];
	
	colors[0] = toARGB(color1);
	colors[1] = toARGB(color2);
	colors[2] = 0;

	if(mode != -1)
		MGPlusSetLinearGradientBrushMode(hbrush, mode);
	MGPlusSetLinearGradientBrushColors(hbrush, colors, 3);
	return TRUE;
}



BOOL fashion_fillRoundRect(HDC hdc, DWORD color1, DWORD color2, int mode, 
	const RECT * rt, int rx, int ry)
{
	BOOL bret = FALSE;

	INIT_MGPLUGS_ENV(hdc, color1, color2, mode,rt)
	
	if(rx > 0 && ry > 0)
		MGPlusPathAddRoundRectEx(hpath, 0, 0, RECTWP(rt), RECTHP(rt), rx, ry);
	else
		MGPlusPathAddRectangleI(hpath, 0, 0, RECTWP(rt), RECTHP(rt));

	bret = TRUE;

	END_MGPLUGS_ENV(hdc, rt)

	return bret;
}

BOOL fashion_fillHalfRoundRect(HDC hdc, DWORD color1, DWORD color2, int mode, 
	const RECT * rt, int rx, int ry, DWORD round_corners)
{
	BOOL bret = FALSE;

	INIT_MGPLUGS_ENV(hdc, color1, color2, mode,rt)
	
	if(rx > 0 && ry > 0 && round_corners != 0)
	{
		if(NCS_ROUND_CORNER_ALL){
			MGPlusPathAddRoundRectEx(hpath, 0, 0, RECTWP(rt), RECTHP(rt), rx, ry);
		}
		else{
			int startx, starty, endx, endy;
			RECT rttmp = *rt;
			OffsetRect(&rttmp, -rttmp.left, -rttmp.top);
			
			if(round_corners & NCS_ROUND_CORNER_LEFT_TOP)
			{
				MGPlusPathAddArcI(hpath, rx, ry, rx, ry, 90, 90);
				startx = rx;
				starty = 0;
			}
			else
			{
				startx = 0;
				starty = 0;
			}
			if(round_corners & NCS_ROUND_CORNER_RIGHT_TOP)
			{
				endx = rttmp.right - rx;
				endy = 0;
				MGPlusPathAddLineI(hpath, startx, starty, endx, endy);
				MGPlusPathAddArcI(hpath, startx, ry, rx, ry, 0, 90);
				startx = rttmp.right;
				starty = ry;
			}
			else
			{
				endx = rttmp.right;
				endy = 0;
				MGPlusPathAddLineI(hpath, startx, starty, endx, endy);
				startx = rttmp.right;
				starty = 0;
			}
			if(round_corners & NCS_ROUND_CORNER_RIGHT_BOTTOM)
			{
				endx = startx;
				endy = rttmp.bottom - ry;
				MGPlusPathAddLineI(hpath, startx, starty, endx, endy);
				MGPlusPathAddArcI(hpath, startx - rx, endy, rx, ry, 270, 90);
				startx = rttmp.right - rx;
				starty = rttmp.bottom;
			}
			else{
				endx = rttmp.right;
				endy = rttmp.bottom;
				MGPlusPathAddLineI(hpath, startx, starty, endx, endy);
				startx = endx;
				starty = endy;
			}
			if(round_corners & NCS_ROUND_CORNER_LEFT_BOTTOM)
			{
				endx = rx;
				endy = starty;
				MGPlusPathAddLineI(hpath, startx, starty, endx, endy);
				MGPlusPathAddArcI(hpath, startx - rx, endy, rx, ry, 180, 90);
				startx = 0;
				endy = rttmp.bottom - ry;
			}
			else
			{
				endx = 0;
				endy = starty;
				MGPlusPathAddLineI(hpath, startx, starty, endx, endy);
				startx = 0;
				endy = rttmp.bottom;
			}

			endx = startx;
			if(round_corners & NCS_ROUND_CORNER_LEFT_TOP)
			{
				endy = ry;
			}
			else
				endy = 0;

			MGPlusPathAddLineI(hpath, startx, starty, endx, endy);
			//colorser
			MGPlusPathCloseFigure(hpath);
		}
	}
	else
		MGPlusPathAddRectangleI(hpath, 0, 0, RECTWP(rt), RECTHP(rt));

	bret = TRUE;

	END_MGPLUGS_ENV(hdc, rt)

	return bret;

}

static void rect_to_trangles(const RECT *rc, MPPOINT pts[3], int direction)
{
	switch(direction)
	{
	case NCSR_ARROW_UP:
		pts[0].x = (rc->left+rc->right)/2;
		pts[0].y = rc->top;
		pts[1].x = rc->left;
		pts[1].y = rc->bottom;
		pts[2].x = rc->right;
		pts[2].y = rc->bottom;
		break;
	case NCSR_ARROW_DOWN:
		pts[0].x = rc->right;
		pts[0].y = rc->top;
		pts[1].x = rc->left;
		pts[1].y = rc->bottom;
		pts[2].x = (rc->left+rc->right)/2;
		pts[2].y = rc->bottom;
		break;
	case NCSR_ARROW_LEFT:
		pts[0].x = rc->left;
		pts[0].y = (rc->top+rc->bottom)/2;
		pts[1].x = rc->right;
		pts[1].y = rc->top;
		pts[2].x = rc->right;
		pts[2].y = rc->bottom;
		break;
	case NCSR_ARROW_RIGHT:
		pts[0].x = rc->right;
		pts[0].y = (rc->top+rc->bottom)/2;
		pts[1].x = rc->left;
		pts[1].y = rc->top;
		pts[2].x = rc->left;
		pts[2].y = rc->bottom;
		break;
	}
}

BOOL fashion_fillTriangle(HDC hdc, DWORD color1, DWORD color2,
	int mode, const RECT *rtTrangle, int direction)
{
	RECT rttmp;
	BOOL bret = FALSE;
	MPPOINT pts[3];

	//calc triangle
	fashion_getTriangleFromRect(rtTrangle, &rttmp, direction);
	OffsetRect(&rttmp, -rttmp.left,  -rttmp.top);

	INIT_MGPLUGS_ENV(hdc, color1, color2, mode, (&rttmp))

	rect_to_trangles(&rttmp, pts, direction);
	MGPlusPathAddLines(hpath, pts, 3);

	bret = TRUE;

	END_MGPLUGS_ENV(hdc, (&rttmp))

	return bret;

}

/////////////////////////////////////////////

BOOL fashion_getTriangleFromRect(const RECT *rc, RECT *prcTrangle, int arrow)
{
	int w = RECTWP(rc);
	int h = RECTHP(rc);
	int vdelta =((w>>2)) >(h/3)?(h/3):((w>>2));
	int hdelta =((h>>2)) >(w/3)?(w/3):((h>>2));
	if(arrow == NCSR_ARROW_UP)
	{
		prcTrangle->left = rc->left + (w>>1) - vdelta;
		prcTrangle->top  = rc->top + (h/3);
		prcTrangle->right = rc->left + (w>>1) + vdelta;
		prcTrangle->bottom = prcTrangle->top + vdelta;
	}
	else if(arrow == NCSR_ARROW_DOWN)
	{
		prcTrangle->left = rc->left + (w>>1) - vdelta;
		prcTrangle->bottom = rc->bottom - (h/3) - 1;
		prcTrangle->right = rc->left + (w>>1) + vdelta;
		prcTrangle->top = prcTrangle->bottom - vdelta;
	}
	else if(arrow == NCSR_ARROW_LEFT)
	{
		prcTrangle->left = rc->left + (w/3);
		prcTrangle->right = prcTrangle->left + hdelta;
		prcTrangle->top = rc->top + (h>>1) - hdelta;
		prcTrangle->bottom = prcTrangle->top + hdelta*2;
	}
	else
	{
		prcTrangle->right = rc->right - (w/3) -1;
		prcTrangle->left = prcTrangle->right - hdelta;
		prcTrangle->top = rc->top + (h>>1) - hdelta;
		prcTrangle->bottom = prcTrangle->top + hdelta*2;
	}
	
	return TRUE;
}

BOOL fashion_fillEllipse(HDC hdc, DWORD color1, DWORD color2, int mode, const PRECT rt)
{
	int w, h;
	BOOL bret = FALSE;
	INIT_MGPLUGS_ENV(hdc, color1, color2, mode, rt)

	//make circle path
	w = RECTWP(rt)/2;
	h = RECTHP(rt)/2;
	MGPlusPathAddEllipseI(hpath, w, h, w, h ,TRUE);

	bret = TRUE;
	END_MGPLUGS_ENV(hdc, rt)
	return bret;
}


BOOL fashion_drawArrow(HDC hdc, 
		DWORD *shellColors, int shellMode, 
		DWORD* arrowColors, int arrowMode, 
		const RECT *rtTrangle, int rx, int ry, int direction)
{
	BOOL bret = FALSE;
	RECT rttmp;
	MPPOINT pts[3];
	INIT_MGPLUGS_ENV(hdc, shellColors[0], shellColors[1], shellMode, rtTrangle)

	//Add trangle, and fill it
	if(rx > 0 && ry > 0)
		MGPlusPathAddRoundRectEx(hpath, 0, 0, RECTWP(rtTrangle), RECTHP(rtTrangle), rx, ry);
	else
		MGPlusPathAddRectangleI(hpath, 0, 0, RECTWP(rtTrangle), RECTHP(rtTrangle));

	
	//calc triangle
	fashion_getTriangleFromRect(rtTrangle, &rttmp, direction);
	//init brush
	MGPlusSetLinearGradientBrushRect(hbrush, &rttmp);
	if(!fashion_initLinerGrandientBrush(hbrush, arrowColors[0], arrowColors[1], arrowMode))
		goto FAILED;

	//rest path
	MGPlusPathReset(hpath);
	//fill triangle
	rect_to_trangles(&rttmp, pts, direction);
	MGPlusPathAddLines(hpath, pts, 3);

	bret = TRUE;

	END_MGPLUGS_ENV(hdc, rtTrangle)

	return bret;

}

void fashion_draw_middle_gradient_rect(HGRAPHICS hgraphics, 
	HPATH hpath, 
	HBRUSH hbrush, 
	const RECT *rt, int rx, int ry,
	DWORD color1, DWORD color2, int mode)
{
	RECT rt1 = *rt;
	RECT rt2 = *rt;
	if(mode == MP_LINEAR_GRADIENT_MODE_HORIZONTAL){
		rt1.right = (rt1.left + rt1.right)/2;
		rt2.left = rt1.right;
	}
	else
	{
		rt1.bottom = (rt1.top + rt1.bottom)/2;
		rt2.top = rt1.bottom;
	}

	fashion_fill_round_rect(hgraphics, hpath, hbrush,
		color1, color2, mode,
		&rt1, rx, ry);
	
	fashion_fill_round_rect(hgraphics, hpath, hbrush,
		color2, color1, mode,
		&rt2, rx, ry);

}

BOOL fashion_fillMiddleGradientRountRect(HDC hdc, 
	DWORD color_middle, 
	DWORD color_top, 
	DWORD color_bottom, 
	int mode, const RECT *prc, int round_x, int round_y, DWORD flags)
{
	RECT rc;
	BOOL bret = FALSE;
	HBRUSH hbrush = 0;
	HGRAPHICS hgraphics = 0;
	HPATH  hpath = 0;
	ARGB colors[3];

	hbrush = fashion_getBrush();
	hgraphics = fashion_getGraphics(RECTWP(prc), RECTHP(prc));
	hpath = MGPlusPathCreate(MP_PATH_FILL_MODE_WINDING);

	if(!hbrush || !hgraphics || !hpath)
		goto FAILED;

	MGPlusGraphicCopyFromDC(hgraphics, hdc, prc->left, prc->top, RECTWP(prc), RECTHP(prc), 0, 0);
	
	mode = mode==MP_LINEAR_GRADIENT_MODE_VERTICAL?mode:MP_LINEAR_GRADIENT_MODE_HORIZONTAL;

	rc.left = 0;
	rc.top  = 0;
	rc.right = RECTWP(prc);
	rc.bottom = RECTHP(prc); /// 2;


	MGPlusSetLinearGradientBrushMode(hbrush, mode);
	MGPlusSetLinearGradientBrushRect(hbrush, &rc);
	colors[0] = toARGB(color_top);
	colors[1] = toARGB(color_middle);
	colors[2] = toARGB(color_bottom);
	MGPlusSetLinearGradientBrushColors(hbrush, colors,3);

	if(round_x > 0 && round_y > 0)
	{
		MGPlusPathAddRoundRectEx(hpath, rc.left, rc.top, RECTW(rc), RECTH(rc), round_x, round_y);
	}
	else
	{
		MGPlusPathAddRectangleI(hpath, rc.left, rc.top, RECTW(rc), RECTH(rc));
	}
	//draw 
	MGPlusFillPath(hgraphics, hbrush, hpath);
	
	
	bret = TRUE;
	{ END_MGPLUGS_ENV_NO_FILL_PATH(hdc, prc)
	return bret;

}

#endif

