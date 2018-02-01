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
#include "mlayoutpiece.h"
#include "mstaticpiece.h"
#include "mcontainerpiece.h"
#include "mabstractbuttonpiece.h"
#include "mcheckbuttonpiece.h"

#include "mpairpiece.h"
#include "mrenderablepiece.h"
#include "mcheckboxpiece.h"
#include "mradioboxpiece.h"
#include "piece.h"

static void mCheckButtonPiece_construct(mCheckButtonPiece *self, DWORD add_data)
{
	mHotPiece** pieces = (mHotPiece**) add_data;
	mPairPiece * body;
	Class(mAbstractButtonPiece).construct((mAbstractButtonPiece*)self, add_data);

	body = NEWPIECE(mPairPiece);

	if(pieces)
	{
		body->first = pieces[0];
		body->second = pieces[1];
	}

	if(body->first == NULL)
		body->first = (mHotPiece*)NEWPIECE(mCheckBoxPiece);

	mPairPiece_setFirstSizeType(body, NCS_PAIRPIECE_ST_FIXED);
	mPairPiece_setFirstSize(body, 20);
	mPairPiece_setMargin(body, 2);
	mPairPiece_setSpace(body, 2);
	
	self->body = (mHotPiece*)body;
}

static DWORD mCheckButtonPiece_getProperty(mCheckButtonPiece* self, int id)
{
	if(id == NCSP_CHECKBUTTONPIECE_CONTENT){
		if(!self->body)
			return 0;
		return (DWORD)((mPairPiece*)(self->body))->second;
	}
	else if(id == NCSP_PAIRPIECE_FIRST_SIZE){
		mHotPiece *content = ((mPairPiece*)(self->body))->second;
		if(content)
			return _c(content)->getProperty(content, id);
	}

	return Class(mAbstractButtonPiece).getProperty((mAbstractButtonPiece*)self, id);

}

static DWORD mCheckButtonPiece_setProperty(mCheckButtonPiece *self, int id, DWORD value)
{
	if((id >= PIECECOMM_PROP_ALIGN && id <= PIECECOMM_PROP_AUTOWRAP)
		|| (id >= LABELPIECE_PROP_BEGIN && id <= LABELPIECE_PROP_END))
	{
		if(id == PIECECOMM_PROP_ALIGN)
		{
			_c(self->body)->setProperty(self->body, NCSP_PAIRPIECE_SECOND_AS_FIRST, value == NCS_ALIGN_RIGHT);
		}
	}
	else if((id == NCSP_PAIRPIECE_FIRST_SIZE))
	{
		mHotPiece *content = ((mPairPiece*)(self->body))->second;
		if(content)
		{
			return _c(content)->setProperty(content, id, value);
		}
	}

	return Class(mAbstractButtonPiece).setProperty((mAbstractButtonPiece*)self,id, value);
}

BEGIN_MINI_CLASS(mCheckButtonPiece, mAbstractButtonPiece)
	CLASS_METHOD_MAP(mCheckButtonPiece, construct)
	CLASS_METHOD_MAP(mCheckButtonPiece, getProperty)
	CLASS_METHOD_MAP(mCheckButtonPiece, setProperty)
END_MINI_CLASS
