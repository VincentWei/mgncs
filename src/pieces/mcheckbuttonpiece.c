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
