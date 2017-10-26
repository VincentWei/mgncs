/*
 ** $Id: mtoolimgitempiece.c 1681 2017-10-26 06:46:31Z weiym $
 **
 ** mtoolimgitempiece.
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: dongjunjie 
 **  
 ** Create date: 2010年12月24日 
 */
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
#include "mtoolimgpiece.h"
#include "mtoolimgitempiece.h"

static void mToolImageItemPiece_paint(mToolImageItemPiece* self, HDC hdc, mWidget *owner, DWORD add_data)
{
	//accept the state
	//RECT rc_img;
	int idx = 0;
	if(!self->toolImg || !self->toolImg->pbmp)
		return;

	if(self->toolImg->cell_count > 0)
	{
		int check_state = NCS_PIECE_PAINT_GET_CHECK(add_data);
		if((add_data & NCS_PIECE_PAINT_STATE_MASK) == PIECE_STATE_DISABLE)
		{
			idx = 3;
		}
		else
		{
			if(check_state == PIECE_CHECKSTATE_CHECKED)
			{
				idx = 2;
			}
			else
			{
				switch(add_data&NCS_PIECE_PAINT_STATE_MASK){
				case PIECE_STATE_NORMAL:
				case PIECE_STATE_CAPTURED:
					idx = 0; break;
				case PIECE_STATE_HILIGHT:
					idx = 1; break;
				case PIECE_STATE_PUSHED:
					idx = 2; break;
				case PIECE_STATE_DISABLE:
					idx = 3; break;
				}
			}
		}
	
	}
	
	Class(mToolImagePiece).paint((mToolImagePiece*)self, hdc, owner, idx);
}

BEGIN_MINI_CLASS(mToolImageItemPiece, mToolImagePiece)
	CLASS_METHOD_MAP(mToolImageItemPiece, paint)
END_MINI_CLASS

