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
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#define MAXCOLORMAPSIZE         256
#define MAX_LWZ_BITS            12
#define INTERLACE               0x40
#define LOCALCOLORMAP           0x80

#define CM_RED                  0
#define CM_GREEN                1
#define CM_BLUE                 2

#define BitSet(byte, bit)               (((byte) & (bit)) == (bit))
#define ReadOK(file,buffer,len)         MGUI_RWread(file, buffer, len, 1)
#define LM_to_uint(a,b)                 (((b)<<8)|(a))
#define PIX2BYTES(n)                    (((n)+7)/8)


#define DEBUG_PRINT() fprintf(stderr, "line: %d, func: %s\n", __LINE__, __func__)
typedef struct tagGIFSCREEN {
    unsigned int Width;
    unsigned int Height;
    RGB ColorMap [MAXCOLORMAPSIZE];
    unsigned int BitPixel;
    unsigned int ColorResolution;
    unsigned int Background;
    unsigned int AspectRatio;
    int transparent;
    int delayTime;
    int inputFlag;
    int disposal;
} GIFSCREEN;

typedef struct tagIMAGEDESC {
    int Top;
    int Left;
    int Width;
    int Height;
    BOOL haveColorMap;
    int bitPixel;
    int grayScale;
    RGB ColorMap [MAXCOLORMAPSIZE];
    BOOL interlace;
} IMAGEDESC;

static int ZeroDataBlock = 0;

static int bmpComputePitch (int bpp, Uint32 width, Uint32* pitch, BOOL does_round);
static int LWZReadByte (MG_RWops *area, int flag, int input_code_size);
static int GetCode (MG_RWops *area, int code_size, int flag);
static int GetDataBlock (MG_RWops *area, unsigned char *buf);
static int DoExtension (MG_RWops *area, int label, GIFSCREEN* GifScreen);
static int ReadColorMap (MG_RWops *area, int number, RGB* ColorMap);
static int ReadImageDesc (MG_RWops *area, IMAGEDESC* ImageDesc, GIFSCREEN* GifScreen);

static int bmpComputePitch (int bpp, Uint32 width, Uint32* pitch, BOOL does_round)
{
    Uint32 linesize;
    int bytespp = 1;

    if(bpp == 1)
        linesize = PIX2BYTES (width);
    else if(bpp <= 4)
        linesize = PIX2BYTES (width << 2);
    else if (bpp <= 8)
        linesize = width;
    else if(bpp <= 16) {
        linesize = width * 2;
        bytespp = 2;
    } else if(bpp <= 24) {
        linesize = width * 3;
        bytespp = 3;
    } else {
        linesize = width * 4;
        bytespp = 4;
    }

    /* rows are DWORD right aligned*/
    if (does_round)
        *pitch = (linesize + 3) & -4;
    else
        *pitch = linesize;
    return bytespp;
}

