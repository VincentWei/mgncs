/*
 *   This file is part of mGNCS, a component for MiniGUI.
 * 
 *   Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgplus/mgplus.h>

#include "mgncs.h"
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

