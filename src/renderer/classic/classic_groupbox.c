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

#include "mgncs.h"

extern mWidgetRenderer classic_widget_renderer;

static void classic_drawGroupbox (mGroupBox *self, HDC hdc, const RECT *rect)
{
	DWORD color;
   
    color = ncsGetElement(self, NCS_BGC_3DBODY);

	self->renderer->draw3dbox(self, hdc, rect, color, LFRDR_BTN_STATUS_NORMAL);	
}

static void classic_groupbox_class_init (mGroupBoxRenderer* rdr)
{
	classic_widget_renderer.class_init((mWidgetRenderer*)rdr);
    rdr->drawGroupbox = classic_drawGroupbox;
}

mGroupBoxRenderer classic_groupbox_renderer =
{
    "",
	classic_groupbox_class_init,
	(mStaticRenderer*)(void *)&classic_widget_renderer
};

