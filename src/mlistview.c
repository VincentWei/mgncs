/*
 ** $Id: mlistview.c 1681 2017-10-26 06:46:31Z weiym $
 **
 ** The implementation of mListView class.
 **
 ** Copyright (C) 2009 Feynman Software.
 **
 ** All rights reserved by Feynman Software.
 */

#include <stdio.h>
#include <stdlib.h>
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
#include "mdblist.h"
#include "mitem.h"
#include "mscroll_widget.h"
#include "mitem_manager.h"
#include "mitemview.h"
#include "mscrollview.h"
#include "mlistcolumn.h"
#include "mlistitem.h"
#include "mlistview.h"

#include "mtype.h"

#ifdef _MGNCSDB_DATASOURCE
#include "mdatabinding.h"

#include "mdatasource.h"
#endif

#include "mrdr.h"

#ifdef _MGNCSCTRL_LISTVIEW

#define pHdrList (self->hdrList)

#define DEF_ICON_WIDTH  48
#define DEF_ICON_HEIGHT 48

#define LVST_NORMAL	    0x0000    //normal status
#define LVST_BDDRAG	    0x0001    //the border is being dragged
#define LVST_HEADCLICK	0x0002    //the header is being clicked
#define LVST_INHEAD	    0x0004    //mouse move in header
#define LVST_ITEMDRAG   0x0008    //mouse drag item
#define LVST_CAPTURED   0x0010    //captured status
#define LVST_OPERATECOL     0x1000
#define LVST_HIDEHEAD       0x2000
#define LVST_HIDEGRIDLINE   0x4000

#define INDENT 20
#define LV_HDR_TOP  0
#define LV_HDR_HEIGHT  (self->hdrH)
#define DF_3DBOX_NORMAL     0x0000
#define DF_3DBOX_PRESSED    0x0001
#define DF_3DBOX_STATEMASK  0x000F
#define DF_3DBOX_NOTFILL    0x0000
#define DF_3DBOX_FILL       0x0010

static BOOL _get_subitem_rect (mListColumn *colItem, RECT *rc, RECT *rcItem)
{
    if (!colItem || !rc || !rcItem)
        return FALSE;

    rc->top = rcItem->top;
    rc->bottom = rcItem->bottom;
    rc->left = rcItem->left + colItem->startX;
    rc->right = rc->left + colItem->width;

    return TRUE;
}

static BOOL mListView_getSubItemRect(mListView *self,
        HITEM rowItem, int row, int col, RECT *rc)
{
    mListItem *pRow = (mListItem *)rowItem;
    RECT rcItem;
    mListColumn *pCol;

    if (!self || !rc)
        return FALSE;

    if (!pRow)
        pRow = (mListItem*)_c(self)->getItem(self, row);

    if (!pRow)
        return FALSE;

    pCol = (mListColumn *) _c(self)->getColumn(self, col);
    if (!pCol)
        return FALSE;

    if (_c(self)->getRect(self, (HITEM)pRow, &rcItem, TRUE))
        return FALSE;

    return _get_subitem_rect(pCol, rc, &rcItem);
}

static inline void _drawGridLine(mListView *self, HDC hdc, RECT  *rcDraw)
{
    DWORD style = GetWindowStyle(self->hwnd);
    int oldWidth;
    gal_pixel oldColor;

    if ((style & NCSS_LISTV_TREE)
            || (self->status & LVST_HIDEGRIDLINE)
            || (self->gridLineWidth <= 0)) {
        return;
    }

    oldWidth = SetPenWidth(hdc, self->gridLineWidth);
    oldColor = SetPenColor(hdc, DWORD2PIXEL(hdc, self->gridLineColor));
    //Rectangle (hdc, rcDraw->left, rcDraw->top, rcDraw->right, rcDraw->bottom);
    LineEx(hdc, rcDraw->left, rcDraw->top, rcDraw->right, rcDraw->top);
    LineEx(hdc, rcDraw->right, rcDraw->top, rcDraw->right, rcDraw->bottom);

    LineEx(hdc, rcDraw->left, rcDraw->top, rcDraw->left, rcDraw->bottom);
    LineEx(hdc, rcDraw->left, rcDraw->bottom, rcDraw->right, rcDraw->bottom);
    SetPenColor(hdc, oldColor);
    SetPenWidth(hdc, oldWidth);
}

static inline void
_lv_calc_vtext_rect(HDC hdc, const char* text, RECT *rcText, int valign)
{
    RECT rcOut;
    int offset;

    if (valign == 0 || !text || !rcText)
        return;

    CopyRect(&rcOut, rcText);
    DrawText (hdc, text, -1, &rcOut, DT_WORDBREAK | DT_CALCRECT);

    offset = RECTHP(rcText) - RECTH(rcOut);
    if ((valign & NCSF_LSTCLM_BOTTOMALIGN)
            || (valign & NCSF_LSTHDR_BOTTOMALIGN)) {
        rcText->top += (offset > 0 ? offset : 0);
    }
    else if ((valign & NCSF_LSTCLM_VCENTERALIGN)
            || (valign & NCSF_LSTHDR_VCENTERALIGN)) {
        rcText->top += (offset>>1);
    }
}

static void _lv_draw_subitem (mListView* self, HITEM hItem,
        HDC hdc, RECT *rcDraw, int col, DWORD fg_color, BOOL select)
{
    mListItem   *rowItem = (mListItem*)hItem;
    HITEM       subItem;
    mListColumn *colItem;
    RECT        rcSubItem, rcSubItemText;
    UINT        text_format;
    int         nItemHeight, row;
    int         color;
    gal_pixel   oldColor = 0;
    int         ret;
	DWORD		image;
	const char* text;

    subItem = _c(rowItem)->getItem(rowItem, col);
    if (!subItem)
        return;

    SetBkMode (hdc, BM_TRANSPARENT);

    colItem = (mListColumn *) _c(self)->getColumn(self, col);
    _get_subitem_rect(colItem, &rcSubItem, rcDraw);

	if (!DoesIntersect(&rcSubItem, rcDraw))
		return;

    CopyRect (&rcSubItemText, &rcSubItem);
    row = _c(self)->indexOf(self, hItem);

    if (col == 0) {
        //draw whole item background
        if (self->drawItemBk)
            self->drawItemBk(self, hItem, hdc, rcDraw);
        else {
            if (select) //draw hilight item bgc
                self->renderer->drawItem(self, hdc, rcDraw, NCSRS_HIGHLIGHT);
            else if (!(GetWindowExStyle(self->hwnd) & WS_EX_TRANSPARENT)) {
                if (!_c(self)->getBackground(self, row, -1, &color)) {
                    //draw custom item bgc
                    SetBrushColor(hdc, DWORD2PIXEL(hdc, color));
                    FillBox(hdc, rcDraw->left, rcDraw->top,
                            RECTWP(rcDraw), RECTHP(rcDraw));
                }
                else //draw normal item bgc
                    self->renderer->drawItem(self, hdc, rcDraw, NCSRS_NORMAL);
            }
        }
    }

    if (!select && !(GetWindowExStyle(self->hwnd) & WS_EX_TRANSPARENT)) {
        if (!_c(self)->getBackground(self, row, col, &color)) {
            //draw custom item bgc
            SetBrushColor(hdc, DWORD2PIXEL(hdc, color));
            FillBox(hdc, rcSubItem.left, rcSubItem.top,
                    RECTW(rcSubItem), RECTH(rcSubItem));
        }
    }

    _drawGridLine(self, hdc, &rcSubItem);

    rcSubItemText.left += 2;
    rcSubItemText.right -= 2;
    rcSubItemText.top += 2;
    rcSubItemText.bottom -= 2;

    if (col == 0) {
		  rcSubItemText.left += _c(self)->getColumn(self, rowItem->depth)->startX;
        if (_c(self)->getChildCount(self, hItem) > 0) {
            RECT rcFold;
            DWORD flags = LFRDR_TREE_CHILD;

            if (self->renderer) {
                if (_c(rowItem)->isFold(rowItem))
                    flags |= LFRDR_TREE_FOLD;

                //draw fold
                rcFold.left     = rcSubItemText.left-1;
                rcFold.right    = rcFold.left + 10;
                rcFold.top      = rcSubItemText.top  + 6;
                rcFold.bottom   = rcFold.top + 10;

                if (GetWindowStyle(self->hwnd) & NCSS_LISTV_WITHICON) {
                    flags |= LFRDR_TREE_WITHICON;
                    }

                self->renderer->drawFold (self, hdc, &rcFold, fg_color,
                    (flags&LFRDR_TREE_FOLD ? rowItem->foldIcon : rowItem->unfoldIcon),
                    flags, rowItem->nrChild);
                }
        }
    }

    //draw custom item
    if (self->drawSubItem) {
        self->drawSubItem ((mItemView*)self, subItem, hdc, &rcSubItem);
        return;
    }

    nItemHeight = _c(self)->getItemHeight(self, hItem);
    image = _c(rowItem)->getImage(rowItem, subItem);

    //draw image
    if (image) {
        DWORD flags = _c(rowItem)->getFlags(rowItem, subItem);
        int width, height;

        if (flags & NCSF_ITEM_USEBITMAP) {
            PBITMAP bmp = (PBITMAP)image;
            height = (bmp->bmHeight < nItemHeight)? bmp->bmHeight : nItemHeight;
            rcSubItem.top += (nItemHeight - height) / 2;
            rcSubItem.left += 2;
            FillBoxWithBitmap (hdc, rcSubItem.left, rcSubItem.top, 0, height, bmp);
            rcSubItemText.left = rcSubItem.left + bmp->bmWidth + 2;
        }
        else if (flags & NCSF_ITEM_USEICON) {
            GetIconSize ((HICON)image, &width, &height);
            height = (height < nItemHeight) ? height : nItemHeight;
            rcSubItem.top += (nItemHeight - height) / 2;
            rcSubItem.left += 2;
            DrawIcon (hdc, rcSubItem.left, rcSubItem.top, 0, height,
                    (HICON)image);
            rcSubItemText.left = rcSubItem.left + width + 2;
        }
    }

    text = _c(rowItem)->getText(rowItem, (HITEM)subItem);
    ret = _c(self)->getForeground(self, row, col, &color);
    if (!ret) {
        oldColor = SetTextColor(hdc, DWORD2PIXEL(hdc, color));
    }

    if (text) {
        DWORD flags = _c(pHdrList)->getFlags(pHdrList, (HITEM)colItem);

        _lv_calc_vtext_rect(hdc, text, &rcSubItemText, flags & NCSF_LSTCLM_VALIGNMASK);

        text_format = DT_WORDBREAK;
        //horizontal alignment
        if (flags & NCSF_LSTCLM_RIGHTALIGN)
            text_format |= DT_RIGHT;
        else if (flags & NCSF_LSTCLM_CENTERALIGN)
            text_format |= DT_CENTER;
        else
            text_format |= DT_LEFT;
        DrawText (hdc, text, -1, &rcSubItemText, text_format);
    }

    if (!ret)
        SetTextColor(hdc, oldColor);
}

