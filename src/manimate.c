#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSCTRL_ANIMATE

#define ID_TIMER 10001

#include "iconview_content_data.c"

#ifdef _MGNCS_GUIBUILDER_SUPPORT
static PBITMAP* get_default_bmparray(void) {
	static PBITMAP pbmps[2] = { NULL, NULL };
	if(pbmps[0] == NULL)
		pbmps[0] = get_iconview_content();
	return pbmps;
}
#endif

static void mAnimate_construct(mAnimate *self, DWORD addData)
{
	self->path = NULL;
	self->interval = 100;
	self->mem_dc = 0;
	Class(mStatic).construct((mStatic*)self, addData);
}

static void mAnimate_destroy(mAnimate *self)
{
//	if(self->path != NULL)
//		free(self->path);

	if(self->mem_dc)
		DeleteMemDC(self->mem_dc);

	Class(mStatic).destroy((mStatic*)self);
}

static mStaticPiece* mAnimate_createBody(mAnimate *self)
{
	DWORD dwStyle = GetWindowStyle(self->hwnd);
	mAnimatePiece *body = NEWPIECE(mAnimatePiece);

	if(dwStyle & NCSS_ANMT_AUTOLOOP)
		_c(body)->setProperty(body, NCSP_ANIMATEPIECE_AUTOLOOP, 1);
	if(dwStyle & NCSS_ANMT_SCALE)
		_c(body)->setProperty(body, NCSP_ANIMATEPIECE_SCALE, 1);
	if(dwStyle & NCSS_ANMT_AUTOFIT)
		_c(body)->setProperty(body, NCSP_ANIMATEPIECE_AUTOFIT, 1);
	if(dwStyle & NCSS_ANMT_AUTOPLAY)
	{
		_c(body)->setProperty(body, NCSP_ANIMATEPIECE_AUTOPLAY, 1);
		self->state = ANIM_PLAY;
	}
#ifdef _MGNCS_GUIBUILDER_SUPPORT
	_c(body)->setProperty(body, NCSP_ANIMATEPIECE_BMPARRAY, (DWORD)get_default_bmparray());
#endif
	return (mStaticPiece*)body;
}

static BOOL set_animate_source(mAnimate* self, int id, DWORD value)
{
	if(_c((mAnimatePiece*)(self->body))->setProperty((mAnimatePiece*)(self->body), id, value)){

		if (mAnimatePiece_isAutofit((mAnimatePiece*)(self->body))) {
			RECT rc;
			GetWindowRect(self->hwnd, &rc);
			MoveWindow(self->hwnd, rc.left, rc.top, 0, 0, TRUE);
		}
		else
		{
			InvalidateRect(self->hwnd, NULL, TRUE);
		}
		return TRUE;
	}
	return FALSE;
}

static BOOL mAnimate_setProperty(mAnimate *self, int id, DWORD value)
{
	if(id >= NCSP_ANMT_MAX)
		return FALSE;

	switch(id)
	{
		case NCSP_ANMT_MEM:
			return set_animate_source(self, NCSP_ANIMATEPIECE_MEM, value);
		case NCSP_ANMT_GIFFILE:
			self->path = (char *)value;
			return set_animate_source(self, NCSP_ANIMATEPIECE_GIFFILE, value);
		case NCSP_ANMT_DIR:
			self->path = (char *)value;
			return set_animate_source(self, NCSP_ANIMATEPIECE_DIR, value);
		case NCSP_ANMT_BMPARRAY:
			return set_animate_source(self, NCSP_ANIMATEPIECE_BMPARRAY, value);
		case NCSP_ANMT_AFRAME:
			return set_animate_source(self, NCSP_ANIMATEPIECE_AFRAME, value);
		case NCSP_ANMT_INTERVAL:
			{
				int interval = (int)value;
				if(interval < 0)
					return FALSE;
				self->interval = interval & 0x00FFFFFF;
				if(self->state == ANIM_PLAY || self->state == ANIM_PAUSE){
					KillTimer(self->hwnd, ID_TIMER);
					SetTimer (self->hwnd, ID_TIMER, self->interval);
				}
			}
			return TRUE;
	}
	return Class(mStatic).setProperty((mStatic*)self, id, value);
}

