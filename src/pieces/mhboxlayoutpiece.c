
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
#include "mlayoutpiece.h"
#include "mboxlayoutpiece.h"
#include "mhboxlayoutpiece.h"

static void mHBoxLayoutPiece_recalcBox(mHBoxLayoutPiece *self)
{
	int _sizes[256];
	int *sizes;
	if(self->count <= 0)
		return;

	if(self->count < sizeof(_sizes)/sizeof(int))
		sizes = _sizes;
	else
	{
#ifdef _HAVE_ALLOCA
		sizes = (int*)alloca(self->count * sizeof(int));
#else
		sizes = (int*)malloc(self->count * sizeof(int));
#endif
	}

	if(mBoxLayoutPiece_calcSize((mBoxLayoutPiece*)self,self->right - self->left ,sizes))
	{
		int i;
		RECT rc = { self->left + self->margin, self->top + self->margin, 0, self->bottom - self->margin};
		for(i=0; i<self->count; i++)
		{
			rc.right = rc.left + sizes[i];
			if(self->cells[i])
				_c(self->cells[i])->setRect(self->cells[i], &rc);
			rc.left = rc.right + self->space;
		}
	}


#ifndef _HAVE_ALLOCA
	if(sizes && sizes != _sizes)
	{
		free(sizes);
	}
#endif
}

static int mHBoxLayoutPiece_getCellAutoSize(mHBoxLayoutPiece *self, int i)
{
	RECT rc;
	if(i < 0 || i >= self->count || self->cells[i] == NULL)
		return 0;

	if(!_c(self->cells[i])->getRect(self->cells[i], &rc))
		return 0;
	return RECTW(rc);
}

BEGIN_MINI_CLASS(mHBoxLayoutPiece, mBoxLayoutPiece)
	CLASS_METHOD_MAP(mHBoxLayoutPiece, recalcBox)
	CLASS_METHOD_MAP(mHBoxLayoutPiece, getCellAutoSize)
END_MINI_CLASS

