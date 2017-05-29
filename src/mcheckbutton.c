
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncsconfig.h"
#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#include "mwidget.h"
#include "mbutton.h"
#include "mcheckbutton.h"
#include "mrdr.h"
#include "piece.h"

#include "mstatic.h"
#include "mcheckbutton.h"
#include "mgroupbox.h"
#include "mbuttongroup.h"

static mHotPiece * mCheckButton_createButtonBody(mCheckButton *self, DWORD dwStyle, mHotPiece * content)
{
	mHotPiece *body;
	mHotPiece *pieces[2]={NULL, content};

	body = (mHotPiece*)NEWPIECEEX(mCheckButtonPiece, pieces);
	_c(body)->setProperty(body, NCSP_ABP_CHECKABLE, 1);
    _c(body)->setProperty(body, NCSP_LABELPIECE_ALIGN, NCS_ALIGN_LEFT);
	if(dwStyle & NCSS_BUTTON_AUTOCHECK)
		_c(body)->setProperty(body, NCSP_ABP_AUTOCHECK, 1);
	if(dwStyle & NCSS_BUTTON_3DCHECK)
		_c(body)->setProperty(body, NCSP_ABP_3DCHECK,1);
			
	return (mHotPiece*)body;
}

static void mCheckButton_onPaint(mCheckButton *self, HDC hdc, const PCLIPRGN pinv_clip)
{
	mHotPiece* content = NULL;
    _SUPER(mWidget, self, onPaint, hdc, pinv_clip);
 
	content = (mHotPiece*)GetBodyProp(NCSP_CHECKBUTTONPIECE_CONTENT);
	if(GetFocus(GetParent(self->hwnd)) == self->hwnd)
	{
		RECT rc;

		if(content && !(_c(content)->getRect(content, &rc)))
			GetClientRect(self->hwnd, &rc);
		if(self->renderer)
			self->renderer->drawFocusFrame(self, hdc, &rc);
		else
			ncsCommRDRDrawFocusFrame(self->hwnd, hdc, &rc);
	}
}

static BOOL mCheckButton_onEraseBkgnd(mCheckButton* self, HDC hdc, const RECT *pinv)
{
	return Class(mWidget).onEraseBkgnd((mWidget*)self, hdc, pinv);
}

BEGIN_CMPT_CLASS(mCheckButton, mButton)
	CLASS_METHOD_MAP(mCheckButton, createButtonBody)
	CLASS_METHOD_MAP(mCheckButton, onPaint)
	CLASS_METHOD_MAP(mCheckButton, onEraseBkgnd)
END_CMPT_CLASS