static void _lv_draw_item (mItemView *self, HITEM hItem, HDC hdc, RECT *rcDraw)
{
    int         i, count;
    mListItem   *row = (mListItem *)hItem;
    mListView   *lv_self;
    DWORD       fg_color;
    gal_pixel   fc;
    BOOL        isSelected = FALSE;

    if (!rcDraw || !RECTHP(rcDraw) || !row)
        return;

    lv_self = (mListView*)self;
    if (!lv_self)
        return;

    count = _c(lv_self)->getColumnCount(lv_self);

    /*set basic info*/
    if (!_c(lv_self)->isSelected(lv_self, hItem)) {
        fg_color = ncsGetElement((mWidget*)lv_self, NCS_FGC_WINDOW);
    }
    else {
        isSelected = TRUE;
        fg_color = ncsGetElement((mWidget*)lv_self, NCS_FGC_HILIGHT_ITEM);
    }

    fc = RGBA2Pixel (hdc, GetRValue(fg_color), GetGValue(fg_color),
            GetBValue(fg_color), GetAValue(fg_color));
    SetTextColor (hdc, fc);

    for (i = 0; i < count; i++) {
        _lv_draw_subitem (lv_self, hItem, hdc, rcDraw, i, fg_color, isSelected);
    }
}

static void _draw_def_header (mListView *self, HDC hdc)
{
    mListColumn *pListColumn = NULL;
    list_t *me, *first;
    int item_w = 0;
    DWORD fg_color;
    gal_pixel fc;
    RECT rcClient, rcButton, rcText;
    DWORD flags = NCSRS_NORMAL;
    UINT format;
    char *text;

    GetClientRect (self->hwnd, &rcClient);

    if (self->status & LVST_HEADCLICK && self->status & LVST_INHEAD)
        flags = NCSRS_HIGHLIGHT;

    SetBkMode (hdc, BM_TRANSPARENT);

    first = _c(pHdrList)->getQueue(pHdrList);

    for (me = first->next; me != first; me = me->next) {
        pListColumn = (mListColumn*)_c(pHdrList)->getListEntry(me);
        rcButton.left   = pListColumn->startX;
        rcButton.top    = LV_HDR_TOP;
        rcButton.right  = pListColumn->startX + pListColumn->width;
        rcButton.bottom = LV_HDR_TOP + LV_HDR_HEIGHT;

        _c(self)->contentToWindow(self, &rcButton.left, NULL);
        _c(self)->contentToWindow(self, &rcButton.right, NULL);

        fg_color = ncsGetElement((mWidget*)self, NCS_BGC_3DBODY);
        self->renderer->drawHeader (self, hdc, &rcButton, fg_color, flags);

        item_w += pListColumn->width;

        if (pListColumn->flags & NCSF_LSTHDR_CENTERALIGN)
            format = DT_CENTER;
        else if (pListColumn->flags & NCSF_LSTHDR_RIGHTALIGN)
            format = DT_RIGHT;
        else
            format = DT_LEFT;

        fg_color = ncsGetElement((mWidget*)self, NCS_FGC_WINDOW);
        fc = RGBA2Pixel (hdc, GetRValue(fg_color), GetGValue(fg_color),
                GetBValue(fg_color), GetAValue(fg_color));
        SetTextColor (hdc, fc);

        SetRect (&rcText, pListColumn->startX - self->contX + 4,
                        LV_HDR_TOP+2,
                        pListColumn->startX - self->contX -2 + pListColumn->width,
                        LV_HDR_TOP+LV_HDR_HEIGHT);

        text = _c(pListColumn)->getItemString(pListColumn);
        _lv_calc_vtext_rect(hdc, text, &rcText, pListColumn->flags & NCSF_LSTHDR_VALIGNMASK);
        DrawText (hdc, text, -1, &rcText, format);
    }

    /*draws the right most unused header*/
    rcButton.left   = item_w - self->contX + 1;
    rcButton.top    = LV_HDR_TOP;
    rcButton.right  = rcClient.right + 2;
    rcButton.bottom = LV_HDR_TOP + LV_HDR_HEIGHT;
    fg_color = ncsGetElement((mWidget*)self, NCS_BGC_3DBODY);
    self->renderer->drawHeader (self, hdc, &rcButton, fg_color, flags);
}

static void _draw_custom_header(mListView *self, HDC hdc)
{
    RECT rcClient, rcHdr, rcItem;
    int i = 0;
    mListColumn *pListColumn = NULL;
    list_t *me, *first;

    GetClientRect (self->hwnd, &rcClient);

    CopyRect(&rcHdr, &rcClient);
    CopyRect(&rcItem, &rcHdr);
    rcHdr.top = LV_HDR_TOP;
    rcHdr.bottom = LV_HDR_TOP + LV_HDR_HEIGHT;

    /* drawing header background */
    self->drawHdrBk (self, 0, hdc, &rcHdr);

    /* drawing header items */
    first = _c(pHdrList)->getQueue(pHdrList);
    for (me = first->next; me != first; me = me->next) {
        pListColumn = (mListColumn*)_c(pHdrList)->getListEntry(me);

        rcItem.left = pListColumn->startX;
        rcItem.right = pListColumn->startX + pListColumn->width;

        _c(self)->contentToWindow(self, &rcItem.left, NULL);
        _c(self)->contentToWindow(self, &rcItem.right, NULL);
        self->drawHdrItem (self, (HITEM)pListColumn, hdc, &rcItem);
        i ++;
    }
}

static void mListView_drawHeader (mListView *self, HDC hdc)
{
    if ((self->status & LVST_HIDEHEAD) || LV_HDR_HEIGHT == 0) {
        return;
    }

    if (self->drawHdrBk && self->drawHdrItem) {
        _draw_custom_header(self, hdc);
    }
    else {
        _draw_def_header(self, hdc);
    }
}

#define LEFTMARGIN      1
#define TOPMARGIN       1
#define RIGHTMARGIN     1
#define BOTTOMMARGIN    1

#define root    (self->rootItem)
static BOOL _init_root_item (mListView* self)
{
    if (!root) {
        self->status &= ~LVST_OPERATECOL;
        root = (mListItem *)NEW(mListItem);
        self->status |= LVST_OPERATECOL;
    }

    if (!root)
        return FALSE;

    root->flags = 0;
    root->nrChild = 0;
    root->parent = 0;
    root->depth = -1;

    return TRUE;
}

static void _destroy_root_item (mListView* self)
{
    self->status &= ~LVST_OPERATECOL;
    DELETE((mItem*)root);
    self->status |= LVST_OPERATECOL;
}

static int
_def_compare_item (mListView* self, HITEM hItem1, HITEM hItem2, int ncol)
{
    mListItem *p1 = (mListItem*)hItem1;
    mListItem *p2 = (mListItem*)hItem2;
    mItem *psub1, *psub2;
    NCS_CB_CMPSTR strCmp;

    psub1 = (mItem*)_c(p1)->getItem(p1, ncol);
    psub2 = (mItem*)_c(p2)->getItem(p2, ncol);

    if (psub1 == NULL || psub2 == NULL)
        return -1;

    strCmp = _c(self)->getStrCmpFunc(self);

    if (strCmp)
        return strCmp (_c(p1)->getText(p1, (HITEM)psub1),
                    _c(p2)->getText(p2, (HITEM)psub2), (size_t)-1);
    else
        return strcasecmp(_c(p1)->getText(p1, (HITEM)psub1),
                    _c(p2)->getText(p2, (HITEM)psub2));
}

static int _lv_cmp_func (mItemManager *manager, HITEM hItem1, HITEM hItem2)
{
    mListView *self;
    int ret;
    NCS_LSTCLM_SORTDATA sortdata;

    if (!manager)
        return -1;

    self = (mListView*)(manager->obj);
    if (!self)
        return -1;

    if (self->pfnSort) {
        sortdata.column = 0;
        sortdata.losorted = 0;
        sortdata.obj = manager->obj;
        ret = self->pfnSort(hItem1, hItem2, &sortdata);
        if (ret == 0)
            ret = _def_compare_item (self, hItem1, hItem2, 1);
    }
    else {
        ret = _def_compare_item (self, hItem1, hItem2, self->curCol);
    }

    if ( (self->sort == NCSID_LSTCLM_LOSORTED && ret < 0) ||
            (self->sort == NCSID_LSTCLM_HISORTED && ret > 0) )
        return -1;
    return 1;
}

static void mListView_construct (mListView *self, DWORD addData)
{
	g_stmItemViewCls.construct((mItemView*)self, addData);

    LV_HDR_HEIGHT       = GetWindowFont(self->hwnd)->size + 6;
    self->hdrW          = 0;

    /*inherited from mItemView */
    self->defItemHeight = LV_HDR_HEIGHT;

    self->dragX         = 0;
    self->sort          = NCSID_LSTCLM_LOSORTED;
    self->curCol        = 0;
    self->pfnSort       = NULL;
    self->gridLineWidth = 0;
    self->gridLineColor = 0x777777;

    self->dragedHead    = -1;
    self->clickedHead   = NULL;
    self->status        = LVST_NORMAL;
    self->drawHdrBk     = NULL;
    self->drawHdrItem   = NULL;
    self->drawItemBk    = NULL;
    self->drawSubItem   = NULL;

    /* inherited from mItemView */
    self->itemOps.drawItem = _lv_draw_item;

    /* inherited from mScrollWidget */
    _c(self)->initMargins(self, LEFTMARGIN,
            LV_HDR_HEIGHT+TOPMARGIN, RIGHTMARGIN, BOTTOMMARGIN);
    self->vStepVal      = self->defItemHeight;

    //mItemManager
    pItemList->strCmp   = strncmp;
    root                = NULL;
    pItemList->itemCmp  = _lv_cmp_func;
    pHdrList            = (mItemManager*)NEW(mItemManager);
}

