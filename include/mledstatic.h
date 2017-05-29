
#ifndef _MGUI_NCSCTRL_LEDLABEL_H
#define _MGUI_NCSCTRL_LEDLABEL_H
 
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \defgroup ControlLedStatic mLEDLabel
 * @{
 */

#include "mwidget.h"

/**
 * \def NCSCTRL_LEDLABEL
 * \brief the name of mLEDLabel
 */

#define NCSCTRL_LEDLABEL   NCSCLASSNAME("ledlabel") 


typedef struct _mLEDLabel mLEDLabel;
typedef struct _mLEDLabelClass mLEDLabelClass;
typedef struct _mLEDLabelRenderer mLEDLabelRenderer;

/* define mLEDLabel */
#define mLEDLabelHeader(clsName)  \
	mStaticHeader(clsName) 

/**
 * \struct mLEDLabel
 * \brief LedLabel class, derived from \ref mStatic
*/

struct _mLEDLabel
{
	mLEDLabelHeader(mLEDLabel)
};

/* define mLEDLabelClass */
#define mLEDLabelClassHeader(clsName, parentClass)    \
	mStaticClassHeader(clsName, parentClass) 

/**
 * \struct mLEDLabelClass
 * \brief VTable of mLEDLabel, derived from \ref mStaticClass
 */

struct _mLEDLabelClass
{
	mLEDLabelClassHeader(mLEDLabel, mStatic)
};


/* define IRendererStatic */
#define mLEDLabelRendererHeader(clsName, parentClass) \
	mStaticRendererHeader(clsName, parentClass)

/**
 * \struct mLEDLabelRenderer
 * \brief mLEDLabel renderer interface, derived from \ref mStaticRenderer
 */
struct  _mLEDLabelRenderer {
	mLEDLabelRendererHeader(mLEDLabel, mStatic)
};

/**
 * \var g_stmButtonCls
 * \brief global mLEDLabelClass
 */
MGNCS_EXPORT extern mLEDLabelClass g_stmLEDLabelCls;

/* define property of static */

/**
 * \enum mLEDLabelProp
 * \brief the properties id of \ref mLEDLabel
 */

enum mLEDLabelProp {
	NCSP_LEDLBL_COLOR = NCSP_STATIC_MAX + 1,
	NCSP_LEDLBL_WIDTH,
	NCSP_LEDLBL_HEIGHT,
	NCSP_LEDLBL_GAP,
	NCSP_LEDLBL_MAX,
};

/**
 * \enum enumLEDLabelNotify
 * \brief the notification code of \ref mLEDLabel
 */
enum enumLEDLabelNotify {
	NCSN_LEDLBL_MAX = NCSN_STATIC_MAX + 1
};

// BOOL ncsLedDrawText(HDC hdc, char *str, int len, const RECT *rect, DWORD uflag, int width, int height);

/**
 * @} end of ControlLedStatic
 */
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_NCSCTRL_LEDLABEL_H */

