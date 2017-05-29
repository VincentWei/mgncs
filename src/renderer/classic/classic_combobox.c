/**
 ** classic_combobox.c
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

extern mWidgetRenderer classic_widget_renderer;

static void classic_drawDropDownBtn (mCombobox *self, HDC hdc, const RECT *rect)
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

static void classic_combobox_class_init (mComboboxRenderer* rdr)
{
	classic_widget_renderer.class_init((mWidgetRenderer*)rdr);
	rdr->drawDropDownBtn = classic_drawDropDownBtn;
}

mComboboxRenderer classic_combobox_renderer =
{
    "classic",
	classic_combobox_class_init,
	(void*)&classic_widget_renderer
};

