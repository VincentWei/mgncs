
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
#include "mpanel.h"
#include "mmainwnd.h"
#include "piece.h"

#define WS_EX_MODALDISABLED     0x10000000L

static int close_horvering_captured_piece(mWidget*self, int flags)
{
	HWND hChild;
	mHotPiece *piece;
	if(flags == 0)
		return 0;
	
	if(flags & 1)
	{
		if((piece=(mHotPiece*)mWidget_getCapturedHotPiece(self)))
		{
			mWidget_releaseCapturedHotPiece();
			_c(piece)->processMessage(piece, MSG_MOUSEMOVEIN, FALSE, 0, (mObject*)self);
			flags &=~1;
		}
	}
	if(flags & 2)
	{
		if((piece=(mHotPiece*)mWidget_getHoveringFocus(self)))
		{
			mWidget_releaseHoveringFocus();
			_c(piece)->processMessage(piece, MSG_MOUSEMOVEIN, FALSE, 0, (mObject*)self);
			flags &=~2;
		}
	}
	if(flags & 4)
	{
		if((piece=(mHotPiece*)mWidget_getInputFocus(self)))
		{
			mWidget_releaseInputFocus();
			_c(piece)->processMessage(piece, MSG_MOUSEMOVEIN, FALSE, 0, (mObject*)self);
			flags &= ~4;
		}
	}

	if(flags == 0)
	{
		return 0;
	}

	hChild = GetNextChild(self->hwnd, HWND_NULL);
	
	while(hChild != HWND_INVALID && hChild != HWND_NULL)
	{
		mWidget* child = ncsObjFromHandle(hChild);
		if(child && child->hwnd == hChild)
		{
			if(close_horvering_captured_piece(child,flags) == 0)
				return 0;
		}
		hChild = GetNextChild(self->hwnd, hChild);
	}

	return flags;
}

static int mMainWnd_wndProc(mMainWnd *self, int message, WPARAM wParam, LPARAM lParam)
{
	switch(message){
	case MSG_SETTEXT:
	case MSG_SETCURSOR:
	case MSG_NCSETCURSOR:
		return DefaultMainWinProc(self->hwnd, message, wParam, lParam);

	case MSG_KILLFOCUS:
		close_horvering_captured_piece((mWidget*)self,3);	
		break;

	case MSG_CLOSE:
        ncsDestroyWindow((mWidget*)self, 0);
        return 0;
	}

	return Class(mPanel).wndProc((mPanel*)self, message, wParam, lParam);
}

