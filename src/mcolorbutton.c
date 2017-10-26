
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <mgutils/mgutils.h>

#include "mgncsconfig.h"
#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#include "mwidget.h"
#include "mcolorbutton.h"
#include "mrdr.h"
#include "piece.h"

#ifdef _MGNCSCTRL_COLORBUTTON

typedef struct _my_color_dlg_data {
	COLORDLGDATA data;
	const char* rdr_name;
}MY_COLOR_DLG_DATA;

static void set_all_window_elements(HWND hwnd, const char* rdr)
{
	//set children
	HWND hchild = HWND_NULL;
	while(IsWindow(hchild = GetNextChild(hwnd, hchild)))
	{
		set_all_window_elements(hchild, rdr);
	}
	SetWindowElementRenderer(hwnd, rdr, NULL);
}

static int color_dlg_proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	if(message == MSG_INITDIALOG)
	{
		MY_COLOR_DLG_DATA *pdata = (MY_COLOR_DLG_DATA*)lParam;
		int ret;
		if(pdata && pdata->rdr_name){
			ret = DefColorDialogProc(hWnd, message, wParam, lParam);
			set_all_window_elements(hWnd, pdata->rdr_name);
			return ret;
		}
	}

	return DefColorDialogProc(hWnd, message, wParam, lParam);
}

static BOOL mColorButton_onPiece(mColorButton* self, mHotPiece *piece, int event_id, DWORD param)
{
	if(event_id == NCSN_ABP_CLICKED)
	{
		mPushButtonPiece * push = (mPushButtonPiece*)piece;
		//DWORD color = _c(push->content)->getProperty(push->content, NCSN_RECTPIECE_FILLCOLOR);
		//open the color dialog
	//	MY_COLOR_DLG_DATA pcdd = {0};
		MY_COLOR_DLG_DATA pcdd;
		memset(&pcdd, 0, sizeof(pcdd));
		pcdd.rdr_name = self->renderer->rdr_name;
		if(ColorSelectDialog(NULL, self->hwnd, color_dlg_proc, (COLORDLGDATA*)&pcdd))
		{
			DWORD color = MakeRGBA(pcdd.data.r, pcdd.data.g, pcdd.data.b, 255);
			_c(push->content)->setProperty(push->content, NCSP_RECTPIECE_FILLCOLOR,color);
			mHotPiece_update(piece, (mObject*)self, TRUE);
			ncsNotifyParentEx((mWidget*)self, NCSN_CLRBTN_COLORCHANGED,color);
		}
	}
	return FALSE;
}

static mObject * mColorButton_createBody(mColorButton *self)
{
	mRectPiece *piece = NEWPIECE(mRectPiece);
	mPushButtonPiece *push = NEWPIECE(mPushButtonPiece);
	push->content = (mHotPiece*)piece;
	//add event
	ncsAddEventListener((mObject*)push,(mObject*)self, (NCS_CB_ONPIECEEVENT)mColorButton_onPiece, NCSN_ABP_CLICKED);
	return (mObject*)push;
}

static DWORD mColorButton_setProperty(mColorButton* self, int id, DWORD value)
{
	if( id >= NCSP_CLRBTN_MAX)
		return 0;

	switch(id)
	{
	case NCSP_CLRBTN_CURCOLOR:
		if(_c(HP(self->body))->setProperty(HP(self->body), NCSP_RECTPIECE_FILLCOLOR, value))
		{
			mHotPiece_update(HP(self->body), (mObject*)self, TRUE);
			return TRUE;
		}
		return FALSE;
	}

	return Class(mWidget).setProperty((mWidget*)self, id, value);
}

static int mColorButton_onSizeChanged(mColorButton *self, RECT *rcClient)
{
	RECT rc;
	mPushButtonPiece * push = (mPushButtonPiece*)(HP(self->body));
	Class(mWidget).onSizeChanged((mWidget*)self, rcClient);

	if(!push || push->content == NULL)
		return 0;
	rc.left = 8;
	rc.top  = 4;
	rc.right = RECTWP(rcClient) - 8;
	rc.bottom = RECTHP(rcClient) - 4;

	_c(push->content)->setRect(push->content, &rc);

	return 0;
}

static DWORD mColorButton_getProperty(mColorButton* self, int id)
{
	if( id >= NCSP_CLRBTN_MAX)
		return (DWORD)-1;

	switch(id)
    {
        case NCSP_CLRBTN_CURCOLOR:
		{
			mPushButtonPiece * push = (mPushButtonPiece*)(HP(self->body));
			if(!push || push->content == NULL)
				return (DWORD)-1;
			return _c(push->content)->getProperty(push->content, NCSP_RECTPIECE_FILLCOLOR);
		}
    }

	return Class(mWidget).getProperty((mWidget*)self, id);
}


BEGIN_CMPT_CLASS(mColorButton, mWidget)
	CLASS_METHOD_MAP(mColorButton, createBody)
	CLASS_METHOD_MAP(mColorButton, setProperty);
	CLASS_METHOD_MAP(mColorButton, getProperty);
	CLASS_METHOD_MAP(mColorButton, onSizeChanged);
END_CMPT_CLASS

#endif		//_MGNCSCTRL_COLORBUTTON