static void mListView_destroy(mListView* self)
{
    Class(mItemView).destroy((mItemView*)self);

    if (pHdrList)
        DELETE((mItem*)pHdrList);

    _destroy_root_item(self);
}
/*============= item =================== */
static int _getSubItems(mListView *self, mListItem *parent,
        int index, HITEM *pPrev, HITEM *pNext)
{
    int i = 0;
    HITEM first = 0, next = 0;

    if (!parent || parent->nrChild == 0
            || index > parent->nrChild)
        return -1;

    if (!pPrev && !pNext)
        return -1;

	if (index < 0)
		index = parent->nrChild;

    first = _M(self, getRelatedItem, (HITEM)parent, NCSID_LISTV_IR_FIRSTCHILD);

    if (index == 0) {
        if (pPrev)
            *pPrev = 0;
        if (pNext)
            *pNext = first;
    } else if (index == parent->nrChild) {
		if (pPrev)
			*pPrev = _M(self, getRelatedItem, (HITEM)parent, NCSID_LISTV_IR_LASTCHILD);
		if (pNext)
			*pNext = 0;
	} else {
        while (first) {
            next = _M(self, getRelatedItem, first, NCSID_LISTV_IR_NEXTSIBLING);
            if (i == index - 1)
                break;

            i++;
            first = next;
        }

        if (pPrev)
            *pPrev = first;

        if (pNext)
            *pNext = next;
    }

    return 0;
}

static void _get_sort_tree_pos (mListView *self,
        mListItem *parent, const char* text, HITEM *prev, HITEM *next)
{
    HITEM first = 0, tmp = 0;
    char *string;
    NCS_CB_CMPSTR strCmp;
    int ret, row;

    if (!self || !parent || !text ||parent->nrChild <= 0)
        return;

    strCmp = _c(self)->getStrCmpFunc(self);
    if (!strCmp)
        return;

    first = _c(self)->getRelatedItem(self, (HITEM)parent, NCSID_LISTV_IR_FIRSTCHILD);

    while (first) {

        tmp = first;

        row = _c(self)->getItem(self, first);
        string = (char*)_c(self)->getItemText(self, row, self->curCol);
        ret = strCmp(text, string, -1);

        if ( (self->sort == NCSID_LSTCLM_LOSORTED && ret < 0) ||
                (self->sort == NCSID_LSTCLM_HISORTED && ret > 0) ) {
            break;
        }

        first = _c(self)->getRelatedItem(self, first, NCSID_LISTV_IR_NEXTSIBLING);
    }

    if (prev)
        *prev = first ?
            _c(self)->getRelatedItem(self, first, NCSID_LISTV_IR_PREVSIBLING) : tmp;

    if (next)
        *next = first;
}


static char* _get_sort_column_text (mListView *self, NCS_LISTV_ITEMDATA *data)
{
    int i, column;

    column = _c(self)->getColumnCount(self);

    for (i = 0; i < column; i++) {
        if (data->col == self->curCol && data->text)
            return data->text;

        data++;
    }
    return NULL;
}

static HITEM mListView_addItem(mListView *self, NCS_LISTV_ITEMINFO *info)
{
    int         i, column, dstH;
    mListItem   *newItem;
    mItem       *subItem;
    HITEM       prev = 0, next = 0;
    BOOL        isFold;
    mListItem   *parent;
    DWORD       style;
    char*       text = NULL;
	
    if (!info || _c(self)->getColumnCount(self) <= 0)
        return 0;

    if (!root && !_init_root_item(self))
        return 0;

    parent = (mListItem*)info->parent;

    if (parent) {
        IncludeWindowStyle (self->hwnd, NCSS_LISTV_TREE);
    }
    else {
        parent = root;
    }

    style = GetWindowStyle(self->hwnd);

    //get insert position
    if ((style & NCSS_LISTV_SORT)) {
        if (NULL != (text = _get_sort_column_text(self, info->data))){
            _get_sort_tree_pos(self, parent, text, &prev, &next);
		}
    } else {
        _getSubItems(self, parent, info->index, &prev, &next);
	}

    if (next)
        prev = 0;
    else {
        if (prev)
           prev = _c(self)->getRelatedItem(self, prev, NCSID_LISTV_IR_LASTCHILD);
        else {
            prev = (parent == root) ? 0 : (HITEM)parent;
        }
    }
    if (info->height > 0)
        dstH = info->height;
    else
        dstH = self->defItemHeight;

    isFold = _c(parent)->isFold(parent);

    //create item
    self->status &= ~LVST_OPERATECOL;
    newItem = (mListItem*) _c(self)->createItem(self, prev, next,
                            info->index, isFold?0:dstH, NULL, 0, &info->retPos, TRUE);
    self->status |= LVST_OPERATECOL;

    if (!newItem)
        return 0;

    newItem->showHeight = dstH;
    newItem->nrChild = 0;
    newItem->flags = info->flags;
    newItem->addData = info->addData;
    newItem->foldIcon = info->foldIcon;
    newItem->unfoldIcon = info->unfoldIcon;


    _c((mListItem*)parent)->addChild((mListItem*)parent, newItem);

    //add sub item
    column = _c(self)->getColumnCount(self);

    for (i = 0; i < column; i++) {
        subItem = (mItem*)_c(newItem)->createItem(newItem, 0, 0, i, NULL);
        if (subItem) {
            subItem->height = dstH;

            if (info->data) {
                NCS_LISTV_ITEMDATA *data = info->data;
                int j;


                if (data->row != info->index)
                    continue;

                for (j = 0; j < MIN(info->dataSize, column); j++) {
                    if (i == data->col) {
                        if (data->flags==NCSF_ITEM_USEBITMAP || data->flags==NCSF_ITEM_USEICON) {
                            subItem->flags |= data->flags;
                            subItem->image = data->image;
                        }

                        if (data->text) {
                            _c(newItem)->setText(newItem, (HITEM)subItem, data->text);
                        }

                        if (data->textColor)
                            _c(newItem)->setForeground(newItem, data->col, &(data->textColor));
                    }
                    data++;
                }
            }
        }
    }
    //sort
    if (GetWindowStyle(self->hwnd) & NCSS_LISTV_SORT)
        _c(self)->sort(self, NULL, self->curCol, self->sort);

    return (HITEM)newItem;
}

/*=============== sub item ====================*/
static BOOL mListView_getItemInfo(mListView* self, NCS_LISTV_ITEMDATA *info)
{
    HITEM   subItem;
    mListItem *pRow = NULL;

    if (!info)
        return FALSE;


    if (!INSTANCEOF(self, mListView) || !info)
        return FALSE;

    pRow = (mListItem*)_c(self)->getItem(self, info->row);

    if (!pRow)
        return FALSE;

    subItem = _c(pRow)->getItem(pRow, info->col);

    if (!subItem)
        return FALSE;

    info->text = (char*)_c(pRow)->getText(pRow, subItem);
    _c(self)->getForeground(self, info->row,
                        info->col, &(info->textColor));
    info->flags = _c(pRow)->getFlags(pRow, subItem);
    info->image = _c(pRow)->getImage(pRow, subItem);

    return TRUE;
}

static BOOL mListView_setItemInfo(mListView* self,
        NCS_LISTV_ITEMDATA *info)
{
    HITEM   subItem;
    RECT    rcSubItem;
    mListItem *pRow = NULL;

    if (!INSTANCEOF(self, mListView) || !info)
        return FALSE;

    pRow = (mListItem*)_c(self)->getItem(self, info->row);

    if (!pRow)
        return FALSE;

    subItem = _c(pRow)->getItem(pRow, info->col);

    if (!subItem)
        return FALSE;

    if (info->text)
        _c(pRow)->setText(pRow, subItem, info->text);

    if (info->textColor)
        _c(self)->setForeground(self,
                info->row, info->col, &(info->textColor));

    if (info->flags==NCSF_ITEM_USEBITMAP || info->flags==NCSF_ITEM_USEICON) {
        ((mItem*)subItem)->flags |= info->flags;
        _c(pRow)->setImage(pRow, subItem, info->image);
    }


    if (mListView_getSubItemRect(self, (HITEM)pRow, info->row, info->col, &rcSubItem))
        InvalidateRect(self->hwnd, &rcSubItem, TRUE);
    else
        _c(self)->refreshItem(self, (HITEM)pRow, NULL);
    return TRUE;
}

static const char* mListView_getItemText(mListView* self, int row, int col)
{
    mItem* subItem;
    mListItem* pRow;

    if (!INSTANCEOF(self, mListView))
        return NULL;

    pRow = (mListItem*)_c(self)->getItem(self, row);

    if (!pRow)
        return NULL;

    subItem = (mItem*) _c(pRow)->getItem(pRow, col);

    if (subItem)
        return _c(subItem)->getItemString(subItem);

    return NULL;
}

static int mListView_getItemTextLen(mListView* self, int row, int col)
{
    const char* text;

    if (!INSTANCEOF(self, mListView))
        return -1;

    text = _c(self)->getItemText(self, row, col);

    if (text)
        return strlen(text);

    return -1;
}

static BOOL mListView_setItemText(mListView* self, int row, int col, const char* str)
{
    mItem* subItem;
    mListItem* pRow;
    BOOL ret;
    RECT rcSubItem;

    if (!INSTANCEOF(self, mListView))
        return FALSE;

    pRow = (mListItem*)_c(self)->getItem(self, row);

    if (!pRow)
        return FALSE;

    subItem = (mItem*) _c(pRow)->getItem(pRow, col);

    if (!subItem)
		return FALSE;

	ret = _c(subItem)->setItemString(subItem, str);

    if (mListView_getSubItemRect(self, (HITEM)pRow, row, col, &rcSubItem))
        InvalidateRect(self->hwnd, &rcSubItem, TRUE);
    else
        _c(self)->refreshItem(self, (HITEM)pRow, NULL);

    return ret;
}


/*=============== column ====================*/
static BOOL _get_column_rect(mListView *self, mListColumn* pCol, RECT *rc)
{
    if (rc && pCol) {
        rc->left = pCol->startX + 1;
        rc->top = LV_HDR_TOP;
        rc->right = rc->left + pCol->width - 2;
        rc->bottom = LV_HDR_HEIGHT + rc->top;
        return TRUE;
    }
    return FALSE;
}


