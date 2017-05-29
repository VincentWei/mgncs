
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
#include "mstatic.h"
#include "mrdr.h"
#include "piece.h"

/*static BOOL mStatic_onCreate(mStatic *self, LPARAM lParam)
{
	printf("-- static oncreate=%p\n", self->body);
	if(self->body)
	{
		SetBodyProp(NCSP_LABELPIECE_LABEL, 
			(DWORD)GetWindowCaption(self->hwnd));
		InvalidateRect(self->hwnd, NULL, TRUE);
	}
	return TRUE;
}*/

static void mStatic_onLButtonDown(mStatic *self)
{
    ncsNotifyParent((mWidget*)self, NCSN_WIDGET_CLICKED);
}

static mObject* mStatic_createBody(mStatic *self)
{
	return (mObject*)NEWPIECE(mLabelPiece);

}

static BOOL mStatic_setProperty(mStatic *self, int id, DWORD value)
{
	if(id >= NCSP_STATIC_MAX)
		return FALSE;
	
	switch(id)
    {
        case NCSP_STATIC_ALIGN:
			if(Body && SetBodyProp(NCSP_LABELPIECE_ALIGN, value))
			{
	            InvalidateRect(self->hwnd, NULL, TRUE);
				return TRUE;
			}
            return FALSE;
        case NCSP_STATIC_VALIGN:
			if(Body&& SetBodyProp(NCSP_LABELPIECE_VALIGN, value)){
    	        InvalidateRect(self->hwnd, NULL, TRUE);
				return TRUE;
			}
            return FALSE;
        case NCSP_STATIC_AUTOWRAP:
			if(Body && SetBodyProp(NCSP_LABELPIECE_AUTOWRAP, value))
			{
            	InvalidateRect(self->hwnd, NULL, TRUE);
				return TRUE;
			}
            return FALSE;
    }

	return Class(mWidget).setProperty((mWidget*)self, id, value);
}

static DWORD mStatic_getProperty(mStatic *self, int id)
{
	if(id >= NCSP_STATIC_MAX)
		return 0;
	
	switch(id)
    {
        case NCSP_STATIC_ALIGN:
			return Body?GetBodyProp(NCSP_LABELPIECE_ALIGN):0;
        case NCSP_STATIC_VALIGN:
			return Body?GetBodyProp(NCSP_LABELPIECE_VALIGN):0;
        case NCSP_STATIC_AUTOWRAP:
			return Body?GetBodyProp(NCSP_LABELPIECE_AUTOWRAP):0;
    }
	
	return Class(mWidget).getProperty((mWidget*)self, id);
}

static int mStatic_wndProc(mStatic* self, int message, WPARAM wParam, LPARAM lParam)
{	
    switch(message){
        case MSG_LBUTTONDBLCLK:
            ncsNotifyParent((mWidget *)self, NCSN_WIDGET_DBCLICKED);
			break;
        case MSG_FONTCHANGED:
            InvalidateRect (self->hwnd, NULL, TRUE);
            return 0;
            
  /*      case MSG_SETTEXT:
            SetWindowCaption (self->hwnd, (const char*)lParam);
			if(self->body)
			{
				SetBodyProp(NCSP_LABELPIECE_LABEL, 
					(DWORD)GetWindowCaption(self->hwnd));
				InvalidateRect(self->hwnd, NULL, TRUE);
			}
            return 0;
    */    default:
            break;
    }
	
	return Class(mWidget).wndProc((mWidget*)self, message, wParam, lParam);
}

BEGIN_CMPT_CLASS(mStatic, mWidget)
	CLASS_METHOD_MAP(mStatic, createBody)
//	CLASS_METHOD_MAP(mStatic, onCreate)
	CLASS_METHOD_MAP(mStatic, setProperty)
	CLASS_METHOD_MAP(mStatic, getProperty)
	CLASS_METHOD_MAP(mStatic, onLButtonDown)
	CLASS_METHOD_MAP(mStatic, wndProc)
END_CMPT_CLASS