static DWORD mMainWnd_doModal(mMainWnd*self, BOOL bAutoDestroy)
{
	MSG Msg;
	HWND hOwner, hFocus;
	BOOL isActive;
	DWORD ret = 0;
	BOOL bNoCleanOwner = FALSE;
	HWND hwnd;

    hFocus = GetNextDlgTabItem (self->hwnd, (HWND)0, FALSE);
    if (hFocus)
        SetFocus (hFocus);

	IncludeWindowStyle(self->hwnd, NCSS_MNWND_MODE);

    if (!(GetWindowExStyle(self->hwnd) & WS_EX_DLGHIDE))
        ShowWindow (self->hwnd, SW_SHOWNORMAL);

	hOwner = GetHosting(self->hwnd);

	if(hOwner && hOwner != HWND_DESKTOP){
		if(IsWindowEnabled(hOwner)){
			EnableWindow(hOwner, FALSE);
			IncludeWindowExStyle (hOwner, WS_EX_MODALDISABLED);
		}
		/*Throw the messages of hOwner*/
		 while (PeekPostMessage (&Msg, hOwner,
			 MSG_KEYDOWN, MSG_KEYUP, PM_REMOVE));
	}

	hwnd = self->hwnd; 
	/*
		VERY IMPORTANT: In windows, when user call DestroyMainWindow, the memory pointed by "self"
		would be free. and self's memory would be pad by unknown values. so we should save hwnd to
		avoid the self->hwnd changed
	*/

	while(GetMessage(&Msg, hwnd))
	{
		if(Msg.hwnd == hwnd)
		{
			if(Msg.message == MSG_MNWND_ENDDIALOG)
			{
				ret = Msg.lParam;
				break;
			}
			else if(Msg.message == MSG_NCDESTROY)
			{
				bAutoDestroy = TRUE;
				ret = 0;
				break;
			}
		}
		else 
		{
			if(Msg.message == MSG_NCDESTROY)
			{
				HWND hosting = hOwner;
				while(Msg.hwnd != hosting && hosting!=HWND_INVALID && hosting!=HWND_NULL && hosting != HWND_DESKTOP)
					hosting = GetHosting(hosting);
				if(IsWindow(hosting) && hosting != HWND_DESKTOP)
				{
					bAutoDestroy = TRUE;
					ret = 0;
					//reput the message destory
					PostMessage(Msg.hwnd, Msg.message, Msg.wParam, Msg.lParam);
					break;
				}
			}
		}

		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	if(self->hwnd != hwnd) //Maybe DestroyMainWindow is called
		return ret;

    isActive = (GetActiveWindow() == self->hwnd);

    if (!bNoCleanOwner &&  hOwner != HWND_DESKTOP && hOwner != HWND_NULL) {
		if (GetWindowExStyle (hOwner) & WS_EX_MODALDISABLED) {
			EnableWindow (hOwner, TRUE);
			ExcludeWindowExStyle (hOwner, WS_EX_MODALDISABLED);
			if (isActive && IsWindowVisible (hOwner)) {
				ShowWindow (hOwner, SW_SHOWNORMAL);
				SetActiveWindow (hOwner); 
			}    
		}    
	}

	if((GetWindowStyle(self->hwnd)&NCSS_MNWND_MODE) && bAutoDestroy)
	{
		HWND hwnd = self->hwnd;
		//destroy
		DestroyMainWindow(self->hwnd);
		MainWindowCleanup(hwnd);
	}
	else
	{
		ExcludeWindowStyle(self->hwnd, NCSS_MNWND_MODE);
	}
	
	return ret;
}



static BOOL mMainWnd_endDialog(mMainWnd* self, int code)
{
    SendNotifyMessage(self->hwnd, MSG_MNWND_ENDDIALOG, 0, (LPARAM)code);
	return TRUE; 
}

static BOOL mMainWnd_isMainObj(mMainWnd* self)
{
	return TRUE;
}

static mComponent * mMainWnd_setReleated(mMainWnd* self, mComponent* comp, int releated)
{
	mComponent* old = NULL;
	
	switch(releated)
	{
	case NCS_CMPT_PARENT:
		old = self->parentCmp;
		self->parentCmp = comp;
		break;
	case NCS_CMPT_NEXT:
		old = self->nextCmp;
		self->nextCmp = comp;
        if(comp)
    		_c(comp)->setReleated(comp, (mComponent*)self, NCS_CMPT_PREV);
		break;
	case NCS_CMPT_PREV:
		old = self->prevCmp;
		self->prevCmp = comp;
		break;
	default:
		return Class(mPanel).setReleated((mPanel*)self, comp, releated);
	}
	return old;
}

static mComponent* mMainWnd_getReleated(mMainWnd* self, int releated)
{
	switch(releated)
	{
	case NCS_CMPT_PARENT:
		return self->parentCmp;
	case NCS_CMPT_PREV:
		return self->prevCmp;
	case NCS_CMPT_NEXT:
		return self->nextCmp;
	}

	return Class(mPanel).getReleated((mPanel*)self, releated);
}

static void mMainWnd_destroy(mMainWnd* self)
{

	if(self->parentCmp)
	{
		//remove from parent
		_c(self->parentCmp)->removeChild(self->parentCmp, (mComponent*)self);	
	}
	return Class(mPanel).destroy((mPanel*)self);
}

BEGIN_CMPT_CLASS(mMainWnd, mPanel)
	CLASS_METHOD_MAP(mMainWnd, doModal)
	CLASS_METHOD_MAP(mMainWnd, destroy)
	CLASS_METHOD_MAP(mMainWnd, endDialog)
	CLASS_METHOD_MAP(mMainWnd, wndProc);
	CLASS_METHOD_MAP(mMainWnd, isMainObj);
	CLASS_METHOD_MAP(mMainWnd, setReleated)
	CLASS_METHOD_MAP(mMainWnd, getReleated)
END_CMPT_CLASS


mMainWndClass * gpMainWndClass = &Class(mMainWnd);

