/**
 ** skin_combobox.c
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

#include "mgncsconfig.h"
#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#include "mwidget.h"
#include "mpanel.h"
#include "mdblist.h"
#include "mitem.h"
#include "mscroll_widget.h"
#include "mitem_manager.h"
#include "mlistbox.h"
#include "mitemview.h"
#include "mscrollview.h"
#include "medit.h"
#include "msledit.h"
#include "mcombobox.h"
#include "mrdr.h"

#ifdef _MGNCS_RDR_SKIN

extern mWidgetRenderer skin_widget_renderer;


static void skin_drawDropDownBtn (mCombobox *self, HDC hdc, const RECT *rect)
{
	DWORD key;
    DRAWINFO _di;
    const BITMAP *bmp;
    
    key = ncsGetElement(self, NCS_IMAGE_ARROWSHELL);
    if (!(bmp = GetBitmapFromRes(key)))
        return;

    _di.bmp      = bmp;
    _di.nr_line  = 4;
    _di.nr_col   = 1;
    _di.idx_col  = 0;
    _di.idx_line = (IS_SET (self, NCSF_CMBOX_HILIGHT)) ? 1 : 0;
    _di.margin1  = 0;
    _di.margin2  = 0;
    _di.direct   = FALSE;
    _di.flip     = FALSE;
    _di.style    = DI_FILL_STRETCH;

	//draw shell
    ncsSkinDraw (hdc, rect, &_di);

	//draw arrow
	self->renderer->drawArrow(self, hdc, 
            rect, NCSR_ARROW_DOWN, 0L, NCSRF_FILL);
}

static void skin_combobox_class_init (mComboboxRenderer* rdr)
{
	skin_widget_renderer.class_init((mWidgetRenderer*)rdr);
	rdr->drawDropDownBtn = skin_drawDropDownBtn;
}

mComboboxRenderer skin_combobox_renderer =
{
    "skin",
	skin_combobox_class_init,
	(void*)&skin_widget_renderer,
};
#endif

