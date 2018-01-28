
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
#include "mstaticpiece.h"
#include "mrenderablepiece.h"
#include "marrowpiece.h"
#include "mrightarrowpiece.h"

static void mRightArrowPiece_paint(mRightArrowPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	if(self->rdr_func)
		self->rdr_func((mHotPiece*)self, hdc, (mObject*)owner, add_data|NCS_ARROWPIECE_RIGHT);
}

BEGIN_MINI_CLASS(mRightArrowPiece, mArrowPiece)
	CLASS_METHOD_MAP(mRightArrowPiece, paint)
END_MINI_CLASS

