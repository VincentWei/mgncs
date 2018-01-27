/*
 ** $Id: fashion_combobox.c 1681 2017-10-26 06:46:31Z weiym $
 **
 ** The fashion renderer implementation of mCombobox.
 **
 ** Copyright (C) 2009 Feynman Software.
 **
 ** All rights reserved by Feynman Software.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSCTRL_COMBOBOX

#ifdef _MGNCS_RDR_FASHION

extern mWidgetRenderer fashion_widget_renderer;

static void fashion_drawDropDownBtn (mCombobox *self, HDC hdc, const RECT *rect)
{
	DWORD fgc_3d;
    DWORD status;

    fgc_3d = ncsGetElement(self, NCS_BGC_3DBODY);

    if (IS_SET (self, NCSF_CMBOX_HILIGHT)) {
		status = NCSRS_HIGHLIGHT;
    } else {
		status = NCSRS_NORMAL;
    }

	//draw 3dbox
	self->renderer->draw3dbox(self, hdc, rect, fgc_3d, status|NCSRF_FILL);
	//draw arrow
	self->renderer->drawArrow(self, hdc, rect, NCSR_ARROW_DOWN,
		ncsGetElement(self, NCS_FGC_WINDOW), NCSRF_FILL);
}

static void fashion_combobox_class_init (mComboboxRenderer* rdr)
{
	fashion_widget_renderer.class_init((mWidgetRenderer*)rdr);
	rdr->drawDropDownBtn = fashion_drawDropDownBtn;
}

mComboboxRenderer fashion_combobox_renderer =
{
    "fashion",
	fashion_combobox_class_init,
	(void*)&fashion_widget_renderer
};
#endif //_MGNCS_RDR_FASHION
#endif //_MGNCSCTRL_COMBOBOX
