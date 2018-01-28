/* 
 ** $Id:$
 **
 ** The implementation of mRadioNode class.
 **
 ** Copyright (C) 2009~2010 Feynman Software.
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
