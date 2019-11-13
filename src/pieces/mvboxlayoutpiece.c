///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/*
 *   This file is part of mGNCS, a component for MiniGUI.
 * 
 *   Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */

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


