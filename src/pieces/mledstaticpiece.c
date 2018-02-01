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
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/fixedmath.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "mhotpiece.h"
#include "mstaticpiece.h"
#include "mlabelpiece.h"
#include "mledstaticpiece.h"

#ifdef _MGNCSCTRL_LEDLABEL

typedef struct _led_size_info{
	int width;
	int height;
	int w_border;
	int h_border;
}led_size_info;

typedef int (*fill_led_point)(led_size_info *pled_info, int x, int y, POINT *pt);

#define SET_LED_POS(pt, _x,_y) do{ \
	(pt).x = (_x);                 \
	(pt).y = (_y);                 \
}while(0)
static inline int led_max(int a, int b) {
	return a > b ? a : b;
}
static inline int led_min(int a, int b) {
	return a < b ? a : b;
}
//draw top led
static int fill_top_led_point(led_size_info *pled_info, int x, int y, POINT *pt)
{
	SET_LED_POS(pt[0], x + 1, y);
	SET_LED_POS(pt[1], x + pled_info->width - 1, y);
	SET_LED_POS(pt[2], x + pled_info->width - pled_info->w_border - 1, y + pled_info->h_border);
	SET_LED_POS(pt[3], x + pled_info->w_border + 1, y + pled_info->h_border);
	return 4;
}

static int fill_up_right_led_point(led_size_info *pled_info, int x, int y, POINT *pt)
{
	SET_LED_POS(pt[0], x + pled_info->width, y + 1);
	SET_LED_POS(pt[1], x + pled_info->width, y + (pled_info->height >> 1) - 1);
	SET_LED_POS(pt[2], x + pled_info->width - pled_info->w_border, y + ((pled_info->height - pled_info->h_border) >> 1) - 1);
	SET_LED_POS(pt[3], x + pled_info->width - pled_info->w_border, y + pled_info->h_border + 1);
	return 4;
}

static int fill_down_right_led_point(led_size_info *pled_info, int x, int y, POINT *pt)
{
	SET_LED_POS(pt[0], x + pled_info->width, y + (pled_info->height>>1) + 1);
	SET_LED_POS(pt[1], x + pled_info->width, y + pled_info->height - 1);
	SET_LED_POS(pt[2], x + pled_info->width - pled_info->w_border, y + pled_info->height - pled_info->h_border - 1);
	SET_LED_POS(pt[3], x + pled_info->width - pled_info->w_border, y + ((pled_info->height + pled_info->h_border )>>1) + 1);
	return 4;
}

static int fill_bottom_led_point(led_size_info *pled_info, int x, int y, POINT *pt)
{
	SET_LED_POS(pt[0], x + 1, y + pled_info->height);
	SET_LED_POS(pt[1], x + pled_info->w_border + 1, y + pled_info->height - pled_info->h_border);
	SET_LED_POS(pt[2], x + pled_info->width - pled_info->w_border -1, y + pled_info->height - pled_info->h_border);
	SET_LED_POS(pt[3], x + pled_info->width - 1, y + pled_info->height);
	return 4;
}

static int fill_down_left_led_point(led_size_info* pled_info, int x, int y, POINT *pt)
{
	SET_LED_POS(pt[0], x, y + (pled_info->height>>1) + 1);
	SET_LED_POS(pt[1], x, y + pled_info->height - 1);
	SET_LED_POS(pt[2], x + pled_info->w_border, y + pled_info->height - pled_info->h_border - 1);
	SET_LED_POS(pt[3], x + pled_info->w_border, y + ((pled_info->height + pled_info->h_border)>>1) + 1);
	return 4;
}

static int fill_up_left_led_point(led_size_info* pled_info, int x, int y, POINT *pt)
{
	SET_LED_POS(pt[0], x, y + 1);
	SET_LED_POS(pt[1], x, y + (pled_info->height>>1) - 1);
	SET_LED_POS(pt[2], x + pled_info->w_border, y + ((pled_info->height - pled_info->h_border)>>1) -1);
	SET_LED_POS(pt[3], x + pled_info->w_border, y + pled_info->h_border + 1);
	return 4;
}

