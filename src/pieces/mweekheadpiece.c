
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
#include "mweekheadpiece.h"

static BOOL mWeekHeadPiece_setProperty(mWeekHeadPiece * self, int id, DWORD value)
{
	if(id == NCSP_WEEKHEADPIECE_WEEKNAME)
	{
		self->week_names = (char**)value;
		return TRUE;
	}
	return Class(mRenderablePiece).setProperty((mRenderablePiece*)self, id, value);
}

BEGIN_MINI_CLASS(mWeekHeadPiece, mRenderablePiece)
	CLASS_METHOD_MAP(mWeekHeadPiece, setProperty)
END_MINI_CLASS

