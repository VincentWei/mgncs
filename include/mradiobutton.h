
#ifndef _MGUI_NCSCTRL_RDOBTN_H
#define _MGUI_NCSCTRL_RDOBTN_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \defgroup ControlRadioButton mRadioButton
 * @{
 */

/**
 * \def NCSCTRL_RADIOBUTTON
 * \brief the name of Radiobutton control
 */

#define NCSCTRL_RADIOBUTTON        NCSCLASSNAME("radiobutton")

typedef struct _mRadioButton mRadioButton;
typedef struct _mRadioButtonClass mRadioButtonClass;
typedef struct _mRadioButtonRenderer mRadioButtonRenderer;

#define mRadioButtonHeader(className) \
	mCheckButtonHeader(className) 

/**
 * \struct mRadioButton
 * \brief define the member of Radiobutton, derived from \ref mCheckButton
*/
struct _mRadioButton
{
	mRadioButtonHeader(mRadioButton)
};

#define mRadioButtonClassHeader(className, parentClass) \
	mCheckButtonClassHeader(className, parentClass)
/**
 * \struct mRadioButtonClass
 * \brief Radiobutton struct of control, derived from \ref mCheckButtonClass
*/

struct _mRadioButtonClass
{
	mRadioButtonClassHeader(mRadioButton, mCheckButton)
};

#define mRadioButtonRendererHeader(clsName, parentClass) \
	mCheckButtonRendererHeader(clsName, parentClass) \
    void (*drawRadiobutton)(clsName* self,HDC hdc, const RECT* pRect, int status);

/**
 * \struct mRadioButtonRender
 * \brief Radiobutton renderer interface, derived from \ref mCheckButtonRenderer
 *
 *  \sa mCheckButton, mCheckButtonRenderer
 *
 * - void (\b *drawRadiobutton)( clsName \a *self, HDC \a hdc, const Rect \a *pRect, int \a status);
 *   draw Radiobutton using renderer
 *   \param *self - this pointer of Radiobutton
 *   \param hdc - DC of control
 *   \param *pRect - the rectangle of Radiobutton frame
 *   \param status - the Radiobutton state
 *
 */
/* define the render of image static */
struct _mRadioButtonRenderer {
	mRadioButtonRendererHeader(mRadioButton, mCheckButton)
};

/**
 * \var g_stmRadioButtonCls
 * \brief global mRadioButtonClass
 */
MGNCS_EXPORT extern mRadioButtonClass g_stmRadioButtonCls;

/**
 * \enum mRadioButtonProp
 * \brief the properties id of \ref mRadioButton
 */

/*property*/
enum mRadioButtonProp {
	NCSP_RDOBTN_MAX = NCSP_CHKBTN_MAX + 1,
};

#define NCSS_RDOBTN_SHIFT NCSS_CHKBTN_SHIFT

/**
 * \enum enumRadioButtonNotify
 * \brief the notification id of \ref mRadioButton
 */

enum enumRadioButtonNotify{
	NCSN_RDOBTN_MAX = NCSN_CHKBTN_MAX + 1
};

/**
 * @} end of ControlRadioButton
 */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_NCSCTRL_RDOBTN_H */

