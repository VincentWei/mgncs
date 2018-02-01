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
#include <minigui/fixedmath.h>

#include "mgncs.h"

#if _MGNCSCTRL_SCROLLBAR

#define SCROLLBAR_ARROW_SIZE  16
#define MIN_THUMB_SIZE 10

#define REACH_MIN 1
#define REACH_MAX 2

static mSliderPiece* get_slider_piece(mScrollBar *self)
{
	DWORD dwStyle;

	if(!Body)
		return NULL;

	dwStyle = GetWindowStyle(self->hwnd);

	if(dwStyle & NCSS_SCRLBR_LEFTDBLARROWS)
	{
		return (mSliderPiece*)_c(((mBoxLayoutPiece*)(Body)))->getCell((mBoxLayoutPiece*)Body, 2);
	}
	else if(dwStyle & NCSS_SCRLBR_RIGHTDBLARROWS)
	{
		return (mSliderPiece*)_c(((mBoxLayoutPiece*)(Body)))->getCell((mBoxLayoutPiece*)Body,
			(dwStyle&NCSS_SCRLBR_ARROWS)?1:0);
	}
	else if(dwStyle&NCSS_SCRLBR_ARROWS)
		return (mSliderPiece*)_c(((mBoxLayoutPiece*)(Body)))->getCell((mBoxLayoutPiece*)Body, 1);

	return (mSliderPiece*)Body;
}



static inline void set_scrollbtn_disable(mHotPiece * piece, mScrollBar*self, BOOL bdisable)
{
	BOOL piece_enable;

	//printf("piece=%p\n", piece);
	if(!piece)
		return ;

	piece_enable = _c(piece)->isEnabled(piece);

	//printf("piece_eanble=%d, disable=%d\n",piece_enable, bdisable);

	if( (piece_enable && !bdisable ) || (!piece_enable && bdisable))
		return ;

	//printf("---disable :%d\n",bdisable);

	_c(piece)->enable(piece, !bdisable);
	mHotPiece_update((mHotPiece*)piece, (mObject*)self, TRUE);
}

static void set_reach(mScrollBar *self, int reach)
{
	DWORD  dwStyle;
	mBoxLayoutPiece * boxlayout;
	if(!Body)
		return ;

	dwStyle = GetWindowStyle(self->hwnd);
	boxlayout = (mBoxLayoutPiece*)Body;

	if(dwStyle&NCSS_SCRLBR_LEFTDBLARROWS)
	{
		//[<][>][-------=-------][>] or [<][>][------=------]
		set_scrollbtn_disable(_c(boxlayout)->getCell(boxlayout, 0),self, reach==REACH_MIN?TRUE:FALSE);
		//printf("reach--max ,reach=%d\n",reach);
		set_scrollbtn_disable(_c(boxlayout)->getCell(boxlayout, 1),self, reach==REACH_MAX?TRUE:FALSE);
		if(dwStyle&NCSS_SCRLBR_ARROWS)
			set_scrollbtn_disable(_c(boxlayout)->getCell(boxlayout, 3),self, reach==REACH_MAX?TRUE:FALSE);
	}
	else if(dwStyle&NCSS_SCRLBR_RIGHTDBLARROWS)
	{
		//[<][------=------][<][>] or [-----=-------][<][>]
		int count = 3;
		if(dwStyle&NCSS_SCRLBR_ARROWS){
			count = 4;
			set_scrollbtn_disable(_c(boxlayout)->getCell(boxlayout, 0), self, reach==REACH_MIN?TRUE:FALSE);
		}

		set_scrollbtn_disable(_c(boxlayout)->getCell(boxlayout, count-2),self, reach==REACH_MIN?TRUE:FALSE);
		set_scrollbtn_disable(_c(boxlayout)->getCell(boxlayout, count-1),self, reach==REACH_MAX?TRUE:FALSE);
	}
	else if(dwStyle & NCSS_SCRLBR_ARROWS)
	{
		//[<][------=------][>]
		set_scrollbtn_disable(_c(boxlayout)->getCell(boxlayout, 0), self, reach==REACH_MIN?TRUE:FALSE);
		set_scrollbtn_disable(_c(boxlayout)->getCell(boxlayout, 2), self, reach==REACH_MAX?TRUE:FALSE);
	}
}

