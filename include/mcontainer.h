
/**
 * \file mcontainer.h
 * \author XiaoweiYan
 * \date 2009/02/24
 *
 * This file includes the definition of mContainer control.
 *
 \verbatim

    Copyright (C) 2009 Feynman Software.

    All rights reserved by Feynman Software.

    This file is part of mgncs, which is new control
    set of MiniGUI.

  	\endverbatim
 */


/**
 * $Id: mcontainer.h 1683 2017-10-26 06:52:09Z weiym $
 *
 *          Copyright (C) 2009 Feynman Software.
 */
#ifdef _MGNCSCTRL_CONTAINER

#ifndef _MGUI_WIDGET_CTNR_H
#define _MGUI_WIDGET_CTNR_H


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \defgroup Control_Container mContainer
 * @{
 */

/**
 * \def NCSCTRL_CONTAINER
 * \brief The control class name of mContainer.
 */
#define NCSCTRL_CONTAINER        NCSCLASSNAME("container")

typedef struct _mContainer mContainer;
typedef struct _mContainerClass mContainerClass;
typedef struct _mContainerRenderer mContainerRenderer;

#define mContainerHeader(className) \
	mScrollWidgetHeader(className) \
    HWND hPanel;

/**
 * \struct mContainer
 * \brief The structure of mContainer, which derived from \a mScrollWidget.
 *        It is a control with scrollbars which is capable of containing other
 *        controls. When the user uses the scrollbars, it will scroll its content
 *        automatically.
 *
 *      - hPanel \n
 *      The handle of panel in scrollcontainer.
 */
struct _mContainer
{
	mContainerHeader(mContainer)
};

#define mContainerClassHeader(clsName, parentClass)                   \
	mScrollWidgetClassHeader(clsName, parentClass)                    \
    BOOL (*addIntrinsicControls)(clsName* self, const PCTRLDATA pCtrl, int nCount); \
    HWND (*setFocus)(clsName* self, int id); \
    HWND (*getFocus)(clsName* self); \
	HWND (*getPanel)(clsName* self); \
	void (*adjustContent)(clsName *self);

/**
 * \struct mContainerClass
 * \brief The virtual function table of mContainer, which derived from
 *        mScrollWidgetClass.
 *
 *  - BOOL (*\b addIntrinsicControls)(mContainer *self, const PCTRLDATA pCtrl, int nCount); \n
 *    Add intrinsic controls in MiniGUI to container.
 *      \param pCtrl      The pointer to control data in MiniGUI V3.0.x.
 *      \param nCount     The number of controls.
 *      \return           TRUE for success, otherwise FALSE.
 *
 *  - HWND (* \b setFocus)(mContainer *self, int id); \n
 *    Set focus control.
 *      \param id         The identifier of active child.
 *      \return           The handle to the old active child of its parent.
 *
 *  - HWND (*\b getFocus)(mContainer *self);\n
 *    Get focus control.
 *      \return           The handle to the active child.
 *
 *  - HWND (*\b getPanel)(mContainer *self);\n
 *    Get the panel handle.
 *     	\return 		  The panel handler of container.
 *
 *  - void (*\b adjustContent)(mContainer *self); \n
 *    Adjust the content size.
 */
struct _mContainerClass
{
	mContainerClassHeader(mContainer, mScrollWidget)
};

#define mContainerRendererHeader(clsName, parentClass) \
	mScrollWidgetRendererHeader(clsName, parentClass)

/**
 * \struct mContainerRenderer
 * \brief The renderer structure of mContainer, which inheried from
 *        mScrollWidgetRenderer.
 */
struct  _mContainerRenderer {
	mContainerRendererHeader(mContainer, mScrollWidget)
};


/**
 * \var g_stmContainerCls
 * \brief The instance of mContainerClass.
 *
 * \sa mContainerClass
 */
MGNCS_EXPORT extern mContainerClass g_stmContainerCls;

/**
 * \def NCSS_CTNR_SHIFT
 * \brief The bits used by mContainer in style.
 */
#define NCSS_CTNR_SHIFT NCSS_SWGT_SHIFT

/**
 * \enum mContainerNotify
 * \brief The notification code id of mContainer.
 */
enum mContainerNotify {
    /**
     * The maximum value of mContainer notification code id.
     */
	NCSN_CTNR_MAX = NCSN_SWGT_MAX + 1
};

    /** @} end of Control_Container */
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_WIDGET_CTNR_H */
#endif 	//_MGNCSCTRL_CONTAINER
