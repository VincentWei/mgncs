/**
 * \file mlistcolumn.h
 * \author XiaoweiYan
 * \date 2009/02/24
 *
 * This file includes the definition of mListColumn.
 *
 \verbatim

    Copyright (C) 2009 Feynman Software.

    All rights reserved by Feynman Software.

    This file is part of mgncs, which is new control 
    set of MiniGUI.

  	\endverbatim
 */

/**
 * $Id: mlistcolumn.h 1116 2010-12-02 04:03:35Z dongjunjie $
 *
 *      Copyright (C) 2009 Feynman Software.
 */

#ifndef _MGUI_WIDGET_LSTCLM_H
#define _MGUI_WIDGET_LSTCLM_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \defgroup Widget_ListColumn mListColumn
 * @{
 */

typedef struct _mListColumn mListColumn;
typedef struct _mListColumnClass mListColumnClass;

/**
 * \enum ncsLstClmSortType
 * \brief The sort type.
 */
typedef enum 
{
    /**
     * No sort.
     */
    NCSID_LSTCLM_NOTSORTED = 0,

    /**
     * Sort ascending.
     */
    NCSID_LSTCLM_HISORTED,

    /**
     * Sort descending.
     */
    NCSID_LSTCLM_LOSORTED,

    NCSID_LSTCLM_MAXVALUE
}ncsLstClmSortType;

/**
 * \struct NCS_LSTCLM_SORTDATA
 * \brief The sort type.
 */
typedef struct _NCS_LSTCLM_SORTDATA
{
    /**
     * The column index.
     */
    int     column;

    /**
     * The sort type.
     */
    int     losorted;

    /**
     * The control class which obtains comparison items.
     */
    mWidget *obj;
}NCS_LSTCLM_SORTDATA;

/**
 * \var typedef int (*NCS_CB_LISTV_CMPCLM)(HITEM nItem1, HITEM nItem2, NCS_LSTCLM_SORTDATA *sortData)
 * \brief The callback of comparison column function.
 */
typedef int (*NCS_CB_LISTV_CMPCLM)(HITEM nItem1, HITEM nItem2, NCS_LSTCLM_SORTDATA *sortData);

/**
 * \def NCSF_LSTCLM_LEFTALIGN
 * \brief Horizontal left align.
 */
#define NCSF_LSTCLM_LEFTALIGN       (0x00<<NCSF_ITEM_SHIFT)

/**
 * \def NCSF_LSTCLM_RIGHTALIGN
 * \brief Horizontal right align.
 */
#define NCSF_LSTCLM_RIGHTALIGN      (0x01<<NCSF_ITEM_SHIFT)

/**
 * \def NCSF_LSTCLM_CENTERALIGN
 * \brief Horizontal center align.
 */
#define NCSF_LSTCLM_CENTERALIGN     (0x02<<NCSF_ITEM_SHIFT)

/**
 * \def NCSF_LSTCLM_TOPALIGN
 * \brief Vertical top align.
 */
#define NCSF_LSTCLM_TOPALIGN        (0x00<<NCSF_ITEM_SHIFT)

/**
 * \def NCSF_LSTCLM_BOTTOMALIGN
 * \brief Vertical bottom align.
 */
#define NCSF_LSTCLM_BOTTOMALIGN     (0x04<<NCSF_ITEM_SHIFT)

/**
 * \def NCSF_LSTCLM_VCENTERALIGN
 * \brief Vertical center align.
 */
#define NCSF_LSTCLM_VCENTERALIGN    (0x08<<NCSF_ITEM_SHIFT)

/**
 * \def NCSF_LSTCLM_VALIGNMASK
 * \brief The mask of vertical align.
 */
#define NCSF_LSTCLM_VALIGNMASK      (0x0C<<NCSF_ITEM_SHIFT)

/**
 * \def NCSF_LSTCLM_SHIFT
 * \brief The bits used by mListColumn in flags.
 */
#define NCSF_LSTCLM_SHIFT           (NCSF_ITEM_SHIFT+4)

#define mListColumnHeader(className)\
	mItemHeader(className)          \
    int startX;                     \
    int width;                      \
    ncsLstClmSortType       sort;   \
    NCS_CB_LISTV_CMPCLM     pfnCmp;

/**
 * \struct mListColumn
 * \brief The structure of mListColumn. It's used as header by mListView.
 *
 *  - startX\n
 *    The start x coordination. \n
 *
 *  - width\n
 *    The column width.
 *
 *  - sort\n
 *    The sort type.
 *
 *  - pfnCmp\n
 *    The comparison function.
 */
struct _mListColumn
{
	mListColumnHeader(mListColumn)
};

#define mListColumnClassHeader(clsName, superCls)   \
	mItemClassHeader(clsName, superCls)

/**
 * \struct mListColumnClass
 * \brief The virtual function table of mListColumn, which derived from
 *        mItemClass.
 */
struct _mListColumnClass
{
	mListColumnClassHeader(mListColumn, mItem)
};

/**
 * \enum mListColumnProp
 * \brief The properties id of mListColumn.
 */
typedef enum 
{
    /**
     * The start x coordination.
     */
    NCSP_LSTCLM_POSX = NCSP_ITEM_MAX + 1,

    /**
     * The column width.
     */
    NCSP_LSTCLM_WIDTH,

    /**
     * The sort type.
     */
    NCSP_LSTCLM_SORTTYPE,

    /**
     * The comparison function.
     */
    NCSP_LSTCLM_CMPFUNC,

    /**
     * The maximum value of ListColumn properties id.
     */
	NCSP_LSTCLM_MAX,
}mListColumnProp;

/**
 * \var g_stmListColumnCls
 * \brief The instance of mListColumnClass.
 *
 * \sa mListColumnClass
 */
MGNCS_EXPORT extern mListColumnClass g_stmListColumnCls;

    /** @} end of Widget_ListColumn */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_WIDGET_LSTCLM_H */

