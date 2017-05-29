/**
 * \file mlist.h
 * \author XiaoweiYan
 * \date 2009/11/10
 *
 * This file includes the definition of mList.
 *
 \verbatim

    Copyright (C) 2009~2010 Feynman Software.

    All rights reserved by Feynman Software.

    This file is part of mgncs, which is new control 
    set of MiniGUI.

  	\endverbatim
 */

/**
 * $Id: mlist.h 1803 2010-03-19 02:21:53Z dongjunjie $
 *
 *      Copyright (C) 2009~2010 Feynman Software.
 */

#ifndef _MGUI_NCSCTRL_LIST_H
#define _MGUI_NCSCTRL_LIST_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \defgroup Widget_List mList
 * @{
 */

/**
 * \def NCSCTRL_LIST
 * \brief the name of mList
 */
#define NCSCTRL_LIST                NCSCLASSNAME("list")

typedef struct _mList mList;
typedef struct _mListClass mListClass;
typedef struct _mListRenderer mListRenderer;

/**
 * \def NCSS_LIST_LOOP
 * \brief Travel all items circularly.
 */
#define NCSS_LIST_LOOP              (0x0001<<NCSS_ASTLST_SHIFT)

/**
 * \def NCSS_LIST_MULTIPLE
 * \brief Allow user to select multiple items. Reserved.
 */
#define NCSS_LIST_MULTIPLE          (0x0002<<NCSS_ASTLST_SHIFT)

/**
 * \def NCSS_LIST_SHIFT
 * \brief The bits used by mList in style.
 */
#define NCSS_LIST_SHIFT             (NCSS_ASTLST_SHIFT+2) 

#define mListHeader(clsName)        \
	mAbstractListHeader(clsName)    \
    list_t  selList;                \
    mListLayout *layout;            \
    mNode   *hiliteNode;            \
    Uint16  nodesHeight;

/**
 * \struct mList
 *
 * \brief The structure of mList, which derived from mAbstractList.
 *        It is the normal list control that can show nodes in 
 *        different layout.
 *
 *  - hiliteNode\n
 *    The high lighted node.
 *
 *  - layout\n
 *    The layout function ops.
 *
 *  - nodesHeight\n
 *    The total height of children directly. 
 */
struct _mList
{
	mListHeader(mList)
};

#define mListClassHeader(clsName, superCls)             \
	mAbstractListClassHeader(clsName, superCls)         \
    void (*resetContent)(clsName*);                     \
    void (*removeAll)(clsName*);                        \
    void (*enterSubList)(clsName*, mNode* selNode);     \
    void (*backUpList)(clsName*, mNode* selNode);       \
    BOOL (*setNodeDefSize)(clsName*, int defW,int defH);\
    BOOL (*setLayout)(clsName*, mListLayout* layout, int defNodeW, int defNodeH); \
    mListLayout* (*getLayout)(clsName*);
    
/**
 * \struct mListClass
 * \brief The virtual function table of mList, which derived from
 *        mAbstractListClass.
 *
 *  - void (*\b resetContent)(mList *self);\n                     
 *    Reset all property and remove all nodes.
 *
 *  - void (*\b removeAll)(mList *self);\n                        
 *    Remove all nodes.
 *
 *  - BOOL (*\b selectNode)(mList *self, mNode *node, BOOL select);\n 
 *    Select of deselect the specified nodes.
 *      \param node     The specified node.
 *      \param select   Whether select node or not.
 *      \return TRUE on success, otherwise FALSE.
 *
 *  - BOOL (*\b setCurSel)(mList *self, int index);\n             
 *    Hilight the specified node.
 *      \param index    The index of the specified node.
 *      \return TRUE on success, otherwise FALSE.
 *
 *  - mNode* (*\b getCurSel)(mList *self);\n                      
 *    Get the hilight node.
 *
 *  - void (*\b enterSubList)(mList *self, mNode *selNode);\n 
 *    Enter sub list.
 *
 *  - void (*\b backUpList)(mList *self, mNode *selNode);\n 
 *    Back upper list.
 */
struct _mListClass
{
	mListClassHeader(mList, mAbstractList)
};

#define mListRendererHeader(clsName, parentClass) \
	mAbstractListRendererHeader(clsName, parentClass)

/**
 * \struct mListRenderer
 * \brief The structure of mList renderer, which inheried
 *        from mAbstractListRenderer.
 */
struct  _mListRenderer {
	mListRendererHeader(mList, mAbstractList)
};


/**
 * \enum ncsListLayoutID
 * \brief The layout id of mList.
 */
enum ncsListLayoutID
{
    /**
     * The default layout. List.
     */
    NCS_LIST_LLIST = 0,

    /**
     * The iconview layout.
     */
    NCS_LIST_LICONV,

    NCS_LIST_LICONH,

    NCS_LIST_LCBOXH,

    NCS_LIST_LGROUP,

    NCS_LIST_LUSERDEFINED,
    /**
     * The maximum value of mList layout id.
     */
    NCS_LIST_MAX,
};

enum mListEvent
{
    NCSE_LIST_FONTCHANGED = 100,
    NCSE_LIST_SIZECHANGED,
    NCSE_LIST_ROOTCHANGING, //new root
    NCSE_LIST_ROOTCHANGED,  //new root
};

enum mListDirKeyFlags{
    NCSF_LIST_UP     = 0x01,
    NCSF_LIST_DOWN   = 0x02,
    NCSF_LIST_LEFT   = 0x04,
    NCSF_LIST_RIGHT  = 0x08,
    NCSF_LIST_HOME   = 0x10,
    NCSF_LIST_END    = 0x20,
    NCSF_LIST_LOOP   = 0x100,
    NCSF_LIST_DIRMASK= 0xFF,
};
/**
 * \enum mListProp
 * \brief The properties id of mList.
 */
enum mListProp
{
    /**
     * The layout id. ncsListLayoutID
     */
    NCSP_LIST_LAYOUT = NCSP_ASTLST_MAX + 1,

    /**
     * The default node width.
     */
    NCSP_LIST_DEFNODEWIDTH,

    /**
     * The default node height.
     */
    NCSP_LIST_DEFNODEHEIGHT,

    /**
     * The maximum value of mList properties id.
     */
    NCSP_LIST_MAX,
};

/**
 * \enum mListNotify
 * \brief The notification id of mList.
 */
enum mListNotify
{
    /**
     * Selected node changed. 
     * - param : mNode*, the pointer to selected node.
     */
    NCSN_LIST_SELCHANGED = NCSN_ASTLST_MAX + 1,

    /**
     * User has pressed the ENTER key.
     * - param : mNode*, current selected node.
     */
    NCSN_LIST_ENTER,

    /**
     * User has pressed the BackSpace key.
     * - param : mNode*, current selected node.
     */
    NCSN_LIST_BACKSPACE,

    /**
     * It will enter sub list.
     * - param : mNode*, current selected node.
     */
    NCSN_LIST_ENTERSUBLIST, 

    /**
     * It will back upper list.
     * - param : mNode*, current selected node.
     */
    NCSN_LIST_BACKUPLIST,

    NCSN_LIST_ROOTNODECHANGED,
    /**
     * The maximum value of mAbstractList notification code id.
     */
    NCSN_LIST_MAX,
};

/**
 * \var g_stmListCls
 * \brief The instance of mListClass.
 *
 * \sa mListClass
 */
MGNCS_EXPORT extern mListClass g_stmListCls;

/**
 * \fn void mList_foreachNode(mList *list, void (*foreach)(mNode *node, void* param), void* param);
 */

MGNCS_EXPORT void mList_foreachNode(mList *list, void (*foreach)(mNode *node, void* param), void* param);

    /** @} end of Widget_List */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_NCSCTRL_LIST_H */

