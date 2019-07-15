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
#include <dirent.h>
#include <sys/stat.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "manimateframes.h"
#include "mbmparrayanimateframes.h"

#define NCSF_BAAF_AUTODELETE 1

static void maxFrameSize(mBmpArrayAnimateFrames* self)
{
	mBmpArrayAnimateFrame *frame;
	int max_width = 0, max_height = 0;

	frame = self->frames;
	
	max_width =  frame->bmp->bmWidth;
	max_height =  frame->bmp->bmHeight;

	frame = frame->next;
	while(frame != NULL)
	{
		if( frame->bmp->bmWidth > max_width)
			max_width =  frame->bmp->bmWidth;
		if( frame->bmp->bmHeight > max_height)
			max_height =  frame->bmp->bmHeight;

		frame = frame->next;
	}

	self->max_width = max_width;
	self->max_height = max_height;
}

static void mBmpArrayAnimateFrames_construct(mBmpArrayAnimateFrames *self, DWORD addData)
{
	Class(mObject).construct((mObject*)self, 0);

	self->frames = (mBmpArrayAnimateFrame*)addData;
	
	if (self->frames == NULL)
	{
		return;
	}
	self->cur_frame = NULL;
	maxFrameSize(self);
}			

BOOL mBmpArrayAnimateFrames_getMaxFrameSize(mBmpArrayAnimateFrames* self, int *pwidth, int *pheight)
{
	*pwidth = self->max_width;
	*pheight = self->max_height;
	
	return TRUE;
}

static void mBmpArrayAnimateFrames_destroy (mBmpArrayAnimateFrames* self)
{
	mBmpArrayAnimateFrame *tmp, *frame;
	
	frame = self->frames;
	while(frame) {
		tmp = frame->next;
		
		if(self->flags & NCSF_BAAF_AUTODELETE && frame->bmp)
		{
			UnloadBitmap(frame->bmp);
			free(frame->bmp);
		}

		free(frame);
		frame = tmp;
	}
//	free (self);
}

int mBmpArrayAnimateFrames_drawFrame(mBmpArrayAnimateFrames* self, HDC hdc, BOOL bScaled, RECT *pRect, int align, int valign, BOOL bScale)
{
	if(self->frames == NULL)
		return NCSR_ANIMATEFRAME_FAILED;

	if(self->cur_frame == NULL)
		self->cur_frame = self->frames;

	if(!self->cur_frame)
		return NCSR_ANIMATEFRAME_FAILED;

	if(bScale)
	{	
		FillBoxWithBitmap(hdc, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top, (((mBmpArrayAnimateFrame*)(self->cur_frame))->bmp));
	} 
	else
	{
		mBmpArrayAnimateFrame* frame = (mBmpArrayAnimateFrame*)(self->cur_frame);
		int x, y, w, h;
		int xo = 0, yo = 0;
		w = frame->bmp->bmWidth;
		h = frame->bmp->bmHeight;
		switch(align)
		{
		case NCS_ALIGN_RIGHT:
			x = pRect->right - w;
			break;
		case NCS_ALIGN_CENTER:
			x = pRect->left + (RECTWP(pRect) - w) / 2;
			break;
		default:
			x = pRect->left;
			if(w > RECTWP(pRect))
				w = RECTWP(pRect);
		}

		if(x < pRect->left){
			xo = pRect->left - x;
			x = pRect->left;
			w = RECTWP(pRect);
		}

		switch(valign)
		{
		case NCS_VALIGN_BOTTOM:
			y = pRect->bottom - h;
			break;
		case NCS_VALIGN_CENTER:
			y = pRect->top + (RECTHP(pRect) - h) / 2;
			break;
		default:
			y = pRect->top;
			if(h > RECTHP(pRect))
				h = RECTHP(pRect);
		}
		if(y < pRect->top){
			yo = pRect->top - y;
			y = pRect->top;
			h = RECTHP(pRect);
		}

		FillBoxWithBitmapPart(hdc, x, y, w, h,
				frame->bmp->bmWidth,
				frame->bmp->bmHeight,
				frame->bmp,
				xo, yo);
	}

	return NCSR_ANIMATEFRAME_OK;
}
	
static int mBmpArrayAnimateFrames_nextFrame(mBmpArrayAnimateFrames* self)
{
	if(self->cur_frame == NULL)
	{
		self->cur_frame = self->frames;
		return NCSR_ANIMATEFRAME_OK;
	}
	self->cur_frame = ((mBmpArrayAnimateFrame*)(self->cur_frame))->next;
	return self->cur_frame ? NCSR_ANIMATEFRAME_OK : NCSR_ANIMATEFRAME_LASTFRAME;
}
	
BEGIN_MINI_CLASS(mBmpArrayAnimateFrames, mAnimateFrames)
	CLASS_METHOD_MAP(mBmpArrayAnimateFrames, construct)
	CLASS_METHOD_MAP(mBmpArrayAnimateFrames, destroy)
	CLASS_METHOD_MAP(mBmpArrayAnimateFrames, getMaxFrameSize)
	CLASS_METHOD_MAP(mBmpArrayAnimateFrames, drawFrame)
	CLASS_METHOD_MAP(mBmpArrayAnimateFrames, nextFrame)
