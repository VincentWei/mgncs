#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "mhotpiece.h"
#include "mstaticpiece.h"
#include "mrectpiece.h"

static void mRectPiece_construct(mRectPiece *self, DWORD add_data)
{
	Class(mHotPiece).construct((mHotPiece*)self, add_data);
	self->fill_color   = MakeRGBA(255,255,255,255);
	self->border_color = MakeRGBA(0,0,0,255);
	self->border_size  = 1;
	self->x_radius     = 0;
	self->y_radius     = 0;
}

static void mRectPiece_paint(mRectPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
    RECT rc;
    int oldsize, i;
    gal_pixel pen_color, brush_color = 0L;

    _c(self)->getRect(self, &rc);

    if(GetAValue(self->fill_color) != 0) //Not Transparent
    {
        brush_color = SetBrushColor(hdc, 
                ncsColor2Pixel(hdc, self->fill_color));
    }
    pen_color = SetPenColor(hdc, ncsColor2Pixel(hdc, self->border_color));

    if(self->x_radius > 0 && self->y_radius > 0) // round rectangle 
    {
        int rx, ry;
        int _border = self->border_size;
        rx = self->x_radius;
        ry = self->y_radius;

        if(rx > ((self->right-self->left)>>1) - self->border_size)
            rx = ((self->right-self->left)>>1) - self->border_size;
        if(ry > ((self->bottom-self->top)>>1) - self->border_size)
            ry = ((self->bottom-self->top)>>1) - self->border_size;

        oldsize = SetPenWidth(hdc, self->border_size);

		_border >>= 1;

        RoundRect(hdc, rc.left + _border, rc.top + _border, 
                rc.right - (_border==0?1:_border), rc.bottom - (_border==0?1:_border), rx, ry);

        SetPenWidth(hdc, oldsize);
    } 
    else //not round rectangle 
    {
        FillBox(hdc, rc.left , rc.top , RECTW(rc), RECTH(rc));

        for (i = 0; i < self->border_size; i++)
            Rectangle(hdc, rc.left+i, rc.top+i, rc.right-1-i, rc.bottom-1-i);
    }

    if(brush_color != 0L) //Not Transparent
    {
        SetBrushColor(hdc, brush_color);
    }

    SetPenColor(hdc, pen_color);
}

