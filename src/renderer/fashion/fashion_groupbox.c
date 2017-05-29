
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
#include "mstatic.h"
#include "mgroupbox.h"
#include "mrdr.h"
#include "fashion_common.h"

#ifdef _MGNCS_RDR_FASHION
extern mWidgetRenderer fashion_widget_renderer;

static void fashion_drawGroupbox (mGroupBox *self, HDC hdc, const RECT *rc)
{
	DWORD color;
	int flat = ncsGetElement(self, NCS_MODE_USEFLAT);
	int rx = ncsGetElement(self, NCS_METRICS_3DBODY_ROUNDX);
	int ry = ncsGetElement(self, NCS_METRICS_3DBODY_ROUNDY);

	int bkid = NCS_BGC_3DBODY;
	color = ncsGetElement(self, bkid);

	//draw 3d round box
	if(flat && flat != -1)
	{
		fashion_drawRoundRect(hdc, rc, rx, ry, color);
	}
	else
	{
		ncsCommRDRDraw3DRoundBox(hdc, rc, rx, ry, color, 0);
	}
   
}

static void fashion_groupbox_class_init (mGroupBoxRenderer* rdr)
{
	fashion_widget_renderer.class_init((mWidgetRenderer*)rdr);
    rdr->drawGroupbox = fashion_drawGroupbox;
}

mGroupBoxRenderer fashion_groupbox_renderer =
{
    "",
	fashion_groupbox_class_init,
	(void*)&fashion_widget_renderer
};


#endif

