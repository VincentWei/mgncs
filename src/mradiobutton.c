
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

static mObject * mRadioButton_createButtonBody(mRadioButton *self, DWORD dwStyle, mHotPiece * content)
{
    mHotPiece *body;
    mHotPiece *radiobox = (mHotPiece*)NEWPIECE(mRadioBoxPiece);
    mHotPiece *pieces[2]={radiobox, content};

    body = (mHotPiece*)NEWPIECEEX(mCheckButtonPiece, pieces);
    _c(body)->setProperty(body, NCSP_LABELPIECE_ALIGN, NCS_ALIGN_LEFT);

    _c(body)->setProperty(body, NCSP_ABP_CHECKABLE, 1);
    if(dwStyle & NCSS_BUTTON_AUTOCHECK)
        _c(body)->setProperty(body, NCSP_ABP_AUTOCHECK, 1);

    return (mObject*)body;
}

BEGIN_CMPT_CLASS(mRadioButton, mCheckButton)
	CLASS_METHOD_MAP(mRadioButton, createButtonBody)
END_CMPT_CLASS
