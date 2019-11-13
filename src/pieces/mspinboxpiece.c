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
#include "mcontainerpiece.h"
#include "mspinnerpiece.h"
#include "mspinboxpiece.h"

#ifdef _MGNCSCTRL_SPINBOX

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

#endif //_MGNCSCTRL_SPINBOX
