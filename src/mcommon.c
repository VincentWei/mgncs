
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mcommon.h"

//default Allocator

void * def_alloc(void *pold, size_t new_size)
{
	if(new_size <= 0)
		new_size = 1;

	if(pold)
		return realloc(pold, new_size);
	else
		return malloc(new_size);
}

void def_free(void *p)
{
	if(p)
		free(p);
}

static Allocator defAllocator = {
	def_alloc,
	NULL,
	def_free
};


Allocator * ncsGetDefaultAllocator()
{
	return &defAllocator;
}

static inline void calc_pos_align(int *px, int *py, const RECT *rc, int imgw, int imgh, int align, int valign)
{
	if(align == NCS_ALIGN_CENTER)
		*px = (rc->left + rc->right - imgw)/2;
	else if(align == NCS_ALIGN_RIGHT)
		*px = (rc->right - imgw);
	else
		*px = rc->left;

	if(valign == NCS_VALIGN_CENTER)
		*py = (rc->top + rc->bottom - imgh)/2;
	else if(valign == NCS_VALIGN_BOTTOM)
		*py = (rc->bottom - imgh);
	else
		*py = rc->top;
}

void ncsDrawImage(HDC hdc, PBITMAP pbmp, const RECT * rc, int mode, int align, int valign)
{
    int x = 0, y = 0;
	int imgw, imgh;

	if(pbmp == NULL)
		return;

    switch(mode)
    {
        case NCS_DM_SCALED :
            {
                FillBoxWithBitmap(hdc, rc->left, rc->top,
                        RECTWP(rc), RECTHP(rc), pbmp);
                break;
            }
        case NCS_DM_TILED :
            {
                imgw = pbmp->bmWidth;
                imgh = pbmp->bmHeight;

                for(x = rc->left; x < rc->right; x += imgw)
                {
                    for(y = rc->top; y < rc->bottom; y += imgh)
                        FillBoxWithBitmap(hdc, x, y, imgw, imgh, pbmp);
                }

                break;
            }
        case NCS_DM_NORMAL :
        default:
            {
                imgw = pbmp->bmWidth;
                imgh = pbmp->bmHeight;

				calc_pos_align(&x, &y, rc, imgw, imgh, align, valign);

                FillBoxWithBitmap(hdc, x, y, imgw, imgh, pbmp);

                break;
            }
    }

}

void ncsInitDrawInfo(ImageDrawInfo *idi)
{
	if(!idi)
		return;
	memset(idi, 0, sizeof(ImageDrawInfo));
}

void ncsCleanImageDrawInfo(ImageDrawInfo * idi)
{
	if(!idi)
		return;

	if(idi->flag == IMG_FLAG_UNLOAD)
	{
		switch(idi->img_type)
		{
		case IMG_TYPE_BITMAP:
			UnloadBitmap(idi->img.pbmp);
			free(idi->img.pbmp);
			break;
		case IMG_TYPE_ICON:
			DestroyIcon(idi->img.hIcon);
			break;
		case IMG_TYPE_MYBITMAP:
			UnloadMyBitmap(idi->img.pmybmp);
			free(idi->img.pmybmp);
			break;
		}
	}
#if MINIGUI_MAJOR_VERSION < 3
	else if(idi->flag == IMG_FLAG_RELEASE_RES)
	{
		ReleaseRes(idi->img.key);
	}
#endif

	ncsInitDrawInfo(idi);
}

static int get_image_type_by_extend(const char* extend)
{
	if(extend == NULL)
		return IMG_TYPE_UNKNOWN;

	//is icon?
	if(strcasecmp(extend, "ico") == 0)
		return IMG_TYPE_ICON;
	else
	{
		static const char* support_images [] = {
			"bmp",
#ifdef _MGIMAGE_PNG
			"png",
#endif
#ifdef _MGIMAGE_JPG
			"jpg", "jpeg",
#endif
#ifdef _MGIMAGE_GIF
			"gif",
#endif
#ifdef _MGIMAGE_LBM
			"lbm",
#endif
#ifdef _MGIMAGE_PCX
			"pcx",
#endif
#ifdef _MGIMAGE_TGA
			"tga",
#endif
			NULL
		};
		int i;

		for(i = 0; support_images[i]; i++)
		{
			if(strcasecmp(support_images[i], extend) == 0)
				return IMG_TYPE_BITMAP;
		}
	}


	return IMG_TYPE_UNKNOWN;
}

