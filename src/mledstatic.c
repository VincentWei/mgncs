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
    <http://www.minigui.com/en/about/licensing-policy/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSCTRL_LEDLABEL

static void mLEDLabel_construct(mLEDLabel * self, DWORD addData)
{
	Class(mStatic).construct((mStatic*)self, addData);
}

static mObject* mLEDLabel_createBody(mLEDLabel * self)
{
	return (mObject*)NEWPIECE(mLedstaticPiece);
}

static void mLEDLabel_onPaint(mLEDLabel *self, HDC hdc, const CLIPRGN *inv)
{
	if (Body)
		_c(Body)->paint(Body, hdc, (mObject*)self, GetWindowStyle(self->hwnd));
}

static BOOL mLEDLabel_setProperty(mLEDLabel *self, int id, DWORD value)
{
	if(id >= NCSP_LEDLBL_MAX)
		return FALSE;

	switch(id)
	{
		case NCSP_STATIC_ALIGN:
			if(Body && SetBodyProp(NCSP_LEDLBLPIECE_ALIGN, value))
			{
	            InvalidateRect(self->hwnd, NULL, TRUE);
				return TRUE;
			}
            return FALSE;

		case NCSP_STATIC_VALIGN:
			if(Body && SetBodyProp(NCSP_LEDLBLPIECE_VALIGN, value))
			{
	            InvalidateRect(self->hwnd, NULL, TRUE);
				return TRUE;
			}
            return FALSE;
		case NCSP_STATIC_AUTOWRAP:
			if(Body && SetBodyProp(NCSP_LEDLBLPIECE_AUTOWRAP, value))
			{
	            InvalidateRect(self->hwnd, NULL, TRUE);
				return TRUE;
			}
            return FALSE;
		case NCSP_LEDLBL_COLOR:
			if(Body && _c((mLedstaticPiece*)(self->body))->setProperty((mLedstaticPiece*)(self->body), NCSP_LEDLBLPIECE_COLOR, value))
			{
				InvalidateRect(self->hwnd, NULL, TRUE);
				return TRUE;
			}
			return FALSE;
		case NCSP_LEDLBL_WIDTH:
			if(Body && _c((mLedstaticPiece*)(self->body))->setProperty((mLedstaticPiece*)(self->body), NCSP_LEDLBLPIECE_WIDTH, value))
			{
				InvalidateRect(self->hwnd, NULL, TRUE);
				return TRUE;
			}
			return FALSE;
		case NCSP_LEDLBL_HEIGHT:
			if(Body && _c((mLedstaticPiece*)(self->body))->setProperty((mLedstaticPiece*)(self->body), NCSP_LEDLBLPIECE_HEIGHT, value))
			{
				InvalidateRect(self->hwnd, NULL, TRUE);
				return TRUE;
			}
			return FALSE;
		case NCSP_LEDLBL_GAP:
			if(Body && _c((mLedstaticPiece*)(self->body))->setProperty((mLedstaticPiece*)(self->body), NCSP_LEDLBLPIECE_GAP, value))
			{
				InvalidateRect(self->hwnd, NULL, TRUE);
				return TRUE;
			}
			return FALSE;
	}
	return Class(mStatic).setProperty((mStatic*)self, id, value);
}

static DWORD mLEDLabel_getProperty(mLEDLabel* self, int id)
{
	if(id >= NCSP_LEDLBL_MAX)
		return 0;

	switch(id)
	{
	case NCSP_LEDLBL_COLOR:
		return Body?GetBodyProp(NCSP_LEDLBLPIECE_COLOR):0;
	case NCSP_LEDLBL_WIDTH:
		return Body?GetBodyProp(NCSP_LEDLBLPIECE_WIDTH):0;
	case NCSP_LEDLBL_HEIGHT:
		return Body?GetBodyProp(NCSP_LEDLBLPIECE_HEIGHT):0;
	case NCSP_LEDLBL_GAP:
		return Body?GetBodyProp(NCSP_LEDLBLPIECE_GAP):0;
	}
	return Class(mStatic).getProperty((mStatic*)self, id);
}

/*
static LRESULT mLEDLabel_wndProc(mLEDLabel *self, UINT message, WPARAM wParam, LPARAM lParam)
{
	int str_len;

	switch(message)
	{
		case MSG_SETTEXT:
			if (lParam == 0L){
				_c((mLedstaticPiece*)(self->body))->setProperty((mLedstaticPiece*)(self->body), NCSP_LEDLBLPIECE_TEXT, lParam);
				return 0;
			}
			fprintf(stderr, "----- lparam = %s\n", lParam);
			str_len = strlen((const char*)lParam);
			_c((mLedstaticPiece*)(self->body))->setProperty((mLedstaticPiece*)(self->body), NCSP_LEDLBLPIECE_TEXT, lParam);
			return str_len;
		case MSG_GETTEXT:
			fprintf(stderr, " GETTEXT=========\n");
			break;
	}

	return Class(mStatic).wndProc((mStatic*)self, message, wParam, lParam);
}
*/

