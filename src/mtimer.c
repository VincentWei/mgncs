///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
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
    <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

static HWND mTimer_getParent(mTimer* self)
{
	mWidget *widget = SAFE_CAST(mWidget, self->parent);

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

static BOOL mTimer_timerProc(HWND hwnd, LINT id, DWORD total_count)
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
		return SetTimerEx(hwnd, (LINT)self, self->interval, (TIMERPROC)mTimer_timerProc);
	}

	return FALSE;
}

static void mTimer_stop(mTimer *self)
{
	HWND hwnd = _c(self)->getParent(self);

	KillTimer(hwnd, (LINT)self);
}

static BOOL mTimer_setProperty(mTimer* self, int id, DWORD value)
{
	if(id >= NCSP_TIMER_MAX)
		return FALSE;
	switch(id)
	{
	case NCSP_TIMER_INTERVAL:
	{
		HWND hwnd = NULL;
		BOOL isStart = FALSE;
		hwnd = _c(self)->getParent(self);
		if(HWND_NULL == hwnd)
			return FALSE;
		isStart = IsTimerInstalled(hwnd, (LINT)self);
		if(isStart){
			_M(self, stop);
			self->interval = value;
			return _M(self, start);
		} else {
			self->interval = value;
			return TRUE;
		}
	}
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


