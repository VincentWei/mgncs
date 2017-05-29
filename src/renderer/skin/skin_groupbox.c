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

#ifdef _MGNCS_RDR_SKIN

extern mWidgetRenderer skin_widget_renderer;

static void skin_drawGroupbox (mGroupBox *self, HDC hdc, const RECT *rect)
{
	DWORD color;
   
    color = ncsGetElement(self, NCS_BGC_3DBODY);

	self->renderer->draw3dbox(self, hdc, rect, color, 0);	
}

static void skin_groupbox_class_init (mGroupBoxRenderer* rdr)
{
	skin_widget_renderer.class_init((mWidgetRenderer*)rdr);
    rdr->drawGroupbox = skin_drawGroupbox;
}

mGroupBoxRenderer skin_groupbox_renderer =
{
    "",
	skin_groupbox_class_init,
	(void*)&skin_widget_renderer
};


#endif

