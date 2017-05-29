/* 
 ** $Id: fashion_listview.c 641 2009-11-10 01:48:49Z xwyan $
 **
 ** The fashion renderer implementation of mListView control.
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

