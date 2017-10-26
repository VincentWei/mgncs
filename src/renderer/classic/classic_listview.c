
/*
 ** $Id: classic_listview.c 1681 2017-10-26 06:46:31Z weiym $
 **
 ** The classic renderer implementation of mListView control.
 **
 ** Copyright (C) 2009 Feynman Software.
 **
 ** All rights reserved by Feynman Software.
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
#include "mscroll_widget.h"
#include "mdblist.h"
#include "mitem.h"
#include "mitem_manager.h"
#include "mitemview.h"
#include "mscrollview.h"
#include "mlistcolumn.h"
#include "mlistitem.h"
#include "mlistview.h"
#include "mrdr.h"

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

