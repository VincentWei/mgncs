///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
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

#include "mgncs.h"

#ifdef _MGNCSCTRL_LISTVIEW

#ifdef _MGNCS_RDR_FASHION

extern mWidgetRenderer fashion_widget_renderer;

static char* _get_icon_file(const char* rdr_name, char* file, char* _szValue)
{
    char szValue[MAX_NAME + 1];
    char *iconname;

    strcpy(szValue, "icon/");
    iconname = szValue + strlen(szValue);

    if (GetMgEtcValue (rdr_name, file,
                iconname, sizeof(szValue)-(iconname-szValue)) < 0 ) {
        _MG_PRINTF ("SYSRES: can't get %s's value from section %s in etc.\n",
                file, rdr_name);
        return NULL;
    }
    strcpy(_szValue, szValue);
    return _szValue;
}

static void fashion_drawFold (mListView *self, HDC hdc,
        const RECT* pRect, DWORD color, HICON icon, int status, int next)
{
    if (status & LFRDR_TREE_WITHICON) {
        int w, h;
        int centerX, centerY;
        char szValue[255];
        HICON hFoldIcon   = (HICON)RetrieveRes (_get_icon_file("fashion", (char*)SYSICON_TREEFOLD, szValue));
        HICON hUnFoldIcon = (HICON)RetrieveRes (_get_icon_file("fashion", (char*)SYSICON_TREEUNFOLD, szValue));

        w = RECTWP(pRect);
        h = RECTHP(pRect);

        if(w < 4 || h < 4) return;

        centerX = pRect->left + (w>>1);
        centerY = pRect->top + (h>>1);

        if (status & LFRDR_TREE_CHILD) {
            if (status & LFRDR_TREE_FOLD) {
                if (hFoldIcon)
                    DrawIcon (hdc, centerX, centerY - (h>>1),
                            h, h, icon ? icon : hFoldIcon);
            }
            else {
                if (hUnFoldIcon)
                    DrawIcon (hdc, centerX, centerY - (h>>1),
                            h, h, icon ? icon : hUnFoldIcon);
            }
        }
    }
    else {
        int which;

        if (status & LFRDR_TREE_CHILD) {
            if (status & LFRDR_TREE_FOLD)
                which = NCSR_ARROW_RIGHT;
            else
                which = NCSR_ARROW_DOWN;
        }
        else
            return;

        return self->renderer->drawArrow(self, hdc, pRect, which, color, NCSRF_FILL);
    }
}

static void fashion_drawHeader (mListView *self, HDC hdc, const RECT *pRect, DWORD color, int flag)
{
    RECT gradient_rect;
    DWORD bkColor;

    if (pRect->right < pRect->left || pRect->bottom < pRect->top) {
        return;
    }

    bkColor = ncsGetElement((mWidget*)self, NCS_BGC_3DBODY);

    gradient_rect.left = pRect->left + 1;
    gradient_rect.right = pRect->right;
    gradient_rect.top = pRect->top + 1;
    gradient_rect.bottom = pRect->bottom - 1;

/*	fashion_fillRoundRect(hdc,
        ncsCommRDRCalc3dboxColor(bkColor, NCSR_COLOR_LIGHTER),
        ncsCommRDRCalc3dboxColor(bkColor, NCSR_COLOR_DARKER),
		ncsGetElement(self, NCS_MODE_BGC),
        &gradient_rect,
		ncsGetElement(self, NCS_METRICS_3DBODY_ROUNDX),
		ncsGetElement(self, NCS_METRICS_3DBODY_ROUNDY));*/

	fashion_widget_renderer.draw3dbox((mWidget*)self,
            hdc,&gradient_rect, bkColor, 0);
}

static void fashion_listview_class_init (mListViewRenderer* rdr)
{
	fashion_widget_renderer.class_init((mWidgetRenderer*)rdr);
	rdr->drawFold = fashion_drawFold;
	rdr->drawHeader = fashion_drawHeader;
}

mListViewRenderer fashion_listview_renderer = {
    "",
	fashion_listview_class_init,
	(mItemViewRenderer*)(void *)&fashion_widget_renderer
};

#endif
#endif //_MGNCSCTRL_LISTVIEW