static int fill_mid_led_point(led_size_info* pled_info, int x, int y, POINT *pt)
{
	SET_LED_POS(pt[0], x + pled_info->w_border + 1, y + ((pled_info->height - pled_info->h_border)>>1));
	SET_LED_POS(pt[1], x + pled_info->width - pled_info->w_border - 1, y + ((pled_info->height - pled_info->h_border)>>1));
	SET_LED_POS(pt[2], x + pled_info->width - 1, y + (pled_info->height >>1));
	SET_LED_POS(pt[3], x + pled_info->width - pled_info->w_border - 1, y + ((pled_info->height + pled_info->h_border)>>1));
	SET_LED_POS(pt[4], x + pled_info->w_border + 1, y + ((pled_info->height + pled_info->h_border)>>1));

	SET_LED_POS(pt[5], x + 1, y + (pled_info->height >>1));

	return 6;
}

static int fill_dot_point(led_size_info *pled_info, int x, int y, POINT *pt)
{
	int max = led_max(pled_info->w_border, pled_info->h_border);

	SET_LED_POS(pt[0], x, y + pled_info->height - max);
	SET_LED_POS(pt[1], x + max, y + pled_info->height - max);
	SET_LED_POS(pt[2], x + max, y + pled_info->height);
	SET_LED_POS(pt[3], x, y + pled_info->height);
	return 4;
}

static int fill_up_colon_point(led_size_info* pled_info, int x, int y, POINT *pt)
{
	int min = led_min(pled_info->w_border, pled_info->h_border);
	x = x + ((pled_info->width - min) >> 1);
	y = y + (pled_info->height>>2);

	SET_LED_POS(pt[0], x, y);
	SET_LED_POS(pt[1], x + min, y);
	SET_LED_POS(pt[2], x + min, y + min);
	SET_LED_POS(pt[3], x, y + min);
	return 4;

}

static int fill_down_colon_point(led_size_info* pled_info, int x, int y, POINT *pt)
{
	int min = led_min(pled_info->w_border, pled_info->h_border);
	x = x + ((pled_info->width - min) >> 1);
	y = y + ((pled_info->height>>2)*3);

	SET_LED_POS(pt[0], x, y);
	SET_LED_POS(pt[1], x + min, y);
	SET_LED_POS(pt[2], x + min, y + min);
	SET_LED_POS(pt[3], x, y + min);
	return 4;
}

static fill_led_point fillPoints[] = {
	fill_top_led_point,
	fill_up_right_led_point,
	fill_down_right_led_point,
	fill_bottom_led_point,
	fill_down_left_led_point,
	fill_up_left_led_point,
	fill_mid_led_point,
	fill_dot_point,
	fill_up_colon_point,
	fill_down_colon_point
};

static void init_led_info(led_size_info *led_info, int width, int height)
{
	led_info->width = width;
	led_info->height = height;
	led_info->w_border = width / 5;
	led_info->h_border = height / 5;
}

static void draw_led_by_mask(HDC hdc, int x, int y, led_size_info * led_info, int mask)
{
	POINT pts[6];
	int i;
	for(i = 0; i< 10; i++)
	{
		if((1<<i) & mask)
		{
			int count = fillPoints[i](led_info, x, y, pts);
			FillPolygon(hdc, pts, count);
		}
	}
}



static unsigned int GetCharMask(char c)
{
	if (c >= '0'&& c <= '9')
	{
		static int digital_masks[] = {0x3F,0x06,0x5B, 0x4F, 0x66,0x6D, 0x7D, 0x07, 0x7F, 0x6F}; //0123456789
		return digital_masks[c - '0'];
	}
	else if (c == ':')
		return 0x300;
	else if (c == '.')
		return 0x80;
	else if (c == '-')
		return 0x40;

	return 0;
}

static void mLedstaticPiece_construct(mLedstaticPiece *self, DWORD add_data)
{
	Class(mStaticPiece).construct((mStaticPiece*)self, add_data);
	self->format = (NCS_ALIGN_CENTER << 24) | (NCS_VALIGN_CENTER << 16) | 5;
	self->width = 20;
	self->height = 30;
	mLedstaticPiece_setAutoWrap(self, 0);
	_c(self)->setProperty(self, NCSP_LEDLBLPIECE_COLOR, 0xFFFFFFFF);
}


