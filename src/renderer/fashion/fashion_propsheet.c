
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#include "mscroll_widget.h"
#include "mcontainer.h"
#include "mpage.h"
#include "mpropsheet.h"
#include "mrdr.h"
#include "fashion_common.h"

#ifdef _MGNCSCTRL_PROPSHEET

#ifdef _MGNCS_RDR_FASHION

#define ICON_OFFSET 2

extern mWidgetRenderer fashion_widget_renderer;

static void fashion_resetHeadArea (mPropSheet *self,
        RECT* rcClient, DWORD style)
{
    int defHeight = 16 + 4 + 2* ICON_OFFSET;

    self->headRect.left = 0;
    self->headRect.right = RECTWP(rcClient);

    if ((style&NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM) {
        self->headRect.bottom = RECTHP(rcClient);
    }
    else {
        self->headRect.bottom = defHeight;
    }
    self->headRect.top = self->headRect.bottom - defHeight;

    self->maxTabWidth = RECTW(self->headRect);
    //16 for button size
    self->scrollTabWidth = RECTW(self->headRect) - 2*16;
}

static void fashion_getRect(mPropSheet *self,
        RECT* rcClient, RECT* rcResult, int which)
{
    int     btnSize = 16;
    DWORD   style = GetWindowStyle (self->hwnd);

    switch (which) {
        case NCSF_PRPSHT_BORDER:
        case NCSF_PRPSHT_PAGE:
        {
            rcResult->left = rcClient->left;
            rcResult->right = rcClient->right;

            if ((style & NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM) {
                rcResult->top = rcClient->top;
                rcResult->bottom = rcClient->bottom - RECTH(self->headRect) + 1;
            }
            else {
                rcResult->top = self->headRect.bottom - 1;
                rcResult->bottom = rcClient->bottom;
            }
            if (NCSF_PRPSHT_BORDER == which)
                break;

            rcResult->left += 1;
            rcResult->right -= 1;
            rcResult->top += 1;
            rcResult->bottom -= 1;
            break;
        }
        case NCSF_PRPSHT_TAB:
        {
            rcResult->top = self->headRect.top;
            rcResult->bottom = self->headRect.bottom;

            if ((style&NCSS_PRPSHT_BTNMASK) == NCSS_PRPSHT_SCROLLABLE
                    &&self->btnShow) {
                rcResult->left = self->headRect.left + btnSize;
                rcResult->right = self->headRect.right - btnSize;
            }
            else {
                rcResult->left = self->headRect.left;
                rcResult->right = self->headRect.right;
            }

            return;
        }
        case NCSF_PRPSHT_LEFT:
            if ((style&NCSS_PRPSHT_BTNMASK) == NCSS_PRPSHT_SCROLLABLE
                    &&self->btnShow) {
                rcResult->left = self->headRect.left;
                rcResult->right = rcResult->left + btnSize;
                rcResult->top = self->headRect.top;
                rcResult->bottom = self->headRect.bottom;
            }
            else {
                SetRectEmpty(rcResult);
            }
            return;

        case NCSF_PRPSHT_RIGHT:
            if ((style&NCSS_PRPSHT_BTNMASK) == NCSS_PRPSHT_SCROLLABLE
                    &&self->btnShow) {
                rcResult->right = self->headRect.right;
                rcResult->left = rcResult->right - btnSize;
                rcResult->top = self->headRect.top;
                rcResult->bottom = self->headRect.bottom;
            }
            else {
                SetRectEmpty(rcResult);
            }
            return;

        default:
            break;
    }
}

static void fashion_drawBorder(mPropSheet *self,
        HDC hdc, RECT* rcBorder)
{
   	DWORD color, round_corners, style;
	int rx, ry, bkid;
	rx = ncsGetElement((mWidget*)self, NCS_METRICS_3DBODY_ROUNDX);
	ry = ncsGetElement((mWidget*)self, NCS_METRICS_3DBODY_ROUNDY);
    style = GetWindowStyle (self->hwnd);

	bkid = NCS_BGC_3DBODY;
	color = ncsGetElement((mWidget*)self, bkid);
	round_corners = NCS_BORDER_ALL;

	if((style&NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM)
	{
		round_corners |= NCS_ROUND_CORNER_TOPS;
	}
	else
	{
		round_corners |= NCS_ROUND_CORNER_BOTTOMS;
	}

    ncsCommRDRDrawHalfRoundRect(hdc, rcBorder, rx, ry, color, round_corners);
}

static void fashion_drawScrollBtn(mPropSheet *self, HDC hdc,
        RECT* rcBtn, int which)
{
    DWORD color;

    color = ncsGetElement((mWidget*)self, NCS_FGC_WINDOW);

	self->renderer->drawArrow(self, hdc, rcBtn, which, color, TRUE);
}

static void fashion_drawTab(mPropSheet *self, HDC hdc,
        RECT* rcTab, const char* title, HICON hIcon, BOOL active)
{
    DWORD   style = GetWindowStyle (self->hwnd);
	int rx = ncsGetElement((mWidget*)self, NCS_METRICS_3DBODY_ROUNDX);
	int ry = ncsGetElement((mWidget*)self, NCS_METRICS_3DBODY_ROUNDY);
	RECT rt = *rcTab;
	DWORD color = ncsGetElement((mWidget*)self, NCS_BGC_3DBODY);
	int mode = style &(NCSS_PRPSHT_BOTTOM|NCSS_PRPSHT_TOP)?0:1;
	int round_flag = 0;
	int border_flag = NCS_BORDER_ALL;

	rt.right ++;
    if (rt.right > self->headRect.right)
        rt.right = self->headRect.right;

	switch(style&NCSS_PRPSHT_TABMASK) {
        case NCSS_PRPSHT_BOTTOM:   round_flag = NCS_ROUND_CORNER_BOTTOMS; break;
        case NCSS_PRPSHT_TOP:      round_flag = NCS_ROUND_CORNER_TOPS;   break;
	}

    if (!active) {
        if ((style&NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM) {
			rt.bottom -= 2;
        } else {
			rt.top += 2;
        }
    }
	else
	{
		if ((style&NCSS_PRPSHT_TABMASK) == NCSS_PRPSHT_BOTTOM) {
			border_flag &= ~NCS_BORDER_TOP;
		} else {
			border_flag &= ~NCS_BORDER_BOTTOM;
		}
        color = ncsCommRDRCalc3dboxColor(color, 100);
	}


	fashion_fillMiddleGradientRountRect(hdc,
		color,
		ncsCommRDRCalc3dboxColor(color, 200),
		ncsCommRDRCalc3dboxColor(color, 150),
		mode,
		&rt,
		rx, ry,
		round_flag);

	ncsCommRDRDrawHalfRoundRect(hdc, &rt, rx, ry, color, round_flag|border_flag);

    /* draw the ICON */
    if (hIcon) {
        int icon_x, icon_y;
		int y;
		GetIconSize(hIcon, &icon_x, &icon_y);
		if(icon_x > RECTW(rt) )
			icon_x = RECTW(rt) ;
		if(icon_y > RECTH(rt))
			icon_y = RECTH(rt) ;

		y = (rt.top + rt.bottom - icon_y) / 2;

        DrawIcon (hdc, rt.left , y , icon_x, icon_y, hIcon);
		rt.left = rt.left + 2 + icon_x;
    }


    /* draw the TEXT */
	SetBkMode(hdc, BM_TRANSPARENT);
	DrawText(hdc, title, -1, &rt, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
}


static void fashion_propsheet_class_init (mPropSheetRenderer* rdr)
{
	fashion_widget_renderer.class_init((mWidgetRenderer*)rdr);
	rdr->resetHeadArea = fashion_resetHeadArea;
	rdr->getRect = fashion_getRect;
	rdr->drawScrollBtn = fashion_drawScrollBtn;
	rdr->drawBorder = fashion_drawBorder;
	rdr->drawTab = fashion_drawTab;
}

mPropSheetRenderer fashion_propsheet_renderer = {
    "",
	fashion_propsheet_class_init,
	&fashion_widget_renderer
};



#endif
#endif //_MGNCSCTRL_PROPSHEET