END_MINI_CLASS


static mBmpArrayAnimateFrame* add_one_frame(PBITMAP pbmp, mBmpArrayAnimateFrame* current)
{
	mBmpArrayAnimateFrame *af;
	if(!pbmp)
		return NULL;
	
	af = (mBmpArrayAnimateFrame*)calloc(1, sizeof(mBmpArrayAnimateFrame));
	af->bmp = pbmp;
	af->prev = current;
	af->next = NULL;
	if(current)
		current->next = af;
	return af;
}

static mBmpArrayAnimateFrame* add_frame_from_file(const char* file, mBmpArrayAnimateFrame* current)
{
	BITMAP bmp;
	PBITMAP pbmp;
	if(!file)
		return NULL;
	if(LoadBitmapFromFile(HDC_SCREEN, &bmp, file) != 0)
		return NULL;

	pbmp = (PBITMAP)malloc(sizeof(BITMAP));
	memcpy(pbmp, &bmp, sizeof(BITMAP));
	
	return add_one_frame(pbmp, current);
}

static inline const char* get_extension (const char* filename)
{
    const char* ext;

    if (NULL != (ext = strrchr (filename, '.')))
        return ext + 1;

    return NULL;
}

static const char *bitmap_types [] =
{
   "bmp", 
#ifdef _MGIMAGE_LBM
   "lbm", 
#endif
#ifdef _MGIMAGE_PCX
   "pcx",
#endif
#ifdef _MGIMAGE_TGA
   "tga", 
#endif
#ifdef _MGIMAGE_GIF
   "gif", 
#endif
#ifdef _MGIMAGE_JPG
   "jpg",
   "jpeg",
#endif
#ifdef _MGIMAGE_PNG
   "png", 
#endif
};

static inline BOOL is_image_file (const char *file)
{
	int i;
	const char *type = get_extension (file);
	
	if (type == NULL)
		return FALSE;
	
	for (i = 0; i < TABLESIZE(bitmap_types); i++)
	{
		if (strcmp(type, bitmap_types[i]) == 0){
			return TRUE;
		}
	}
	return FALSE;
}

mBmpArrayAnimateFrames* ncsCreateAnimateFramesFromDIR(const char* path)
{
	mBmpArrayAnimateFrame * frames = NULL, *current = NULL;
	mBmpArrayAnimateFrames *baaf = NULL;
	DIR *dir;
	struct dirent * ent;
	char szPath[MAX_PATH];
	int  len = 0;

	if(!path)
		return NULL;

	dir = opendir(path);

	if(!dir)
	{
		const char* str = strrchr(path, '/');
		if(!str)
			str = strrchr(path, '\\');
		if(!str)
			return NULL;

		strncpy(szPath, path, str - path);
		szPath[str - path] = 0;
		dir = opendir(szPath);
	}
	else
		strcpy(szPath, path);
	

	if(!dir)
		return NULL;

	len = strlen(szPath);
	szPath[len++] = '/';

	while((ent = readdir(dir)) != NULL) {
		struct stat ftype;
		if( strcmp(ent->d_name, ".") == 0
			|| strcmp(ent->d_name, "..") == 0
			|| !is_image_file (ent->d_name)) {
			continue;
		}

		strcpy(szPath + len, ent->d_name);
		
		if(stat(szPath, &ftype) < 0) {
			continue;
		}
		if(!S_ISREG(ftype.st_mode)) {
			continue;
		}

		current = add_frame_from_file(szPath, current);
		if(current && frames == NULL)
			frames = current;
	}

	closedir(dir);

	
	baaf = NEWEX(mBmpArrayAnimateFrames, (DWORD)frames);

	baaf->flags |= NCSF_BAAF_AUTODELETE;
	return baaf;
}

mBmpArrayAnimateFrames * ncsCreateAnimateFramesFromImageList(const char ** file_list, int count)
{
	int i;
	mBmpArrayAnimateFrame* frames = NULL, *current = NULL;
	mBmpArrayAnimateFrames *baaf = NULL;

	if(!file_list)
		return NULL;

	if(count == 0)
		count = -1;

	for(i = 0; i != count && file_list[i]; i++)
	{
		current = add_frame_from_file(file_list[i], current);
		if(current && frames == NULL)
			frames = current;
	}

	baaf =  NEWEX(mBmpArrayAnimateFrames, (DWORD)frames);

	baaf->flags |= NCSF_BAAF_AUTODELETE;
	return baaf;

}

mBmpArrayAnimateFrames * ncsCreateAnimateFramesFromBmpArray(PBITMAP * bmps, int count, BOOL auto_delete)
{
	int i;
	mBmpArrayAnimateFrame* frames = NULL, *current = NULL;
	mBmpArrayAnimateFrames *baaf = NULL;
	if(!bmps)
		return NULL;

	if(count <= 0)
		count = -1;

	for(i = 0; i != count && bmps[i] ; i ++)
	{
		current = add_one_frame(bmps[i], current);
		if(current && frames == NULL)
			frames = current;
	}

	baaf = NEWEX(mBmpArrayAnimateFrames, (DWORD)frames);

	if(auto_delete)
		baaf->flags |= NCSF_BAAF_AUTODELETE;

	return baaf;
}

