/**
 * \file mscrollview.h
 * \author XiaoweiYan
 * \date 2009/06/13
 *
 * This file includes the definition of ScrollView.
 *
 \verbatim

    Copyright (C) 2009 Feynman Software.

    All rights reserved by Feynman Software.

    This file is part of mgncs, which is new control
    set of MiniGUI.

  	\endverbatim
 */

/**
 * $Id: mscrollview.h 1683 2017-10-26 06:52:09Z weiym $
 *
 *      Copyright (C) 2009 Feynman Software.
 */
#ifndef _MGUI_CTRL_SCRLV_H
#define _MGUI_CTRL_SCRLV_H


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \defgroup Widget_ScrollView mScrollView
 * @{
 */

/**
 * The structure of scrollview item information.
 */
typedef struct _NCS_SCRLV_ITEMINFO
{
    /** The index of item. */
    int     index;
    /** The height of item. */
    int     height;
    /** The additional data of item. */
    DWORD   addData;
}NCS_SCRLV_ITEMINFO;

typedef struct _mScrollView mScrollView;
typedef struct _mScrollViewClass mScrollViewClass;
typedef struct _mScrollViewRenderer mScrollViewRenderer;

/**
 * \def NCSCTRL_SCROLLVIEW
 * \brief The control class name of mScrollView.
 */
#define NCSCTRL_SCROLLVIEW NCSCLASSNAME("scrollview")

/**
 * \def NCSS_SCRLV_SHIFT
 * \brief The bits used by mScrollView in style.
 */
#define NCSS_SCRLV_SHIFT       (NCSS_ITEMV_SHIFT)

/**
 * \def NCSS_SCRLV_UPNOTIFY
 * \sa NCSS_ITEMV_UPNOTIFY
 */
#define NCSS_SCRLV_UPNOTIFY    NCSS_ITEMV_UPNOTIFY

/**
 * \def NCSS_SCRLV_LOOP
 * \sa NCSS_ITEMV_LOOP
 */
#define NCSS_SCRLV_LOOP        NCSS_ITEMV_LOOP

/**
 * \def NCSS_SCRLV_SORT
 * \sa NCSS_ITEMV_AUTOSORT
 */
#define NCSS_SCRLV_SORT        NCSS_ITEMV_AUTOSORT

#define mScrollViewHeader(className)    \
    mItemViewHeader(className)


/**
 * \struct mScrollView
 * \brief The structure of mScrollView control, which derived from mItemView.
 */
struct _mScrollView
{
	mScrollViewHeader(mScrollView)
};

#define mScrollViewClassHeader(clsName, parentClass)    \
    mItemViewClassHeader(clsName, parentClass)          \
    HITEM (*addItem)(clsName*, NCS_SCRLV_ITEMINFO *info, int *pos);

/**
 * \struct mScrollViewClass
 * \brief The virtual function table of mScrollView, which derived from mItemViewClass.
 *
 *  - HITEM (*addItem)(mIconView *self, \ref NCS_SCRLV_ITEMINFO *info, int *pos);\n
 *    Add a new item according to \a info.
 *      \param info     The item information for iconview.
 *      \param pos      the position in which item is inserted.
 */
struct _mScrollViewClass
{
	mScrollViewClassHeader(mScrollView, mItemView)
};

#define mScrollViewRendererHeader(clsName, parentClass) \
	mItemViewRendererHeader(clsName, parentClass)

/**
 * \struct mScrollViewRenderer
 * \brief The structure of mScrollView renderer, which inheried
 *        from mItemViewRenderer.
 */
struct  _mScrollViewRenderer {
	mScrollViewRendererHeader(mScrollView, mItemView)
};

/**
 * \enum mScrollViewProp
 * \brief The properties id of mScrollView.
 */
enum mScrollViewProp
{
    /**
     * The maximum value of mScrollView properties id.
     */
	NCSP_SCRLV_MAX = NCSP_ITEMV_MAX + 1,
};

/**
 * \enum mScrollViewNotify
 * \brief The notification code id of mScrollView.
 */
enum mScrollViewNotify
{
    /** Mouse clicked */
    NCSN_SCRLV_CLICKED = NCSN_ITEMV_CLICKED,
    /** Selected item changed. */
    NCSN_SCRLV_SELCHANGED = NCSN_ITEMV_SELCHANGED,
    /** Selected item is changing. */
    NCSN_SCRLV_SELCHANGING = NCSN_ITEMV_SELCHANGING,
    /**
     * The maximum value of mScrollView notification code id.
     */
    NCSN_SCRLV_MAX = NCSN_ITEMV_MAX + 1,
};

/**
 * \var g_stmScrollViewCls
 * \brief The instance of mScrollViewClass.
 *
 * \sa mScrollViewClass
 */
MGNCS_EXPORT extern mScrollViewClass g_stmScrollViewCls;
    /** @} end of Widget_ScrollView */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_SCRLV_H */
