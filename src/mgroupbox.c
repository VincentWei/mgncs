
#include <stdio.h>
#include <stdlib.h>
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
#include "mstatic.h"
#include "mgroupbox.h"
#include "mrdr.h"

static void mGroupBox_onPaint(mGroupBox *self, HDC hdc, const PCLIPRGN pinv_clip)
{
    RECT        rcClient;
	const char *spCaption = NULL;
    PLOGFONT    font;

    GetClientRect (self->hwnd, &rcClient);
 
    font = GetWindowFont(self->hwnd);
    spCaption = GetWindowCaption (self->hwnd);

    if (spCaption)
	{
		SIZE size;
		RECT rect;
		DWORD fgcolor;
		if (GetWindowExStyle (self->hwnd) & WS_EX_TRANSPARENT)
			SetBkMode(hdc, BM_TRANSPARENT);
		else
			SetBkMode (hdc, BM_OPAQUE);

		if(GetWindowStyle(self->hwnd) & WS_DISABLED)
			fgcolor = ncsGetElement(self, NCS_FGC_DISABLED_ITEM);
		else
			fgcolor = ncsGetElement(self, NCS_FGC_3DBODY);
		
		SetTextColor(hdc, ncsColor2Pixel(hdc,fgcolor));

		SetBkColor(hdc, GetWindowBkColor (self->hwnd));
		TextOut (hdc, font->size, 2, spCaption);

		GetTextExtent (hdc, spCaption, -1, &size);
		rect.left   = font->size - 1;
		rect.right  = rect.left + size.cx + 2;
		rect.top    = 0;
		rect.bottom = size.cy;
		ExcludeClipRect (hdc, &rect);
	}

	rcClient.top += (font->size >> 1);

	((mGroupBoxRenderer*)(self->renderer))->drawGroupbox(self, hdc, &rcClient);
}

static int mGroupBox_hitTest(mGroupBox *self, int x, int y)
{
	RECT rc;
	GetWindowRect(self->hwnd, &rc);
	if(!PtInRect(&rc, x, y))
		return HT_OUT;

	if(RECTW(rc) >20 && RECTH(rc) > 20)
	{
		rc.left += 10;
		rc.top += 10;
		rc.right -= 10;
		rc.bottom -= 10;
		if(PtInRect(&rc, x, y))
			return HT_TRANSPARENT;
	}
	
	return HT_CLIENT;
	
}

static int mGroupBox_wndProc(mGroupBox* self,  
        int message, WPARAM wParam, LPARAM lParam)
{	
    if(message == MSG_HITTEST)
    {
        return ((mGroupBoxClass*)_c(self))->hitTest(self,(int)wParam, (int)lParam);
    }
	
	return Class(mStatic).wndProc((mStatic *)self, message, wParam, lParam);
}

BEGIN_CMPT_CLASS(mGroupBox, mStatic)
	CLASS_METHOD_MAP(mGroupBox, onPaint)
	CLASS_METHOD_MAP(mGroupBox, hitTest)
	CLASS_METHOD_MAP(mGroupBox, wndProc)
END_CMPT_CLASS