static void mListView_showColumn(mListView* self, mListColumn *column)
{
    RECT rcItem;
    int pos_x;

    if (!column || !_get_column_rect(self, column, &rcItem))
        return;

    if (rcItem.left >= self->contX && rcItem.right <= self->contX + self->visWidth) {
        _c(self)->contentToWindow(self, &rcItem.left, &rcItem.top);
        _c(self)->contentToWindow(self, &rcItem.right, &rcItem.bottom);
        InvalidateRect(self->hwnd, &rcItem, TRUE);
        return ;
    }

    pos_x = rcItem.left;
    if (rcItem.right <= self->contX) {
        pos_x = rcItem.left;
    }
    else if (rcItem.left > self->contX) {
        pos_x = rcItem.right;
    }
    _c(self)->makePosVisible(self, pos_x, self->contY);
}

static void mListView_setSortColumn(mListView* self, mListColumn* column)
{
    int index = _c(self)->getColumnIndex(self, column);
    if (index >= 0) {
        self->curCol = index;
        _c(self)->sort(self, NULL, self->curCol, column->sort);
    }
}

static void mListView_setSortDirection(mListView* self, ncsLstClmSortType direction)
{
    mListColumn* column = _c(self)->getSortColumn(self);
    if (!column) {
        return;
    }

    if (_c(column)->setProperty(column, NCSP_LSTCLM_SORTTYPE, direction))
        _c(self)->sort(self, NULL, self->curCol, direction);
}

static ncsLstClmSortType mListView_getSortDirection(mListView* self)
{
    mListColumn* column = _c(self)->getSortColumn(self);
    if (!column) {
        return -1;
    }

    return _c(column)->getProperty(column, NCSP_LSTCLM_SORTTYPE);
}

static mListColumn* mListView_getSortColumn(mListView* self)
{
    return _c(self)->getColumn(self, self->curCol);
}

static int mListView_getColumnIndex(mListView* self, mListColumn *column)
{
    return _c(pHdrList)->indexOf(pHdrList, (HITEM)column);
}

static mListColumn* mListView_getColumn(mListView* self, int index)
{
    return (mListColumn*) _c(pHdrList)->getItem(pHdrList, index);
}

static int mListView_getColumnWidth(mListView* self, int index)
{
    mListColumn* pListColumn = (mListColumn*)_c(pHdrList)->getItem(pHdrList, index);
    if (pListColumn)
        return pListColumn->width;
    return -1;
}

static int mListView_getColumnCount(mListView* self)
{
    return _c(pHdrList)->getItemCount(pHdrList);
}

/* offset the tail subitems from nCols */
static int
sAddOffsetToTailSubItem (mListView *self, int start_column, int offset)
{
    mListColumn *pListColumn = NULL;
    int i = 0;
    list_t *me, *first;

    if (start_column < 0 || start_column >= pHdrList->count)
        return -1;

    first = _c(pHdrList)->getQueue(pHdrList);

    for (me = first->next; me != first; me = me->next) {
        if (i >= start_column) {
            pListColumn = (mListColumn*)_c(pHdrList)->getListEntry(me);
            pListColumn->startX += offset;
        }
        i ++;
    }

    return 0;
}

#define NCS_LVCOLUMN_DEFWIDTH   62
#define NCS_LVCOLUMN_MINWIDTH   10

static inline void _set_cont_width(mListView* self, int newWidth)
{
    int width, oldWidth = self->contWidth;

    width = _c(self)->setProperty(self, NCSP_SWGT_CONTWIDTH, newWidth);

    //should invalidate header region
    if (oldWidth != width) {
        InvalidateRect(self->hwnd, NULL, TRUE);
    }
}

static int mListView_insertItem(mListView* self, HITEM hItem,
        HITEM prev, HITEM next, int index, int *pos)
{
    if (self->itemOps.initItem) {
        if (self->itemOps.initItem ((mItemView*)self, hItem) <0) {
            return -1;
        }
    }

    if (self->status & LVST_OPERATECOL)
        return _c(pHdrList)->insertItem(pHdrList, hItem, prev, next, index, pos);
    else
        return _c(pItemList)->insertItem(pItemList, hItem, prev, next, index, pos);
}


static HITEM mListView_createItem (mListView *self, HITEM prev, HITEM next,
        int index, int height, const char* text, DWORD addData, int *pos, BOOL adjust)
{
    HITEM newItem;
    int   ret = -1;

    if (self->status & LVST_OPERATECOL)
        newItem = (HITEM)NEW(mListColumn);
    else
        newItem = (HITEM)NEW(mListItem);

    if (!newItem)
        return -1;

    if (height >= 0)
        ((mItem*)newItem)->height = height;
    else
        ((mItem*)newItem)->height = self->defItemHeight;

    ((mItem*)newItem)->addData = addData;

    if (text && strlen(text) >= 1) {
        _c((mItem*)newItem)->setItemString((mItem*)newItem, text);
    }

    ret = _c(self)->insertItem(self, newItem, prev, next, index, pos);

    if (ret < 0) {
       DELETE((mItem*)newItem);
       return 0;
    }

    if (self->status & LVST_OPERATECOL)
        _set_cont_width(self, self->hdrW);
    else
        _c(self)->adjustItemsHeight(self, _c(self)->getItemHeight(self, newItem));

    return (HITEM)newItem;
}

static void _add_sub_item(mListView* self, int index)
{
    list_t      *me, *first;
    mListItem   *row;
    mItem       *newItem;

    first = _c(self)->getQueue(self);

    for (me = first->next; me != first; me = me->next) {
        row = (mListItem*) _c(self)->getListEntry(self, me);

        //create a new item
        newItem = (mItem*)_c(row)->createItem(row, 0, 0, index, NULL);
        if (newItem) {
            newItem->height = self->defItemHeight;
        }
    }
}

static BOOL mListView_addColumn(mListView* self, NCS_LISTV_CLMINFO *info)
{
    int index, nrCol, pos;
    mListColumn *pListColumn = NULL, *pFrontColumn = NULL;

    if (!info)
        return FALSE;

    index = info->index;
    nrCol = _c(self)->getColumnCount(self);

    if (index < 0 || index > nrCol)
        return FALSE;

    //lock status
    self->status |= LVST_OPERATECOL;
    pListColumn =
        (mListColumn*) _c(self)->createItem(self, 0, 0, index, 0, NULL, 0, &pos, TRUE);
    //unlock status
    self->status &= ~LVST_OPERATECOL;

    if (!pListColumn)
        return FALSE;

    if (info->sort >= NCSID_LSTCLM_NOTSORTED && info->sort < NCSID_LSTCLM_MAXVALUE)
        pListColumn->sort = info->sort;
    else
        pListColumn->sort = NCSID_LSTCLM_NOTSORTED;
    pListColumn->pfnCmp = info->pfnCmp;
    pListColumn->image = info->image;
    pListColumn->flags = info->flags;

    if (info->width <= 0)
        pListColumn->width = NCS_LVCOLUMN_DEFWIDTH;
    else if (info->width < NCS_LVCOLUMN_MINWIDTH)
        pListColumn->width = NCS_LVCOLUMN_MINWIDTH;
    else
        pListColumn->width = info->width;

    if (info->text)
        _c(pHdrList)->setText(pHdrList, (HITEM)pListColumn, info->text);

    if (pos > 0) {
        pFrontColumn = (mListColumn*) _c(pHdrList)->getItem(pHdrList, pos - 1);
        if (pFrontColumn) {
            pListColumn->startX = pFrontColumn->startX + pFrontColumn->width;
        }
    }
    else
        pListColumn->startX = 0;

    //add sub item
    _add_sub_item(self, pos);
    sAddOffsetToTailSubItem(self, pos + 1, pListColumn->width);

    self->hdrW += pListColumn->width;
    _set_cont_width(self, self->hdrW);

    return TRUE;
}

static void _del_sub_item(mListView* self, int index)
{
    list_t  *me, *first;
    mListItem *row;
    HITEM   delItem;

    first = _c(self)->getQueue(self);

    for (me = first->next; me != first; me = me->next) {
        //get row item
        row = (mListItem*) _c(self)->getListEntry(self, me);
        //get column item
        delItem = _c(row)->getItem(row, index);
        //delete
        _c(row)->removeItem(row, delItem);
    }

}

static BOOL mListView_delColumn(mListView* self, int index)
{
    int offset, count = _c(self)->getColumnCount(self);
    mListColumn *pListColumn = NULL;

    if (index < 0 || index > count)
        return FALSE;

    pListColumn = (mListColumn*) _c(self)->getColumn(self, index);
    if (!pListColumn)
        return FALSE;

    offset = -(pListColumn->width);

    if (count == 1)
        _c(self)->removeAll(self);
    else {
        sAddOffsetToTailSubItem(self, index + 1, offset);
        //del sub item
        _del_sub_item(self, index);
    }

    //remove column
    _c(pHdrList)->removeItem(pHdrList, (HITEM)pListColumn);

    self->hdrW += offset;
    _set_cont_width(self, self->hdrW);

    return TRUE;
}

/*============== ================*/

static int _get_item_indent (mListView *self, mListItem *item)
{
    if (!item)
        return -1;

    return INDENT * item->depth;
}

static int mListView_isInLVItem (mListView *self, int mouseX, int mouseY,
                     mListItem **pRet, POINT *pt)
{
    HITEM row;
    int ret;

    _c(self)->windowToContent(self, &mouseX, &mouseY);
    ret = _c(self)->inItem(self, 0, mouseY, &row, NULL);

    if (ret < 0)
        return ret;

    if (pRet && row) {
        *pRet = (mListItem *)row;
    }

    if (pt) {
        pt->x = mouseX;
        pt->y = mouseY;
    }
    return ret;
}

static void mListView_onPaint(mListView *self, HDC hdc, const PCLIPRGN pinv_clip)
{
    if (_c(self)->isFrozen(self))
        return;

    mListView_drawHeader(self, hdc);
    Class(mItemView).onPaint((mItemView*)self, hdc, pinv_clip);
}

static void _toggle_sort_status (mListColumn *pCol)
{
    switch (pCol->sort)
    {
        case NCSID_LSTCLM_NOTSORTED:
            pCol->sort = NCSID_LSTCLM_LOSORTED;
            break;
        case NCSID_LSTCLM_HISORTED:
            pCol->sort = NCSID_LSTCLM_LOSORTED;
            break;
        case NCSID_LSTCLM_LOSORTED:
            pCol->sort = NCSID_LSTCLM_HISORTED;
            break;
		default:
			break;
    }
}

