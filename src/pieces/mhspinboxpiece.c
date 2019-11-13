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
#include "mhspinboxpiece.h"

#include "mlayoutpiece.h"
#include "mboxlayoutpiece.h"
#include "mhboxlayoutpiece.h"

#include "mabstractbuttonpiece.h"
#include "marrowbuttonpiece.h"

#ifdef _MGNCSCTRL_SPINBOX

#define ARROW_SIZE 12

static void mHSpinBoxPiece_construct(mHSpinBoxPiece *self, DWORD add_data)
{
	//create boxlayout
	mHBoxLayoutPiece *hboxlayout = NEWPIECEEX(mHBoxLayoutPiece, 3);
	_c(hboxlayout)->setCellInfo(hboxlayout, 0, ARROW_SIZE, NCS_LAYOUTPIECE_ST_FIXED, FALSE);
	_c(hboxlayout)->setCellInfo(hboxlayout, 2, ARROW_SIZE, NCS_LAYOUTPIECE_ST_FIXED, FALSE);
	_c(hboxlayout)->setCell(hboxlayout, 0, (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, NCS_ARROWPIECE_LEFT));
	_c(hboxlayout)->setCell(hboxlayout, 2, (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, NCS_ARROWPIECE_RIGHT));
	_c(hboxlayout)->setCell(hboxlayout, 1, (mHotPiece*)add_data);

	Class(mSpinBoxPiece).construct((mSpinBoxPiece*)self, (DWORD)hboxlayout);
}

static mHotPiece * mHSpinBoxPiece_getIncPiece(mHSpinBoxPiece*self)
{
	return self->body?(_c((mHBoxLayoutPiece*)(self->body))->getCell((mHBoxLayoutPiece*)(self->body), 2)):NULL;
}

static mHotPiece * mHSpinBoxPiece_getDecPiece(mHSpinBoxPiece*self)
{
	return self->body?(_c((mHBoxLayoutPiece*)(self->body))->getCell((mHBoxLayoutPiece*)(self->body), 0)):NULL;
}

static mHotPiece * mHSpinBoxPiece_getSpinnedPiece(mHSpinBoxPiece*self)
{
	return self->body?(_c((mHBoxLayoutPiece*)(self->body))->getCell((mHBoxLayoutPiece*)(self->body), 1)):NULL;
}

BEGIN_MINI_CLASS(mHSpinBoxPiece, mSpinBoxPiece)
	CLASS_METHOD_MAP(mHSpinBoxPiece, construct   )
	CLASS_METHOD_MAP(mHSpinBoxPiece, getIncPiece )
	CLASS_METHOD_MAP(mHSpinBoxPiece, getDecPiece )
	CLASS_METHOD_MAP(mHSpinBoxPiece, getSpinnedPiece )
END_MINI_CLASS

#endif //_MGNCSCTRL_SPINBOX
