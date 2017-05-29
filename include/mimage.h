
#ifndef _MGUI_NCSCTRL_IMAGESTATIC_H
#define _MGUI_NCSCTRL_IMAGESTATIC_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
#include "mstatic.h"

/**
 * \defgroup ControlImage mImage
 * @{
 */

/**
 * \def NCSCTRL_IMAGE
 * \brief the name of image control
 */
#define NCSCTRL_IMAGE        NCSCLASSNAME("image")

typedef struct _mImage mImage;
typedef struct _mImageClass mImageClass;
typedef struct _mImageRenderer mImageRenderer;

#define mImageHeader(clsName)   \
	mStaticHeader(clsName)      

/**
 * \struct mImage
 * \brief static control can show a image, derived from \ref mStatic
 */
struct _mImage
{
	mImageHeader(mImage)
};

#define mImageClassHeader(clsName, parentClass) \
	mStaticClassHeader(clsName, parentClass)
/**
 * \struct mImageClass
 * \brief ImageClass struct of image control, derived from \ref mStaticClass
 */

struct _mImageClass
{
	mImageClassHeader(mImage, mStatic)
};

#define mImageRendererHeader(clsName, parentClass) \
	mStaticRendererHeader(clsName, parentClass)
/**
 * \struct mImageRenderer
 * \brief image class renderer interface, derived from \ref mStaticRenderer
 */
/* define the render of image static */
struct _mImageRenderer {
	mImageRendererHeader(mImage, mStatic)
};

/**
 * \var g_stmImageCls
 * \brief global mImageClass
 */

MGNCS_EXPORT extern mImageClass g_stmImageCls;

/**
 * \enum mImageProp
 * \brief the properties id of \ref mImage
 */

/* property */
enum mImageProp {
	/**
	 * \brief Set the image content
	 *  - Type PBITMAP
	 *  - Read Write
	 *  - set the pointer to image content
	 *
	 */
	NCSP_IMAGE_IMAGE = NCSP_STATIC_MAX + 1,
	/**
	 * \brief Set the image content
	 *  - Type char*
	 *  - Read Write
	 *  - set the image file name pointer
	 *
	 */
	NCSP_IMAGE_IMAGEFILE,
	/**
	 * \brief Set image draw mode
	 *  - Type \ref enumNCSImageDrawMode
	 *  - Read Write
	 *  - set the image draw mode to NCS_DM_NORMAL/NCS_DM_SCALED/NCS_DM_TILED
	 *
	 */
	NCSP_IMAGE_DRAWMODE,
	NCSP_IMAGE_MAX
};

#define NCSS_IMAGE_SHIFT  NCSS_STATIC_SHIFT

/**
 * \enum enumImageNotify
 * \brief the notification code of \ref mImage
 */
enum enumImageNotify{
	NCSN_IMAGE_MAX = NCSN_STATIC_MAX + 1
};

/**
 * @} end of ControlImage
 */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_NCSCTRL_STATIC_H */

