/* 
    This file is part of mGNCS, a component for MiniGUI.

    Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.
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
#include "mabstractbuttonpiece.h"
#include "marrowbuttonpiece.h"

#include "mstaticpiece.h"
#include "mrenderablepiece.h"
#include "marrowpiece.h"
#include "mleftarrowpiece.h"
#include "muparrowpiece.h"
#include "mrightarrowpiece.h"
#include "mdownarrowpiece.h"

#if defined (_MGNCSCTRL_IMWORDSEL) || defined (_MGNCSCTRL_SPINNER) || defined (_MGNCSCTRL_SCROLLBAR)

static void mArrowButtonPiece_construct(mArrowButtonPiece *self, DWORD add_data)
{
	Class(mAbstractButtonPiece).construct((mAbstractButtonPiece*)self, add_data);

	switch((int)add_data)
	{
	case NCS_ARROWPIECE_RIGHT:
		self->body = (mHotPiece*)NEWPIECE(mRightArrowPiece);
		break;
	case NCS_ARROWPIECE_UP:
		self->body = (mHotPiece*)NEWPIECE(mUpArrowPiece);
		break;
	case NCS_ARROWPIECE_DOWN:
		self->body = (mHotPiece*)NEWPIECE(mDownArrowPiece);
		break;
	case NCS_ARROWPIECE_LEFT:
	default:
		self->body = (mHotPiece*)NEWPIECE(mLeftArrowPiece);
		break;
	}
}

static void mArrowButtonPiece_paint(mArrowButtonPiece *self, HDC hdc, mWidget * owner, DWORD add_data)
{
	Class(mAbstractButtonPiece).paint((mAbstractButtonPiece*)self, hdc, (mObject*)owner, add_data|NCS_PIECE_PAINT_ARROW_SHELL);

}

BEGIN_MINI_CLASS(mArrowButtonPiece, mAbstractButtonPiece)
	CLASS_METHOD_MAP(mArrowButtonPiece, construct)
	CLASS_METHOD_MAP(mArrowButtonPiece, paint)
END_MINI_CLASS

#endif		//_MGNCSCTRL_IMWORDSEL

