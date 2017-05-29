/**
 * \file mchecknode.h
 * \author WangJian
 * \date 2010/01/26
 *
 * This file includes the definition of mRadioNode.
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

#ifndef _MGUI_WIDGET_RADIONODE_H
#define _MGUI_WIDGET_RADIONODE_H
 
typedef struct _mRadioNode  mRadioNode;
typedef struct _mRadioNodeClass mRadioNodeClass;

#define mRadioNodeHeader(className)  \
    mNodeHeader(className)  \
    mNode *selNode;

struct _mRadioNode
{
	mRadioNodeHeader(mRadioNode)
};

#define mRadioNodeClassHeader(clsName, superCls)        \
	mNodeClassHeader(clsName, superCls)                 \
    BOOL (*setRadioNode)(clsName*, mCheckNode *node);   \
    BOOL (*setRadioNodeByIndex)(clsName*, int idx); 
    

struct _mRadioNodeClass
{
	mRadioNodeClassHeader(mRadioNode, mNode)
};

MGNCS_EXPORT extern mRadioNodeClass g_stmRadioNodeCls;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_WIDGET_RADIONODE_H */