BOOL ncsSetImageDrawInfoByFile(ImageDrawInfo *idi, const char* image_file, int drawMode, BOOL bas_mybitmp)
{
	const char* strExtend = NULL;

	if(idi == NULL || image_file == NULL)
		return FALSE;

	ncsCleanImageDrawInfo(idi);

	strExtend = strrchr(image_file, '.');
	if(strExtend == NULL)
		return FALSE;

	idi->img_type = get_image_type_by_extend(strExtend+1);

	if(idi->img_type == IMG_TYPE_UNKNOWN)
		return FALSE;

	switch(idi->img_type)
	{
	case IMG_TYPE_BITMAP:
		if(bas_mybitmp)
		{
			idi->img.pmybmp = (PMYBITMAP)calloc(1, sizeof(MYBITMAP));
			if(LoadMyBitmapFromFile(idi->img.pmybmp, NULL, image_file) != 0)
			{
				free(idi->img.pmybmp);
				ncsInitDrawInfo(idi);
				return FALSE;
			}
			idi->img_type = IMG_TYPE_MYBITMAP;
		}
		else
		{
			idi->img.pbmp = (PBITMAP)calloc(1, sizeof(BITMAP));
			if(LoadBitmapFromFile(HDC_SCREEN, idi->img.pbmp, image_file) != 0)
			{
				free(idi->img.pbmp);
				ncsInitDrawInfo(idi);
				return FALSE;
			}
		}
		break;
	case IMG_TYPE_ICON:
		idi->img.hIcon = LoadIconFromFile(HDC_SCREEN, image_file, 0);
		break;

	}

	idi->flag = IMG_FLAG_UNLOAD;

	idi->drawMode = drawMode;

	return TRUE;

}


BOOL ncsSetImageDrawInfoByRes(ImageDrawInfo *idi, RES_KEY key, int drawMode, int res_type)
{
	if(!idi || key == RES_KEY_INVALID)
		return FALSE;

	ncsCleanImageDrawInfo(idi);

	switch(res_type)
	{
	case RES_TYPE_IMAGE:
		idi->img_type = IMG_TYPE_BITMAP;
		break;
	case RES_TYPE_MYBITMAP:
		idi->img_type = IMG_TYPE_MYBITMAP;
		break;
	case RES_TYPE_ICON:
		idi->img_type = IMG_TYPE_ICON;
		break;
	default:
		return FALSE;
	}

	idi->img.key = key;

	idi->flag = IMG_FLAG_RELEASE_RES;

	idi->drawMode = drawMode;

	return TRUE;
}

BOOL ncsSetImageDrawInfo(ImageDrawInfo *idi, void* pimg, int drawMode, int img_type)
{
	if(idi == NULL )
		return FALSE;

	if(img_type <= 0 && img_type > IMG_TYPE_MYBITMAP)
		return FALSE;

	ncsCleanImageDrawInfo(idi);

	idi->drawMode = drawMode;

	if(pimg != NULL)
	{
		idi->img_type = img_type;
		idi->img.pbmp = (PBITMAP)pimg;
		idi->flag = IMG_FLAG_IGNORE;
	}

	return TRUE;
}


static void draw_my_bitmap(HDC hdc, PMYBITMAP pmybmp, const RECT *prc, int mode, int align, int valign);

static void draw_icon(HDC hdc, HICON hIcon, const RECT *prc, int algin, int mode,int valign);

