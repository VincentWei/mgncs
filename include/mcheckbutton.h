
#ifndef _MGUI_NCSCTRL_CHKBTN_H
#define _MGUI_NCSCTRL_CHKBTN_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \defgroup ControlCheckButton mCheckButton
 * @{
 */

/**
 * \def NCSCTRL_CHECKBUTTON
 * \brief the name of checkbutton class
 */

#define NCSCTRL_CHECKBUTTON        NCSCLASSNAME("checkbutton")

typedef struct _mCheckButton mCheckButton;
typedef struct _mCheckButtonClass mCheckButtonClass;
typedef struct _mCheckButtonRenderer mCheckButtonRenderer;

#define mCheckButtonHeader(className) \
	mButtonHeader(className) 

/**
 * \struct mCheckButton
 * \brief CheckButton, derived from \ref mButton
 */

struct _mCheckButton
{
	mCheckButtonHeader(mCheckButton)
};

#define mCheckButtonClassHeader(className, parentClass) \
	mButtonClassHeader(className, parentClass)

/**
 * \struct mCheckButtonClass
 * \brief checkbutton class struct of button control, derived from mButtonClass
 */
 
struct _mCheckButtonClass
{
	mCheckButtonClassHeader(mCheckButton, mButton)
};

#define mCheckButtonRendererHeader(clsName, parentClass) \
	mButtonRendererHeader(clsName, parentClass) \
    void (*drawCheckbutton)(clsName *self, HDC hdc, const RECT* pRect, int status);

/**
 * \struct mCheckButtonRenderer
 * \brief Checkbutton class renderer interface, derived from \ref mButtonRenderer
 *
 *  \sa mCheckButton, mCheckButtonClass, mButton, mButtonClass
 *
 * - void (\b *drawCheckbutton)( clsName \a *self, HDC \a hdc, const Rect \a *pRect, int \a status);
 *   draw checkbutton using renderer
 *   \param *self - this pointer of checkbutton
 *   \param hdc - DC of control
 *   \param *pRect - the rectangle of checkbutton frame
 *   \param status - the button state
 *
 */

/* define the render of image static */
struct _mCheckButtonRenderer {
	mCheckButtonRendererHeader(mCheckButton, mButton)
};

/**
 * \var g_stmCheckButtonCls
 * \brief global mCheckButtonClass
 */
MGNCS_EXPORT extern mCheckButtonClass g_stmCheckButtonCls;

/**
 * \enum mCheckButtonProp
 * \brief the properties id of \ref mCheckButton, derived from mButtonProp
 */
enum mCheckButtonProp{
	NCSP_CHKBTN_MAX = NCSP_BUTTON_MAX + 1
};

/**
 * \enum mCheckButtonNotify
 * \brief the notification id of \ref mCheckButton
 */

enum mCheckButtonNotify{
	NCSN_CHKBTN_MAX = NCSN_BUTTON_MAX + 1
};

#define NCSS_CHKBTN_SHIFT NCSS_BUTTON_SHIFT

/**
 * @} end of ControlCheckButton
 */
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_NCSCTRL_RECTANGLE_H */