#define NCS_LVCOL_MINWIDTH 10
static BOOL _set_column_width(mListView *self, int col, int width)
{
    int offset;
    mListColumn *pCol;

    if (!self)
        return FALSE;

    pCol = (mListColumn*) _c(self)->getColumn(self, col);

    if (!pCol)
        return FALSE;

    if (width < NCS_LVCOL_MINWIDTH)
        width = NCS_LVCOL_MINWIDTH;

    offset = width - pCol->width;
    pCol->width = width;
    sAddOffsetToTailSubItem(self, col + 1, offset);
    self->hdrW += offset;

    return TRUE;
}

enum {
    NCST_LDOWN,
    NCST_LMOVE,
    NCST_LUP,
};

static void _drag_border(mListView *self, int x, int y, int status)
{
    RECT rect;
    int offset, oldRop;
    HDC hdc;
    mListColumn *pDrag = NULL;
    gal_pixel oldColor;

    if (status == NCST_LMOVE || status == NCST_LUP)
        pDrag = (mListColumn*)_c(self)->getColumn(self, self->dragedHead);

    if (status == NCST_LMOVE) {
        /*the column width should not less than the min value*/
        if ((pDrag->startX - self->contX + NCS_LVCOLUMN_MINWIDTH) > x - 1)
            return;
    }

    GetClientRect (self->hwnd, &rect);
    hdc = GetClientDC(self->hwnd);
    oldColor = SetPenColor(hdc, DWORD2PIXEL(hdc, self->gridLineColor));
    oldRop = SetRasterOperation (hdc, ROP_XOR);

    if (status == NCST_LDOWN || status == NCST_LMOVE)
        LineEx(hdc, x, 0, x, RECTH(rect));
    if (status == NCST_LMOVE || status == NCST_LUP)
        LineEx(hdc, self->dragX, 0, self->dragX, RECTH(rect));

    SetRasterOperation (hdc, oldRop);
    SetPenColor(hdc, oldColor);

    if (status == NCST_LDOWN || status == NCST_LMOVE) {
        self->dragX = x;
    }

    if (status == NCST_LUP) {
        if (x < self->dragX)
            x = self->dragX;

        offset = x - (pDrag->startX + pDrag->width - self->contX);
        _set_column_width(self, self->dragedHead, pDrag->width + offset);
        mListView_drawHeader(self, hdc);
    }

    ReleaseDC (hdc);
}

static int mListView_inHead (mListView *self, int mouseX, int mouseY,
                     mListColumn **pRet)
{
    int nPosition = 0;
    list_t *me, *first;
    RECT rect;
    mListColumn *pListColumn = NULL;

    if (self->status & LVST_HIDEHEAD)
        return -1;

    _c(self)->windowToContent(self, &mouseX, NULL);

    first = _c(pHdrList)->getQueue(pHdrList);
    for (me = first->next; me != first; me = me->next) {
        pListColumn = (mListColumn*)_c(pHdrList)->getListEntry(me);
        if (_get_column_rect(self, pListColumn, &rect))
            if (PtInRect (&rect, mouseX, mouseY))
                break;

        nPosition++;
    }

    if (!pListColumn  || (nPosition < 0) ||
            (nPosition >= _c(self)->getColumnCount(self))) {
        if (pRet)
            *pRet = NULL;
        return -1;
    }

    if (pRet)
        *pRet = pListColumn;
    return nPosition;
}

static int
lvInWhichHeadBorder (mListView *self, int mouseX, int mouseY, mListColumn **pRet)
{
    int nPos = 0;
    mListColumn *pListColumn = NULL;
    BOOL bGet = FALSE;
    list_t *me, *first;

    if (self->status & LVST_HIDEHEAD)
        return -1;

    _c(self)->windowToContent(self, &mouseX, NULL);

    first = _c(pHdrList)->getQueue(pHdrList);

    for (me = first->next; me != first; me = me->next) {
        pListColumn = (mListColumn*)_c(pHdrList)->getListEntry(me);

        if ( (mouseX >= (pListColumn->startX + pListColumn->width - 1))
                && (mouseX <= (pListColumn->startX + pListColumn->width))
                && (mouseY >= 0) && (mouseY <= LV_HDR_HEIGHT) ) {
            bGet = TRUE;
            break;
        }
        nPos ++;
    }

    if (pRet)
        *pRet = bGet ? pListColumn : NULL;

    return bGet ? nPos : -1;
}

static int mListView_onLButtonDown(mListView* self, int x, int y, DWORD key_flags)
{
    int col = -1, row;
    POINT pt;
    RECT rect, rcClient;
    mListColumn *pCol;
    mListItem *pRow;
    int mouseX = x, mouseY = y;

    GetClientRect (self->hwnd, &rcClient);

    if (GetCapture() == self->hwnd)
        return 1;

    SetCapture (self->hwnd);
    self->status |= LVST_CAPTURED;

    if (LV_HDR_HEIGHT > 0)
        col = mListView_inHead(self, mouseX, mouseY, &pCol);

    if (col >=0) {
        self->status |= LVST_HEADCLICK;
        self->status |= LVST_INHEAD;
        self->clickedHead = pCol;

        SetRect (&rect, pCol->startX - self->contX, LV_HDR_TOP,
                pCol->startX - self->contX + pCol->width,
                LV_HDR_TOP + LV_HDR_HEIGHT);

        InvalidateRect (self->hwnd, &rect, TRUE);
    }
    else {
        col = lvInWhichHeadBorder(self, mouseX, mouseY, &pCol);

        if (LV_HDR_HEIGHT > 0 && col >= 0) {
            if (!(GetWindowStyle(self->hwnd) & NCSS_LISTV_NOHDRDRAG)) {
                self->status |= LVST_BDDRAG;
                self->dragedHead = col;
                _drag_border(self, mouseX, mouseY, NCST_LDOWN);
            }
        }
        else if ((row = mListView_isInLVItem(self, mouseX, mouseY, &pRow, &pt))>=0)
        {
            int indent = _c(self)->getColumn(self, pRow->depth)->startX;

            if (_c(self)->getChildCount(self, (HITEM)pRow) > 0
                    && pt.x > indent && pt.x < indent + 9 + 4) {
                BOOL bFold = (_c(pRow)->isFold(pRow)) ? FALSE : TRUE;
                _c(self)->foldItem(self, (HITEM)pRow, bFold);
                }
            if ((HITEM)pRow != _c(self)->getHilight(self)) {
                _c(self)->hilight(self, (HITEM)pRow);
                _c(self)->showItem(self, (HITEM)pRow);
                }
            ncsNotifyParent((mWidget*)self, NCSN_LISTV_CLICKED);
        }
    }

    return 1;
}

static int mListView_onLButtonUp(mListView* self, int x, int y, DWORD key_flags)
{
    int col = -1;
    mListColumn *pCol;

    if (GetCapture() != self->hwnd)
        return 1;

    if (!(self->status & LVST_CAPTURED))
        return 1;

    self->status &= ~LVST_CAPTURED;
    ReleaseCapture();
    ScreenToClient(self->hwnd, &x, &y);

    if (self->status & LVST_HEADCLICK) {
        self->status &= ~LVST_HEADCLICK;
        self->status &= ~LVST_INHEAD;

        col = mListView_inHead(self, x, y, NULL);
        if (col < 0)
            return -1;

        pCol = (mListColumn*) _c(self)->getColumn(self, col);

        if (!pCol || pCol != self->clickedHead)
            return -1;

        _toggle_sort_status (pCol);
        _c(self)->sort(self, NULL, col, pCol->sort);

        InvalidateRect (self->hwnd, NULL, TRUE);
    }
    else if (self->status & LVST_BDDRAG) {
        self->status &= ~LVST_BDDRAG;
        _drag_border(self, x, y, NCST_LUP);
        _set_cont_width(self, self->hdrW);
    }

    return 1;
}

static int _processRButtonMsg(mListView* self, int x, int y, DWORD key_flags, BOOL downMsg)
{
    int col, row;
    RECT rcClient;
    mListColumn *pCol;

    GetClientRect (self->hwnd, &rcClient);

    if ((col = mListView_inHead(self, x, y, &pCol)) >= 0) {
        ncsNotifyParentEx((mWidget*)self,
                downMsg? NCSN_LISTV_HDRRDOWN : NCSN_LISTV_HDRRUP, col);
    }
    else if ((row = mListView_isInLVItem(self, x, y, NULL, NULL))>=0) {
        ncsNotifyParentEx((mWidget*)self,
                downMsg? NCSN_LISTV_ITEMRDOWN : NCSN_LISTV_ITEMRUP, row);
    }

    if (downMsg)
        return Class(mItemView).onRButtonDown((mItemView*)self, x, y, key_flags);
    else
        return 1;
}

static int mListView_onRButtonDown(mListView* self, int x, int y, DWORD key_flags)
{
    return _processRButtonMsg(self, x, y, key_flags, TRUE);
}

static int mListView_onRButtonUp(mListView* self, int x, int y, DWORD key_flags)
{
    return _processRButtonMsg(self, x, y, key_flags, FALSE);
}

static int mListView_onLButtonDBClk(mListView* self, int x, int y, DWORD key_flags)
{
    int col;
    mListColumn* pCol;
    mListItem* pRow;

    if ((col = mListView_inHead(self, x, y, &pCol)) >= 0)
        return 1;

    if (mListView_isInLVItem(self, x, y, &pRow, NULL) >= 0) {
        ncsNotifyParent((mWidget*)self, NCSN_LISTV_ITEMDBCLK);

        if (_c(self)->getChildCount(self, (HITEM)pRow) > 0) {
            BOOL bFold = (_c(pRow)->isFold(pRow)) ? FALSE : TRUE;
            _c(self)->foldItem(self, (HITEM)pRow, bFold);
        }
    }

    return 1;
}

static int mListView_onMouseMove(mListView* self, int x, int y, DWORD key_flags)
{
    if (self->status & LVST_HEADCLICK) {
        RECT rc;
        mListColumn *pCol;

        if (GetCapture() != self->hwnd)
            return 1;

        ScreenToClient (self->hwnd, &x, &y);
        pCol = self->clickedHead;
        _get_column_rect(self, pCol, &rc);

        if (PtInRect(&rc, x, y)) {
            if (!(self->status & LVST_INHEAD)) {
                self->status |= LVST_INHEAD;
                InvalidateRect(self->hwnd, &rc, TRUE);
            }
        }
        else if (self->status & LVST_INHEAD) {
            rc.left -= 1;
            rc.right += 1;
            self->status &= ~LVST_INHEAD;
            InvalidateRect (self->hwnd, &rc, TRUE);
        }
    }
    else if (self->status & LVST_BDDRAG) {
        ScreenToClient (self->hwnd, &x, &y);
        _drag_border(self, x, y, NCST_LMOVE);
    }

    return 1;
}

