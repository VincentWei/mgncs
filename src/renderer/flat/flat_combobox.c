/**
 ** flat_combobox.c
 **
 ** wangjian<wangjian@minigui.org>
 **
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

#ifdef _MGNCS_RDR_FLAT

extern mWidgetRenderer flat_widget_renderer;

static void flat_drawDropDownBtn (mCombobox *self, HDC hdc, const RECT *rect)
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

static void flat_combobox_class_init (mComboboxRenderer* rdr)
{
	flat_widget_renderer.class_init((mWidgetRenderer*)rdr);
	rdr->drawDropDownBtn = flat_drawDropDownBtn;
}

mComboboxRenderer flat_combobox_renderer =
{
    "flat",
	flat_combobox_class_init,
	(void*)&flat_widget_renderer
};

#endif
#endif //_MGNCSCTRL_COMBOBOX

