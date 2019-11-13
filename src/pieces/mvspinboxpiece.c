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
#include "mcontainerpiece.h"
#include "mspinnerpiece.h"
#include "mspinboxpiece.h"
#include "mvspinboxpiece.h"

#include "mlayoutpiece.h"
#include "mpairpiece.h"

#include "mabstractbuttonpiece.h"
#include "marrowbuttonpiece.h"

#ifdef _MGNCSCTRL_SPINBOX

#define ARROW_SIZE 12

static void mVSpinBoxPiece_construct(mVSpinBoxPiece *self, DWORD add_data)
{
	mPairPiece *pair, *pair_arrows;
	//create a vert spinbox
	pair = NEWPIECE(mPairPiece);
	//by default, spinbox in the right
	_c(pair)->setProperty(pair, NCSP_PAIRPIECE_SECOND_AS_FIRST, 1);
	_c(pair)->setProperty(pair, NCSP_PAIRPIECE_FIRST_SIZE_TYPE, NCS_PAIRPIECE_ST_FIXED);
	_c(pair)->setProperty(pair, NCSP_PAIRPIECE_FIRST_SIZE, ARROW_SIZE);

	pair_arrows = NEWPIECE(mPairPiece);
	mPairPiece_setVert(pair_arrows);
	pair_arrows->first  = (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, NCS_ARROWPIECE_UP);
	pair_arrows->second = (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, NCS_ARROWPIECE_DOWN);

	pair->first = (mHotPiece*)pair_arrows;
	pair->second = (mHotPiece*)add_data;

	Class(mSpinBoxPiece).construct((mSpinBoxPiece*)self, (DWORD)pair);
}

static mHotPiece * mVSpinBoxPiece_getIncPiece(mVSpinBoxPiece*self)
{
	mPairPiece * pair = (mPairPiece*)(self->body);
	if(!pair)
		return NULL;
	pair = ((mPairPiece*)(pair->first));
	return pair ? pair->first : NULL;
}

static mHotPiece * mVSpinBoxPiece_getDecPiece(mVSpinBoxPiece*self)
{
	mPairPiece * pair = (mPairPiece*)(self->body);
	if(!pair)
		return NULL;
	pair = ((mPairPiece*)(pair->first));
	return pair ? pair->second : NULL;
}

static mHotPiece * mVSpinBoxPiece_getSpinnedPiece(mVSpinBoxPiece*self)
{
	return self->body?(((mPairPiece*)(self->body))->second):NULL;
}

BEGIN_MINI_CLASS(mVSpinBoxPiece, mSpinBoxPiece)
	CLASS_METHOD_MAP(mVSpinBoxPiece, construct   )
	CLASS_METHOD_MAP(mVSpinBoxPiece, getIncPiece )
	CLASS_METHOD_MAP(mVSpinBoxPiece, getDecPiece )
	CLASS_METHOD_MAP(mVSpinBoxPiece, getSpinnedPiece )
END_MINI_CLASS

#endif //_MGNCSCTRL_SPINBOX
