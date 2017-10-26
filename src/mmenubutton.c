
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
#include "mpopmenumgr.h"
#include "mmenubutton.h"
#include "mrdr.h"
#include "piece.h"

#ifdef _MGNCSCTRL_MENUBUTTON

static void mMenuButton_construct(mMenuButton *self, DWORD addData)
{
	Class(mButton).construct((mButton *)self, addData);

	self->popmenu = NULL;
	self->cur_item = 0;
}

static void mMenuButton_destroy(mMenuButton *self)
{
	if(self->popmenu)
		_c(self->popmenu)->release(self->popmenu);

	Class(mButton).destroy((mButton*)self);
}

static mObject * mMenuButton_createContent(mMenuButton *self, DWORD dwStyle)
{
	mHotPiece  * pieces[2] ;
	mPairPiece * pair;
	pieces[1] = (mHotPiece*)(Class(mButton).createContent((mButton*)self, dwStyle));
	if(!pieces[1])
		return NULL;

	pieces[0] = (mHotPiece*)NEWPIECE(mDownArrowPiece);
	pair = NEWPIECEEX(mPairPiece, pieces);
	_c(pair)->setProperty(pair, NCSP_PAIRPIECE_SECOND_AS_FIRST, 1);
	_c(pair)->setProperty(pair, NCSP_PAIRPIECE_FIRST_SIZE_TYPE, NCS_PAIRPIECE_ST_FIXED);
	_c(pair)->setProperty(pair, NCSP_PAIRPIECE_FIRST_SIZE, 20);
	_c(pair)->setProperty(pair, NCSP_PAIRPIECE_MARGIN, 5);
	return (mObject*)pair;
}

static BOOL mMenuButton_onPiece(mMenuButton *self, mHotPiece *sender, int event_id, DWORD param)
{
	if(event_id == NCSN_ABP_CLICKED)
	{
		if(self->popmenu)
		{
			RECT rc;
			HMENU hMenu = _c(self->popmenu)->createMenu(self->popmenu);
			if(!hMenu)
				return FALSE;

			GetWindowRect(self->hwnd, &rc);
			WindowToScreen(GetParent(self->hwnd), &rc.left, &rc.bottom);
			TrackPopupMenu(hMenu, 0, rc.left, rc.bottom, self->hwnd);
		}
		ncsNotifyParent((mWidget*)self, NCSN_WIDGET_CLICKED);	
	}
    else if(event_id == NCSN_ABP_PUSHED)
	{	
		ncsNotifyParent((mWidget*)self, NCSN_BUTTON_PUSHED);	
	}
	
	return TRUE;
}

static mObject * mMenuButton_createBody(mMenuButton * self)
{
	DWORD dwStyle = GetWindowStyle(self->hwnd);

	mHotPiece *content = (mHotPiece*)(_M(self, createContent, dwStyle));
	mHotPiece *body = (mHotPiece*)_M(self, createButtonBody, dwStyle, (mObject*)content);
	if(body)
	{
		int event_ids[]={NCSN_ABP_CLICKED, NCSN_ABP_PUSHED, 0};
		ncsAddEventListeners((mObject*)body, (mObject*)self, 
				(NCS_CB_ONPIECEEVENT)mMenuButton_onPiece, event_ids);
	}
	return (mObject*)body;
}

static void update_cur_item(mMenuButton *self, BOOL bupdate)
{
	MENUITEMINFO mii;
	if(!self->popmenu
        || !_c(self->popmenu)->getMenuItem(self->popmenu, self->cur_item, &mii, self->cur_item==0?FALSE:TRUE))
		return ;

	//set text
	if(mii.type != MFT_SEPARATOR )
	{
		SetWindowCaption(self->hwnd,(const char*)mii.typedata);
		_c(HP(self->body))->setProperty(HP(self->body), NCSP_LABELPIECE_LABEL,(DWORD)GetWindowCaption(self->hwnd));
	}

	if(mii.type == MFT_BITMAP)
	{
		_c(HP(self->body))->setProperty(HP(self->body),NCSP_IMAGEPIECE_IMAGE, mii.typedata);
	}

	if(mii.type == MFT_BMPSTRING && mii.mask & MIIM_CHECKMARKS)
	{
		_c(HP(self->body))->setProperty(HP(self->body), NCSP_IMAGEPIECE_IMAGE, (DWORD)mii.checkedbmp);
	}

	if(self->cur_item == 0)
		self->cur_item = mii.id;


	if(bupdate)
		InvalidateRect(self->hwnd, NULL, TRUE);

}
static BOOL set_cur_item(mMenuButton *self, int idx, BOOL bupdate)
{
	if(!self->popmenu)
		return FALSE;

	if(self->cur_item == idx)
		return FALSE;

	self->cur_item = idx;
	update_cur_item(self, bupdate);
	return TRUE;
}