static int mListView_wndProc(mListView* self,
            int message, WPARAM wParam, LPARAM lParam)
{
    switch(message){
        case MSG_SETCURSOR:
            {
                int mouseX = LOSWORD (lParam);
                int mouseY = HISWORD (lParam);

                if (!(GetWindowStyle(self->hwnd) & NCSS_LISTV_NOHDRDRAG) &&
                        ((self->status & LVST_BDDRAG) ||
                        (lvInWhichHeadBorder (self, mouseX, mouseY, NULL) >= 0))) {
                    SetCursor (GetSystemCursor (IDC_SPLIT_VERT));
                    return 0;
                }
            }
    }

	return Class(mItemView).wndProc((mItemView*)self,
                                message, wParam, lParam);
}

static void mListView_onHScroll (mListView* self, int code, int mouseX)
{
    RECT rc;

    GetClientRect (self->hwnd, &rc);
    rc.bottom = rc.top + LV_HDR_HEIGHT + 1;
    InvalidateRect (self->hwnd, &rc, TRUE);

    Class(mScrollWidget).onHScroll((mScrollWidget*)self, code, mouseX);
}

static void mListView_sort(mListView *self, NCS_CB_LISTV_CMPCLM func, int col, ncsLstClmSortType sort)
{
    NCS_CB_LISTV_CMPCLM   pCmp;
    mListColumn *pCol;

    if (func)
        pCmp = func;
    else {
        pCol = (mListColumn*)_c(self)->getColumn(self, col);
        if (!pCol)
            return;
        pCmp = pCol->pfnCmp;
    }

    self->sort = sort;
    self->curCol = col;
    self->pfnSort = pCmp;

    _c(self)->sortItems(self, _lv_cmp_func);
}

#if 0
static void _dump_child_ptr (mListView *self, mListItem *parent)
{
    int i = 0, count;
    mListItem *child = parent;

    if (!parent)
        return;

    if (parent == root)
        child = NULL;

    count = parent->nrChild;

    while (count--){
        child = _c(self)->getNext(self, child);
        fprintf (stderr, "child:%d's ptr:%p \n", i, child);
        i++;
    }
}
#endif

static int _sort_tree_item (mListView *self,
        mListItem* parent, NCS_CB_CMPITEM pfn)
{
    HITEM first, pci, pci2, tmp;
    int ret;

    if (!pfn)
        return -1;

    if (_c(parent)->getChildCount(parent) <= 1)
        return 0;

    first = _c(self)->getRelatedItem(self, (HITEM)parent, NCSID_LISTV_IR_FIRSTCHILD);

    pci = first;

    while (first) {
        _sort_tree_item(self, (mListItem*)first, pfn);
        first = _c(self)->getRelatedItem(self, first, NCSID_LISTV_IR_NEXTSIBLING);
    }

    while (pci) {
        ret = 0;

        for (pci2 = _c(self)->getRelatedItem(self, (HITEM)parent, NCSID_LISTV_IR_FIRSTCHILD);
            pci2 != 0;
            pci2 = _c(self)->getRelatedItem(self, pci2, NCSID_LISTV_IR_NEXTSIBLING))
        {
            if (pci == pci2)
                break;

            ret = pfn (pItemList, pci, pci2);
            if (ret < 0)
                break;
        }

        tmp = _c(self)->getRelatedItem(self, pci, NCSID_LISTV_IR_NEXTSIBLING);

        if (ret < 0) {
            _c(pItemList)->moveItem(pItemList, (mItem*)pci,
                    1 + _c(self)->getChildCount(self, pci), (mItem*)pci2);
        }

        pci = tmp;
    }

    return 0;
}

static void mListView_sortItems(mListView *self, NCS_CB_CMPITEM func)
{
    if (func) {
        _c(self)->freeze(self, TRUE);
        if (GetWindowStyle(self->hwnd) & NCSS_LISTV_TREE)
            _sort_tree_item(self, root, func);
        else
            _c(pItemList)->sortItems(pItemList, func);
        _c(self)->freeze(self, FALSE);
    }
}

static void mListView_setCustomDrawHeader(mListView* self, NCS_CB_LISTV_CSTMHDROPS *func)
{
    if (func) {
        self->drawHdrBk = func->pfnDrawHdrBk;
        self->drawHdrItem = func->pfnDrawHdrItem;
    }
}

static int _set_head_height(mListView* self, int height)
{
    RECT rc, rcClient;
    int oldH;

    if (height == LV_HDR_HEIGHT)
        return LV_HDR_HEIGHT;

    oldH = LV_HDR_HEIGHT;

    if (height < 0)
        height = 0;
    LV_HDR_HEIGHT = height;

    rc.left = LEFTMARGIN;
    rc.right = RIGHTMARGIN;
    rc.bottom = BOTTOMMARGIN;
    rc.top =  LV_HDR_HEIGHT + TOPMARGIN;

    _c(self)->setProperty(self, NCSP_SWGT_MARGINRECT, (DWORD)&rc);

    //refresh header region
    GetClientRect(self->hwnd, &rcClient);
    rc.left   = self->leftMargin;
    rc.right  = rc.left + RECTW(rcClient);
    rc.bottom = self->topMargin;
    rc.top    = rc.bottom - LV_HDR_HEIGHT;
    InvalidateRect(self->hwnd, &rc, TRUE);

    return oldH;
}

static int mListView_getChildCount (mListView* self, HITEM hItem)
{
    if (!hItem|| !INSTANCEOF(hItem, mListItem))
        return -1;

    return _c((mListItem*)hItem)->getChildCount((mListItem*)hItem);
}

static HITEM mListView_findItem(mListView* self, NCS_LISTV_FINDINFO *info)
{
    int j = 0, total_nr, colNum;
    BOOL byStartIdx;
    NCS_CB_CMPSTR strCmp;
    mListItem *pRowItem = (mListItem*)info->parent;

    if (!info)
        return 0;

    if (!pRowItem) {
        byStartIdx = TRUE;
        pRowItem = (mListItem*)_c(self)->getItem(self, info->startIdx);
        if (!pRowItem)
            pRowItem = (mListItem*)_c(self)->getItem(self, 0);
        total_nr = 1; //large than j
    }
    else {
        byStartIdx = FALSE;
        total_nr = _c(self)->getChildCount(self, (HITEM)pRowItem);
    }

    strCmp = _c(self)->getStrCmpFunc(self);
    colNum = _c(self)->getColumnCount(self);

    while (pRowItem && j < total_nr) {
        if (info->type == NCSID_LISTV_FT_ADDDATA) {
            if (info->data.addData == _c(self)->getAddData(self, (HITEM)pRowItem))
                return (HITEM)pRowItem;
        }
        else if (info->type == NCSID_LISTV_FT_TEXT) {
            HITEM subItem;
            const char* text;
            list_t *me, *first;
            int curIdx = 0, matchIdx = 0, retCmp;

            first = _c(pRowItem)->getQueue(pRowItem);
            for (me = first->next; me != first; me = me->next) {
                subItem = _c(pRowItem)->getListEntry(me);
                text = _c(pRowItem)->getText(pRowItem, subItem);

                retCmp = strCmp(text, info->data.string.text[matchIdx], (size_t)-1);
                curIdx++;

                if (retCmp != 0) {
                    //no enough column to match, go to next row item.
                    if (colNum - curIdx < info->data.string.size - matchIdx) {
                        break;
                    }

                    continue;
                }

                matchIdx++;
                if (matchIdx >= info->data.string.size) {
                    return (HITEM)pRowItem;
                }
            }

            if (!byStartIdx)
                j++;
        }
        pRowItem = (mListItem*)_c(self)->getNext(self, (HITEM)pRowItem);
    }
    return 0;
}

static int mListView_removeItem(mListView* self, HITEM hItem)
{
    mListItem *item = (mListItem *)hItem;
    int nrChild;
    HITEM next;

    if (!item)
        return -1;

    if (item->parent)
        _c(item->parent)->delChild(item->parent, item);

    nrChild = item->nrChild;

    while (nrChild--) {
        next = _c(self)->getNext(self, hItem);
        Class(mItemView).removeItem((mItemView*)self, next);
    }

    return Class(mItemView).removeItem((mItemView*)self, hItem);
}

static BOOL mListView_removeAll(mListView* self)
{
    int width, contWidth = self->contWidth;

    Class(mItemView).removeAll((mItemView*)self);
    _init_root_item(self);
    //set content width
    width = _c(self)->setProperty(self, NCSP_SWGT_CONTWIDTH, contWidth);

    if (contWidth != width)
        InvalidateRect(self->hwnd, NULL, TRUE);

    return TRUE;
}

static HITEM mListView_getRelatedItem(mListView* self,
        HITEM hItem, ncsListVIRType type)
{
    mListItem *item = (mListItem *)hItem;
    if (!item)
        return 0;

    switch (type) {
        case NCSID_LISTV_IR_PARENT:
            return (HITEM)_c(item)->getParent(item);

        case NCSID_LISTV_IR_FIRSTCHILD:
        {
            if (item->nrChild <= 0)
                return 0;

            if (item == root)
                return _c(self)->getItem(self, 0);
            else
                return _c(self)->getNext(self, (HITEM)item);
        }

        case NCSID_LISTV_IR_LASTCHILD:
        {
            int idx;

            if (item->nrChild == 0)
                return (HITEM)item;

            if (item != root) {
                idx = _c(self)->indexOf(self, (HITEM)item);

                if (idx < 0)
                    return 0;
            }
            else
                idx = -1;

            return _c(self)->getItem (self, idx + item->nrChild);
        }

        case NCSID_LISTV_IR_NEXTSIBLING:
        {
			if (GetWindowStyle(self->hwnd) & NCSS_LISTV_TREE){
				HITEM last, tmp;
				last = _c(self)->getRelatedItem(self, (HITEM)item, NCSID_LISTV_IR_LASTCHILD);
				tmp = _c(self)->getRelatedItem(self, (HITEM)item->parent, NCSID_LISTV_IR_LASTCHILD);
				if (tmp == last)
					return 0;
				return _c(self)->getNext (self, last);
			} else {
				return _M(self, getNext, item);
			}
        }

        case NCSID_LISTV_IR_PREVSIBLING:
        {
            HITEM first, tmp;

            first = _c(self)->getRelatedItem(self,
                            (HITEM)item->parent, NCSID_LISTV_IR_FIRSTCHILD);

            if (!first || (HITEM)item == first)
                return 0;

            while (first) {
                tmp = _c(self)->getRelatedItem(self, first, NCSID_LISTV_IR_NEXTSIBLING);
                if (tmp == (HITEM)item)
                    break;

                first = tmp;
            }

            return first;
        }
    }
    return 0;
}

