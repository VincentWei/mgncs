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
    The implementation of mScrollView class.

    This file is part of mGNCS, a component for MiniGUI.

    Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

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

