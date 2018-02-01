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
    <http://www.minigui.com/en/about/licensing-policy/>.
 */

#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSCTRL_SLIDER

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
            if(Body && SetBodyProp(NCSP_SLIDERPIECE_POS, value))
	    {
    	        InvalidateRect(self->hwnd, NULL, TRUE);
        	return TRUE;
            }
            return FALSE;
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

#endif //_MGNCSCTRL_SLIDER