static BOOL mScrollBar_onPiece(mScrollBar *self, mHotPiece * sender, int event_id, DWORD param)
{
	int notif_id = 0;

	switch(event_id)
	{
	case NCSN_SLIDERPIECE_POSCHANGED:
		notif_id = NCSN_SCRLBR_CHANGED;
		set_reach(self, 0);
		break;
	case NCSN_SLIDERPIECE_REACHMAX:
		notif_id = NCSN_SCRLBR_REACHMAX;
		set_reach(self, REACH_MAX);
		break;
	case NCSN_SLIDERPIECE_REACHMIN:
		notif_id = NCSN_SCRLBR_REACHMIN;
		set_reach(self, REACH_MIN);
		break;
	default:
		return TRUE;
	}

	if(notif_id > 0)
		ncsNotifyParent((mWidget*)self, notif_id);

	return FALSE;
}

static BOOL mScrollBar_onLeftPiece(mScrollBar *self, mHotPiece *sender, int event_id, DWORD param)
{
	mSliderPiece* slider = get_slider_piece(self);
	if(slider)
	{
		RECT rc;
		_c(slider)->stepLine(slider, TRUE);
		_c(slider)->getRect(slider, &rc);
		InvalidateRect(self->hwnd, &rc, TRUE);
	}
	return FALSE;
}

static BOOL mScrollBar_onRightPiece(mScrollBar *self, mHotPiece *sender, int event_id, DWORD param)
{
	mSliderPiece* slider = get_slider_piece(self);
	if(slider)
	{
		RECT rc;
		_c(slider)->stepLine(slider, FALSE);
		_c(slider)->getRect(slider, &rc);
		InvalidateRect(self->hwnd, &rc, TRUE);
	}
	return FALSE;
}

