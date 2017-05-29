
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCS_RDR_SKIN
extern mWidgetRenderer skin_widget_renderer;
extern mGroupBoxRenderer skin_groupbox_renderer;
extern mComboboxRenderer skin_combobox_renderer;
extern mPropSheetRenderer skin_propsheet_renderer;
extern mListViewRenderer skin_listview_renderer;

#define RDR_ENTRY(CLASSNAME, classname) \
	{ NCSCTRL_##CLASSNAME, (mWidgetRenderer*)(void*)(&(skin_##classname##_renderer))}

extern void skin_init_boxpiece_renderer(void);

BOOL ncsInitSkinRenderers(void)
{
	int i;
	NCS_RDR_ENTRY entries [] = {
		RDR_ENTRY(WIDGET,widget),
		RDR_ENTRY(COMBOBOX, combobox),
		RDR_ENTRY(GROUPBOX,groupbox),
		RDR_ENTRY(STATIC,widget),
		RDR_ENTRY(PROPSHEET, propsheet),
		RDR_ENTRY(LISTVIEW, listview),
	};

	for(i=0; i< sizeof(entries)/sizeof(NCS_RDR_ENTRY); i++)
	{
		entries[i].renderer->class_init(entries[i].renderer);
		if(entries[i].renderer->init_self)
			entries[i].renderer->init_self(entries[i].renderer);
	}

	skin_init_boxpiece_renderer();

	return ncsRegisterCtrlRDRs("skin", entries,
            sizeof(entries)/sizeof(NCS_RDR_ENTRY));
}

    
static PBITMAP get_sub_bitmap (const DRAWINFO *di)
{
    Uint8  *start_bits;
    int     i, w, h, pitch;
    PBITMAP _sbmp = NULL;

    if (di->nr_col <= 0 || di->nr_line <= 0){
        return NULL;
    }

    if (NULL == (_sbmp = (PBITMAP)calloc (1, sizeof(BITMAP)))){
        fprintf (stderr, "no memory for sub bmp in skin LF.");
        return NULL;
    }

	memcpy(_sbmp, di->bmp, sizeof(BITMAP));

    w = di->bmp->bmWidth / di->nr_col;
    h = di->bmp->bmHeight / di->nr_line; 
	pitch = di->bmp->bmPitch / di->nr_col;

	_sbmp->bmWidth = w;
	_sbmp->bmHeight = h;

    start_bits = di->bmp->bmBits 
        + h * di->idx_line * di->bmp->bmPitch 
        + w * di->idx_col * di->bmp->bmBytesPerPixel;

	_sbmp->bmBits = start_bits;

	if(_sbmp->bmType & BMP_TYPE_ALPHA_MASK) {
		int pitch = ((w + 3) & ~3);
		_sbmp->bmAlphaMask = calloc(1, h * pitch);
		start_bits = di->bmp->bmAlphaMask 
			+ h * di->idx_line * ((di->bmp->bmWidth + 3) & ~3)
			+ w * di->idx_col;
		for( i = 0; i < h; i ++) {
			memcpy(_sbmp->bmAlphaMask + pitch * i , start_bits, w);
			start_bits += ((di->bmp->bmWidth + 3) & ~3);
		}
	}

    if (di->flip) {
        VFlipBitmap (_sbmp, _sbmp->bmBits);
    }

    return _sbmp;
}

static void unload_sub_bitmap (PBITMAP sub_bmp)
{
    if (sub_bmp)
    {
		sub_bmp->bmBits = NULL;
        UnloadBitmap (sub_bmp);
        free (sub_bmp);
        sub_bmp = NULL;
    }
    return;
}

void ncsSkinDraw(HDC hdc, const RECT* rc, const DRAWINFO *di)
{
    BOOL    _sub = FALSE;
    PBITMAP sub_bmp = NULL;
    int     subw = 0, subh = 0;
    int     new_left = 0, new_top=  0;
    int     margin1 = di->margin1, margin2 = di->margin2;

    if (di->nr_col > 1 || di->nr_line > 1){
        _sub = TRUE;
        sub_bmp = get_sub_bitmap (di);
    } else {
        _sub = FALSE;
        sub_bmp = (PBITMAP) di->bmp;
    }

    if (di->direct){
        if (di->style == DI_FILL_STRETCH) {
            FillBoxWithBitmap (hdc, rc->left, rc->top, 
                    RECTWP(rc), RECTHP(rc), sub_bmp);
        }
        else {/*tile*/
            /*draw up*/
            if (margin1)
                FillBoxWithBitmapPart (hdc, rc->left, rc->top, 
                        RECTWP(rc), di->margin1,
                        RECTWP(rc), 0, sub_bmp, 0, 0);

            /*draw center*/
            if (0 != di->nr_line)
                subh = di->bmp->bmHeight / di->nr_line; 
            
            new_top = rc->top + margin1;

            do {
                if (new_top + subh - margin1 - margin2 <= rc->bottom - margin2)
                    FillBoxWithBitmapPart (hdc, rc->left, new_top, 
                            RECTWP(rc), subh - margin1 - margin2,
                            RECTWP(rc), 0, sub_bmp, 0, margin1);
                else
                    FillBoxWithBitmapPart (hdc, rc->left, new_top, 
                            RECTWP(rc), rc->bottom - margin2 - new_top,
                            RECTWP(rc), 0, sub_bmp, 0, margin1);

                new_top += subh - margin1 - margin2;
            } while (new_top < rc->bottom-margin2);

            /*draw down*/
            if (margin2)
                FillBoxWithBitmapPart (hdc, rc->left, rc->bottom - di->margin2, 
                        RECTWP(rc), margin2, 
                        RECTWP(rc), 0, sub_bmp, 0, subh -margin2);
        }
    } else{
        if (di->style == DI_FILL_STRETCH) {
            FillBoxWithBitmap (hdc, rc->left, rc->top, 
                    RECTWP(rc), RECTHP(rc), sub_bmp);
        }
        else {/*tile*/
            /*draw left*/
            if (margin1)
                FillBoxWithBitmapPart (hdc, rc->left, rc->top, 
                        di->margin1, RECTHP(rc), 0, RECTHP(rc), sub_bmp, 0, 0);

            /*draw center*/
            if (0 != di->nr_col)
                subw = di->bmp->bmWidth / di->nr_col;
            
            new_left = rc->left + margin1;

            do {
                if ((new_left + subw - margin1 - margin2) <= rc->right - margin2)
                    FillBoxWithBitmapPart (hdc, new_left, rc->top, 
                            subw-margin1-margin2, RECTHP(rc),
                            0, RECTHP(rc), sub_bmp, margin1, 0);
                else
                    FillBoxWithBitmapPart (hdc, new_left, rc->top, 
                            rc->right - margin2 - new_left, RECTHP(rc),
                            0, RECTHP(rc), sub_bmp, margin1, 0);

                new_left += subw - margin1 - margin2;
            } while (new_left < rc->right-margin2);

            /*draw right*/
            if (margin2)
                FillBoxWithBitmapPart (hdc, rc->right - di->margin2, rc->top, 
                        di->margin2, RECTHP(rc), 
                        0, RECTHP(rc), sub_bmp, (subw -margin2), 0);
        }
    }

    if (_sub)
        unload_sub_bitmap (sub_bmp);

    return;
}
#endif

