
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
#include "mhspinboxpiece.h"

#include "mlayoutpiece.h"
#include "mboxlayoutpiece.h"
#include "mhboxlayoutpiece.h"

#include "mabstractbuttonpiece.h"
#include "marrowbuttonpiece.h"

#ifdef _MGNCSCTRL_SPINBOX

#define ARROW_SIZE 12

static void mHSpinBoxPiece_construct(mHSpinBoxPiece *self, DWORD add_data)
{
	//create boxlayout
	mHBoxLayoutPiece *hboxlayout = NEWPIECEEX(mHBoxLayoutPiece, 3);
	_c(hboxlayout)->setCellInfo(hboxlayout, 0, ARROW_SIZE, NCS_LAYOUTPIECE_ST_FIXED, FALSE);
	_c(hboxlayout)->setCellInfo(hboxlayout, 2, ARROW_SIZE, NCS_LAYOUTPIECE_ST_FIXED, FALSE);
	_c(hboxlayout)->setCell(hboxlayout, 0, (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, NCS_ARROWPIECE_LEFT));
	_c(hboxlayout)->setCell(hboxlayout, 2, (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, NCS_ARROWPIECE_RIGHT));
	_c(hboxlayout)->setCell(hboxlayout, 1, (mHotPiece*)add_data);

	Class(mSpinBoxPiece).construct((mSpinBoxPiece*)self, (DWORD)hboxlayout);
}

static mHotPiece * mHSpinBoxPiece_getIncPiece(mHSpinBoxPiece*self)
{
	return self->body?(_c((mHBoxLayoutPiece*)(self->body))->getCell((mHBoxLayoutPiece*)(self->body), 2)):NULL;
}

static mHotPiece * mHSpinBoxPiece_getDecPiece(mHSpinBoxPiece*self)
{
	return self->body?(_c((mHBoxLayoutPiece*)(self->body))->getCell((mHBoxLayoutPiece*)(self->body), 0)):NULL;
}

static mHotPiece * mHSpinBoxPiece_getSpinnedPiece(mHSpinBoxPiece*self)
{
	return self->body?(_c((mHBoxLayoutPiece*)(self->body))->getCell((mHBoxLayoutPiece*)(self->body), 1)):NULL;
}

BEGIN_MINI_CLASS(mHSpinBoxPiece, mSpinBoxPiece)
	CLASS_METHOD_MAP(mHSpinBoxPiece, construct   )
	CLASS_METHOD_MAP(mHSpinBoxPiece, getIncPiece )
	CLASS_METHOD_MAP(mHSpinBoxPiece, getDecPiece )
	CLASS_METHOD_MAP(mHSpinBoxPiece, getSpinnedPiece )
END_MINI_CLASS

#endif //_MGNCSCTRL_SPINBOX
