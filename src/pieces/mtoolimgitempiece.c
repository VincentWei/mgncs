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
	
	Class(mToolImagePiece).paint((mToolImagePiece*)self, hdc, (mObject *)owner, idx);
}

BEGIN_MINI_CLASS(mToolImageItemPiece, mToolImagePiece)
	CLASS_METHOD_MAP(mToolImageItemPiece, paint)
END_MINI_CLASS