static int LWZReadByte (MG_RWops *area, int flag, int input_code_size)
{
    int code, incode;
    register int i;
    static int fresh = FALSE;
    static int code_size, set_code_size;
    static int max_code, max_code_size;
    static int firstcode, oldcode;
    static int clear_code, end_code;
    static int table[2][(1 << MAX_LWZ_BITS)];
    static int stack[(1 << (MAX_LWZ_BITS)) * 2], *sp;

    if (flag) {
        set_code_size = input_code_size;
        code_size = set_code_size + 1;
        clear_code = 1 << set_code_size;
        end_code = clear_code + 1;
        max_code_size = 2 * clear_code;
        max_code = clear_code + 2;

        GetCode(area, 0, TRUE);

        fresh = TRUE;

        for (i = 0; i < clear_code; ++i) {
            table[0][i] = 0;
            table[1][i] = i;
        }
        for (; i < (1 << MAX_LWZ_BITS); ++i)
            table[0][i] = table[1][0] = 0;

        sp = stack;

        return 0;
    } else if (fresh) {
        fresh = FALSE;
        do {
            firstcode = oldcode = GetCode(area, code_size, FALSE);
        } while (firstcode == clear_code);
        return firstcode;
    }
    if (sp > stack)
        return *--sp;

    while ((code = GetCode(area, code_size, FALSE)) >= 0) {
        if (code == clear_code) {
            for (i = 0; i < clear_code; ++i) {
                table[0][i] = 0;
                table[1][i] = i;
            }
            for (; i < (1 << MAX_LWZ_BITS); ++i)
                table[0][i] = table[1][i] = 0;
            code_size = set_code_size + 1;
            max_code_size = 2 * clear_code;
            max_code = clear_code + 2;
            sp = stack;
            firstcode = oldcode = GetCode(area, code_size, FALSE);
            return firstcode;
        } else if (code == end_code) {
            int count;
            unsigned char buf[260];

            if (ZeroDataBlock)
                return -2;

            while ((count = GetDataBlock(area, buf)) > 0);

            if (count != 0) {
                /*
                 * fprintf (stderr,"missing EOD in data stream (common occurence)");
                 */
            }
            return -2;
        }
        incode = code;

        if (code >= max_code) {
            *sp++ = firstcode;
            code = oldcode;
        }
        while (code >= clear_code) {
            *sp++ = table[1][code];
            if (code == table[0][code]) {
                return -1;
            }
            code = table[0][code];
        }

        *sp++ = firstcode = table[1][code];

        if ((code = max_code) < (1 << MAX_LWZ_BITS)) {
            table[0][code] = oldcode;
            table[1][code] = firstcode;
            ++max_code;
            if ((max_code >= max_code_size) &&
                (max_code_size < (1 << MAX_LWZ_BITS))) {
                max_code_size *= 2;
                ++code_size;
            }
        }
        oldcode = incode;

        if (sp > stack)
            return *--sp;
    }
    return code;
}


static int GetCode(MG_RWops *area, int code_size, int flag)
{
    static unsigned char buf[280];
    static int curbit, lastbit, done, last_byte;
    int i, j, ret;
    unsigned char count;

    if (flag) {
        curbit = 0;
        lastbit = 0;
        done = FALSE;
        return 0;
    }
    if ((curbit + code_size) >= lastbit) {
        if (done) {
            if (curbit >= lastbit)
            return -1;
        }
        buf[0] = buf[last_byte - 2];
        buf[1] = buf[last_byte - 1];

        if ((count = GetDataBlock(area, &buf[2])) == 0)
            done = TRUE;

        last_byte = 2 + count;
        curbit = (curbit - lastbit) + 16;
        lastbit = (2 + count) * 8;
    }
    ret = 0;
    for (i = curbit, j = 0; j < code_size; ++i, ++j)
        ret |= ((buf[i / 8] & (1 << (i % 8))) != 0) << j;

    curbit += code_size;

    return ret;
}

static int GetDataBlock (MG_RWops *area, unsigned char *buf)
{
    unsigned char count;

    if (!ReadOK(area, &count, 1))
        return 0;
    ZeroDataBlock = (count == 0);

    if ((count != 0) && (!ReadOK(area, buf, count)))
        return 0;
    return count;
}

static int DoExtension (MG_RWops *area, int label, GIFSCREEN* GifScreen)
{
    static unsigned char buf[256];

    switch (label) {
    case 0x01:                        /* Plain Text Extension */
        while (GetDataBlock (area, (unsigned char *) buf) != 0);
        break;
    case 0xff:                        /* Application Extension */
        while (GetDataBlock (area, (unsigned char *) buf) != 0);
        break;
    case 0xfe:                        /* Comment Extension */
        while (GetDataBlock (area, (unsigned char *) buf) != 0);
        return 0;
    case 0xf9:                        /* Graphic Control Extension */
        GetDataBlock (area, (unsigned char *) buf);
        GifScreen->disposal = (buf[0] >> 2) & 0x7;//000 000 0 0 the middle 2 bit is disposal
        GifScreen->inputFlag = (buf[0] >> 1) & 0x1;//000 000 0 0 the secand last bit 
                            //is user input flag
        GifScreen->delayTime = LM_to_uint(buf[1], buf[2]);
        if ((buf[0] & 0x1) != 0)// 000 000 0 0 the last bit is transparent flag
            GifScreen->transparent = buf[3];
        else
            GifScreen->transparent = -1;

        while (GetDataBlock (area, (unsigned char *) buf) != 0);
        return 0;
    default:
        while (GetDataBlock (area, (unsigned char *) buf) != 0);
        break;
    }

    return 0;
}