static void draw_len_char(HDC hdc, int x, int y, led_size_info* pled_info, char ch)
{
	draw_led_by_mask(hdc, x, y, pled_info, GetCharMask(ch));
}

#define LED_LINE_SPACE  2
static int get_led_height(const char* str, int len, int width, int height, int total_width, int char_space)
{
	int h = 0;
	int i;
	int w = 0;
	if(str == NULL)
		return 0;

	h = height;
	for(i=0; i<len; i++)
	{
		if(str[i] == '\n')
		{
			h += height + LED_LINE_SPACE;
			w = 0;
		}
		else
		{
			w += width;
			if(w >= total_width)
			{
				h += height + LED_LINE_SPACE;
				w = 0;
			}
			w += char_space;
		}
	}
	return h;
}

static int cacl_text_start(const char* str, int *plen,int align, int width, int total_width, int x, int char_space)
{
	int w;
	int i;
	int len;

	len = *plen;
	w = 0;
	i = 0;

	while(i < len)
	{
		if(str[i] == '\n')
		{
			i ++;
			break;
		}
		else
		{
			w += width;
			if( w > total_width)
			{
				w -= (char_space + width);
				break;
			}
			else if(w == total_width)
			{
				i ++;
				break;
			}
			w += char_space;
		}
		i ++;
	}

	if( w <= 0)
		w = width;

	*plen = i;

	if(align == DT_LEFT)
		return x;
	else if(align == DT_CENTER)
		return x + (total_width -  w) / 2;
	else
		return x + total_width - w - 1;
}


BOOL ncsLedDrawText(HDC hdc, char *str, int len, RECT *rect, unsigned int format, int width, int height, unsigned int color, int char_space)
{
	int i;
	int x ,y;
	gal_pixel old_brush;
	led_size_info led_info;
	if (width == 0 || height == 0)
		return FALSE;

	if(len <= 0)
		len = strlen(str);

	init_led_info(&led_info, width, height);

	old_brush = SetBrushColor(hdc, ncsColor2Pixel(hdc, color));

	if(char_space == 0)
		char_space = 2;

	if(format & DT_SINGLELINE)
	{
		int align = format & (DT_LEFT|DT_CENTER|DT_RIGHT);
		y = (rect->top + rect->bottom - height) / 2;
		if(align == DT_LEFT)
		{
			x = rect->left;
		}
		else
		{
			//get the length of text
			int total = len*(width+char_space) - char_space;
			if(align == DT_CENTER)
			{
				x = (rect->right + rect->left - total) / 2;
			}
			else
			{
				x = rect->right - total;
			}
		}

		//draw texts
		for(i = 0; i < len; i++)
		{
			draw_len_char(hdc, x, y, &led_info, str[i]);
			x += width + char_space;
		}
	}
	else
	{
		int align = format & (DT_LEFT|DT_CENTER|DT_RIGHT);
		int valign = format & (DT_BOTTOM|DT_TOP|DT_VCENTER);
		int draw_len;

		//get y
		if(valign == DT_TOP)
			y = rect->top;
		else
		{
			int h = get_led_height(str,len, width, height, RECTWP(rect), char_space);
			if(valign == DT_BOTTOM)
				y = rect->bottom - h;
			else
				y = (rect->top + rect->bottom - h) / 2;
		}

		//draw every line
		i = 0;
		draw_len = len;
		x = cacl_text_start(str, &draw_len, align, width, RECTWP(rect), rect->left,  char_space);
		while(i < len)
		{
			//draw a line
			int j;
			for(j = 0; j < draw_len && str[i+j] != '\n'; j ++)
			{
				draw_len_char(hdc, x, y, &led_info, str[i+j]);
				x += width + char_space;
			}
			i += draw_len;
			if(i >= len)
				break;
			draw_len = len - i;
			x = cacl_text_start(str + i, &draw_len, align, width, RECTWP(rect), rect->left, char_space);
			y += (height + LED_LINE_SPACE);
		}
	}

	SetBrushColor(hdc, old_brush);

	return TRUE;
}

