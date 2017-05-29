
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncsconfig.h"
#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#include "mwidget.h"
#include "mdblist.h"
#include "mitem.h"
#include "mscroll_widget.h"
#include "mitem_manager.h"
#include "mitemview.h"
#include "mscrollview.h"
#include "medit.h"

/*=============== support double buffer dc ============*/
#ifdef _MGNCS_USE_DBEDIT
static void mEdit_initBuffDC(mEdit *self)
{
    RECT rc;
    HDC hdc;
    int w, h;
    
    GetClientRect(self->hwnd, &rc);
    if (self->db_dc != 0) {
        w = GetGDCapability(self->db_dc, GDCAP_MAXX) + 1;
        h = GetGDCapability(self->db_dc, GDCAP_MAXY) + 1;
        if (w == RECTW(rc) && h == RECTH(rc)) {
            return;
        }
        DeleteCompatibleDC(self->db_dc);
    }

    hdc = GetClientDC(self->hwnd);
    self->db_dc = CreateCompatibleDCEx(hdc, RECTW(rc), RECTH(rc));
    ReleaseDC(hdc);

    if (self->db_dc == HDC_INVALID) {
        fprintf (stderr, "Error: Create double buffer error!\n");
        return;
    }
    DefaultControlProc(self->hwnd, MSG_ERASEBKGND, (WPARAM)self->db_dc, 0);
}

static void mEdit_construct (mEdit *self, DWORD addData)
{
	g_stmScrollViewCls.construct((mScrollView*)self, addData);
	self->db_dc = 0;
}
#endif

static void mEdit_destroy(mEdit* self)
{
#if _MGNCS_USE_DBEDIT
	if(self->db_dc)
		DeleteCompatibleDC(self->db_dc);
#endif
    //SendMessage(self->hwnd, MSG_KILLFOCUS, 0, 0);
    DefaultControlProc(self->hwnd, MSG_KILLFOCUS, 0, 0);
	Class(mScrollView).destroy((mScrollView*)self);
}

static void call_paint(NCS_CB_ONPAINT onPaint, mEdit * self, const PCLIPRGN prgn)
{
	HDC hdc = mEdit_getDC(self);
	RECT rc;
	HDC hdc_dev = BeginPaint(self->hwnd);
	BOOL btrans = GetWindowExStyle(self->hwnd) & WS_EX_TRANSPARENT;
	GetClientRect(self->hwnd, &rc);
	if(btrans)
	{
		BitBlt(hdc_dev, 0, 0, RECTW(rc), RECTH(rc), hdc, 0, 0, 0);
	}

	onPaint((mWidget*)self, hdc, prgn);
	BitBlt(hdc, 0, 0, RECTW(rc), RECTH(rc), hdc_dev, 0, 0, 0);
	EndPaint(self->hwnd, hdc_dev);
	mEdit_releaseDC(self, hdc);
}

static int mEdit_wndProc(mEdit *self, int message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case MSG_SETTEXT:
		if(lParam== 0L){
			return _c(self)->setContent(self, NULL, 0, 0);
		}
		return _c(self)->setContent(self, (const char*)lParam, 0, -1);
	case MSG_GETTEXT:
		{
			return _c(self)->getContent(self, (char*)lParam, (int)wParam, 0, -1);
		}
		break;
	case MSG_GETTEXTLENGTH:
		return _c(self)->getTextLength(self);
	case MSG_DOESNEEDIME:
		return  !((GetWindowStyle(self->hwnd) & NCSS_EDIT_READONLY ) || (GetWindowExStyle(self->hwnd)&NCSS_EX_UNNEEDIME));
#ifdef _MGNCS_USE_DBEDIT
	case MSG_CHANGESIZE:
        DefaultControlProc(self->hwnd, MSG_CHANGESIZE, wParam, lParam);
		mEdit_initBuffDC(self);
		return 0;

	case MSG_PAINT:
		call_paint((NCS_CB_ONPAINT)_c(self)->onPaint, self, (const PCLIPRGN)lParam);
		return 0;
	case MSG_ERASEBKGND:
		return Class(mScrollView).wndProc((mScrollView*)self, message, (WPARAM)(mEdit_getDC(self)), lParam);	
#endif
	}

	return Class(mScrollView).wndProc((mScrollView*)self, message, wParam, lParam);
}


#ifdef _MGNCS_USE_DBEDIT
static BOOL mEdit_callUserHandler(mEdit* self, void *handler, int message, WPARAM wParam, LPARAM lParam, int *pret)
{
	if(pret)
		*pret = 0;
	switch(message)
	{
	case MSG_ERASEBKGND:
		return ((NCS_CB_ONERASEBKGND)handler)((mWidget*)self, mEdit_getDC(self), (const PRECT)lParam);	
	case MSG_PAINT:
		call_paint((NCS_CB_ONPAINT)handler, self, (const PCLIPRGN)lParam);
		return TRUE;
	}
	return Class(mWidget).callUserHandler((mWidget*)self, handler, message, wParam, lParam, pret);
}
#endif


BEGIN_CMPT_CLASS (mEdit, mScrollView)	
	CLASS_METHOD_MAP(mEdit, wndProc)
#ifdef _MGNCS_USE_DBEDIT
	CLASS_METHOD_MAP(mEdit, construct)
	CLASS_METHOD_MAP(mEdit, callUserHandler)
#endif
	CLASS_METHOD_MAP(mEdit, destroy)
END_CMPT_CLASS