static mObject * mScrollBar_createBody(mScrollBar *self)
{
	int  count = 2;
	int left_type;
	int right_type;
    RECT rc;

	int event_ids[] = {
		NCSN_SLIDERPIECE_POSCHANGED,
		NCSN_SLIDERPIECE_REACHMAX,
		NCSN_SLIDERPIECE_REACHMIN,
		0
	};
	DWORD dwStyle = GetWindowStyle(self->hwnd);

	mBoxLayoutPiece * boxlayout = NULL;
	mBoxLayoutPieceClass * boxlayout_cls = NULL;

	mSliderPiece * sliderPiece = NEWPIECE(mSliderPiece);

	sliderPiece->body = (mHotPiece*)NEWPIECE(mScrollBarPiece);
	sliderPiece->thumb = (mHotPiece*)NEWPIECE(mScrollThumbBoxPiece);

	if(dwStyle & NCSS_SCRLBR_VERTICAL)
	{
		boxlayout_cls = (mBoxLayoutPieceClass*)(void *)&Class(mVBoxLayoutPiece);
		left_type = NCS_ARROWPIECE_UP;
		right_type = NCS_ARROWPIECE_DOWN;
		_c(sliderPiece)->setProperty(sliderPiece, NCSP_SLIDERPIECE_DIRECTION,1);
	}
	else
	{
		boxlayout_cls = (mBoxLayoutPieceClass*)(void *)&Class(mHBoxLayoutPiece);
		left_type = NCS_ARROWPIECE_LEFT;
		right_type = NCS_ARROWPIECE_RIGHT;
	}

	if(dwStyle & NCSS_SCRLBR_ARROWS)
	{
		count = 3;
	}

	if(dwStyle & NCSS_SCRLBR_LEFTDBLARROWS)
	{
		count += 1;
		// [<][>][----=------][>] or [<][>][------=------]
		boxlayout = (mBoxLayoutPiece*)ncsNewPiece((mHotPieceClass*)boxlayout_cls, count);
		_c(boxlayout)->setCellInfo(boxlayout, 0, SCROLLBAR_ARROW_SIZE, 0, 0);
		_c(boxlayout)->setCell(boxlayout, 0, (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, left_type));
		ncsAddEventListener((mObject*)_c(boxlayout)->getCell(boxlayout, 0),
                (mObject*)self,
                (NCS_CB_ONPIECEEVENT)mScrollBar_onLeftPiece,
                NCSN_ABP_PUSHED);

		_c(boxlayout)->setCellInfo(boxlayout, 1, SCROLLBAR_ARROW_SIZE, 0, 0);
		_c(boxlayout)->setCell(boxlayout, 1, (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, right_type));
		ncsAddEventListener((mObject*)_c(boxlayout)->getCell(boxlayout, 1),
                (mObject*)self,
                (NCS_CB_ONPIECEEVENT)mScrollBar_onRightPiece,
                NCSN_ABP_PUSHED);

		_c(boxlayout)->setCell(boxlayout, 2, (mHotPiece*)sliderPiece);

		if(count > 3)
		{
			_c(boxlayout)->setCellInfo(boxlayout, 3, SCROLLBAR_ARROW_SIZE, 0, 0);
			_c(boxlayout)->setCell(boxlayout, 3, (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, right_type));
			ncsAddEventListener((mObject*)_c(boxlayout)->getCell(boxlayout, 3),
                    (mObject*)self,
                    (NCS_CB_ONPIECEEVENT)mScrollBar_onRightPiece,
                    NCSN_ABP_PUSHED);
		}
	}
	else if(dwStyle & NCSS_SCRLBR_RIGHTDBLARROWS)
	{
		count += 1;
		// [<][----=------][<][>] or [-----=-----][<][>]
		boxlayout = (mBoxLayoutPiece*)ncsNewPiece((mHotPieceClass*)boxlayout_cls, count);
		_c(boxlayout)->setCellInfo(boxlayout, count-2, SCROLLBAR_ARROW_SIZE, 0, 0);
		_c(boxlayout)->setCell(boxlayout, count-2, (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, left_type));
		ncsAddEventListener((mObject*)_c(boxlayout)->getCell(boxlayout, count-2),
                (mObject*)self,
                (NCS_CB_ONPIECEEVENT)mScrollBar_onLeftPiece,
                NCSN_ABP_PUSHED);

		_c(boxlayout)->setCellInfo(boxlayout, count-1, SCROLLBAR_ARROW_SIZE, 0, 0);
		_c(boxlayout)->setCell(boxlayout, count-1, (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, right_type));
		ncsAddEventListener((mObject*)_c(boxlayout)->getCell(boxlayout, count-1),
                (mObject*)self,
                (NCS_CB_ONPIECEEVENT)mScrollBar_onRightPiece,
                NCSN_ABP_PUSHED);

		_c(boxlayout)->setCell(boxlayout, count-3, (mHotPiece*)sliderPiece);

		if(count>3)
		{
			_c(boxlayout)->setCellInfo(boxlayout, 0, SCROLLBAR_ARROW_SIZE, 0, 0);
			_c(boxlayout)->setCell(boxlayout, 0, (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, left_type));
			ncsAddEventListener((mObject*)_c(boxlayout)->getCell(boxlayout, 0),
                    (mObject*)self,
                    (NCS_CB_ONPIECEEVENT)mScrollBar_onLeftPiece,
                    NCSN_ABP_PUSHED);
		}

	}
	else
	{
		if(count >= 3)
		{
			//[<][----=----][>]
			boxlayout = (mBoxLayoutPiece*)ncsNewPiece((mHotPieceClass*)boxlayout_cls, count);
			_c(boxlayout)->setCellInfo(boxlayout, 0, SCROLLBAR_ARROW_SIZE, 0, 0);
			_c(boxlayout)->setCell(boxlayout, 0, (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, left_type));
			ncsAddEventListener((mObject*)_c(boxlayout)->getCell(boxlayout, 0),
                    (mObject*)self,
                    (NCS_CB_ONPIECEEVENT)mScrollBar_onLeftPiece,
                    NCSN_ABP_PUSHED);
			_c(boxlayout)->setCellInfo(boxlayout, 2, SCROLLBAR_ARROW_SIZE, 0, 0);
			_c(boxlayout)->setCell(boxlayout, 2, (mHotPiece*)NEWPIECEEX(mArrowButtonPiece, right_type));
			ncsAddEventListener((mObject*)_c(boxlayout)->getCell(boxlayout, 2),
                    (mObject*)self,
                    (NCS_CB_ONPIECEEVENT)mScrollBar_onRightPiece,
                    NCSN_ABP_PUSHED);
			_c(boxlayout)->setCell(boxlayout, 1, (mHotPiece*)sliderPiece);
		}
		else
		{
			//[-----=-----]
			//unneed use boxlayout
		}
	}

	ncsAddEventListeners( (mObject*)sliderPiece,
            (mObject*)self,(NCS_CB_ONPIECEEVENT)mScrollBar_onPiece, event_ids);

    GetClientRect(self->hwnd, &rc);
    _c(sliderPiece)->setRect(sliderPiece, &rc);
    _c(sliderPiece)->setProperty(sliderPiece, NCSP_SLIDERPIECE_POS, sliderPiece->cur_pos);

    if (boxlayout)
        return (mObject*)boxlayout;

    return (mObject*)sliderPiece;
}

