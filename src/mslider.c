/*
**
** mslider.c
**
** wangjian<wangjian@minigui.prg>
**
** 2009-3-16.
**
*/

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
#include "mslider.h"
#include "mrdr.h"

#include "piece.h"

static BOOL mSlider_setProperty (mSlider* self, int id, DWORD value)
{
    if (id >= NCSP_SLIDER_MAX)
        return FALSE;

    switch (id)
    {
        case NCSP_SLIDER_MAXPOS: 
			if(Body && SetBodyProp(NCSP_SLIDERPIECE_MAX, value))
	        { 
    	        InvalidateRect(self->hwnd, NULL, TRUE);
            
        	    return TRUE;
        	}
			return FALSE;
        case NCSP_SLIDER_MINPOS:
			if(Body && SetBodyProp(NCSP_SLIDERPIECE_MIN, value))
	        { 
    	        InvalidateRect(self->hwnd, NULL, TRUE);
            
        	    return TRUE;
        	}
			return FALSE;
       
        case NCSP_SLIDER_CURPOS:
            {
                RECT rc, rcOld, rcNew;
                mHotPiece *thumbPiece = NULL;
                
                if(!Body)
                    return FALSE;
                thumbPiece = ((mSliderPiece *)self->body)->thumb;
                
                if (NULL == thumbPiece) 
                    return FALSE;
                _M(thumbPiece, getRect, &rcOld);

                SetBodyProp(NCSP_SLIDERPIECE_POS, value);
                
                _M(thumbPiece, getRect, &rcNew);
                GetBoundRect(&rc, &rcOld, &rcNew);
    	        InvalidateRect(self->hwnd, &rc, TRUE);
        	    
                return TRUE;
            }
        case NCSP_SLIDER_LINESTEP:
			if(Body)
				SetBodyProp(NCSP_SLIDERPIECE_LINESTEP, value);
			return TRUE;
        case NCSP_SLIDER_PAGESTEP:
			if(Body)
				SetBodyProp(NCSP_SLIDERPIECE_PAGESTEP, value);
			return TRUE;
    }

	return Class(mWidget).setProperty ((mWidget*)self, id, value);
}

static DWORD mSlider_getProperty (mSlider* self, int id)
{
	if (id >= NCSP_SLIDER_MAX)
		return 0;
	
	switch (id)
	{
        case NCSP_SLIDER_MAXPOS: 
            return (DWORD)(Body?GetBodyProp(NCSP_SLIDERPIECE_MAX):-1);
        case NCSP_SLIDER_MINPOS:
            return (DWORD)(Body?GetBodyProp(NCSP_SLIDERPIECE_MIN):-1);
        case NCSP_SLIDER_CURPOS:
            return (DWORD)(Body?GetBodyProp(NCSP_SLIDERPIECE_POS):-1);
        case NCSP_SLIDER_LINESTEP:
            return (DWORD)(Body?GetBodyProp(NCSP_SLIDERPIECE_LINESTEP):-1);
        case NCSP_SLIDER_PAGESTEP:
            return (DWORD)(Body?GetBodyProp(NCSP_SLIDERPIECE_PAGESTEP):-1);
    }

	return Class(mWidget).getProperty ((mWidget*)self, id);
}


BEGIN_CMPT_CLASS (mSlider, mWidget)	
	CLASS_METHOD_MAP (mSlider, setProperty  )
	CLASS_METHOD_MAP (mSlider, getProperty  )
	SET_DLGCODE(DLGC_WANTALLKEYS)
END_CMPT_CLASS

