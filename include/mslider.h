/**
 * \file mslider.h
 * \author Wangjian<wangjian@minigui.org>
 * \date 2009/03/01
 *
 * This file includes the definition of mSlider control.
 *
 \verbatim

    Copyright (C) 2009 Feynman Software.

    All rights reserved by Feynman Software.

    This file is part of mgncs, which is new control set of MiniGUI.

 \endverbatim
 */
#ifdef _MGNCSCTRL_SLIDER

#ifndef _MGUI_NCSCTRL_SLIDER_H
#define _MGUI_NCSCTRL_SLIDER_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * \defgroup ControlSlider mSlider
 * @{
 */

/*
 * \def NCSCTRL_SLIDER
 * \brief the name of slider control
*/

#define NCSCTRL_SLIDER   NCSCLASSNAME("slider")

typedef struct _mSlider            mSlider;
typedef struct _mSliderClass       mSliderClass;
typedef struct _mSliderRenderer    mSliderRenderer;

#define mSliderHeader(Class) \
    mWidgetHeader(Class)

/**
 * \struct mSlider
 * \brief the slider struct of slider control, derived from \ref mWidget.
 *
 * \sa mSliderClass
 */
struct _mSlider
{
	mSliderHeader(mSlider)
};

#define mSliderClassHeader(clsName, parentClass) \
	mWidgetClassHeader(clsName, parentClass)

/**
 * \struct mSliderClass
 * \brief the VTable of \a mSlider, derived from \ref mWidgetClass.
 *
 * \sa mWidgetClass
 */
struct _mSliderClass
{
	mSliderClassHeader(mSlider, mWidget)
};

/**
 * \var g_stmSliderCls
 * \brief the instance of mSliderClass
 *
 * \sa mSliderClass
 */
MGNCS_EXPORT extern mSliderClass g_stmSliderCls;

#define mSliderRendererHeader(clsName, parentClass)  \
	mWidgetRendererHeader(clsName, parentClass)  \
	void (*calcSlider)(clsName *, DWORD calc_area); \
	void (*drawSlider)(clsName *, HDC hdc);

enum mSliderArea
{
	NCSSLIDER_AREA_INCARROW = 0x01,
	NCSSLIDER_AREA_DECARROW = 0x02,
	NCSSLIDER_AREA_ARROWS = 0x03,
	NCSSLIDER_AREA_SLIDER = 0x04,
	NCSSLIDER_AREA_TRACK = 0x08,
	NCSSLIDER_AREA_ALL = 0xFFFF //
};

/**
 * \struct mSliderRenderer
 * \brief Slider class's Renderer interface, derived from \ref mWidgetRenderer.
 *
 * \sa mSlider, mSliderClass
 */
struct _mSliderRenderer
{
	mSliderRendererHeader(mSlider, mWidget)
};

/**
 * \enum mSliderProp
 * Slider Properties define
 *
 */
enum mSliderProp {
	/**
	 * The max pos of slider
	 *	- Type : int
	 *	- ReadWrite
	 */
    NCSP_SLIDER_MAXPOS = NCSP_WIDGET_MAX + 1,  /**< value for maximum position*/
	/**
	 * The min pos of slider
	 * 	- Type : int
	 * 	- ReadWrite
	 */
    NCSP_SLIDER_MINPOS,                        /**< value for minimum position*/
	/**
	 * The current position of slider
	 * 	- Type : int
	 * 	- ReadWrite
	 */
    NCSP_SLIDER_CURPOS,                        /**< value for current position*/
	/**
	 * The line step length of slider
	 * 	- Type : int
	 * 	- ReadWirte
	 */
    NCSP_SLIDER_LINESTEP,                      /**< line step length*/
	/**
	 * The page step length of slider
	 * 	- Type : int
	 *	- ReadWrite
	 */
    NCSP_SLIDER_PAGESTEP,                      /**< page step length*/
    NCSP_SLIDER_MAX
};

/**
 * \def NCSS_SLIDER_HORIZONTAL
 * \brief horizontal slider
 */
#define NCSS_SLIDER_HORIZONTAL          0x0000L

/**
 * \def NCSS_SLIDER_VERTICAL
 * \brief vertical slider
 */
#define NCSS_SLIDER_VERTICAL            (0x0001L<<NCSS_WIDGET_SHIFT)

#define NCSS_SLIDER_SHIFT      (NCSS_WIDGET_SHIFT + 2)


/**
 * \enum enumSliderNotify
 * Slider Notify Codes
 */
enum enumSliderNotify {
	/**
	 * When current position canged
	 * 	- param : new current pos
	 */
	NCSN_SLIDER_CHANGED = NCSN_WIDGET_MAX, /**< position changed*/
	/**
	 * When the current position reach max pos
	 */
	NCSN_SLIDER_REACHMAX,                  /**< reached the max value*/
	/**
	 * When the current positon reach min pos
	 */
	NCSN_SLIDER_REACHMIN,                  /**< reached the min value*/
	//NCSN_SLIDER_STARTDRAG,                 /**< start to drag it*/
	//NCSN_SLIDER_STOPDRAG,                  /**< stop to drag it*/
	NCSN_SLIDER_MAX
};

/** @} end of Control_Slider*/

/** slider proctected function */

enum mSliderHitTest{
	NCSSLIDER_HT_NONE = 0,
	NCSSLIDER_HT_SLIDER ,
	NCSSLIDER_HT_TRACK,
	NCSSLIDER_HT_INCARROW,
	NCSSLIDER_HT_DECARROW
};
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_NCSCTRL_SLIDER_H */
#endif //_MGNCSCTRL_SLIDER