BEGIN_CMPT_CLASS(mLEDLabel, mStatic)
	CLASS_METHOD_MAP(mLEDLabel, construct)
	CLASS_METHOD_MAP(mLEDLabel, createBody)
	CLASS_METHOD_MAP(mLEDLabel, onPaint)
	CLASS_METHOD_MAP(mLEDLabel, setProperty)
	CLASS_METHOD_MAP(mLEDLabel, getProperty)
END_CMPT_CLASS
#if 0
static PLOGFONT plog_led;
static BOOL init_led_bmp_file(void);

static BOOL mLEDLabel_onCreate(mLEDLabel *self, LPARAM lParam)
{
	if(plog_led || init_led_bmp_file())
	{
		Class(mStatic).onCreate((mStatic*)self, lParam);
		SetWindowFont(self->hwnd,plog_led);
	}
	return TRUE;
}

/*static void mLEDLabel_construct(mLEDLabel *self, LPARAM lParam)
{
	if(!plog_led)
		init_led_bmp_file();

	Class(mStatic).construct((mStatic*)self,lParam);

	SetWindowFont(self->hwnd, plog_led);
}*/

static LRESULT mLEDLabel_wndProc(mLEDLabel *self, UINT message, WPARAM wParam, LPARAM lParam)
{
	//FIXED ME: by defalut, MSG_FONTCHANGED will call UpdateWindow, it cause the bug 3863
	// I think, UpdateWindow has bugs, but i don't known how to fixed it
	if(message == MSG_FONTCHANGED)
		return 0;
	return Class(mStatic).wndProc((mStatic*)self, message, wParam, lParam);
}

BEGIN_CMPT_CLASS(mLEDLabel, mStatic)
//	CLASS_METHOD_MAP(mLEDLabel, construct)
	CLASS_METHOD_MAP(mLEDLabel, onCreate)
	CLASS_METHOD_MAP(mLEDLabel, wndProc)
END_CMPT_CLASS


///////////////////////////////////////////////////////
//default static led file
#include "ledimages.c"


static DEVFONT * dev_led_font = NULL;

#define LOAD_LED_IMAGE(img) \
	LoadBitmapFromMem(HDC_SCREEN, &led_bmp_##img, _mgir_gif_##img##_data, sizeof(_mgir_gif_##img##_data), "gif")
#define DEC_IMG(img) \
	static BITMAP led_bmp_##img

DEC_IMG(sub);
DEC_IMG(add);
DEC_IMG(backslash);
DEC_IMG(digital);
DEC_IMG(gt);
DEC_IMG(lt);
DEC_IMG(upperalpha);
DEC_IMG(loweralpha);

static BOOL init_led_bmp_file()
{
	LOAD_LED_IMAGE(sub);
	LOAD_LED_IMAGE(add);
	LOAD_LED_IMAGE(backslash);
	LOAD_LED_IMAGE(digital);
	LOAD_LED_IMAGE(gt);
	LOAD_LED_IMAGE(lt);
	LOAD_LED_IMAGE(loweralpha);
	LOAD_LED_IMAGE(upperalpha);
	dev_led_font = CreateBMPDevFont("bmp-led-rrncnn-16-26-ISO8859-1", &led_bmp_digital, ".", 13, 16);
	AddGlyphsToBMPFont(dev_led_font, &led_bmp_loweralpha, "a", 26, 16);
	AddGlyphsToBMPFont(dev_led_font, &led_bmp_upperalpha, "A",26, 16);
	AddGlyphsToBMPFont(dev_led_font, &led_bmp_sub,  "-", 1, 16);
	AddGlyphsToBMPFont(dev_led_font, &led_bmp_add,  "+", 1, 16);
	AddGlyphsToBMPFont(dev_led_font, &led_bmp_backslash,  "\\", 1, 16);
	AddGlyphsToBMPFont(dev_led_font, &led_bmp_gt,  ">", 1, 16);
	AddGlyphsToBMPFont(dev_led_font, &led_bmp_lt,  "<", 1, 16);

	plog_led = CreateLogFont (FONT_TYPE_NAME_BITMAP_BMP, "led",
					"ISO8859-1",
					FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN,
					FONT_SETWIDTH_NORMAL, FONT_SPACING_CHARCELL,
					FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
					16, 0);

	return plog_led != NULL;
}
#endif
#if 0
static void AdjustPtPos(char ***tmpl, int width, int height, const RECT *rect, DWORD uflag)
{
}

static unsigned int GetCharMask(char c)
{

}

static BOOL DrawLedChar(HDC hdc, const Point **pts, unsigned int mask)
{
	int i;

	SetBrushColor(hdc, fg_color);

	for (i = 0; i < 16; i++)
	{
		if (char_mask & mask[i])
			FillPolygon(hdc, pts[i], vt_num[i]);
	}

	return true;
}

BOOL ncsLedDrawText(HDC hdc, char *str, int len, const RECT *rect, DWORD uflag, int width, int height)
{
	int i;

	Point **pts;
	unsigned int char_mask;

	pts = AdjustPtPos(led_vector_data, width, height, rect, uflag);

	for (i = 0; i < len; i++)
	{
		char_mask = GetCharMask(str[i]);
		DrawLedChar(hdc, pts, char_mask);
	}
}
#endif
#endif //_MGNCSCTRL_LEDLABEL
