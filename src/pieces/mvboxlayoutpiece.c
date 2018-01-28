
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
#include "mvboxlayoutpiece.h"

static void mVBoxLayoutPiece_recalcBox(mVBoxLayoutPiece *self)
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

	if(mBoxLayoutPiece_calcSize((mBoxLayoutPiece*)self,self->bottom - self->top ,sizes))
	{
		int i;
		RECT rc = { self->left + self->margin, self->top + self->margin, self->right - self->margin, 0};
		for(i=0; i<self->count; i++)
		{
			rc.bottom = rc.top + sizes[i];
			if(self->cells[i])
				_c(self->cells[i])->setRect(self->cells[i], &rc);
			rc.top = rc.bottom + self->space;
		}
	}


#ifndef _HAVE_ALLOCA
	if(sizes && sizes != _sizes)
	{
		free(sizes);
	}
#endif	
}

static int mVBoxLayoutPiece_getCellAutoSize(mVBoxLayoutPiece *self, int i)
{
	RECT rc;
	if(i < 0 || i >= self->count || self->cells[i] == NULL)
		return 0;
	
	if(!_c(self->cells[i])->getRect(self->cells[i], &rc))
		return 0;
	return RECTH(rc);
}



BEGIN_MINI_CLASS(mVBoxLayoutPiece, mBoxLayoutPiece)
	CLASS_METHOD_MAP(mVBoxLayoutPiece, recalcBox)
	CLASS_METHOD_MAP(mVBoxLayoutPiece, getCellAutoSize)
END_MINI_CLASS


