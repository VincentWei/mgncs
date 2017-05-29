
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

#include "mhotpiece.h"
#include "mstaticpiece.h"
#include "mrenderablepiece.h"
#include "marrowpiece.h"
#include "mdownarrowpiece.h"

static void mDownArrowPiece_paint(mDownArrowPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	if(self->rdr_func)
		self->rdr_func((mHotPiece*)self, hdc, (mObject*)owner, add_data|NCS_ARROWPIECE_DOWN);
}

BEGIN_MINI_CLASS(mDownArrowPiece, mArrowPiece)
	CLASS_METHOD_MAP(mDownArrowPiece, paint)
END_MINI_CLASS