void ncsImageDrawInfoDraw(ImageDrawInfo *idi, HDC hdc, const RECT* prc, int align, int valign)
{
	void * img;
	if(idi == NULL || hdc == 0 || prc == NULL)
		return;


	img = (void*)(idi->flag != IMG_FLAG_RELEASE_RES?idi->img.pbmp:GetResource(idi->img.key));

	if(img == NULL)
		return ;

	switch(idi->img_type)
	{
	case IMG_TYPE_BITMAP:
		{
			ncsDrawImage(hdc, (PBITMAP)img,	prc, idi->drawMode, align, valign);
		}
		break;
	case IMG_TYPE_MYBITMAP:
		draw_my_bitmap(hdc, (PMYBITMAP)img, prc, idi->drawMode, align, valign);
		break;
	case IMG_TYPE_ICON:
		draw_icon(hdc, (HICON)img, prc, idi->drawMode, align, valign);
		break;
	}

}

static void draw_my_bitmap(HDC hdc, PMYBITMAP pmybmp, const RECT *rc, int mode, int align, int valign)
{
	if(!pmybmp)
		return;

	switch(mode)
	{
	case NCS_DM_SCALED:
		if(RECTWP(rc) == pmybmp->w && RECTHP(rc) == pmybmp->h)
			FillBoxWithMyBitmap(hdc, rc->left, rc->top, pmybmp, NULL);
		else
		{
			BITMAP bmp;
			if(ExpandMyBitmap(hdc, &bmp, pmybmp, NULL, 0)!=0) //failed
				return ;

			FillBoxWithBitmap(hdc, rc->left, rc->top,
				RECTWP(rc), RECTHP(rc), &bmp);

			UnloadBitmap(&bmp);
		}
		break;
	case NCS_DM_TILED:
		{
			int w = pmybmp->w;
			int h = pmybmp->h;
			int x, y;
			FillBoxWithMyBitmap(hdc, rc->left, rc->top, pmybmp, NULL);
			for(x = rc->left + w; x < rc->right; x += w)
			{
				for(y = rc->top; y < rc->bottom; y+= h)
				{
					BitBlt(hdc, rc->left, rc->top, w, h, hdc, x, y, 0);
				}
			}
		}
		break;
	case NCS_DM_NORMAL:
	default:
		{
			int w = pmybmp->w;
			int h = pmybmp->h;
			int x, y;

			calc_pos_align(&x, &y, rc, w, h, align, valign);

			FillBoxWithMyBitmap(hdc, x, y, pmybmp, NULL);
		}
		break;
	}
}

static void draw_icon(HDC hdc, HICON hIcon, const RECT *rc,int mode, int align, int valign)
{
	if(!hIcon)
		return;

	switch(mode)
	{
	case NCS_DM_SCALED:
		DrawIcon(hdc, rc->left, rc->top, RECTWP(rc), RECTHP(rc), hIcon);
		break;

	case NCS_DM_TILED:
		{
			int w, h, x, y;
			GetIconSize(hIcon, &w, &h);
			for(x = rc->left; x < rc->right; x += w)
			{
				for(y = rc->top; y < rc->bottom; y += h)
				{
					DrawIcon(hdc, x, y, w, h, hIcon);
				}
			}
		}
		break;
	case NCS_DM_NORMAL:
	default:
		{
			int w, h, x, y;
			GetIconSize(hIcon, &w, &h);

			calc_pos_align(&x, &y, rc,  w, h, align, valign);

			DrawIcon(hdc, x, y, w, h, hIcon);
		}
		break;
	}
}


BOOL ncsImageDrawInfoGetImageSize(ImageDrawInfo *idi, int *px, int *py)
{
	if(!idi || idi->img.pbmp == NULL || !px || !py)
		return FALSE;

	switch(idi->img_type)
	{
	case IMG_TYPE_BITMAP:
		*px = idi->img.pbmp->bmWidth;
		*py = idi->img.pbmp->bmHeight;
		return TRUE;
	case IMG_TYPE_MYBITMAP:
		*px = idi->img.pmybmp->w;
		*py = idi->img.pmybmp->h;
		return TRUE;
	case IMG_TYPE_ICON:
		GetIconSize(idi->img.hIcon, px, py);
		return TRUE;
	}
	return FALSE;
}
