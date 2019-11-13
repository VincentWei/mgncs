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

extern mWidgetRenderer skin_widget_renderer;


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

extern char* gui_GetIconFile(const char* rdr_name, char* file, char* _szValue);
static void skin_drawFold (mListView *self, HDC hdc,
        const RECT* pRect, DWORD color, HICON icon, int status, int next)
{
    if (status & LFRDR_TREE_WITHICON) {
        int w, h;
        int centerX, centerY;
        char szValue[255];
        HICON hFoldIcon   =
            (HICON)RetrieveRes (_get_icon_file("skin", (char*)SYSICON_TREEFOLD, szValue));
        HICON hUnFoldIcon =
            (HICON)RetrieveRes (_get_icon_file("skin", (char*)SYSICON_TREEUNFOLD, szValue));

        w = RECTWP(pRect);
        h = RECTHP(pRect);

        if(w < 4 || h < 4) return;

        centerX = pRect->left + (w>>1);
        centerY = pRect->top + (h>>1);

        if (status & LFRDR_TREE_CHILD) {
            if (status & LFRDR_TREE_FOLD) {
                if (hFoldIcon)
                    DrawIcon (hdc, centerX, centerY - (h>>1),
                            h, h, icon?icon:hFoldIcon);
            }
            else {
                if (hUnFoldIcon)
                    DrawIcon (hdc, centerX, centerY - (h>>1),
                            h, h, icon?icon:hUnFoldIcon);
            }
        }
    }
    else {
        const BITMAP* bitmap;
        DWORD file;
        int flag = 0;

        file = ncsGetElement((mWidget*)self, NCS_IMAGE_LISTV_TREE);
        if (!(bitmap = GetBitmapFromRes (file)))
            return;

        if (status & LFRDR_TREE_CHILD) {
            if (status & LFRDR_TREE_FOLD)
                flag = 0;
            else
                flag = 1;

            if (bitmap) {
                FillBoxWithBitmapPart (hdc, pRect->left, pRect->top,
                        bitmap->bmWidth, (bitmap->bmHeight)>>1,
                        0, 0, bitmap, 0, flag * ((bitmap->bmHeight)>>1));
            }
        }
    }
}

static void skin_drawHeader (mListView *self, HDC hdc, const RECT *pRect, DWORD color, int flag)
{
    const BITMAP* bitmap;
    DWORD file;
    DRAWINFO _di;

    file = ncsGetElement((mWidget*)self, NCS_IMAGE_LISTV_HDR);
    if (!(bitmap = GetBitmapFromRes (file)))
        return;

    _di.bmp      = bitmap;
    _di.nr_line  = 4;
    _di.nr_col   = 1;
    _di.idx_line = 0;
    _di.idx_col  = 0;
    _di.margin1  = 2;
    _di.margin2  = 2;
    _di.direct   = FALSE;
    _di.flip     = FALSE;
    _di.style    = DI_FILL_TILE;

    ncsSkinDraw (hdc, pRect, &_di);
}

static void skin_listview_class_init (mListViewRenderer* rdr)
{
	skin_widget_renderer.class_init((mWidgetRenderer*)rdr);
	rdr->drawFold = skin_drawFold;
	rdr->drawHeader = skin_drawHeader;
}

mListViewRenderer skin_listview_renderer = {
    "",
	skin_listview_class_init,
	(mItemViewRenderer*)(void *)&skin_widget_renderer,
};

#endif //_MGNCSCTRL_LISTVIEW