static void mListView_setColumnWidth(mListView *self, int col, int width)
{
    if (_set_column_width(self, col, width))
        _set_cont_width(self, self->hdrW);
}

static int mListView_foldItem(mListView *self, HITEM hItem, BOOL fold)
{
    int i;
    mListItem *tmp, *pItem;

    if (!hItem || !INSTANCEOF(hItem, mListItem))
        return -1;

    pItem = (mListItem*)hItem;
    if (!_c(pItem)->setFold(pItem, fold))
        return -1;

    _c(self)->freeze(self, TRUE);

    tmp = pItem;
    for (i = 0; i < pItem->nrChild; i++) {
        int newh;
        tmp = (mListItem *) _c(self)->getNext(self, (HITEM)tmp);
        newh = fold ? 0 : tmp->showHeight;
        _c(self)->setItemHeight(self, (HITEM)tmp, newh);

        if (tmp->nrChild) {
            if (fold) {
                //skip child
                if(_c(tmp)->isFold(tmp)) {
                    i += _c(self)->getChildCount(self, (HITEM)tmp);
                    tmp = (mListItem *)_c(self)->getRelatedItem(self, (HITEM)tmp, NCSID_LISTV_IR_LASTCHILD);
                    continue;
                }
                else {
                    _c(tmp)->setFold(tmp, fold);
                }
            }
            else {
                i += _c(self)->getChildCount(self, (HITEM)tmp);
                tmp = (mListItem*)_c(self)->getRelatedItem(self, (HITEM)tmp, NCSID_LISTV_IR_LASTCHILD);
            }
        }
    }
    _c(self)->freeze(self, FALSE);

    ncsNotifyParentEx((mWidget*)self,
        fold ? NCSN_LISTV_FOLDITEM : NCSN_LISTV_UNFOLDITEM,
        (DWORD)hItem);
    return 0;
}

static BOOL mListView_setHeadText(mListView *self, int col, const char* text)
{
	HITEM hCol;

    if (!self)
        return FALSE;

    hCol = _c(pHdrList)->getItem(pHdrList, col);

    if (hCol && _c(pHdrList)->setText(pHdrList, hCol, text)) {
        RECT rcClient;
        GetClientRect(self->hwnd, &rcClient);
        rcClient.bottom = rcClient.top + LV_HDR_HEIGHT;
        InvalidateRect (self->hwnd, &rcClient, TRUE);
        return TRUE;
    }

    return FALSE;
}

static int mListView_setItemHeight (mListView *self, HITEM hItem, int height)
{
    mListItem* item = (mListItem*)hItem;
    int diff, itemH;

    if (!self || !item || height < 0 || height == item->height) return -1;

    if (height)
        item->showHeight = height;

    diff = Class(mItemView).setItemHeight((mItemView*)self, hItem, height);

    //refresh client item
    itemH = _c(self)->getItemHeight(self, hItem);
    if (itemH > 0) {
        RECT rcItem, rcClient;
        if (_c(self)->getRect (self, hItem, &rcItem, FALSE) == 0) {
            GetClientRect(self->hwnd, &rcClient);
            if (self->contY + RECTH(rcClient) >= rcItem.top)
                InvalidateRect(self->hwnd, NULL, TRUE);
        }
    }

    return height;
}

static BOOL _set_head_visible(mListView* self, BOOL show)
{
    RECT rc;
    if (show && (self->status & LVST_HIDEHEAD)) {
        self->status &= ~LVST_HIDEHEAD;
        rc.left = self->leftMargin;
        rc.right = self->rightMargin;
        rc.top = self->topMargin+LV_HDR_HEIGHT;
        rc.bottom = self->bottomMargin;
        _c(self)->setProperty(self, NCSP_SWGT_MARGINRECT, (DWORD)&rc);
    }
    else if (!show && !(self->status & LVST_HIDEHEAD)) {
        self->status |= LVST_HIDEHEAD;
        rc.left = self->leftMargin;
        rc.right = self->rightMargin;
        rc.top = self->topMargin-LV_HDR_HEIGHT;
        rc.bottom = self->bottomMargin;
        _c(self)->setProperty(self, NCSP_SWGT_MARGINRECT, (DWORD)&rc);
    }
    else
        return FALSE;

    return TRUE;
}

static void mListView_setBackground(mListView *self,
        int row, int col, int *color)
{
    mListItem *pRow = (mListItem*)_c(self)->getItem(self, row);
    RECT    rcSubItem;

    if (!pRow)
        return;

    _c(pRow)->setBackground(pRow, col, color);

    if (mListView_getSubItemRect(self, (HITEM)pRow, row, col, &rcSubItem))
        InvalidateRect(self->hwnd, &rcSubItem, TRUE);
    else
        _c(self)->refreshItem(self, (HITEM)pRow, NULL);
}

static int mListView_getBackground(mListView *self,
        int row, int col, int *color)
{
    mListItem *pRow = (mListItem*)_c(self)->getItem(self, row);

    if (!pRow)
        return -1;

    return _c(pRow)->getBackground(pRow, col, color);
}

static void mListView_setForeground(mListView *self,
        int row, int col, int *color)
{
    mListItem *pRow = (mListItem*)_c(self)->getItem(self, row);
    RECT    rcSubItem;

    if (!pRow)
        return;

    _c(pRow)->setForeground(pRow, col, color);

    if (mListView_getSubItemRect(self, (HITEM)pRow, row, col, &rcSubItem))
        InvalidateRect(self->hwnd, &rcSubItem, TRUE);
    else
        _c(self)->refreshItem(self, (HITEM)pRow, NULL);
}

static int mListView_getForeground(mListView *self,
        int row, int col, int *color)
{
    mListItem *pRow = (mListItem*)_c(self)->getItem(self, row);

    if (!pRow)
        return -1;

    return _c(pRow)->getForeground(pRow, col, color);
}

static BOOL mListView_setProperty(mListView* self, int id, DWORD value)
{

#ifdef _MGNCS_GUIBUILDER_SUPPORT
	if(id == NCSP_DEFAULT_CONTENT)
	{
    	NCS_LISTV_CLMINFO lvcol;
		char szText[100];
		HITEM hItem;
		NCS_LISTV_ITEMINFO info;
		int i,j;
		memset(&lvcol,0, sizeof(lvcol));
		lvcol.text = szText;
		lvcol.width = 80;
		lvcol.flags = NCSF_LSTCLM_CENTERALIGN | NCSF_LSTHDR_CENTERALIGN;
		for(i=0; i< 3; i++)
		{
			lvcol.index = i;
			sprintf(szText, "Header %d", i+1);
			_c(self)->addColumn(self, &lvcol);
		}

		memset(&info, 0, sizeof(info));
		info.height = 25;
		info.dataSize = 3;
		for(i=0; i<3; i++)
		{
			NCS_LISTV_ITEMDATA subdatas[3];
			char szText[3][32];
			info.parent = 0;
			info.index = i;
			info.data = subdatas;
			//add item
			memset(subdatas, 0, sizeof(subdatas));
			for(j=0; j<3; j++)
			{
				subdatas[j].row = i;
				subdatas[j].col = j;
				subdatas[j].text = szText[j];
				sprintf(szText[j],"LVItem%d %d", i, j);
			}
			hItem = _c(self)->addItem(self, &info);
		}

		return TRUE;
	}
#endif

	if( id >= NCSP_LISTV_MAX)
		return FALSE;

	switch(id)
	{
        case NCSP_LISTV_HDRHEIGHT:
            return _set_head_height(self, value);

        case NCSP_LISTV_HDRWIDTH:
            return FALSE;

        case NCSP_LISTV_HDRVISIBLE:
            return _set_head_visible(self, value);

        case NCSP_LISTV_SORTCOLUMN:
        {
            mListColumn *column = _c(self)->getColumn(self, value);
            if (column) {
                _c(self)->setSortColumn(self, column);
                return TRUE;
            }
            return FALSE;
        }

        case NCSP_LISTV_GRIDLINEWIDTH:
            if (value >= 0) {
                self->gridLineWidth = value;
                InvalidateRect(self->hwnd, NULL, TRUE);
                return TRUE;
            }
            return FALSE;

        case NCSP_LISTV_GRIDLINECOLOR:
            self->gridLineColor = value;
            InvalidateRect(self->hwnd, NULL, TRUE);
            return TRUE;

        case NCSP_LISTV_COLCOUNT:
            return FALSE;
    }

	return Class(mItemView).setProperty((mItemView*)self, id, value);
}

static DWORD mListView_getProperty(mListView* self, int id)
{
	if( id >= NCSP_LISTV_MAX)
		return -1;

	switch (id)
    {
        case NCSP_LISTV_HDRVISIBLE:
            return !(self->status & LVST_HIDEHEAD);

        case NCSP_LISTV_HDRHEIGHT:
            return self->hdrH;

        case NCSP_LISTV_HDRWIDTH:
            return self->hdrW;

        case NCSP_LISTV_SORTCOLUMN:
            return self->curCol;

        case NCSP_LISTV_GRIDLINEWIDTH:
            return self->gridLineWidth;

        case NCSP_LISTV_GRIDLINECOLOR:
            return self->gridLineColor;

        case NCSP_LISTV_COLCOUNT:
            return _c(self)->getColumnCount(self);
    }

	return Class(mItemView).getProperty((mItemView*)self, id);
}

static void mListView_setAutoSortItem (mListView *self)
{
    IncludeWindowStyle (self->hwnd, NCSS_LISTV_SORT);
}

static NCS_CB_CMPITEM
mListView_setItemCmpFunc(mListView *self, NCS_CB_CMPITEM func)
{
    return pItemList->itemCmp;
}