static void scrollbar_recalc(mScrollBar *self)
{
	RECT rc;
	GetClientRect(self->hwnd, &rc);
	_c(self)->onSizeChanged(self, &rc);
}

static BOOL mScrollBar_setProperty (mScrollBar* self, int id, DWORD value)
{
	mHotPiece *slider;
    if (id >= NCSP_SCRLBR_MAX)
        return FALSE;

	slider = (mHotPiece*)get_slider_piece(self);

	if(!slider)
		return FALSE;

	switch(id)
	{
	case NCSP_SCRLBR_MAXPOS:
		if(_c(slider)->setProperty(slider,NCSP_SLIDERPIECE_MAX,value))
		{
			scrollbar_recalc(self);
			mHotPiece_update((mHotPiece*)slider, (mObject*)self, TRUE);
			return TRUE;
		}
		return FALSE;
	case NCSP_SCRLBR_MINPOS:
		if(_c(slider)->setProperty(slider,NCSP_SLIDERPIECE_MIN,value))
		{
			scrollbar_recalc(self);
			mHotPiece_update((mHotPiece*)slider, (mObject*)self, TRUE);
			return TRUE;
		}
		return FALSE;
	case NCSP_SCRLBR_CURPOS:
		if(_c(slider)->setProperty(slider,NCSP_SLIDERPIECE_POS,value))
		{
			scrollbar_recalc(self);
			mHotPiece_update((mHotPiece*)slider, (mObject*)self, TRUE);
			return TRUE;
		}
		return FALSE;
	case NCSP_SCRLBR_LINESTEP:
		if(_c(slider)->setProperty(slider,NCSP_SLIDERPIECE_LINESTEP,value))
		{
			scrollbar_recalc(self);
			mHotPiece_update((mHotPiece*)slider, (mObject*)self, TRUE);
			return TRUE;
		}
		return FALSE;
	case NCSP_SCRLBR_PAGESTEP:
		if(_c(slider)->setProperty(slider,NCSP_SLIDERPIECE_PAGESTEP,value))
		{
			scrollbar_recalc(self);
			mHotPiece_update((mHotPiece*)slider, (mObject*)self, TRUE);
			return TRUE;
		}
		return FALSE;
	}


	return Class(mSlider).setProperty ((mSlider*)self, id, value);
}

static DWORD mScrollBar_getProperty (mScrollBar* self, int id)
{
	mSliderPiece * slider;
	if (id >= NCSP_SCRLBR_MAX)
		return 0;

	slider = get_slider_piece(self);

	if(!slider)
		return FALSE;

	switch(id)
	{
	case NCSP_SCRLBR_MAXPOS:
		return _c(slider)->getProperty(slider,NCSP_SLIDERPIECE_MAX);
	case NCSP_SCRLBR_MINPOS:
		return _c(slider)->getProperty(slider,NCSP_SLIDERPIECE_MIN);
	case NCSP_SCRLBR_CURPOS:
		return _c(slider)->getProperty(slider,NCSP_SLIDERPIECE_POS);
	case NCSP_SCRLBR_LINESTEP:
		return _c(slider)->getProperty(slider,NCSP_SLIDERPIECE_LINESTEP);
	case NCSP_SCRLBR_PAGESTEP:
		return _c(slider)->getProperty(slider,NCSP_SLIDERPIECE_PAGESTEP);
	}

	return Class(mSlider).getProperty ((mSlider*)self, id);
}


