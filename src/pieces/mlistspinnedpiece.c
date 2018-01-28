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
#include "mlabelpiece.h"
#include "mlistspinnedpiece.h"

static BOOL mListSpinnedPiece_setProperty(mListSpinnedPiece *self, int id, DWORD value)
{
	switch(id)
	{
	case NCSP_LABELPIECE_LABEL:
		return FALSE;
	case PIECECOMM_PROP_POS:
		if(!self->list)
			return FALSE;
		self->str = self->list[(int)value];
		return TRUE;
	case NCSP_MLISTSPINNEDPIECE_LIST:
		self->list = (const char **)value;
		return TRUE;
	}
	return Class(mLabelPiece).setProperty((mLabelPiece*)self, id, value);
}

BEGIN_MINI_CLASS(mListSpinnedPiece, mLabelPiece)
	CLASS_METHOD_MAP(mListSpinnedPiece, setProperty)
END_MINI_CLASS

