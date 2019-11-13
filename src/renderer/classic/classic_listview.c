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
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
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

extern mWidgetRenderer classic_widget_renderer;

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

static void classic_drawFold (mListView *self, HDC hdc,
        const RECT* pRect, DWORD color, HICON icon, int status, int next)
{
    int i;
    int pen_width;
    int w, h, minSize;
    int centerX, centerY;
    gal_pixel old_pen_color;

    w = RECTWP(pRect);
    h = RECTHP(pRect);

    if(w < 4 || h < 4) return;

    centerX = pRect->left + (w>>1);
    centerY = pRect->top + (h>>1);

    if (status & LFRDR_TREE_WITHICON) {
        char szValue[255];
        HICON hFoldIcon   =
            (HICON)RetrieveRes (_get_icon_file("classic", (char*)SYSICON_TREEFOLD, szValue));
        HICON hUnFoldIcon =
            (HICON)RetrieveRes (_get_icon_file("classic", (char*)SYSICON_TREEUNFOLD, szValue));

        if (status & LFRDR_TREE_CHILD) {
            if (status & LFRDR_TREE_FOLD) {
                if (hFoldIcon)
                    DrawIcon (hdc, centerX, centerY - (h>>1),
                            h, h, icon? icon : hFoldIcon);
            }
            else {
                if (hUnFoldIcon)
                    DrawIcon (hdc, centerX, centerY - (h>>1),
                            h, h, icon ? icon : hUnFoldIcon);
            }
        }
    }
    else {
        old_pen_color = SetPenColor(hdc, RGBA2Pixel(hdc,GetRValue(color),
                    GetGValue(color), GetBValue(color), GetAValue(color)));

        minSize = MIN(w, h)>>1;
        pen_width = (minSize>>3) + 1;

        centerX = pRect->left + (w>>1);
        centerY = pRect->top + (h>>1);

        if (status & LFRDR_TREE_CHILD) {
            MoveTo (hdc, centerX, centerY - h + minSize);
            LineTo (hdc, centerX, centerY - minSize);
            MoveTo (hdc, centerX + minSize, centerY);

            if(status & LFRDR_TREE_FOLD)
                LineTo (hdc, centerX + w, centerY);
            else
                LineTo (hdc, centerX + w, centerY);
        }
        else {
            MoveTo (hdc, centerX, centerY - h + minSize);
            LineTo (hdc, centerX, centerY);
            LineTo (hdc, centerX + w, centerY);
        }

        if (!(status & LFRDR_TREE_CHILD)) {
            SetPenColor(hdc, old_pen_color);
            return;
        }

        for(i = 0; i < pen_width; i++)
        {
            Rectangle(hdc, centerX - minSize + i, centerY - minSize + i,
                    centerX + minSize - 1 - i, centerY + minSize - 1 - i);

            MoveTo(hdc, centerX - minSize + 2 * pen_width,
                    centerY - (pen_width>>1) + i);
            LineTo(hdc, centerX + minSize -1 - 2 * pen_width,
                    centerY - (pen_width>>1) + i);

            if(status & LFRDR_TREE_FOLD)
            {
                MoveTo(hdc, centerX - (pen_width>>1) + i,
                        centerY - minSize + 2 * pen_width);
                LineTo(hdc, centerX - (pen_width>>1) + i,
                        centerY + minSize - 1 - 2 * pen_width);
            }
        }

        SetPenColor(hdc, old_pen_color);
    }
}

static void classic_drawHeader (mListView *self, HDC hdc, const RECT *pRect, DWORD color, int flag)
{
    DWORD c;
    if(color ==0)
        c = ncsGetElement((mWidget*)self, NCS_BGC_3DBODY);
    else
        c = color;

    self->renderer->draw3dbox(self, hdc, pRect, c, flag | NCSRF_FILL);
}

static void classic_listview_class_init (mListViewRenderer* rdr)
{
	classic_widget_renderer.class_init((mWidgetRenderer*)rdr);
	rdr->drawFold = classic_drawFold;
	rdr->drawHeader = classic_drawHeader;
}

mListViewRenderer classic_listview_renderer = {
    "",
	classic_listview_class_init,
	(mItemViewRenderer*)(void *)&classic_widget_renderer
};

#endif //_MGNCSCTRL_LISTVIEW

