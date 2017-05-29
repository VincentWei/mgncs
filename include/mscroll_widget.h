/**
 * \file mscroll_widget.h
 * \author XiaoweiYan
 * \date 2009/02/24
 *
 * This file includes the definition of ScrollWidget.
 *
 \verbatim

    Copyright (C) 2009 Feynman Software.

    All rights reserved by Feynman Software.

    This file is part of ncs, which is new control 
    set of MiniGUI.

  	\endverbatim
 */

/**
 * $Id: mscroll_widget.h 642 2009-11-10 02:58:43Z xwyan $
 *
 *          Copyright (C) 2009 Feynman Software.
 */

#ifndef _MGUI_WIDGET_SCROLL_H
#define _MGUI_WIDGET_SCROLL_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \defgroup Widget_ScrollWidget mScrollWidget
 * @{
 */


/**
 * \def NCSCTRL_SCROLLWIDGET
 * \brief the name of mScrollWidget control
 */
#define NCSCTRL_SCROLLWIDGET   NCSCLASSNAME("scrollwidget")

typedef struct _mScrollWidget mScrollWidget;
typedef struct _mScrollWidgetClass mScrollWidgetClass;
typedef struct _mScrollWidgetRenderer mScrollWidgetRenderer;

/**
 * \enum ncsSwgtDrawMode
 * \brief Scroll bar drawing mode in ScrollWidget.
 */
enum ncsSwgtDrawMode
{
  /**
   * The scroll bar is shown or hidded automatically.
   */
   NCSID_SWGT_AUTO = 0,
  /**
   * The scroll bar is always visible.
   */
   NCSID_SWGT_ALWAYS,
  /** 
   * The scroll bar is never visbile.
   */
   NCSID_SWGT_NEVER
};

#define mScrollWidgetHeader(className)  \
	mWidgetHeader(className)            \
    unsigned int realContWidth;         \
    unsigned int realContHeight;        \
    unsigned int hStepVal;              \
    unsigned int vStepVal;              \
    int          contX;                 \
    int          contY;                 \
    unsigned int contWidth;             \
    unsigned int contHeight;            \
    unsigned int visWidth;              \
    unsigned int visHeight;             \
    unsigned int leftMargin;            \
    unsigned int topMargin;             \
    unsigned int rightMargin;           \
    unsigned int bottomMargin;          \
    unsigned int drawMode;

/**
 * \struct mScrollWidget
 * \brief The structure of mScrollWidget control, which derived from mWidget.
 *        It is the abstract superclass of all classes which represent controls
 *        that have standard scroll bars.
 *
 *      - stepHVal\n 
 *        The horizontal step value.
 *
 *      - stepVVal\n 
 *        The vertical step value.
 *
 *      - contX\n
 *        The x position of current content.
 *
 *      - contY\n
 *        The y position of current content.
 *
 *      - contWidth\n
 *        The content width.
 *
 *      - contHeight\n
 *        The content height.
 *
 *      - visWidth\n
 *        The visible content width.
 *
 *      - visHeight\n
 *        The visible content height.
 *
 *      - leftMargin\n
 *        The value of left margin.
 *
 *      - topMargin\n
 *        The value of top margin.
 *
 *      - rightMargin\n
 *        The value of right margin.
 *
 *      - bottomMargin\n
 *        The value of bottom margin.
 *
 *      - drawMode\n
 *        The scrollbar drawing mode. It can be one of the following values:
 *          - NCSID_SWGT_AUTO \n
 *            The scroll bar is shown or hided automatically. \n
 *          - NCSID_SWGT_ALWAYS \n
 *            The scroll bar is always visible. \n
 *          - NCSID_SWGT_NEVER \n
 *            The scroll bar is never visbile. \n
 */
struct _mScrollWidget
{
	mScrollWidgetHeader(mScrollWidget)
};

#define mScrollWidgetClassHeader(clsName, parentClass)          \
	mWidgetClassHeader(clsName, parentClass)                    \
    void (*onHScroll)(clsName*, int code, int mouseX);          \
    void (*onVScroll)(clsName*, int code, int mouseY);          \
    void (*initMargins)(clsName*, int l, int t, int r, int b);  \
    void (*moveContent)(clsName*);                              \
    void (*resetViewPort)(clsName*, unsigned int,unsigned int); \
    void (*setScrollInfo)(clsName*, BOOL reDraw);               \
    BOOL (*makePosVisible)(clsName*, int x, int y);             \
    void (*getContRect)(clsName*, RECT *rcCont);                \
    void (*getVisRect)(clsName*, RECT *rcVis);                  \
    void (*viewportToWindow)(clsName*, int *x, int *y);         \
    void (*viewportToContent)(clsName*, int *x, int *y);        \
    void (*windowToViewport)(clsName*, int *x, int *y);         \
    void (*contentToViewport)(clsName*, int *x, int *y);        \
    void (*contentToWindow)(clsName*, int *x, int *y);          \
    void (*windowToContent)(clsName*, int *x, int *y);          \
    void (*refreshRect)(clsName*, const RECT *rc);              \
    BOOL (*isVisible)(clsName*, int x, int y);

/** 
 * \struct mScrollWidgetClass
 * \brief The virtual function table of mScrollWidget, which derived 
 *        from mWidgetClass.
 *
 *  - void (*\b initMargins)(mScrollWidget *self, int l, int t, int r, int b);  \n
 *    Initialize margin rectangle.
 *
 *  - void (*\b moveContent)(mScrollWidget *self); \n
 *    The function is used to move content. It can be implemented by subclass. 
 *
 *  - void (*\b resetViewPort)(mScrollWidget *self, unsigned int visW, unsigned int visH);\n
 *    The function is used to reset viewport. 
 *      \param visW       The width of viewport. 
 *      \param visH       The height of viewport. 
 *
 *  - void (*\b setScrollInfo)(mScrollWidget *self, BOOL reDraw);\n
 *    The function is used to set scrollbar information. 
 *      \param reDraw     Whether draw scrollbar or not. 
 *
 *  - BOOL (*\b makePosVisible)(mScrollWidget *self, int x, int y); \n
 *    The function makes the specified position can be seen. 
 *      \param x          The x coordination of the specified position.
 *      \param y          The y coordination of the specified position.
 *      \return           TRUE for success, otherwise FALSE.
 *
 *  - BOOL (*\b isVisible)(mScrollWidget *self, int x, int y); \n
 *    The function is used to decide whether the specified position is visible. 
 *      \param x          The x coordination of the specified position.
 *      \param y          The y coordination of the specified position.
 *      \return           TRUE for visible, otherwise FALSE.
 *
 *  - void (*\b getVisRect)(mScrollWidget *self, RECT *rcVis);\n
 *    Get visible rectangle.
 *
 *  - void (*\b viewportToWindow)(mScrollWidget *self, int *x, int *y);\n
 *    The visible to window coordinate's transformation.
 *
 *  - void (*\b viewportToContent)(mScrollWidget *self, int *x, int *y);\n
 *    The visible to content coordinate's transformation.
 *
 *  - void (*\b contentToViewport)(mScrollWidget *self, int *x, int *y);\n
 *    The content to visible  coordinate's transformation.
 *
 *  - void (*\b contentToWindow)(mScrollWidget *self, int *x, int *y);\n
 *    The content to window  coordinate's transformation.
 *
 *  - void (*\b windowToViewport)(mScrollWidget *self, int *x, int *y);\n
 *    The window to visible coordinate's transformation.
 *
 *  - void (*\b windowToContent)(mScrollWidget *self, int *x, int *y);\n
 *    The window to content coordinate's transformation.
 *
 *  - void (*\b refreshRect)(mScrollWidget *self, const RECT *rc);\n
 *    Refresh the content in specified region.
 */
struct _mScrollWidgetClass
{
	mScrollWidgetClassHeader(mScrollWidget, mWidget)
};

#define mScrollWidgetRendererHeader(clsName, parentClass) \
	mWidgetRendererHeader(clsName, parentClass)

/**
 * \struct mScrollWidgetRenderer
 * \brief The structure of mScrollWidget renderer, which inheried
 *        from mWidgetRenderer.
 */
struct  _mScrollWidgetRenderer {
	mScrollWidgetRendererHeader(mScrollWidget, mWidget)
};

/**
 * \var g_stmScrollWidgetCls
 * \brief The instance of mScrollWidgetClass.
 *
 * \sa mScrollWidgetClass
 */
MGNCS_EXPORT extern mScrollWidgetClass g_stmScrollWidgetCls;

/**
 * \enum mScrollWidgetProp
 * \brief The properties id of mScrollWidget.
 */
enum mScrollWidgetProp
{
    /**
     * The content width.
     */
    NCSP_SWGT_CONTWIDTH = NCSP_WIDGET_MAX + 1,
    /**
     * The content height. 
     */
	NCSP_SWGT_CONTHEIGHT,
    /**
     * The x coordination of current content. 
     */
	NCSP_SWGT_CONTX,
    /**
     * The y coordination of current content. 
     */
	NCSP_SWGT_CONTY,
    /**
     * The horizontal step value.
     */
	NCSP_SWGT_HSTEPVALUE,
    /**
     * The vertical step value.
     */
	NCSP_SWGT_VSTEPVALUE,
    /**
     * The visual width of content.
     */
	NCSP_SWGT_VISWIDTH,
    /**
     * The visual height of content.
     */
	NCSP_SWGT_VISHEIGHT,
    /**
     * The rectangle information of margins.
     */
	NCSP_SWGT_MARGINRECT,
    /**
     * The scrollbar drawing mode.
     */
	NCSP_SWGT_DRAWMODE,
    /**
     * The maximum value of mScrollWidget properties id.
     */
	NCSP_SWGT_MAX
};

/**
 * \def NCSS_SWGT_SHIFT
 * \brief The bits used by mScrollWidget in style. 
 */
#define NCSS_SWGT_SHIFT  NCSS_WIDGET_SHIFT

/**
 * \enum mScrollWidgetNotify
 * \brief The notification code id of mScrollWidget.
 */
enum mScrollWidgetNotify {
    /**
     * The maximum value of mScrollWidget notification code id.
     */
    NCSN_SWGT_MAX = NCSN_WIDGET_MAX + 1
};

    /** @} end of Widget_ScrollWidget */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_WIDGET_SCROLL_H */

