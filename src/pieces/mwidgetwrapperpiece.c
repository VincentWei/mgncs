
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
#include "mrdr.h"

#include "mhotpiece.h"
#include "mwidgetwrapperpiece.h"

static void mWidgetWrapperPiece_construct(mWidgetWrapperPiece* self, DWORD add_data)
{
	Class(mHotPiece).construct((mHotPiece*)self, add_data);

	self->widget = (mObject*)add_data;
}

/** Should we delete the self->widget? No! widget would be delete
 * by its parent
static void mWidgetWrapperPiece_destroy(mWidgetWrapperPiece* self)
{
	DELETE(self->widget);
	Class(mHotPiece).destroy((mHotPiece*)self);
}*/

static BOOL mWidgetWrapperPiece_setRect(mWidgetWrapperPiece* self, const PRECT prc)
{
	if(!prc)
		return FALSE;

	if(self->widget)
	{
		mWidget * widget = (mWidget*)(self->widget);
		return MoveWindow(widget->hwnd, prc->left, prc->top, RECTWP(prc), RECTHP(prc), TRUE);
	}
	return FALSE;
}

static BOOL mWidgetWrapperPiece_getRect(mWidgetWrapperPiece* self, PRECT prc)
{
	if(!prc)
		return FALSE;

	if(self->widget)
	{
		mWidget * widget = (mWidget*)(self->widget);
		return GetWindowRect(widget->hwnd, prc);
	}
	return FALSE;
}

static BOOL mWidgetWrapperPiece_setRenderer(mWidgetWrapperPiece* self, const char* rdr_name)
{
	if(!rdr_name)
		return FALSE;

	if(self->widget){
		mWidget * widget = (mWidget*)(self->widget);
		widget->renderer = ncsRetriveCtrlRDR(rdr_name, _c(widget)->className);
		SetWindowElementRenderer(widget->hwnd, rdr_name, NULL);
		return widget->renderer != NULL;
	}
	return FALSE;
}

static BOOL mWidgetWrapperPiece_setProperty(mWidgetWrapperPiece* self, int id, DWORD value)
{
	if(self->widget)
	{
		return SendMessage(((mWidget*)(self->widget))->hwnd, MSG_SETPIECE_PROPERTY, (WPARAM)id, (LPARAM)value);
	}
	return FALSE;
}

static DWORD mWidgetWrapperPiece_getProperty(mWidgetWrapperPiece* self, int id)
{
	if(self->widget)
	{
		return SendMessage(((mWidget*)(self->widget))->hwnd, MSG_GETPIECE_PROPERTY, (WPARAM)id, 0);
	}
	return -1;
}



BEGIN_MINI_CLASS(mWidgetWrapperPiece, mHotPiece)
	CLASS_METHOD_MAP(mWidgetWrapperPiece, construct)
	CLASS_METHOD_MAP(mWidgetWrapperPiece, setRect)
	CLASS_METHOD_MAP(mWidgetWrapperPiece, getRect)
	CLASS_METHOD_MAP(mWidgetWrapperPiece, setRenderer)
	CLASS_METHOD_MAP(mWidgetWrapperPiece, setProperty)
	CLASS_METHOD_MAP(mWidgetWrapperPiece, getProperty)
END_MINI_CLASS


