
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

static void mSpinBoxPiece_onPosChanged(mSpinBoxPiece *self)
{
	mHotPiece * spinned = _c(self)->getSpinnedPiece(self);
	_c(spinned)->setProperty(spinned, PIECECOMM_PROP_POS, self->cur_pos);
}

static BOOL mSpinBoxPiece_setProperty(mSpinBoxPiece * self, int id, DWORD value)
{
	switch(id)
	{
	case NCSP_SPNRPIECE_MAXPOS:
	case NCSP_SPNRPIECE_MINPOS:
	case NCSP_SPNRPIECE_CURPOS:
		if(Class(mSpinnerPiece).setProperty((mSpinnerPiece*)self, id, value))
		{
			_c(self)->onPosChanged(self);
			return TRUE;
		}
		return FALSE;
	case PIECECOMM_PROP_DIRECTION:
		return FALSE;
	}
	return Class(mSpinnerPiece).setProperty((mSpinnerPiece*)self, id, value);
}

BEGIN_MINI_CLASS(mSpinBoxPiece, mSpinnerPiece)
	CLASS_METHOD_MAP(mSpinBoxPiece, onPosChanged)
	CLASS_METHOD_MAP(mSpinBoxPiece, setProperty )
END_MINI_CLASS