static int ReadColorMap (MG_RWops *area, int number, RGB* ColorMap)
{
    int i;
    unsigned char rgb[3];

    for (i = 0; i < number; ++i) {
        if (!ReadOK (area, rgb, sizeof(rgb))) {
            return -1;
        }

        ColorMap [i].r = rgb[0];
        ColorMap [i].g = rgb[1];
        ColorMap [i].b = rgb[2];
    }

    return 0;
}

static int ReadGIFGlobal (MG_RWops *area, GIFSCREEN* GifScreen)
{
    unsigned char buf[9];
    unsigned char version[4];

    if (!ReadOK (area, buf, 6))
        return -1;                /* not gif image*/

    if (strncmp((char *) buf, "GIF", 3) != 0)
        return -1;

    strncpy ((char*)version, (char *) buf + 3, 3);
    version [3] = '\0';

    if (strcmp ((const char*)version, "87a") != 0 && strcmp ((const char*)version, "89a") != 0) {
        return -1;                /* image loading error*/
    }

    GifScreen->Background = -1;
    GifScreen->transparent = -1;
    GifScreen->delayTime = -1;
    GifScreen->inputFlag = -1;
    GifScreen->disposal = 0;

    if (!ReadOK (area, buf, 7)) {
        return -1;                /* image loading error*/
    }
    GifScreen->Width = LM_to_uint (buf[0], buf[1]);
    GifScreen->Height = LM_to_uint (buf[2], buf[3]);
    GifScreen->BitPixel = 2 << (buf[4] & 0x07);
    GifScreen->ColorResolution = (((buf[4] & 0x70) >> 3) + 1);
    GifScreen->Background = buf[5];
    GifScreen->AspectRatio = buf[6];

    if (BitSet(buf[4], LOCALCOLORMAP)) {        /* Global Colormap */
        if (ReadColorMap (area, GifScreen->BitPixel, GifScreen->ColorMap)) {
            return -1;                /* image loading error*/
        }
    }

    return 0;
}

static int ReadImageDesc (MG_RWops *area, IMAGEDESC* ImageDesc, GIFSCREEN* GifScreen)
{
    unsigned char buf[16];
    if (!ReadOK (area, buf, 9)) {
        return -1;
    }

    ImageDesc->Top = LM_to_uint (buf[0], buf[1]);
    ImageDesc->Left = LM_to_uint (buf[2], buf[3]);
    ImageDesc->Width = LM_to_uint (buf[4], buf[5]);
    ImageDesc->Height = LM_to_uint (buf[6], buf[7]);
    ImageDesc->haveColorMap = BitSet (buf[8], LOCALCOLORMAP);

    ImageDesc->bitPixel = 1 << ((buf[8] & 0x07) + 1);

    ImageDesc->interlace = BitSet(buf[8], INTERLACE);

    if (ImageDesc->haveColorMap) {
        if (ReadColorMap (area, ImageDesc->bitPixel, ImageDesc->ColorMap) < 0) {
            return -1;
        }
    } else {
        memcpy (ImageDesc->ColorMap, GifScreen->ColorMap, MAXCOLORMAPSIZE*sizeof (RGB));
    }

    return 0;
}



