/*
 ** $Id: mimagebutton.h 1617 2011-03-24 09:48:44Z shidazhi $
 **
 ** mimagebutton.h:
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd.
 **
 ** All rights reserved by Feynman Software.
 **
 ** Current maintainer: dongjunjie
 **
 ** Create date: 2010年12月24日
 */
#ifdef _MGNCSCTRL_IMAGEBUTTON

#ifndef _MGUI_NCSCTRL_IMAGEBUTTON_H
#define _MGUI_NCSCTRL_IMAGEBUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup ControlButton mImageButton
 * @{
 */

/**
 *  \def NCSCTRL_IMAGEBUTTON
 *  \bref the name of image button control
 */
#define NCSCTRL_IMAGEBUTTON NCSCLASSNAME("imagebutton")

DECLARE_OBJECT(mImageButton)


#define mImageButtonHeader(clss) \
	mButtonHeader(clss)

#define mImageButtonClassHeader(clss, superCls) \
	mButtonClassHeader(clss, superCls)

typedef mButtonRenderer mImageButtonRenderer;

DEFINE_OBJECT(mImageButton, mImageButton)

enum mImageButtonProp {
	NCSP_IMGBTN_IMAGE = NCSP_BUTTON_IMAGE,
	NCSP_IMGBTN_TOOLIMAGEFILE = NCSP_BUTTON_MAX + 1,
	NCSP_IMGBTN_TOOLIMAGE,
	NCSP_IMGBTN_MAX
};

enum mImageButtonNotify{
	NCSN_IMGBTN_MAX = NCSN_BUTTON_MAX + 1
};

#ifdef __cplusplus
}
#endif

#endif
#endif		//_MGNCSCTRL_IMAGEBUTTON
