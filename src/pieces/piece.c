///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/*
 *   This file is part of mGNCS, a component for MiniGUI.
 * 
 *   Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "piece.h"
#include "mem-slab.h"

#if 1
#define SLAB_NEW(size)  slab_new_mem(NULL, size, 0)
#define SLAB_DEL(p)     slab_free_mem(NULL, p)
#else
#define SLAB_NEW(size)  calloc(1, size)
#define SLAB_DEL(p)     free(p)
#endif

#define InitPieceClass(className) \
	Class(className).classConstructor((mObjectClass*)(void*)(&(Class(className))))

BOOL init_pieces_classes()
{
	InitPieceClass(mHotPiece);
	InitPieceClass(mStaticPiece);
	InitPieceClass(mContainerPiece);
	InitPieceClass(mLayoutPiece);

	InitPieceClass(mRenderablePiece);
	InitPieceClass(mButtonBoxPiece);
	InitPieceClass(mLabelPiece);
	InitPieceClass(mImagePiece);

#ifdef _MGNCSCTRL_LEDLABEL
	InitPieceClass(mLedstaticPiece);
#endif

	InitPieceClass(mAbstractButtonPiece);

	InitPieceClass(mPairPiece);

	InitPieceClass(mPushButtonPiece);

	InitPieceClass(mImageLabelPiece);

#if defined (_MGNCSCTRL_SCROLLBAR) || defined (_MGNCSCTRL_TRACKBAR)
	InitPieceClass(mSliderPiece);
#endif

	InitPieceClass(mThumbBoxPiece);

#ifdef _MGNCSCTRL_SCROLLBAR
	InitPieceClass(mScrollThumbBoxPiece);
#endif

	InitPieceClass(mTrackBarPiece);

	InitPieceClass(mBoxLayoutPiece);

#if defined (_MGNCSCTRL_IMWORDSEL) || defined (_MGNCSCTRL_MONTHCALENDAR) || defined (_MGNCSCTRL_SPINBOX)
	InitPieceClass(mHBoxLayoutPiece);
#endif

	InitPieceClass(mVBoxLayoutPiece);

#ifdef _MGNCSCTRL_SCROLLBAR
	InitPieceClass(mScrollBarPiece);
#endif

	InitPieceClass(mArrowPiece);
	InitPieceClass(mLeftArrowPiece);
	InitPieceClass(mRightArrowPiece);
	InitPieceClass(mUpArrowPiece);
	InitPieceClass(mDownArrowPiece);

#ifdef _MGNCSCTRL_IMWORDSEL
	InitPieceClass(mArrowButtonPiece);
#endif

#ifdef _MGNCSCTRL_SPINNER
	InitPieceClass(mSpinnerPiece);
#endif

#ifdef _MGNCSCTRL_SPINBOX
	InitPieceClass(mSpinBoxPiece);
	InitPieceClass(mHSpinBoxPiece);
#endif

#ifdef _MGNCSCTRL_SPINBOX
	InitPieceClass(mVSpinBoxPiece);
#endif

	InitPieceClass(mRadioBoxPiece);
	InitPieceClass(mCheckBoxPiece);
	InitPieceClass(mCheckButtonPiece);

#ifdef _MGNCSCTRL_ANIMATE
	InitPieceClass(mAnimatePiece);
#endif

	InitPieceClass(mGridBoxPiece);
	InitPieceClass(mDayGridPiece);

#ifdef _MGNCSCTRL_MONTHCALENDAR
	InitPieceClass(mMonthPiece);
#endif

	InitPieceClass(mNumSpinnedPiece);
	InitPieceClass(mListSpinnedPiece);

	InitPieceClass(mWeekHeadPiece);

	InitPieceClass(mToolImagePiece);
	InitPieceClass(mToolImageItemPiece);

	InitPieceClass(mSeparatorPiece);

#ifdef _MGNCSCTRL_TOOLBAR
	InitPieceClass(mLineLayoutPiece);
#endif

	InitPieceClass(mWidgetWrapperPiece);

	InitPieceClass(mRectPiece);

#ifdef _MGNCSCTRL_PROGRESSBAR
	InitPieceClass(mProgressPiece);
#endif

#ifdef _MGNCSCTRL_IMWORDSEL
	InitPieceClass(mSimpleListPiece);
#endif

	return TRUE;
}


mHotPiece * ncsNewPiece(mHotPieceClass *_class, DWORD add_data)
{
	mHotPiece *piece;

	if(_class == NULL)
		return NULL;

	piece = (mHotPiece*)SLAB_NEW(_class->objSize);
	if(piece == NULL)
		return NULL;

	piece->_class = _class;
	_class->construct(piece, add_data);
	return piece;
}

void ncsDelPiece(mHotPiece* piece)
{
	if(!piece)
		return;

	_c(piece)->destroy(piece);
	SLAB_DEL(piece);
}