static int ReadImage (MG_RWops* area, MYBITMAP* bmp, IMAGEDESC* ImageDesc, GIFSCREEN* GifScreen, int ignore)
{

    unsigned char c;
    int v;
    int xpos = 0, ypos = 0, pass = 0;

    /*
     * initialize the compression routines
     */
    if (!ReadOK (area, &c, 1)) {
        return -1;
    }

    if (LWZReadByte (area, TRUE, c) < 0) {
        return -1;
    }

    /*
     * if this is an "uninteresting picture" ignore it.
     */
    if (ignore) {
        while (LWZReadByte (area, FALSE, c) >= 0);
        return 0;
    }

    bmp->w = ImageDesc->Width;
    bmp->h = ImageDesc->Height;

    bmp->flags = MYBMP_FLOW_DOWN;
    if (GifScreen->transparent >= 0) {
        bmp->flags |= MYBMP_TRANSPARENT;
        bmp->transparent = GifScreen->transparent;
    }
    bmp->frames = 1;
    bmp->depth = 8;
    bmpComputePitch (bmp->depth, bmp->w, &bmp->pitch, TRUE);
    bmp->bits = malloc (bmp->h * bmp->pitch);

    if(!bmp->bits)
        return -1;

    while ((v = LWZReadByte (area, FALSE, c)) >= 0) {
        bmp->bits[ypos * bmp->pitch + xpos] = v;
        ++xpos;
        if (xpos == ImageDesc->Width) {
            xpos = 0;
            if (ImageDesc->interlace) {
                switch (pass) {
                case 0:
                case 1:
                    ypos += 8;
                    break;
                case 2:
                    ypos += 4;
                    break;
                case 3:
                    ypos += 2;
                    break;
                }

                if (ypos >= ImageDesc->Height) {
                    ++pass;
                    switch (pass) {
                    case 1:
                        ypos = 4;
                        break;
                    case 2:
                        ypos = 2;
                        break;
                    case 3:
                        ypos = 1;
                        break;
                    default:
                        goto fini;
                    }
                }
            } else {
                ++ypos;
            }
        }
        if (ypos >= ImageDesc->Height)
            break;
    }

fini:
    if (v >= 0) return 0;
    return -1;
}


static void mGIFAnimateFrames_construct(mGIFAnimateFrames *self, DWORD addData)
{
    unsigned char c;
    int ok = 0;
    MYBITMAP mybmp;
    GIFSCREEN GifScreen;
    IMAGEDESC ImageDesc;
	HCURSOR old;
    mGIFAnimateFrame *frame, *current = NULL;
	MG_RWops* area = (MG_RWops*) addData;

    if (self == NULL) {
		return;
	}
	
	Class(mAnimateFrames).construct((mAnimateFrames*)self, addData);
		
	old = SetCursor(GetSystemCursor(IDC_BUSY));	
   
	if (ReadGIFGlobal (area, &GifScreen) < 0) {
		goto error;
	}
#if 0
	if (GifScreen.Background >= 0) {
        self->bk = GifScreen.ColorMap [GifScreen.Background];
        self->bk.a = 1;
    }
    else {
        self->bk.r = self->bk.g = self->bk.b = 255;
        self->bk.a = 0;
    }
#endif

    if ((ok = ReadOK (area, &c, 1)) == 0) {
        goto error;
    }

	for (; c != ';' && ok > 0; ok = ReadOK (area, &c, 1)) {
		if (c == '!') {
			if ((ok = ReadOK (area, &c, 1)) == 0) {
				goto error;
			}
			DoExtension (area, c, &GifScreen);
		} else if (c == ',') {
			mybmp.bits = NULL;
			if (ReadImageDesc (area, &ImageDesc, &GifScreen) < 0) {
				goto error;
			}
			if (ReadImage (area, &mybmp, &ImageDesc, &GifScreen, 0) < 0) {
				goto error;
			}

			frame = (mGIFAnimateFrame*) calloc(1, sizeof(mGIFAnimateFrame));
			if(!frame){
				UnloadMyBitmap (&mybmp);
				goto error;
			}
			frame->next = NULL;
			frame->off_y = ImageDesc.Left;
			frame->off_x = ImageDesc.Top;
			frame->disposal = GifScreen.disposal;

			frame->delay_time = (GifScreen.delayTime>10)?GifScreen.delayTime:10;

			if(ExpandMyBitmap(HDC_SCREEN, &frame->bmp, &mybmp, ImageDesc.ColorMap, 0) != 0)
			{
				UnloadMyBitmap (&mybmp);
				free(frame);
				goto error;
			}

			if(self->frames == NULL) {
				self->frames = frame;
				current = frame;
				current->prev = NULL;
			} else {
				frame->prev = current;
				current->next = frame;
				current = current->next;
			}

			self->nr_frames++;
			UnloadMyBitmap (&mybmp);
		}
	}
	
	self->cur_frame  = self->frames;
	self->max_width  = GifScreen.Width;
	self->max_height = GifScreen.Height;
	self->mem_dc 	 = CreateCompatibleDCEx(HDC_SCREEN, self->max_width, self->max_height);
	
	SetCursor(old);
	return;

error:
	SetCursor(old);
	Class(mAnimateFrames).destroy((mAnimateFrames*)self);
	fprintf(stderr, "[mAnimateFrames ] Error: read gif file error\n");
	return;
}