static BOOL mMenuButton_setProperty(mMenuButton *self, int id, DWORD value)
{

#ifdef _MGNCS_GUIBUILDER_SUPPORT
	if(id == NCSP_DEFAULT_CONTENT)
	{
		if(!self->popmenu)
		{
			mPopMenuMgr * popmenu = NEW(mPopMenuMgr);
			_c(popmenu)->addItem(popmenu,0, "item1", NULL, 200, 0, NULL, 0);
			_c(popmenu)->addItem(popmenu,0, "item2", NULL, 201, 0, NULL, 0);
			_c(popmenu)->addItem(popmenu,0, "item3", NULL, 202, 0, NULL, 0);
			_c(popmenu)->addItem(popmenu,0, "item4", NULL, 203, 0, NULL, 0);
			_c(self)->setProperty(self, NCSP_MNUBTN_POPMENU,(DWORD)popmenu);
		}
		return TRUE;
	}
#endif

	if(id >= NCSP_MNUBTN_MAX)
		return FALSE;
	switch(id)
	{
	case NCSP_BUTTON_IMAGE_SIZE_PERCENT:
		{
			mPairPiece *pair = (mPairPiece*)(((mPushButtonPiece*)(self->body))->content);
			if(pair && pair->second)
			{
				if(value < 15)
					value = 15;
				else if(value > 85)
					value = 85;

				if(_c(pair->second)->setProperty(pair->second, NCSP_PAIRPIECE_FIRST_SIZE, value))
				{
					RECT rc;
					_c(pair->second)->getRect(pair->second, &rc);
					_c(pair->second)->setRect(pair->second, &rc);
					InvalidateRect(self->hwnd, NULL, TRUE);
					return TRUE;
				}
			}
			return FALSE;
		}
	case NCSP_BUTTON_ALIGN:
		if(self->body)
		{
			mPairPiece * pair = (mPairPiece*)(self->body);
			if(value == NCS_ALIGN_RIGHT)
				_c(HP(self->body))->setProperty(HP(self->body), NCSP_PAIRPIECE_SECOND_AS_FIRST, 0);
			else
				_c(HP(self->body))->setProperty(HP(self->body), NCSP_PAIRPIECE_SECOND_AS_FIRST, 1);
			_c(pair->second)->setProperty(pair->second,PIECECOMM_PROP_ALIGN, value);
		}
		return TRUE;
	case NCSP_MNUBTN_POPMENU:
		if(self->popmenu)
			_c(self->popmenu)->release(self->popmenu);
		self->popmenu = (mPopMenuMgr*)value;
		if(self->popmenu)
			_c(self->popmenu)->addRef(self->popmenu);
		update_cur_item(self, TRUE);
		return TRUE;
	case NCSP_MNUBTN_CURITEM:
		return set_cur_item(self, (int)value, TRUE);
	default:
		return Class(mButton).setProperty((mButton*)self, id, value);
	}
	return FALSE;
}

static DWORD mMenuButton_getProperty(mMenuButton *self, int id)
{
	if(id >= NCSP_MNUBTN_MAX)
		return (DWORD)-1;
	switch(id)
	{
	case NCSP_MNUBTN_POPMENU:
		return (DWORD)(self->popmenu);
	case NCSP_MNUBTN_CURITEM:
		return self->cur_item;
	case NCSP_BUTTON_IMAGE_SIZE_PERCENT:
		{
			mPairPiece *pair = (mPairPiece*)(((mPushButtonPiece*)(self->body))->content);
			return pair && pair->second ?
					_c(pair->second)->getProperty(pair->second, NCSP_PAIRPIECE_FIRST_SIZE):
					0;
		}
	}
	return Class(mButton).getProperty((mButton*)self, id);
}

static int mMenuButton_wndProc(mMenuButton *self, int message, WPARAM wParam, LPARAM lParam)
{
	if(message == MSG_COMMAND)
	{
		if(lParam == 0 && HIWORD(wParam) == 0) //menu
		{
			//reset the text
			set_cur_item(self, wParam, TRUE);
			ncsNotifyParentEx((mWidget*)self, NCSN_MNUBTN_ITEMCHANGED, wParam);
		}
	}
	else if(message == MSG_SETTEXT){
		return 0;
	}

	return Class(mButton).wndProc((mButton*)self, message, wParam, lParam);
}


BEGIN_CMPT_CLASS(mMenuButton, mButton)
	CLASS_METHOD_MAP(mMenuButton, construct    )
	CLASS_METHOD_MAP(mMenuButton, destroy      )
	CLASS_METHOD_MAP(mMenuButton, createBody   )
	CLASS_METHOD_MAP(mMenuButton, setProperty  )
	CLASS_METHOD_MAP(mMenuButton, getProperty  )
	CLASS_METHOD_MAP(mMenuButton, wndProc      )
	CLASS_METHOD_MAP(mMenuButton, createContent)
END_CMPT_CLASS

#endif //_MGNCSCTRL_MENUBUTTON
