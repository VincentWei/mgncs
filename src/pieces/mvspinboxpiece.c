
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
#include "mhotpiece.h"
#include "mcontainerpiece.h"
#include "mspinnerpiece.h"
#include "mspinboxpiece.h"
#include "mvspinboxpiece.h"

#include "mlayoutpiece.h"
#include "mpairpiece.h"

#include "mabstractbuttonpiece.h"
#include "marrowbuttonpiece.h"

#define ARROW_SIZE 12

static void mVSpinBoxPiece_construct(mVSpinBoxPiece *self, DWORD add_data)
{
	mPairPiece *pair, *pair_arrows;
	//create a vert spinbox
	pair = NEWPIECE(mPairPiece);
	//by default, spinbox in the right
	_c(pair)->setProperty(pair, NCSP_PAIRPIECE_SECOND_AS_FIRST, 1);
	_c(pair)->setProperty(pair, NCSP_PAIRPIECE_FIRST_SIZE_TYPE, NCS_PAIRPIECE_ST_FIXED);
	_c(pair)->setProperty(pair, NCSP_PAIRPIECE_FIRST_SIZE, ARROW_SIZE);

	pair_arrows = NEWPIECE(mPairPiece);
	mPairPiece_setVert(pair_arrows);
	pair_arrows->first  = (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, NCS_ARROWPIECE_UP);
	pair_arrows->second = (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, NCS_ARROWPIECE_DOWN);

	pair->first = (mHotPiece*)pair_arrows;
	pair->second = (mHotPiece*)add_data;

	Class(mSpinBoxPiece).construct((mSpinBoxPiece*)self, (DWORD)pair);
}

static mHotPiece * mVSpinBoxPiece_getIncPiece(mVSpinBoxPiece*self)
{
	mPairPiece * pair = (mPairPiece*)(self->body);
	if(!pair)
		return NULL;
	pair = ((mPairPiece*)(pair->first));
	return pair?pair->second:NULL;
}

static mHotPiece * mVSpinBoxPiece_getDecPiece(mVSpinBoxPiece*self)
{
	mPairPiece * pair = (mPairPiece*)(self->body);
	if(!pair)
		return NULL;
	pair = ((mPairPiece*)(pair->first));
	return pair?pair->first:NULL;
}

static mHotPiece * mVSpinBoxPiece_getSpinnedPiece(mVSpinBoxPiece*self)
{
	return self->body?(((mPairPiece*)(self->body))->second):NULL;
}

BEGIN_MINI_CLASS(mVSpinBoxPiece, mSpinBoxPiece)
	CLASS_METHOD_MAP(mVSpinBoxPiece, construct   )
	CLASS_METHOD_MAP(mVSpinBoxPiece, getIncPiece )
	CLASS_METHOD_MAP(mVSpinBoxPiece, getDecPiece )
	CLASS_METHOD_MAP(mVSpinBoxPiece, getSpinnedPiece )
END_MINI_CLASS