static DWORD mAnimate_getProperty(mAnimate *self, int id)
{
	if(id >= NCSP_ANMT_MAX)
		return 0;

	switch(id)
    {
		case NCSP_ANMT_GIFFILE:
			return (DWORD)self->path;
		case NCSP_ANMT_DIR:
			return (DWORD)self->path;
		case NCSP_ANMT_INTERVAL:
			return self->interval;
    }

	return Class(mStatic).getProperty((mStatic*)self, id);
}

static int mAnimate_wndProc(mAnimate* self, int message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case MSG_CREATE:
			SetTimer (self->hwnd, ID_TIMER, self->interval);
			break;
		case MSG_TIMER:
			if(self->state == ANIM_PAUSE || self->state == ANIM_STOP)
			{
				return 0;
			}
		//	RECT rc_orig, rc_new;
		//	GetWindowRect(self->hwnd, &rc_orig);
		//	{
		//		HDC hdc = GetClientDC(self->hwnd);

				//_c(self)->onPaint(self, hdc, NULL);
				//_c((mAnimatePiece*)(self->body))->next((mAnimatePiece*)(self->body), hdc);
			self->paint_flag = 1; //set the flags
			InvalidateRect(self->hwnd, NULL, TRUE);

			//	ReleaseDC(hdc);
		//	}
			return 0;
		case MSG_SIZECHANGING:
			if(mAnimatePiece_isAutofit((mAnimatePiece*)(self->body)) && (((mAnimatePiece*)(self->body))->frame != NULL) )
			{
				const RECT* rcExpect = (const RECT*)wParam;
				RECT* rcResult = (RECT*)lParam;
				int width = RECTWP(rcExpect);
				int height = RECTHP(rcExpect);
				_c((mAnimatePiece*)(self->body))->autofit(((mAnimatePiece*)(self->body)), &width, &height);
				if(width <= 0)
					width = RECTWP(rcExpect);
				if(height <= 0)
					height = RECTHP(rcExpect);
				rcResult->left = rcExpect->left;
				rcResult->top = rcExpect->top;
				rcResult->right = rcResult->left + width ;
				rcResult->bottom = rcExpect->top + height ;
				return 0;
			}
			break;
		case ANIM_STARTPLAY:
			return _c(self)->play(self);
		case ANIM_PAUSE_RESUME:
			return _c(self)->pauseResume(self);
		case ANIM_STOPPLAY:
			return _c(self)->stop(self);
		default:
			break;
	}
	return Class(mStatic).wndProc((mStatic*)self, message, wParam, lParam);
}

static BOOL mAnimate_play(mAnimate* self)
{
	self->state = ANIM_PLAY;
	_c((mAnimatePiece*)(self->body))->reset((mAnimatePiece*)self->body);
	return TRUE;
}

static BOOL mAnimate_pauseResume(mAnimate* self)
{
	if(self->state == ANIM_STOP)
		return FALSE;

	if(self->state == ANIM_PLAY)
		self->state = ANIM_PAUSE;
	else
		self->state = ANIM_PLAY;
	return TRUE;
}

static BOOL mAnimate_stop(mAnimate* self)
{
	self->state = ANIM_STOP;
	_c((mAnimatePiece*)(self->body))->reset((mAnimatePiece*)(self->body));
	InvalidateRect(self->hwnd, NULL, FALSE);
	return TRUE;
}

static BOOL mAnimate_onEraseBkgnd(mAnimate* self, HDC hdc, const RECT *pinv)
{
	if(self->mem_dc)

	{
		SetMemDCColorKey(self->mem_dc, 0, 0);
		return  Class(mStatic).onEraseBkgnd((mStatic*)self, self->mem_dc, pinv);
	}
	else
		return  Class(mStatic).onEraseBkgnd((mStatic*)self, hdc, pinv);
}

static int mAnimate_onSizeChanged(mAnimate* self, RECT *rtClient)
{
	int width, height;
	if(self->mem_dc)
	{
		width = GetGDCapability(self->mem_dc, GDCAP_HPIXEL);
		height = GetGDCapability(self->mem_dc, GDCAP_VPIXEL);
		if(width == RECTWP(rtClient) && height == RECTHP(rtClient))
			goto END;

		DeleteMemDC(self->mem_dc);
		self->mem_dc = 0;
	}
	width = RECTWP(rtClient);
	height = RECTHP(rtClient);
	if(width <= 0 || height <= 0)
		goto END;

	self->mem_dc = CreateCompatibleDCEx(HDC_SCREEN, width, height);

END:
	return Class(mStatic).onSizeChanged((mStatic*)self, rtClient);
}

