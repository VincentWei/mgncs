#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncsconfig.h"
#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#include "mwidget.h"
#include "mrdr.h"

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
