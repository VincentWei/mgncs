
#ifndef _MGUI_NCSCTRL_GRPBOX_H
#define _MGUI_NCSCTRL_GRPBOX_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \defgroup Control_GroupBox mGroupBox
 * @{
 */

/**
 * \def NCSCTRL_GROUPBOX
 * \brief the name of groupbox control
 */

#define NCSCTRL_GROUPBOX        NCSCLASSNAME("groupbox")

typedef struct _mGroupBox mGroupBox;
typedef struct _mGroupBoxClass mGroupBoxClass;
typedef struct _mGroupBoxRenderer mGroupBoxRenderer;

#define mGroupBoxHeader(clsName) \
	mStaticHeader(clsName) 

/**
 * \struct mGroupBox
 * \brief groupbox class, derived from \ref mStatic
 */

struct _mGroupBox
{
	mGroupBoxHeader(mGroupBox)
};

#define mGroupBoxClassHeader(clsName, parentClass)  \
	mStaticClassHeader(clsName, parentClass)        \
    int (*hitTest)(clsName *self, int x, int y);

/**
 * \struct mGroupBoxClass
 * \brief struct of groupbox control, derived from \ref mStaticClass
 *
 * - int (\b *hitTest)(clsName \a *self, int \a x, int \a y); \n
 *   test mouse hit position
 *   \param *self - this pointer of groupbox
 *   \param x - x value of mouse position
 *   \param y - y value of mouse position
 *   \return HT_OUT - mouse hit out of groupbox, HT_TRANSPARENT - mouse hit inside the controlset area of groupbox, HT_CLIENT - mouse hit in groupbox
 */

struct _mGroupBoxClass
{
	mGroupBoxClassHeader(mGroupBox, mStatic)
};

#define mGroupBoxRendererHeader(clsName, parentClass) \
	mStaticRendererHeader(clsName, parentClass) \
    void (*drawGroupbox)(clsName *self, HDC hdc, const RECT *pRect);

/**
 * \struct mGroupBoxRenderer
 * \brief groupbox class renderer interface, derived from \ref mStaticRenderer
 *
 * - void (\b *drawGroupbox)(clsName \a *self, HDC \a hdc, const RECT \a *pRect); \n
 *   draw groupbox using renderer
 *   \param *self - this pointer of groupbox
 *   \param hdc - dc of groupbox control
 *   \param *pRect - rectangle to draw groupbox
 */

/* define the render of image static */
struct _mGroupBoxRenderer {
	mGroupBoxRendererHeader(mGroupBox, mStatic)
};

/**
 * \var g_stmGroupBoxCls
 * \brief global mGroupBoxClass
 */

MGNCS_EXPORT extern mGroupBoxClass g_stmGroupBoxCls;

/**
 * \enum mGroupBoxProp
 * \brief the properties id of \ref mGroupBox
 */

/* property */
enum mGroupBoxProp {
	NCSP_GRPBOX_MAX = NCSP_STATIC_MAX + 1
};

#define NCSS_GRPBOX_SHIFT NCSS_STATIC_SHIFT

/**
 * \enum enumGroupBoxNotify
 * \brief the notification code of \ref mGroupBox
 */
enum enumGroupBoxNotify{
	NCSN_GRPBOX_MAX = NCSN_STATIC_MAX + 1
};

/**
 * @} end of Control_GroupBox
 */
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_NCSCTRL_GRPBOX_H */

