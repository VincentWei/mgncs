
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "mhotpiece.h"
#include "mcontainerpiece.h"
#include "mabstractbuttonpiece.h"
#include "marrowbuttonpiece.h"

#include "mstaticpiece.h"
#include "mrenderablepiece.h"
#include "marrowpiece.h"
#include "mleftarrowpiece.h"
#include "muparrowpiece.h"
#include "mrightarrowpiece.h"
#include "mdownarrowpiece.h"

#if defined (_MGNCSCTRL_IMWORDSEL) || defined (_MGNCSCTRL_SPINNER) || defined (_MGNCSCTRL_SCROLLBAR)

static void mArrowButtonPiece_construct(mArrowButtonPiece *self, DWORD add_data)
{
	Class(mAbstractButtonPiece).construct((mAbstractButtonPiece*)self, add_data);

	switch((int)add_data)
	{
	case NCS_ARROWPIECE_RIGHT:
		self->body = (mHotPiece*)NEWPIECE(mRightArrowPiece);
		break;
	case NCS_ARROWPIECE_UP:
		self->body = (mHotPiece*)NEWPIECE(mUpArrowPiece);
		break;
	case NCS_ARROWPIECE_DOWN:
		self->body = (mHotPiece*)NEWPIECE(mDownArrowPiece);
		break;
	case NCS_ARROWPIECE_LEFT:
	default:
		self->body = (mHotPiece*)NEWPIECE(mLeftArrowPiece);
		break;
	}
}

static void mArrowButtonPiece_paint(mArrowButtonPiece *self, HDC hdc, mWidget * owner, DWORD add_data)
{
	Class(mAbstractButtonPiece).paint((mAbstractButtonPiece*)self, hdc, (mObject*)owner, add_data|NCS_PIECE_PAINT_ARROW_SHELL);

}

BEGIN_MINI_CLASS(mArrowButtonPiece, mAbstractButtonPiece)
	CLASS_METHOD_MAP(mArrowButtonPiece, construct)
	CLASS_METHOD_MAP(mArrowButtonPiece, paint)
END_MINI_CLASS

#endif		//_MGNCSCTRL_IMWORDSEL

