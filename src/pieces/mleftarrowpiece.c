
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
#include "mleftarrowpiece.h"

static void mLeftArrowPiece_paint(mLeftArrowPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	if(self->rdr_func)
		self->rdr_func((mHotPiece*)self, hdc, (mObject*)owner, add_data|NCS_ARROWPIECE_LEFT);
}

BEGIN_MINI_CLASS(mLeftArrowPiece, mArrowPiece)
	CLASS_METHOD_MAP(mLeftArrowPiece, paint)
END_MINI_CLASS

