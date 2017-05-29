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

#include "mhotpiece.h"
#include "mlayoutpiece.h"
#include "mstaticpiece.h"
#include "mlabelpiece.h"

#include "mpairpiece.h"
#include "mrenderablepiece.h"
#include "mcontainerpiece.h"
#include "mabstractbuttonpiece.h"
#include "mcheckbuttonpiece.h"
#include "mradiobuttonpiece.h"
#include "mradioboxpiece.h"

static void mRadioButtonPiece_construct(mRadioButtonPiece *self, DWORD add_data)
{
	mHotPiece** pieces = (mHotPiece**) add_data;
	mHotPiece* mypieces[2];

	if(pieces == NULL)
	{
		pieces = mypieces;
		pieces[0] = NULL;
		pieces[1] =  NEWPIECE(mLabelPiece);
	}

	if(pieces[0] == NULL)
		pieces[0] = (mHotPiece*)NEWPIECE(mRadioBoxPiece);

	Class(mCheckButtonPiece).construct((mCheckButtonPiece*)self, add_data);

}

BEGIN_MINI_CLASS(mRadioButtonPiece, mCheckButtonPiece)
	CLASS_METHOD_MAP(mRadioButtonPiece, construct)
END_MINI_CLASS