static HITEM mListView_getChildItem(mListView *self, HITEM parent, int index)
{
    int i = 0, count;
    HITEM first, parentItem = parent;

    if (!parentItem)
        parentItem = (HITEM) self->rootItem;

    count = _c(self)->getChildCount(self, parentItem);

    if (!count || index < 0 || index + 1 > count)
        return 0;

    first =
        _c(self)->getRelatedItem(self, parentItem, NCSID_LISTV_IR_FIRSTCHILD);

    while (first) {
        if (i == index)
            break;

        first = _c(self)->getRelatedItem(self, first, NCSID_LISTV_IR_NEXTSIBLING);
        i++;
    }

    return first;
}

static int
mListView_onKeyDown(mListView* self, int scancode, int state)
{
    mListItem *hilight;
    BOOL ret = TRUE;

    if (!self)
        return 1;

    //for treeview
    if (scancode == SCANCODE_CURSORBLOCKLEFT
            || scancode == SCANCODE_CURSORBLOCKRIGHT ) {

        hilight = (mListItem*) _c(self)->getHilight(self);
        if (hilight) {
            if (_c(self)->getChildCount(self, (HITEM)hilight)) {

                if (scancode == SCANCODE_CURSORBLOCKLEFT
                        && !_c(hilight)->isFold(hilight)) {
                    _c(self)->foldItem(self, (HITEM)hilight, TRUE);
                    ret = FALSE;
                }
                else if (scancode == SCANCODE_CURSORBLOCKRIGHT
                        && _c(hilight)->isFold(hilight)) {
                    _c(self)->foldItem(self, (HITEM)hilight, FALSE);
                    ret = FALSE;
                }
            }
        }
    }

    if (ret)
        return Class(mItemView).onKeyDown((mItemView*)self, scancode, state);
    else
        return Class(mScrollWidget).onKeyDown((mScrollWidget*)self, scancode, state);
}

#ifdef _MGNCSDB_DATASOURCE
static BOOL mListView_setSpecificData(mListView *self, DWORD key, DWORD value, PFreeSpecificData free_special)
{

	if(key == NCSSPEC_LISTV_HDR)
	{
		mRecordSet * rs = SAFE_CAST(mRecordSet, value);
		NCS_LISTV_CLMINFO colInfo;
		int i;
		if(!rs)
			return FALSE;
		if(!Class(mItemView).setSpecificData((mItemView*)self, key, value, free_special))
			return FALSE;

		_c(self)->freeze(self, TRUE);
		//reset content
		_c(self)->removeAll(self);
		//delete columns
		i = _c(self)->getColumnCount(self);
		while(i>0)
		{
			_c(self)->delColumn(self, 0);
			i --;
		}

		memset(&colInfo, 0, sizeof(colInfo));
		_c(rs)->seekCursor(rs, NCS_RS_CURSOR_BEGIN,0);
		while(!_c(rs)->isEnd(rs))
		{
			colInfo.text = (char*)_c(rs)->getField(rs,1);
			colInfo.image = (DWORD)GetResource(Str2Key((const char*)_c(rs)->getField(rs,2)));
			colInfo.width = (int)_c(rs)->getField(rs, 3);
			colInfo.flags = _c(rs)->getField(rs,4);
			_c(self)->addColumn(self, &colInfo);
			_c(rs)->seekCursor(rs, NCS_RS_CURSOR_CUR,1);
			colInfo.index ++;
		}
		_c(self)->freeze(self,FALSE);
		return TRUE;
	}
	else if(key == NCSSPEC_OBJ_CONTENT)
	{
		mRecordSet * rs = SAFE_CAST(mRecordSet, value);
		NCS_LISTV_ITEMINFO itemInfo;
		NCS_LISTV_ITEMDATA *subdatas;
		int col_count;
		int field_count;
		int i;
		DWORD cursor;

		if(!rs)
			return FALSE;

		field_count = _c(rs)->getFieldCount(rs);
		if(field_count <= 0)
			return FALSE;

		if(!Class(mItemView).setSpecificData((mItemView*)self, key, value, free_special))
			return FALSE;

		col_count = _c(self)->getColumnCount(self);
		if(col_count<=0)
			return FALSE;

		_c(self)->freeze(self, TRUE);
		//reset content
		_c(self)->removeAll(self);

		col_count = col_count < field_count?col_count:field_count;

		subdatas = (NCS_LISTV_ITEMDATA*)calloc(col_count, sizeof(NCS_LISTV_ITEMDATA));
		memset(&itemInfo, 0, sizeof(itemInfo));
		itemInfo.data = subdatas;
		itemInfo.dataSize = col_count;
		itemInfo.height = 25;

		cursor = _c(rs)->seekCursor(rs, NCS_RS_CURSOR_BEGIN,0);
		_BEGIN_AUTO_STR
		while(!_c(rs)->isEnd(rs))
		{
			memset(subdatas, 0, sizeof(NCS_LISTV_ITEMDATA)*col_count);
			_RESET_AUTO_STR
			for(i = 1; i<=col_count; i++)
			{
				DWORD data = _c(rs)->getField(rs, i);
				int type =_c(rs)->getFieldType(rs, i);
				subdatas[i-1].row = itemInfo.index;
				subdatas[i-1].col = i-1;
				if(type == NCS_BT_STR)
				{
					subdatas[i-1].text = (char*)data;
				}
				else
				{
					subdatas[i-1].text = _AUTO_STR(data, type);
				}

			}
			itemInfo.addData = cursor;
			_c(self)->addItem(self, &itemInfo);
			cursor = _c(rs)->seekCursor(rs, NCS_RS_CURSOR_CUR,1);
			itemInfo.index ++;
		}
		_END_AUTO_STR
		free(subdatas);
		_c(self)->freeze(self, FALSE);
		return TRUE;
	}

	return Class(mItemView).setSpecificData((mItemView*)self, key, value, free_special);
}
#endif

static NCS_CB_DRAWITEMBK mListView_setItemBkDraw(mListView *self, NCS_CB_DRAWITEMBK func)
{
    NCS_CB_DRAWITEMBK oldfn;

    oldfn = self->drawItemBk;
    self->drawItemBk = func;

    _c(self)->refreshRect(self, NULL);
    return oldfn;
}

static NCS_CB_DRAWITEM mListView_setSubItemDraw(mListView *self, NCS_CB_DRAWITEM func)
{
    NCS_CB_DRAWITEM oldfn;

    oldfn = self->drawSubItem;
    self->drawSubItem = func;

    _c(self)->refreshRect(self, NULL);
    return oldfn;
}

BEGIN_CMPT_CLASS(mListView, mItemView)
    CLASS_METHOD_MAP(mListView, construct)
	CLASS_METHOD_MAP(mListView, destroy)
	CLASS_METHOD_MAP(mListView, setSubItemDraw)
	CLASS_METHOD_MAP(mListView, setItemBkDraw)
    CLASS_METHOD_MAP(mListView, setProperty);
    CLASS_METHOD_MAP(mListView, getProperty);
    CLASS_METHOD_MAP(mListView, wndProc)
    CLASS_METHOD_MAP(mListView, onPaint);
    CLASS_METHOD_MAP(mListView, onKeyDown);
    CLASS_METHOD_MAP(mListView, onRButtonDown)
    CLASS_METHOD_MAP(mListView, onRButtonUp)
    CLASS_METHOD_MAP(mListView, onLButtonDown)
    CLASS_METHOD_MAP(mListView, onLButtonUp)
    CLASS_METHOD_MAP(mListView, onMouseMove)
    CLASS_METHOD_MAP(mListView, onLButtonDBClk)
    CLASS_METHOD_MAP(mListView, onHScroll)
	CLASS_METHOD_MAP(mListView, setItemHeight)
	CLASS_METHOD_MAP(mListView, insertItem)
	CLASS_METHOD_MAP(mListView, createItem)
	CLASS_METHOD_MAP(mListView, addItem)
    CLASS_METHOD_MAP(mListView, findItem)
    CLASS_METHOD_MAP(mListView, setItemInfo)
    CLASS_METHOD_MAP(mListView, getItemInfo)
    CLASS_METHOD_MAP(mListView, getItemText)
    CLASS_METHOD_MAP(mListView, getItemTextLen)
    CLASS_METHOD_MAP(mListView, setItemText)
    CLASS_METHOD_MAP(mListView, getColumn)
    CLASS_METHOD_MAP(mListView, setSortColumn)
    CLASS_METHOD_MAP(mListView, getSortColumn)
    CLASS_METHOD_MAP(mListView, getColumnIndex)
    CLASS_METHOD_MAP(mListView, setColumnWidth)
    CLASS_METHOD_MAP(mListView, getColumnWidth)
    CLASS_METHOD_MAP(mListView, getColumnCount)
    CLASS_METHOD_MAP(mListView, showColumn)
    CLASS_METHOD_MAP(mListView, getSortDirection)
    CLASS_METHOD_MAP(mListView, setSortDirection)
    CLASS_METHOD_MAP(mListView, getSortColumn)
    CLASS_METHOD_MAP(mListView, delColumn)
    CLASS_METHOD_MAP(mListView, addColumn)
    CLASS_METHOD_MAP(mListView, removeItem)
    CLASS_METHOD_MAP(mListView, removeAll)
    CLASS_METHOD_MAP(mListView, setCustomDrawHeader)
    CLASS_METHOD_MAP(mListView, getRelatedItem)
    CLASS_METHOD_MAP(mListView, foldItem)
    CLASS_METHOD_MAP(mListView, setHeadText)
    CLASS_METHOD_MAP(mListView, sort)
    CLASS_METHOD_MAP(mListView, setBackground)
    CLASS_METHOD_MAP(mListView, setForeground)
    CLASS_METHOD_MAP(mListView, getBackground)
    CLASS_METHOD_MAP(mListView, getForeground)
    CLASS_METHOD_MAP(mListView, getChildCount)
    CLASS_METHOD_MAP(mListView, setAutoSortItem)
    CLASS_METHOD_MAP(mListView, sortItems)
	CLASS_METHOD_MAP(mListView, setItemCmpFunc)
	CLASS_METHOD_MAP(mListView, getChildItem)
#ifdef _MGNCSDB_DATASOURCE
	CLASS_METHOD_MAP(mListView, setSpecificData)
#endif
	SET_DLGCODE(DLGC_WANTARROWS | DLGC_WANTCHARS)
END_CMPT_CLASS


const unsigned char gListVColumnRecordTypes[] = {
	NCS_BT_STR,
	NCS_BT_STR,
	NCS_BT_INT,
	NCS_BT_INT,
	0
};

#endif //_MGNCSCTRL_LISTVIEW
