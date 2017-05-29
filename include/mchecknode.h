/**
 * \file mchecknode.h
 * \author WangJian
 * \date 2010/01/26
 *
 * This file includes the definition of mCheckNode.
 *
 \verbatim

    Copyright (C) 2009~2010 Feynman Software.

    All rights reserved by Feynman Software.

    This file is part of mgncs, which is new control 
    set of MiniGUI.

  	\endverbatim
 */

/**
 * $Id$
 *
 *      Copyright (C) 2009 Feynman Software.
 */

#ifndef _MGUI_WIDGET_CHECKNODE_H
#define _MGUI_WIDGET_CHECKNODE_H
 
typedef struct _mCheckNode  mCheckNode;
typedef struct _mCheckNodeClass mCheckNodeClass;

#define mCheckNodeHeader(className)  \
    mNodeHeader(className)  \
    BOOL isChecked;

struct _mCheckNode
{
	mCheckNodeHeader(mCheckNode)
};

#define mCheckNodeClassHeader(clsName, superCls)    \
	mNodeClassHeader(clsName, superCls)             \
    BOOL (*setCheck)(clsName* , BOOL isCheck); 

struct _mCheckNodeClass
{
	mCheckNodeClassHeader(mCheckNode, mNode)
};

MGNCS_EXPORT extern mCheckNodeClass g_stmCheckNodeCls;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_WIDGET_CHECKNODE_H */

