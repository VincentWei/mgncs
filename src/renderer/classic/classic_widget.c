#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

static const char* classic_name = "classic";

static void classic_uninit(mWidgetRenderer *rdr)
{
	if(rdr->prv_data)
		free(rdr->prv_data);
}

static void classic_drawFocusFrame(mWidget *self, HDC hdc, const RECT* rc)
{
	ncsCommRDRDrawFocusFrame(self->hwnd, hdc, rc);
}

static BOOL classic_drawBkground(mWidget *self, HDC hdc, const RECT* inv_rc)
{
	return ncsCommRDRDrawBkgnd(self->hwnd, hdc, inv_rc);
}


static BOOL classic_init(mWidgetRenderer *rdr)
{
    return TRUE;
}

static void classic_draw3dbox(mWidget *self, HDC hdc, const RECT *rc, DWORD color, DWORD flag)
{
	ncsCommRDRDraw3dbox(hdc, rc, color, flag);
}

static void classic_drawCheckbox(mWidget *self, HDC hdc, const RECT *rc, DWORD flag)
{
	DWORD fgcolor = ncsGetElement(self, NCS_FGC_WINDOW);
	DWORD bgcolor = ncsGetElement(self, NCS_BGC_WINDOW);
	DWORD discolor = ncsGetElement(self, NCS_FGC_DISABLED_ITEM);

	ncsCommRDRDrawCheckbox(hdc, rc, fgcolor, bgcolor, discolor, flag);
}

static void classic_drawRadio(mWidget *self, HDC hdc, const RECT *rc, DWORD flag)
{
	DWORD fgcolor = ncsGetElement(self, NCS_FGC_WINDOW);
	DWORD bgcolor = ncsGetElement(self, NCS_BGC_WINDOW);
	DWORD discolor = ncsGetElement(self, NCS_FGC_DISABLED_ITEM);

	ncsCommRDRDrawRadio(hdc, rc, fgcolor, bgcolor, discolor, flag);
}

static void classic_drawArrow(mWidget *self, HDC hdc, 
        const RECT *rc, int arrow,DWORD basiccolor, DWORD flag)
{
	ncsCommRDRDrawArrow(hdc, rc, arrow, basiccolor, flag&NCSRF_FILL);
}

static void classic_drawItem(mWidget *self, HDC hdc, const RECT *rc, DWORD flag)
{
	DWORD color;
	int id;
	switch(NCSR_STATUS(flag))
	{
	case NCSRS_HIGHLIGHT:
		id = NCS_BGC_HILIGHT_ITEM;
		break;
	case NCSRS_SELECTED:
		id = NCS_BGC_SELECTED_ITEM;
		break;
	case NCSRS_DISABLE:
		id = NCS_BGC_DISABLED_ITEM;
		break;
	case NCSRS_SIGNIFICANT:
	default:
		id = NCS_BGC_WINDOW;
	}

	color = ncsGetElement(self, id);

	ncsCommRDRDrawItem(hdc, rc, color);
}

static void classic_class_init(mWidgetRenderer * rdr)
{
    rdr->rdr_name = classic_name;
	rdr->drawFocusFrame = classic_drawFocusFrame;
	rdr->drawBkground = classic_drawBkground;
	rdr->draw3dbox = classic_draw3dbox;
	rdr->drawCheckbox = classic_drawCheckbox;
	rdr->drawRadio = classic_drawRadio;
	rdr->drawArrow = classic_drawArrow;
	rdr->drawItem = classic_drawItem;
}

mWidgetRenderer classic_widget_renderer = {
	"",
	classic_class_init,
	NULL,
	classic_init,
	classic_uninit,
};

