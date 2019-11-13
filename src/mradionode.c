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
    The implementation of mRadioNode class.

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
    <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "mdblist.h"
#include "mnode.h"
#include "mabstractlist.h"
#include "mchecknode.h"
#include "mradionode.h"

static void mRadioNode_construct(mRadioNode *self, DWORD addData)
{
	Class(mNode).construct((mNode *)self, addData);
    self->selNode = NULL;    
}

static BOOL mRadioNode_setRadioNode(mRadioNode *self, mCheckNode *node)
{
    mCheckNode *old = (mCheckNode*)self->selNode;
    
    self->selNode = (mNode*)node;
    
    if (old)
        _c(old)->setCheck(old, FALSE);

    if (node)
        _c(node)->setCheck(node, TRUE);

    return TRUE;
}

static BOOL mRadioNode_setRadioNodeByIndex(mRadioNode *self, int idx)
{
    mCheckNode *node = (mCheckNode *)_c(self)->getNode(self, idx);
    
    return _c(self)->setRadioNode(self, node);
}

BEGIN_MINI_CLASS(mRadioNode, mNode)
	CLASS_METHOD_MAP(mRadioNode, construct)
	CLASS_METHOD_MAP(mRadioNode, setRadioNode)
	CLASS_METHOD_MAP(mRadioNode, setRadioNodeByIndex)
END_MINI_CLASS
