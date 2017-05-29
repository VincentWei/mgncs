/*
* \file mmainwnd.h
* \author dongjunjie
* \date 2009/01/01
*
* This file include the dialog defines
*
* \verbatim
* 	
* 	This file is part of mgncs, new control of MiniGUI
*
* 	\endverbatim
*
*/

#ifndef _MGUI_CTRL_DLGBOX_H
#define _MGUI_CTRL_DLGBOX_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \defgroup DialogBox mDialogBox
 * @{
 */

/**
 * \def NCSCTRL_DIALOGBOX
 * \brief the name of \ref mDialogBox control
 */
#define NCSCTRL_DIALOGBOX NCSCLASSNAME("dialogbox")

typedef struct _mDialogBox mDialogBox;
typedef struct _mDialogBoxClass mDialogBoxClass;
typedef struct _mDialogBoxRenderer mDialogBoxRenderer;

/* define mDialogBoxRenderer */
#define mDialogBoxRendererHeader(clsName, parentClass) \
	mMainWndRendererHeader(clsName, parentClass)

/**
 * \struct mDialogBoxRenderer
 * \brief the renerer interface of \ref mDialogBox, inherit from \ref mMainWndRenderer
 *
 * \sa mDialogBox, mDialogBoxClass
 */

struct _mDialogBoxRenderer {
	mDialogBoxRendererHeader(mDialogBox, mMainWnd)
};

/* define mDialogBox */
#define mDialogBoxHeader(className) \
	mMainWndHeader(className) 

/**
 * \struct mDialogBox
 * \brief the members of mDialogBox, inherit from mMainWnd
 *
 *  - DWORD \b modeRetValue;\n
 *    The return value after a dialog leave Mode state, return by \b doModal
 *
 * \sa mDialogBoxClass
 */
struct _mDialogBox{
	mDialogBoxHeader(mDialogBox)
};

/* define mDialogBoxClass */
#define mDialogBoxClassHeader(clsName, parentClass)    \
	mMainWndClassHeader(clsName, parentClass)

/**
 * \struct mDialogBoxClass
 * \brief the VTable of \ref mDialogBox, inherit from mMainWndClass
 *
 * \sa mDialogBox
 *
 */

struct _mDialogBoxClass
{
	mDialogBoxClassHeader(mDialogBox, mMainWnd)
};

/**
 * \var g_stmDialogBoxCls
 * \brief the singleton of mDialogBoxClass
 */
MGNCS_EXPORT extern mDialogBoxClass g_stmDialogBoxCls; //Class(mDialogBox);


#define NCSS_DLGBOX_SHIFT  NCSS_MNWND_SHIFT

enum enumDialogBoxNotify {
	NCSN_DLGBOX_MAX = NCSN_MNWND_MAX + 1
};

/** @} end of DialogBox */

#ifdef __cplusplus
}
#endif

#endif

