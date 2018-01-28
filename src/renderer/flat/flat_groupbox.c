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

#ifdef _MGNCS_RDR_FLAT

extern mStaticRenderer flat_widget_renderer;

static void flat_drawGroupbox (mGroupBox *self, HDC hdc, const RECT *rect)
{
	DWORD color;
   
    color = ncsGetElement(self, NCS_BGC_3DBODY);

	self->renderer->draw3dbox(self, hdc, rect, color, LFRDR_BTN_STATUS_NORMAL);	
}

static void flat_groupbox_class_init (mGroupBoxRenderer* rdr)
{
	flat_widget_renderer.class_init((mStaticRenderer*)rdr);
    rdr->drawGroupbox = flat_drawGroupbox;
}

mGroupBoxRenderer flat_groupbox_renderer =
{
    "",
	flat_groupbox_class_init,
	&flat_widget_renderer
};

#endif

