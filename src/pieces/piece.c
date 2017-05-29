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
	InitPieceClass(mLedstaticPiece);

	InitPieceClass(mAbstractButtonPiece);

	InitPieceClass(mPairPiece);

	InitPieceClass(mPushButtonPiece);

	InitPieceClass(mImageLabelPiece);

	InitPieceClass(mSliderPiece);

	InitPieceClass(mThumbBoxPiece);

	InitPieceClass(mScrollThumbBoxPiece);

	InitPieceClass(mTrackBarPiece);

	InitPieceClass(mBoxLayoutPiece);

	InitPieceClass(mHBoxLayoutPiece);

	InitPieceClass(mVBoxLayoutPiece);

	InitPieceClass(mScrollBarPiece);

	InitPieceClass(mArrowPiece);
	InitPieceClass(mLeftArrowPiece);
	InitPieceClass(mRightArrowPiece);
	InitPieceClass(mUpArrowPiece);
	InitPieceClass(mDownArrowPiece);

	InitPieceClass(mArrowButtonPiece);

	InitPieceClass(mSpinnerPiece);
	InitPieceClass(mSpinBoxPiece);
	InitPieceClass(mHSpinBoxPiece);
	InitPieceClass(mVSpinBoxPiece);

	InitPieceClass(mRadioBoxPiece);
	InitPieceClass(mCheckBoxPiece);
	InitPieceClass(mCheckButtonPiece);
	InitPieceClass(mAnimatePiece);
	InitPieceClass(mGridBoxPiece);
	InitPieceClass(mDayGridPiece);
	InitPieceClass(mMonthPiece);

	InitPieceClass(mNumSpinnedPiece);
	InitPieceClass(mListSpinnedPiece);

	InitPieceClass(mWeekHeadPiece);

	InitPieceClass(mToolImagePiece);

	InitPieceClass(mSeparatorPiece);

	InitPieceClass(mLineLayoutPiece);

	InitPieceClass(mWidgetWrapperPiece);

	InitPieceClass(mRectPiece);

	InitPieceClass(mProgressPiece);

	InitPieceClass(mSimpleListPiece);

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