static void mLedstaticPiece_paint(mLedstaticPiece *self, HDC hdc, mWidget *owner, DWORD add_data)
{
	unsigned int style = 0;
	RECT rcClient;
	unsigned int color;

	if ( self->text == NULL)
	{
		return;
	}

	_c(self)->getRect(self, &rcClient);

	if (self->format & (NCS_ALIGN_CENTER << 24)) {
		style |= DT_CENTER;
	} else if (self->format & (NCS_ALIGN_RIGHT << 24)) {
		style |= DT_RIGHT;
	} else
		style |= DT_LEFT;

	if (self->format & (NCS_VALIGN_CENTER << 16)) {
		style |= DT_VCENTER;
	} else if (self->format & (NCS_VALIGN_BOTTOM << 16)) {
		style |= DT_BOTTOM;
	} else
		style |= DT_TOP;

	if (!mLedstaticPiece_isAutoWrap(self))
		style |= DT_SINGLELINE;

	style |= (self->format & 0xFF) << 16;

	if (add_data & WS_DISABLED)
		color = ncsGetElement(owner, NCS_FGC_DISABLED_ITEM);
	else
		color = self->color;
/*	else
	{
		self->color = ncsGetElement(owner, NCS_FGC_3DBODY);
		if (self->color == 0xFFFFFFFF)
			self->color = ncsGetElement(owner, NCS_FGC_WINDOW);
	}
*/
	ncsLedDrawText(hdc, self->text, strlen(self->text), &rcClient, style, self->width, self->height, color, self->format&0xFF);
}

static BOOL mLedstaticPiece_setProperty(mLedstaticPiece *self, const int id, DWORD value)
{
	switch(id)
	{
		case NCSP_LABELPIECE_LABEL:
		case NCSP_LEDLBLPIECE_TEXT:
			self->text = (char*)value;
			break;
		case NCSP_LEDLBLPIECE_COLOR:
			self->color = (unsigned int)value;
			break;
		case NCSP_LEDLBLPIECE_HEIGHT:
			self->height = (unsigned int)value;
			break;
		case NCSP_LEDLBLPIECE_WIDTH:
			self->width = (unsigned int)value;
			break;
		case NCSP_LEDLBLPIECE_ALIGN:
			self->format = ((value&0xFF) << 24) | (self->format & 0x00FFFFFF);
			break;
		case NCSP_LEDLBLPIECE_VALIGN:
			self->format = ((value&0xFF) << 16) | (self->format & 0xFF00FFFF);
			break;
		case NCSP_LEDLBLPIECE_AUTOWRAP:
			mLedstaticPiece_setAutoWrap(self, value);
			break;
		case NCSP_LEDLBLPIECE_GAP:
		{
			int gap = (unsigned int)value;
			if (gap < 255 && gap > 0)
				self->format = (gap&0xFF) | (self->format & 0xFFFFFF00);
			break;
		}
		default:
			return Class(mStaticPiece).setProperty((mStaticPiece*)self, id, value);
	}
	return TRUE;
}

static DWORD mLedstaticPiece_getProperty(mLedstaticPiece* self, int id)
{
	switch(id)
	{
	case NCSP_LABELPIECE_LABEL:
	case NCSP_LEDLBLPIECE_TEXT:
		return (DWORD)(self->text);
	case NCSP_LEDLBLPIECE_COLOR:
		return (DWORD)self->color;
	case NCSP_LEDLBLPIECE_WIDTH:
		return self->width;
	case NCSP_LEDLBLPIECE_HEIGHT:
		return self->height;
	case NCSP_LEDLBLPIECE_ALIGN:
		return self->format >> 24;
	case NCSP_LEDLBLPIECE_VALIGN:
		return (self->format >> 16) & 0xFF;
	case NCSP_LEDLBLPIECE_GAP:
		return self->format & 0xFF;
	}
	return Class(mStaticPiece).getProperty((mStaticPiece*)self, id);
}

BEGIN_MINI_CLASS(mLedstaticPiece, mStaticPiece)
	CLASS_METHOD_MAP(mLedstaticPiece, construct)
	CLASS_METHOD_MAP(mLedstaticPiece, paint)
	CLASS_METHOD_MAP(mLedstaticPiece, setProperty)
	CLASS_METHOD_MAP(mLedstaticPiece, getProperty)
END_MINI_CLASS

#endif //_MGNCSCTRL_LEDLABEL