static LRESULT mScrollBar_wndProc(mScrollBar *self, UINT message, WPARAM wParam, LPARAM lParam)
{

	if(message >= MSG_FIRSTKEYMSG && message <= MSG_LASTKEYMSG)
	{
		mSliderPiece * slider = get_slider_piece(self);
		if(slider)
			return _c(slider)->processMessage(slider, message, wParam, lParam, (mObject*)self);
	}

	return Class(mSlider).wndProc((mSlider*)self, message, wParam, lParam);
}

static int mScrollBar_onSizeChanged(mWidget *self, RECT *rtClient)
{
	if(Body)
	{
		mSliderPiece * slider = get_slider_piece((mScrollBar*)self);
		if(slider && slider->thumb)
		{
			RECT rc_slider;
			RECT rc_thumb;
			int thumb_size;

			if(slider->min > slider->max)
				return 0;

			_c(slider)->getRect(slider, &rc_slider);
			_c(slider->thumb)->getRect(slider->thumb, &rc_thumb);

            //special processing
            if (slider->min == slider->max) {
                if(GetWindowStyle(self->hwnd)&NCSS_SCRLBR_VERTICAL) {
                    rc_thumb.left = rc_slider.left + 1;
                    rc_thumb.right = rc_slider.right - 1;
                    rc_thumb.top = rc_slider.top;
                    rc_thumb.bottom = rc_slider.bottom;
                }
                else {
                    rc_thumb.left = rc_slider.left;
                    rc_thumb.right = rc_slider.right;
                    rc_thumb.top = rc_slider.top + 1;
                    rc_thumb.bottom = rc_slider.bottom - 1;
                }
            }
            else {
                if(GetWindowStyle(self->hwnd)&NCSS_SCRLBR_VERTICAL)
                {
                    //calc size of
                    thumb_size = ((RECTH(rc_slider) * slider->page_step <<8) / (slider->max - slider->min))>>8;
                    if(thumb_size < MIN_THUMB_SIZE)
                        thumb_size = MIN_THUMB_SIZE;
                    rc_thumb.left = rc_slider.left + 1;
                    rc_thumb.right = rc_slider.right - 1;
                    rc_thumb.top = (rc_thumb.top+rc_thumb.bottom)/2 + thumb_size;
                    rc_thumb.bottom = rc_thumb.top + thumb_size;

                }
                else
                {
                    thumb_size = ((RECTW(rc_slider) * slider->page_step <<8) / (slider->max - slider->min))>>8;
                    if(thumb_size < MIN_THUMB_SIZE)
                        thumb_size = MIN_THUMB_SIZE;
                    rc_thumb.top = rc_slider.top +1;
                    rc_thumb.bottom = rc_slider.bottom -1;
                    rc_thumb.left = (rc_thumb.left+rc_thumb.right)/2 + thumb_size;
                    rc_thumb.right = rc_thumb.left + thumb_size;
                }
            }
			_c(slider->thumb)->setRect(slider->thumb, &rc_thumb);
		}
		Class(mSlider).onSizeChanged((mSlider*)self, rtClient);
	}
	return 0;
}

#ifdef _MGNCS_GUIBUILDER_SUPPORT
static BOOL mScrollBar_refresh(mScrollBar* self)
{
	if(Class(mSlider).refresh((mSlider*)self))
	{
		scrollbar_recalc(self);
		return TRUE;
	}
	return FALSE;
}
#endif

BEGIN_CMPT_CLASS (mScrollBar, mSlider)
    CLASS_METHOD_MAP (mScrollBar, createBody )
	CLASS_METHOD_MAP (mScrollBar, setProperty)
	CLASS_METHOD_MAP (mScrollBar, getProperty)
	CLASS_METHOD_MAP (mScrollBar, wndProc    )
	CLASS_METHOD_MAP (mScrollBar, onSizeChanged)
#ifdef _MGNCS_GUIBUILDER_SUPPORT
	CLASS_METHOD_MAP (mScrollBar, refresh)
#endif
END_CMPT_CLASS

#endif //_MGNCSCTRL_SCROLLBAR
