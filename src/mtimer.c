
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
#include "mcomponent.h"
#include "mwidget.h"
#include "minvisible-component.h"
#include "mtimer.h"

static HWND mTimer_getParent(mTimer* self)
{
	mWidget * widget = SAFE_CAST(mWidget,self->parent);

	if(widget)
		return widget->hwnd;
	return HWND_INVALID;
}

static void mTimer_construct(mTimer *self, DWORD addData)
{
	self->interval = 50;
	Class(mInvsbComp).construct((mInvsbComp*)self, addData);
}

static void mTimer_destroy(mTimer *self)
{
	if(self)
	{
		_c(self)->stop(self);
		Class(mInvsbComp).destroy((mInvsbComp*)self);
	}
}

static BOOL mTimer_timerProc(HWND hwnd, int id, DWORD total_count)
{
	mTimer * timer = (mTimer*)id;

	if(timer)
	{
		NCS_CB_ONTIMER onTimer = (NCS_CB_ONTIMER)ncsGetComponentHandler((mComponent*)timer, MSG_TIMER);
		if(onTimer)
		{
			return onTimer(timer, total_count);
		}
		else
		{
			//raise event
			ncsRaiseEvent((mObject*)timer, MSG_TIMER, total_count);
			return TRUE;
		}
	}

	return FALSE;
}

static BOOL mTimer_start(mTimer *self)
{
	HWND hwnd = _c(self)->getParent(self);

	if(IsWindow(hwnd))
	{
		return SetTimerEx(hwnd, (int)self, self->interval, (TIMERPROC)mTimer_timerProc);
	}

	return FALSE;
}

static void mTimer_stop(mTimer *self)
{
	HWND hwnd = _c(self)->getParent(self);

	KillTimer(hwnd, (int)self);
}

static BOOL mTimer_setProperty(mTimer* self, int id, DWORD value)
{
	if(id >= NCSP_TIMER_MAX)
		return FALSE;

	switch(id)
	{
	case NCSP_TIMER_INTERVAL:
		self->interval = value;
		return TRUE;
	}

	return Class(mInvsbComp).setProperty((mInvsbComp*)self, id, value);
}

static DWORD mTimer_getProperty(mTimer *self, int id)
{
	if(id >= NCSP_TIMER_MAX)
		return (DWORD) -1;
	
	switch(id)
	{
	case NCSP_TIMER_INTERVAL:
		return self->interval;
	}

	return Class(mInvsbComp).getProperty((mInvsbComp*)self, id);
}

BEGIN_CMPT_CLASS(mTimer, mInvsbComp)
	CLASS_METHOD_MAP(mTimer, construct)
	CLASS_METHOD_MAP(mTimer, destroy)
	CLASS_METHOD_MAP(mTimer, start)
	CLASS_METHOD_MAP(mTimer, stop)
	CLASS_METHOD_MAP(mTimer, getParent)
	CLASS_METHOD_MAP(mTimer, setProperty)
	CLASS_METHOD_MAP(mTimer, getProperty)
END_CMPT_CLASS


