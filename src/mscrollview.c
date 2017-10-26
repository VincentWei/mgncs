/*
 ** $Id: mscrollview.c 1681 2017-10-26 06:46:31Z weiym $
 **
 ** The implementation of mScrollView class.
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
#include "mdblist.h"
#include "mitem.h"
#include "mscroll_widget.h"
#include "mitem_manager.h"
#include "mitemview.h"
#include "mscrollview.h"

#include "mrdr.h"

static void mScrollView_construct (mScrollView *self, DWORD addData)
{
	g_stmItemViewCls.construct((mItemView*)self, addData);
    _c(self)->initMargins(self, 5, 5, 5, 5);
}

static HITEM mScrollView_addItem(mScrollView *self, NCS_SCRLV_ITEMINFO *info, int *pos)
{
    if (!info)
        return 0;

    return _c(self)->createItem(self, 0, 0, info->index,
            info->height, NULL, info->addData, pos, TRUE);
}


BEGIN_CMPT_CLASS(mScrollView, mItemView)
    CLASS_METHOD_MAP(mScrollView, construct);
    CLASS_METHOD_MAP(mScrollView, addItem);
	SET_DLGCODE(DLGC_WANTARROWS);
END_CMPT_CLASS