static void mAnimate_onPaint(mAnimate* self, HDC hdc, const PCLIPRGN* pclip)
{
	RECT rc;
	DWORD add_data  = 0;
	if(!self->body)
		return;
	if(self->mem_dc == 0)
		return;

	GetClientRect(self->hwnd, &rc);
	if(GetWindowExStyle(self->hwnd) & WS_EX_TRANSPARENT)
	{
		//clear the memdc
		gal_pixel bg_color = GetWindowBkColor(self->hwnd);
		gal_pixel bg_old = SetBrushColor(self->mem_dc, bg_color);
		FillBox(self->mem_dc, 0, 0, RECTW(rc), RECTH(rc));
		SetMemDCColorKey(self->mem_dc, MEMDC_FLAG_SRCCOLORKEY, bg_color);
		SetBrushColor(self->mem_dc, bg_old);
	}

	if(!(GetWindowStyle(self->hwnd)&WS_DISABLED))
		add_data = self->paint_flag;
	_c(Body)->paint(Body, self->mem_dc, (mObject*)self, add_data);

	BitBlt(self->mem_dc, 0, 0, RECTW(rc), RECTH(rc), hdc, 0, 0, 0);
	self->paint_flag = 0;
}

#ifdef _MGNCS_GUIBUILDER_SUPPORT
static BOOL mAnimate_refresh(mAnimate* self)
{
	DWORD dwStyle;
	if(!Body)
		return FALSE;

	dwStyle = GetWindowStyle(self->hwnd);

	_c(Body)->setProperty(Body, NCSP_ANIMATEPIECE_AUTOLOOP, (dwStyle & NCSS_ANMT_AUTOLOOP));
	_c(Body)->setProperty(Body, NCSP_ANIMATEPIECE_SCALE, (dwStyle & NCSS_ANMT_SCALE));
	if(dwStyle & NCSS_ANMT_AUTOFIT)
	{
		RECT rc;
		_c(Body)->setProperty(Body, NCSP_ANIMATEPIECE_AUTOFIT, 1);
		GetWindowRect(self->hwnd, &rc);
		MoveWindow(self->hwnd, rc.left, rc.top, 0, 0, TRUE);
	}
	else
		_c(Body)->setProperty(Body, NCSP_ANIMATEPIECE_AUTOFIT, 0);

	if(dwStyle & NCSS_ANMT_AUTOPLAY)
	{
		_c(Body)->setProperty(Body, NCSP_ANIMATEPIECE_AUTOPLAY, 1);
		self->state = ANIM_PLAY;
	}
	else {
		_c(Body)->setProperty(Body, NCSP_ANIMATEPIECE_AUTOPLAY, 0);
		self->state = ANIM_STOP;
	}

	if(self->mem_dc)
	{
	}

	InvalidateRect(self->hwnd, 0, TRUE);
	return TRUE;
}
#endif

BEGIN_CMPT_CLASS(mAnimate, mStatic)
	CLASS_METHOD_MAP(mAnimate, construct)
	CLASS_METHOD_MAP(mAnimate, wndProc)
	CLASS_METHOD_MAP(mAnimate, createBody)
	CLASS_METHOD_MAP(mAnimate, destroy)
	CLASS_METHOD_MAP(mAnimate, setProperty)
	CLASS_METHOD_MAP(mAnimate, getProperty)
	CLASS_METHOD_MAP(mAnimate, onEraseBkgnd)
	CLASS_METHOD_MAP(mAnimate, play)
	CLASS_METHOD_MAP(mAnimate, pauseResume)
	CLASS_METHOD_MAP(mAnimate, stop)
	CLASS_METHOD_MAP(mAnimate, onSizeChanged)
	CLASS_METHOD_MAP(mAnimate, onPaint)
#ifdef _MGNCS_GUIBUILDER_SUPPORT
	CLASS_METHOD_MAP(mAnimate, refresh)
#endif
END_CMPT_CLASS

#endif		//_MGNCSCTRL_ANIMATE