static void mGIFAnimateFrames_destroy (mGIFAnimateFrames* self)
{
	mGIFAnimateFrame *tmp, *frame;

	frame = self->frames;
	while(frame) {
		tmp = frame->next;
		UnloadBitmap(&frame->bmp);
		free(frame);
		frame = tmp;
	}

	if(self->mem_dc)
		DeleteMemDC(self->mem_dc);

	Class(mAnimateFrames).destroy((mAnimateFrames*)self);
}

static void paint_one_frame(mGIFAnimateFrames* self, mGIFAnimateFrame* frame)
{
	if(!frame)
		return;

	FillBoxWithBitmap(self->mem_dc, frame->off_x, frame->off_y, -1, -1, &frame->bmp);
}

static void restore_bk_color(mGIFAnimateFrames* self, mGIFAnimateFrame* frame, mWidget* owner)
{
	BOOL bTransparent = TRUE;
	gal_pixel bg_color;
	gal_pixel old_color;

	if(owner && IsWindow(owner->hwnd)) {
		bTransparent = GetWindowExStyle(owner->hwnd) & WS_EX_TRANSPARENT;
		if(!bTransparent)
			bTransparent = (owner->bkimg.img.pbmp != NULL);

		bg_color = GetWindowBkColor(owner->hwnd);
	} else {
		bg_color = GetWindowElementPixel(HWND_DESKTOP, WE_BGC_DESKTOP);
	}
	
	if(bTransparent) {
		SetMemDCColorKey(self->mem_dc, MEMDC_FLAG_SRCCOLORKEY, bg_color);
	} else {
		SetMemDCColorKey(self->mem_dc, 0, 0);
	}

	old_color = SetBrushColor(self->mem_dc, bg_color);

	if(frame)
		FillBox(self->mem_dc, frame->off_x, 
				frame->off_y, frame->bmp.bmWidth, frame->bmp.bmHeight);
	else
		FillBox(self->mem_dc, 0, 0, self->max_width, self->max_height);

	SetBrushColor(self->mem_dc, old_color);
}

static void restore_prev_frame(mGIFAnimateFrames* self, mGIFAnimateFrame* frame, mWidget* owner)
{
	if(frame == NULL)
		return;

	switch(frame->disposal) {
		case 2:
			restore_bk_color(self, frame, owner);
			break;
		case 3:
			restore_prev_frame(self, frame->prev, owner);
			break;
	}

	paint_one_frame(self, frame);
}

static void disposal_handle(mGIFAnimateFrames* self, mGIFAnimateFrame* frame, mWidget* owner)
{
	if(NULL == frame) {
		restore_bk_color(self, NULL, owner);
		return;
	}

	switch(frame->disposal) {
		case 2:
			restore_bk_color(self, frame, owner);
			break;
		case 3:
			restore_prev_frame(self, frame->prev, owner);
			break;
	}
}


