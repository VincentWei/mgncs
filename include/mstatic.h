
#ifndef _MGUI_NCSCTRL_STATIC_H
#define _MGUI_NCSCTRL_STATIC_H
 
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \defgroup ControlStatic mStatic
 * @{
 */

#include "mwidget.h"

/**
 * \def NCSCTRL_STATIC
 * \brief the name of mStatic
 */

#define NCSCTRL_STATIC   NCSCLASSNAME("static") 

typedef struct _mStatic mStatic;
typedef struct _mStaticClass mStaticClass;
typedef struct _mStaticRenderer mStaticRenderer;

/* define mStatic */
#define mStaticHeader(clsName)  \
	mWidgetHeader(clsName) \

/**
 * \struct mStatic
 * \brief Static class, derived from \ref mWidget
 *
 */

struct _mStatic
{
	mStaticHeader(mStatic)
};

/* define mStaticClass */
#define mStaticClassHeader(clsName, parentClass)    \
	mWidgetClassHeader(clsName, parentClass)        \

/**
 * \struct mStaticClass
 * \brief struct of Static control, derived from \ref mWidgetClass
 */

struct _mStaticClass
{
	mStaticClassHeader(mStatic, mWidget)
};

/* define IRendererStatic */
#define mStaticRendererHeader(clsName, parentClass) \
	mWidgetRendererHeader(clsName, parentClass)

/**
 * \struct mStaticRenderer
 * \brief Static class renderer interface, derived from \ref mWidgetRenderer
 */
struct  _mStaticRenderer {
	mStaticRendererHeader(mStatic, mWidget)
};

/**
 * \var g_stmButtonCls
 * \brief global mStaticClass
 */
MGNCS_EXPORT extern mStaticClass g_stmStaticCls;

/* define property of static */

/**
 * \enum mStaticProp
 * \brief the properties id of \ref mStatic
 */

enum mStaticProp {
	/**
	 * \brief Align the content of static controller in horizontal mode
	 *  - Type \ref enumNCSAlign
	 *  - Read Write
	 *  - set the horizontal mode of static controller
	 *
	 */ 
	NCSP_STATIC_ALIGN = NCSP_WIDGET_MAX + 1,
	/**
	 * \brief Align the content of static controller in vertical mode
	 *  - Type \ref enumNCSVAlign
	 *  - Read Write
	 *  - set the vertical mode of static controller
	 *
	 */
	NCSP_STATIC_VALIGN,
	/**
	 * \brief Set the content of static controller to autowrap mode
	 *  - Type int
	 *  - Read Write
	 *  - turn on/off the autowrap mode of static content
	 *
	 */
	NCSP_STATIC_AUTOWRAP,
	NCSP_STATIC_MAX
};

/**
 * \def NCSS_STATIC_PREFIX
 * \brief static prefix 
 */

#define NCSS_STATIC_PREFIX  (0x1<<(NCSS_WIDGET_SHIFT))

/**
 * \def NCSS_STATIC_SHIFT
 * \brief static shift
 */
#define NCSS_STATIC_SHIFT  (NCSS_WIDGET_SHIFT + 1)

/**
 * \enum enumStaticNotify
 * \brief the notification code of \ref mStatic
 */
enum enumStaticNotify {
	NCSN_STATIC_MAX = NCSN_WIDGET_MAX + 1
};

/**
 * @} end of ControlStatic
 */
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_NCSCTRL_STATIC_H */

