#ifdef _MGNCSCTRL_TOOLBAR

#ifndef _MGUI_NCSCTRL_TLBAR_H
#define _MGUI_NCSCTRL_TLBAR_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \defgroup Control_ToolBar mToolBar
 * @{
 */

#include "mwidget.h"

/**
 * \def NCSCTRL_TOOLBAR
 * \brief the name of mToolBar
 */

#define NCSCTRL_TOOLBAR   NCSCLASSNAME("toolbar")

typedef struct _mToolBar mToolBar;
typedef struct _mToolBarClass mToolBarClass;
typedef struct _mToolBarRenderer mToolBarRenderer;

/* define mToolBar */
#define mToolBarHeader(clsName)  \
	mWidgetHeader(clsName)

/**
 * \struct mToolBar
 * \brief ToolBar class, derived from \ref mWidget
 *
 */

struct _mToolBar
{
	mToolBarHeader(mToolBar)
};

/* define mToolBarClass */
#define mToolBarClassHeader(clsName, parentClass)    \
	mWidgetClassHeader(clsName, parentClass) \
	BOOL (*addItem)(clsName*, void *item, int type, int min_size, int max_size); \
	int  (*isItemChecked)(clsName *, int id); \
	int  (*checkItem)(clsName *, int id, int state);

/**
 * \struct mToolBarClass
 * \brief struct of ToolBar control, derived from \ref mWidgetClass
 *
 * - BOOL \b addItem(mToolBar *self, void * \a item, int \a type, int \a min_size, int \a max_size); \n
 *   add an toolitem into toolbar
 *   \param item - the toolitem pointer, create by
 *   		- ncsCreatePushToolItem
 *   		- ncsCreateSeparatorItem
 *   		- ncsCreateMenuToolItem
 *   		- ncsCreateCheckToolItem
 *   		- ncsCreateWidgetToolItem
 *   \param type - the indicate the size type of the toolitem, 0 - fixed, 1 - the size is auto resize
 *   \param min_size - the min size of toolitem, when type == 1
 *   \param max_size - the max size of toolitem, when type == 1
 *   \return Sucessed or failed
 *
 * - int \b isItemChecked(mToolBar *self, int id); \n
 *   get the check state of a item
 *   \param id - the id of toolitem
 *   \return the check state of toolitem \n
 *   	- NCS_TOOLITEM_CHECKED
 *   	- NCS_TOOLITEM_UNCHECKED
 *
 * - int \b checkItem(mToolBar*self, int id, int state); \n
 *   set the check state of a item
 *   \param id - the id of toolitem
 *   \param state - the new state of toolitem
 *   \return old state
 *
 *
 */

struct _mToolBarClass
{
	mToolBarClassHeader(mToolBar, mWidget)
};

/* define IRendererToolBar */
#define mToolBarRendererHeader(clsName, parentClass) \
	mWidgetRendererHeader(clsName, parentClass)
/**
 * \struct mToolBarRenderer
 * \brief ToolBar class renderer interface, derived from \ref mWidgetRenderer
 */
struct  _mToolBarRenderer {
	mToolBarRendererHeader(mToolBar, mWidget)
};

/**
 * \var g_stmButtonCls
 * \brief global mToolBarClass
 */
MGNCS_EXPORT extern mToolBarClass g_stmToolBarCls;

/* define property of static */

/**
 * \enum mToolBarProp
 * \brief the properties id of \ref mToolBar
 */

enum mToolBarProp {
	NCSP_TLBAR_MAX = NCSP_WIDGET_MAX + 1,
};


/**
 * \def NCSS_TLBAR_SHIFT
 * \brief static shift
 */
#define NCSS_TLBAR_SHIFT  (NCSS_WIDGET_SHIFT + 1)

/**
 * \enum enumToolBarNotify
 * \brief the notification code of \ref mToolBar
 */
enum enumToolBarNotify {
	/**
	 * when a item changed
	 */
	NCSN_TLBAR_ITEMCHAGED = NCSN_WIDGET_MAX + 1,
	NCSN_TLBAR_MAX
};

/**
 * @} end of Control_ToolBar
 */
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_NCSCTRL_TLBAR_H */
#endif //_MGNCSCTRL_TOOLBAR