static int mGIFAnimateFrames_drawFrame(mGIFAnimateFrames* self, 
		HDC hdc, mWidget *owner, RECT *pRect, int align, int valign, BOOL bScale)
{
	mGIFAnimateFrame* frame = NULL;

	if(!self->mem_dc)
		return NCSR_ANIMATEFRAME_FAILED;

	if(self->frames) {
		if(self->cur_frame == NULL) {
			self->cur_frame = self->frames;
		}
	}
	
	frame = (mGIFAnimateFrame*)self->cur_frame;

	if(frame) {
		disposal_handle(self, frame->prev, owner);
		paint_one_frame(self, frame);
	} else {
		if(self->mem_dc)
			disposal_handle(self, NULL, owner);
	}

	if(bScale) {
		StretchBlt(self->mem_dc, 0, 0, self->max_width, self->max_height, 
				hdc, pRect->left, pRect->top, RECTWP(pRect), RECTHP(pRect), 0);
	} else {
		int x, y;
		int w, h;
		w = self->max_width;
		h = self->max_height;
		switch(align)
		{
		case NCS_ALIGN_RIGHT:
			x = pRect->right - self->max_width;
			break;
		case NCS_ALIGN_CENTER:
			x = (pRect->left + pRect->right - self->max_width) / 2;
			break;
		default:
			x = pRect->left;
			break;
		}
		if(x < pRect->left) {
			w = w - pRect->left + x;
			x = pRect->left;
		}
		
		switch(valign)
		{
		case NCS_VALIGN_BOTTOM:
			y = pRect->bottom - self->max_height;
			break;
		case NCS_VALIGN_CENTER:
			y = (pRect->top + pRect->bottom - self->max_height) / 2;
			break;
		default:
			y = pRect->top;
			break;
		}
		if(y < pRect->top) {
			h = h - pRect->top + y;
			y = pRect->top;
		}

		BitBlt(self->mem_dc, 0, 0, w, h, hdc, x, y, 0);
	}

	return NCSR_ANIMATEFRAME_OK;
}

static BOOL mGIFAnimateFrames_getMaxFrameSize(mGIFAnimateFrames* self, int *pwidth, int *pheight)
{
	*pwidth = self->max_width;
	*pheight = self->max_height;
	return TRUE;
}

static int mGIFAnimateFrames_nextFrame(mGIFAnimateFrames* self)
{
	if(self->cur_frame == NULL) {
		self->cur_frame = self->frames;
		return NCSR_ANIMATEFRAME_OK;
	}

	self->cur_frame = ((mGIFAnimateFrame*)(self->cur_frame))->next;

	return self->cur_frame ? NCSR_ANIMATEFRAME_OK : NCSR_ANIMATEFRAME_LASTFRAME;
}

BEGIN_MINI_CLASS(mGIFAnimateFrames, mAnimateFrames)
	CLASS_METHOD_MAP(mGIFAnimateFrames, construct)
	CLASS_METHOD_MAP(mGIFAnimateFrames, destroy)
	CLASS_METHOD_MAP(mGIFAnimateFrames, drawFrame)
	CLASS_METHOD_MAP(mGIFAnimateFrames, getMaxFrameSize)
	CLASS_METHOD_MAP(mGIFAnimateFrames, nextFrame)
END_MINI_CLASS

///////////////////////////////////////////////////////////////////////////////////////

mGIFAnimateFrames* ncsCreateAnimateFramesFromGIFFile (const char* file)
{
    MG_RWops* area;
    mGIFAnimateFrames* gaf;
    
	//GIFSCREEN GifScreen;
	if (NULL == (area = MGUI_RWFromFile (file, "rb"))) 
	{
		char local_file[256];
		const char *str = file ? strrchr(file, '/') : NULL;
		
		if (str == NULL) {		
			fprintf(stderr, "read giffile error\n");
			return NULL;
		}
		memset(local_file, 0, sizeof(local_file));
		sprintf(local_file, "./res/image/%s", str);
		if (NULL == (area = MGUI_RWFromFile(local_file, "rb"))) {	
			fprintf(stderr, "read local giffile error\n");
			return NULL;
		}
    }

	gaf = NEWEX(mGIFAnimateFrames, (DWORD)area);

	if(gaf == NULL || gaf->frames == NULL)
		return NULL;
	
    MGUI_RWclose (area);

    return gaf;
}

mGIFAnimateFrames* ncsCreateAnimateFramesFromGIFMem (const void* mem, int size)
{
    MG_RWops* area;
    mGIFAnimateFrames* gaf ;

    if (!(area = MGUI_RWFromMem ((void*)mem, size))) {
        return NULL;
    }

	gaf = NEWEX(mGIFAnimateFrames, (DWORD)area);

    MGUI_RWclose (area);

    return gaf;
}