static BOOL mRectPiece_setProperty(mRectPiece *self, int id, DWORD value)
{
	switch(id)
	{
	case NCSP_RECTPIECE_FILLCOLOR:
		self->fill_color = value;
		break;
	case NCSP_RECTPIECE_BORDERCOLOR:
		self->border_color = value;
		break;
	case NCSP_RECTPIECE_BORDERSIZE:
    {
        int bz = (int)value;
		if ((self->right > self->left && self->bottom > self->top) 
        	&& ((self->x_radius << 1) + bz > (self->right - self->left)
                || (self->y_radius << 1) + bz > (self->bottom - self->top)))
            return FALSE;
		self->border_size = bz;
		break;
    }
	case NCSP_RECTPIECE_XRADIUS:
    {
        int rx = (int)value;
        /*if ((rx << 1) + self->border_size > self->right - self->left)
            return FALSE;*/
		self->x_radius = rx;
		break;
    }
	case NCSP_RECTPIECE_YRADIUS:
    {
        int ry = (int)value;
        //if ((ry << 1) + self->border_size > self->bottom - self->top)
        //    return FALSE;
		self->y_radius = ry;
		break;
    }
	case NCSP_RECTPIECE_FILLCLR_RED:
		{
			DWORD clr = self->fill_color;
			self->fill_color = MakeRGBA((unsigned char)value,
				GetGValue(clr),
				GetBValue(clr),
				GetAValue(clr));
		}
		break;
	case NCSP_RECTPIECE_FILLCLR_GREEN:
		{
			DWORD clr = self->fill_color;
			self->fill_color = MakeRGBA(
				GetRValue(clr),
				(unsigned char)value,
				GetBValue(clr),
				GetAValue(clr));
		}
		break;
	case NCSP_RECTPIECE_FILLCLR_BLUE:
		{
			DWORD clr = self->fill_color;
			self->fill_color = MakeRGBA(
				GetRValue(clr),
				GetGValue(clr),
				(unsigned char)value,
				GetAValue(clr));
		}
		break;
	case NCSP_RECTPIECE_FILLCLR_ALPHA:
		{
			DWORD clr = self->fill_color;
			self->fill_color = MakeRGBA(
				GetRValue(clr),
				GetGValue(clr),
				GetBValue(clr),
				(unsigned char)value);
		}
		break;
	case NCSP_RECTPIECE_BRDCLR_RED:
		{
			DWORD clr = self->border_color;
			self->border_color = MakeRGBA((unsigned char)value,
				GetGValue(self->border_color),
				GetBValue(clr),
				GetAValue(clr));
		}
		break;
	case NCSP_RECTPIECE_BRDCLR_GREEN:
		{
			DWORD clr = self->border_color;
			self->border_color = MakeRGBA(
				GetRValue(clr),
				(unsigned char)value,
				GetBValue(clr),
				GetAValue(clr));
		}
		break;
	case NCSP_RECTPIECE_BRDCLR_BLUE:
		{
			DWORD clr = self->border_color;
			self->border_color = MakeRGBA(
				GetRValue(clr),
				GetGValue(clr),
				(unsigned char)value,
				GetAValue(clr));
		}
		break;
	case NCSP_RECTPIECE_BRDCLR_ALPHA:
		{
			DWORD clr = self->border_color;
			self->border_color = MakeRGBA(
				GetRValue(clr),
				GetGValue(clr),
				GetBValue(clr),
				(unsigned char)value);
		}
		break;

	default:
		return Class(mStaticPiece).setProperty((mStaticPiece*)self, id, value);
	}
	return TRUE;
}

static DWORD mRectPiece_getProperty(mRectPiece* self, int id)
{
	switch(id)
	{
	case NCSP_RECTPIECE_FILLCOLOR:
		return self->fill_color;
	case NCSP_RECTPIECE_BORDERCOLOR:
		return self->border_color;
	case NCSP_RECTPIECE_BORDERSIZE:
		return self->border_size;
	case NCSP_RECTPIECE_XRADIUS:
		return self->x_radius;
	case NCSP_RECTPIECE_YRADIUS:
		return self->y_radius;
	case NCSP_RECTPIECE_FILLCLR_RED:
		return GetRValue(self->fill_color);
	case NCSP_RECTPIECE_FILLCLR_GREEN:
		return GetGValue(self->fill_color);
	case NCSP_RECTPIECE_FILLCLR_BLUE:
		return GetBValue(self->fill_color);
	case NCSP_RECTPIECE_FILLCLR_ALPHA:
		return GetAValue(self->fill_color);

	case NCSP_RECTPIECE_BRDCLR_RED:
		return GetRValue(self->border_color);
	case NCSP_RECTPIECE_BRDCLR_GREEN:
		return GetGValue(self->border_color);
	case NCSP_RECTPIECE_BRDCLR_BLUE:
		return GetBValue(self->border_color);
	case NCSP_RECTPIECE_BRDCLR_ALPHA:
		return GetAValue(self->border_color);

	}

	return Class(mStaticPiece).getProperty((mStaticPiece*)self, id);
}

BEGIN_MINI_CLASS(mRectPiece, mStaticPiece)
	CLASS_METHOD_MAP(mRectPiece, construct)
	CLASS_METHOD_MAP(mRectPiece, paint)
	CLASS_METHOD_MAP(mRectPiece, setProperty)
	CLASS_METHOD_MAP(mRectPiece, getProperty)
END_MINI_CLASS

