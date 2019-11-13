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
    The implementation of mListBox class.

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
    <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSDB_DATASOURCE
#include "mdatabinding.h"
#include "mdatasource.h"
#endif

#ifdef _MGNCSCTRL_LISTBOX

//for listbox flag
#define NCSF_LSTBOX_FOCUS   0x0001
#define TOPITEM             (self->contY/self->defItemHeight)

static void mListBox_construct (mListBox *self, DWORD addData)
{
    g_stmItemViewCls.construct((mItemView*)self, addData);

    self->font = CreateLogFontIndirect(
            (PLOGFONT)ncsGetElement((mWidget*)self, WE_FONT_MENU));

    self->itemVisibles = self->visHeight/self->defItemHeight;

    self->itemHilighted = -1;
    self->contHeight = 0;
    self->selItem = self->itemHilighted;
    self->vStepVal = self->defItemHeight;
    _c(self)->initMargins(self, 1, 1, 1, 1);
}

static void mListBox_destroy(mListBox *self)
{
    if (self->font != INV_LOGFONT)
        DestroyLogFont(self->font);

    self->hwnd = HWND_NULL;
    Class(mItemView).destroy((mItemView*)self);
}

#define LST_INTER_BMPTEXT       2
/** minimum height of checkmark */
#define LFRDR_LB_CHECKBMP_MIN   6

static int getCheckMarkHeight (mListBox *self)
{
    int checkmark_height =
        self->defItemHeight - LFRDR_LB_CHECKBMP_MIN;

    if (0 < checkmark_height)
        checkmark_height += checkmark_height >> 2;

    return checkmark_height;
}

static void getCheckMarkSize(mListBox *self, int checkMarkHeight, RECT *checkBoxRect)
{
    if (checkBoxRect) {
        checkBoxRect->left = LST_INTER_BMPTEXT;
        checkBoxRect->top = ((self->defItemHeight - checkMarkHeight) >> 1);
        checkBoxRect->right = checkBoxRect->left + checkMarkHeight;
        checkBoxRect->bottom = checkBoxRect->top + checkMarkHeight;
    }
}


static inline int getItemWidth(mListBox* self, HDC hdc, mItem* item, DWORD style)
{
    int width = LST_INTER_BMPTEXT;

    if (style & NCSS_LSTBOX_CHECKBOX)
        width += getCheckMarkHeight (self) + LST_INTER_BMPTEXT;

    if (style & NCSS_LSTBOX_USEBITMAP && item->image) {
        if (item->flags & NCSF_ITEM_USEBITMAP)
            width += ((PBITMAP)(item->image))->bmWidth;
        else {
            int width;
            GetIconSize ((HICON)item->image, &width, NULL);
            width += width;
        }
        width += LST_INTER_BMPTEXT;
    }

    if (item->string && item->string[0] != '\0') {
        SIZE size;
        HDC client_dc = hdc;
        if (client_dc == 0) {
            client_dc = GetClientDC (self->hwnd);
            if (self->font != INV_LOGFONT) {
                SelectFont (client_dc, self->font);
            }
        }

        GetTextExtent (client_dc, item->string, -1, &size);
        width += size.cx + LST_INTER_BMPTEXT;

        if (hdc == 0) {
            ReleaseDC(client_dc);
        }
    }

    return width;
}

static inline int recalcContWidth(mListBox* self)
{
    DWORD style = GetWindowStyle(self->hwnd);
    list_t *me, *first;
    HITEM hItem;
    int maxWidth = 0, width;

    HDC hdc = GetClientDC (self->hwnd);
    if (self->font != INV_LOGFONT) {
        SelectFont (hdc, self->font);
    }

    first = _c(self)->getQueue(self);
    for (me = first->next; me != first; me = me->next) {
        hItem = _c(self)->getListEntry(self, me);
        width = getItemWidth(self, hdc, (mItem*)hItem, style);

        if (width > maxWidth)
            maxWidth = width;
    }

    ReleaseDC (hdc);
    return maxWidth;
}

static int mListBox_getRectByIdx(mListBox *self,
        int index, RECT *rcItem, BOOL bConv)
{
    RECT rcVis;

    if (!self || !rcItem)
        return -1;

    if (index < 0 || index >= _c(self)->getItemCount(self))
        return -1;

    rcItem->top = index*self->defItemHeight;
    rcItem->left = 0;
    rcItem->bottom = rcItem->top + self->defItemHeight;
    rcItem->right = self->visWidth > self->contWidth ? self->visWidth : self->contWidth;

    if (bConv) {
        _c(self)->contentToWindow(self, &(rcItem->left), &(rcItem->top));
        _c(self)->contentToWindow(self, &(rcItem->right), &(rcItem->bottom));
    }
    
    _M(self, getVisRect, &rcVis);
    IntersectRect(rcItem, rcItem, &rcVis);

    return 0;
}

static int mListBox_getRect (mListBox *self,
        HITEM hItem, RECT *rcItem, BOOL bConv)
{
    int index;
    if (!self || !rcItem || !hItem)
        return -1;

    index = _c(self)->indexOf(self, hItem);
    return mListBox_getRectByIdx(self, index, rcItem, bConv);
}

static void _draw_listbox (mListBox *self, HDC hdc)
{
    DWORD style, flags = 0, image;
    RECT rect, rcText;
    int i, topItem;
    int x = 0, y = 0;
    int checkmark_height;       /** checkmark bmp height */
    int checkmark_status;       /** checkmark bmp status */
    HITEM hItem;

    gal_pixel gp_select_fgc;
    //gal_pixel gp_hilite_fgc;
    gal_pixel gp_sign_fgc;
    gal_pixel gp_disabled_fgc;
    gal_pixel gp_normal_fgc;
    gal_pixel gp_bkcolor;

    if (_c(self)->getItemCount(self) <= 0)
        return;

    style = GetWindowStyle (self->hwnd);

    checkmark_height = getCheckMarkHeight (self);
    topItem = TOPITEM;
    hItem = _c(self)->getItem (self, topItem);

    gp_select_fgc = ncsColor2Pixel(hdc, ncsGetElement((mWidget*)self, NCS_FGC_SELECTED_ITEM));
    //gp_hilite_fgc = ncsColor2Pixel(hdc, ncsGetElement((mWidget*)self, NCS_FGC_HILIGHT_ITEM));
    gp_sign_fgc   = ncsColor2Pixel(hdc, ncsGetElement((mWidget*)self, WE_FGC_SIGNIFICANT_ITEM));
    gp_disabled_fgc = ncsColor2Pixel(hdc, ncsGetElement((mWidget*)self, NCS_FGC_DISABLED_ITEM));
    gp_normal_fgc  = ncsColor2Pixel(hdc, ncsGetElement((mWidget*)self, NCS_FGC_WINDOW));

    SetBkMode (hdc, BM_TRANSPARENT);
    gp_bkcolor = GetBkColor (hdc);

    for (i = 0; hItem && i < (self->itemVisibles + 1); i++) {
        mListBox_getRectByIdx(self, topItem+i, &rect, TRUE);

        if (!_c(self)->isEnabled(self, hItem)) {
            SetTextColor (hdc, gp_disabled_fgc);

            /** render disabled item */
            self->renderer->drawItem (self, hdc, &rect, NCSRS_DISABLE);
        }
        else if (_c(self)->isSelected(self, hItem)) {
            SetTextColor (hdc, gp_select_fgc);

            /** render selected item */
            self->renderer->drawItem (self, hdc, &rect, NCSRS_SELECTED);
        }
        else {
            DWORD dwExStyle = GetWindowExStyle(self->hwnd);
            SetTextColor (hdc, gp_normal_fgc);

            /** render normal item */
            if ( (dwExStyle & 0x20000000L/*WS_EX_CTRLASMAINWIN*/) || !(dwExStyle & WS_EX_TRANSPARENT)) {
                self->renderer->drawItem (self, hdc, &rect, NCSRS_NORMAL);
            }
        }

        flags = _c(self)->getFlags(self, hItem);

        /** render significant item */
        if ( (flags & NCSF_LSTBOX_SIGNIFICANT) &&
             !(flags & NCSF_ITEM_DISABLED)) {
            SetTextColor (hdc, gp_sign_fgc);

            self->renderer->drawItem (self, hdc, &rect, NCSRS_SIGNIFICANT);
        }

        if (self->font != INV_LOGFONT) {
            if (flags & NCSF_LSTBOX_BOLD)
                self->font ->style |= FS_WEIGHT_BOLD;
            else
                self->font->style &= ~FS_WEIGHT_BOLD;
        }

        x = rect.left;
        y = rect.top;
        rcText.right = rect.right;
        rcText.bottom = rect.bottom;

        if (style & NCSS_LSTBOX_CHECKBOX) {
            RECT checkRect;
            checkmark_status = NCSRF_FRAME | NCSRF_FILL;

            getCheckMarkSize(self, checkmark_height, &checkRect);
            OffsetRect(&checkRect, rect.left, rect.top);

            if (flags & NCSF_LSTBOX_CMCHECKED)
                checkmark_status |= NCSRS_SELECTED;
            else if (flags & NCSF_LSTBOX_CMPARTCHECKED)
                checkmark_status |= NCSRS_HALFSELECTED;

            gp_bkcolor = SetBkColor (hdc, gp_bkcolor);

            self->renderer->drawCheckbox(self, hdc, &checkRect, checkmark_status);

            gp_bkcolor = SetBkColor (hdc, gp_bkcolor);

            x = checkRect.right + LST_INTER_BMPTEXT;
        }

        image = _c(self)->getImage(self, hItem);

        if ((style & NCSS_LSTBOX_USEBITMAP) && image) {
            if (flags & NCSF_ITEM_USEBITMAP) {
                FillBoxWithBitmap (hdc, x, y, 0, self->defItemHeight, (PBITMAP)image);
                x += ((PBITMAP)image)->bmWidth;
            }
            else {
                int width;
                DrawIcon (hdc, x, y, 0, self->defItemHeight, (HICON)image);
                GetIconSize ((HICON)image, &width, NULL);
                x += width;
            }
            x += LST_INTER_BMPTEXT;
        }

        rcText.left = x;
        rcText.top = y;
        DrawText(hdc, _c(self)->getText(self, hItem), -1,
                &rcText, DT_VCENTER | DT_SINGLELINE);

        y += self->defItemHeight;
        hItem = _c(self)->getNext(self, hItem);
    }

    SetBkColor (hdc, gp_bkcolor);

    if (self->itemHilighted < TOPITEM
            || self->itemHilighted > (TOPITEM + self->itemVisibles))
        return;

    if (NCSF_LSTBOX_FOCUS & self->listFlag) {
        mListBox_getRectByIdx(self, self->itemHilighted, &rect, TRUE);

        InflateRect (&rect, -1, -1);
        self->renderer->drawFocusFrame (self, hdc, &rect);
    }
}

static void _change_top_item(mListBox *self, int topItem)
{
    if (topItem < 0)
       topItem = 0;

    _c(self)->setProperty(self, NCSP_SWGT_CONTY,
                        topItem*self->defItemHeight);
}

static void mListBox_resetContent(mListBox *self)
{
    RECT rc;

    self->contX = 0;
    self->contY = 0;

    GetClientRect(self->hwnd, &rc);
    self->contWidth = RECTW(rc);
    self->defItemHeight = GetWindowFont(self->hwnd)->size;

    self->itemVisibles = self->visHeight/self->defItemHeight;

    self->vStepVal = self->defItemHeight;
    _change_top_item(self, 0);

    _c(self)->removeAll(self);

    self->itemHilighted = -1;
    self->selItem = self->itemHilighted;
}

static BOOL invalidateUnderItem (mListBox *self, int index, BOOL multi)
{
    RECT rcInv;
    int count, top = TOPITEM;

    count = _c(self)->getItemCount(self);

    if (index > (top + self->itemVisibles) || index > count)
        return FALSE;

    if (index < top)
        return FALSE;

    mListBox_getRectByIdx(self, index, &rcInv, TRUE);

    if (multi)
        rcInv.bottom = rcInv.top + (count - index)*self->defItemHeight;

    if (rcInv.top < rcInv.bottom) {
        InvalidateRect (self->hwnd, &rcInv, TRUE);
    }

    return TRUE;
}

//internal function
static inline int insert(mListBox* self, const char* string,
        DWORD addData, DWORD flag, DWORD image,
        int index, BOOL refresh)
{
    HITEM   item;
    DWORD   style, flags;
    BOOL    isString = TRUE;
    int     itemWidth, itemIndex;

    style = GetWindowStyle (self->hwnd);

    if (style & NCSS_LSTBOX_CHECKBOX
            || style & NCSS_LSTBOX_USEBITMAP) {
        isString = FALSE;
    }
    else {
        isString = TRUE;

        if (string == NULL || string[0] == '\0')
            return -3;
    }

    item = _c(self)->createItem(self, 0, 0, index, -1,
            string, addData, &itemIndex, TRUE);

    if (!item) {
        return -3;
    }

    //set flags
    flags = NCSF_ITEM_NORMAL;
    if (!isString) {
        flags |= (flag & NCSF_ITEM_MASK);
    }
    flags |= (flag & NCSF_ITEM_IMAGEMASK);
    _c(self)->setFlags(self, item, flags);

    //set image
    if ((style & NCSS_LSTBOX_USEBITMAP) && image) {
        _c(self)->setImage(self, item, image);
    }

    itemWidth = getItemWidth(self, 0, (mItem*)item, GetWindowStyle(self->hwnd));

    if (self->realContWidth < itemWidth) {
        _c(self)->setProperty(self, NCSP_LSTBOX_ITEMWIDTH, itemWidth);
    }

    if (refresh && !_c(self)->isFrozen(self)) {
        invalidateUnderItem (self, itemIndex, TRUE);
        _c(self)->setScrollInfo(self, TRUE);
    }

    return itemIndex;
}

static void mListBox_freeze(mListBox *self, BOOL lock)
{
    _c(pItemList)->freeze(pItemList, lock);

    if (!lock) {
        int width = 0;

        _c(self)->setProperty(self,
                NCSP_SWGT_CONTHEIGHT, _c(self)->getTotalHeight(self));

        //update items width
        width = recalcContWidth(self);
        if (width > self->realContWidth)
            _c(self)->setProperty(self, NCSP_LSTBOX_ITEMWIDTH, width);

        InvalidateRect(self->hwnd, NULL, TRUE);
    }
}

static int
mListBox_insertString(mListBox* self,
        const char* string, DWORD addData, int index)
{
    HITEM   item;
    int     itemWidth, itemIndex;

    if (string == NULL || string[0] == '\0')
        return -1;

    item = _c(self)->createItem(self, 0, 0, index, -1,
            string, addData, &itemIndex, TRUE);

    if (!item) {
        return -2;
    }

    if (!_c(self)->isFrozen(self)) {
        itemWidth = getItemWidth(self, 0, (mItem*)item, GetWindowStyle(self->hwnd));

        if (self->realContWidth < itemWidth) {
            _c(self)->setProperty(self, NCSP_LSTBOX_ITEMWIDTH, itemWidth);
        }

        invalidateUnderItem (self, itemIndex, TRUE);
        _c(self)->setScrollInfo(self, TRUE);
    }
    else {
        invalidateUnderItem (self, itemIndex, TRUE);
    }

    return itemIndex;
}


static int mListBox_addString(mListBox* self,
        const char* string, DWORD addData)
{
    return _c(self)->insertString (self, string, addData, -1);
}

static void mListBox_addItems(mListBox* self,
        NCS_LSTBOX_ITEMINFO *info, int count)
{
    int i = 0;
    BOOL didFrozen;
    HITEM   item;
    DWORD   style, flags;
    BOOL    isString = TRUE;
    int     itemIndex;

    if (self == NULL || info == NULL || count ==0)
        return;

    didFrozen = _c(self)->isFrozen(self);
    _c(self)->freeze(self, TRUE);

    style = GetWindowStyle (self->hwnd);
    if (style & NCSS_LSTBOX_CHECKBOX
            || style & NCSS_LSTBOX_USEBITMAP) {
        isString = FALSE;
    }
    else {
        isString = TRUE;
    }

    for (i = 0; i < count; i++) {
        //insert(self, info->string, info->addData, info->flag, info->image, -1, TRUE);

        //we don't add null string item
        if (isString && (info->string == NULL || info->string[0] == '\0'))
            continue;

        item = _c(self)->createItem(self, 0, 0, -1, -1,
                info->string, info->addData, &itemIndex, TRUE);

        if (!item) {
            continue;
        }

        //set flags
        flags = NCSF_ITEM_NORMAL;
        if (!isString) {
            flags |= (info->flag & NCSF_ITEM_MASK);
        }
        flags |= (info->flag & NCSF_ITEM_IMAGEMASK);
        _c(self)->setFlags(self, item, flags);

        //set image
        if (info->image && (style & NCSS_LSTBOX_USEBITMAP)) {
            _c(self)->setImage(self, item, info->image);
        }

        info++;
    }

    if (!didFrozen)
        _c(self)->freeze(self, FALSE);
}

#define ITEM_BOTTOM(x)  (TOPITEM + x->itemVisibles - 1)

static int mListBox_setTopItem (mListBox *self, int index)
{
    int count = _c(self)->getItemCount(self);
    int maxTop;

    if (index < 0)
        index = 0;
    else {
        maxTop = ( count > self->itemVisibles) ?
                    (count - self->itemVisibles) : 0;

        if (index > maxTop)
            index = maxTop;
    }

    if (TOPITEM != index) {
        if (self->itemHilighted < index)
            self->itemHilighted = index;

        if (self->itemHilighted > ITEM_BOTTOM(self))
            self->itemHilighted = ITEM_BOTTOM(self);

        self->selItem = self->itemHilighted;

        _change_top_item(self, index);
    }

    return TOPITEM;
}

static int mListBox_onSizeChanged (mListBox* self, RECT* rcClient)
{
    int newVisH = RECTHP(rcClient) - self->topMargin - self->bottomMargin;
    if (newVisH < 0)
        newVisH = 0;

    //change the number of visible items
    self->itemVisibles = newVisH/self->defItemHeight;
    _c(self)->super->onSizeChanged((void*)self, rcClient);
    return 0;
}

static int mListBox_getItemHeight(mListBox *self, HITEM hItem)
{
    return self->defItemHeight;
}

static int mListBox_setItemHeight (mListBox *self, HITEM hItem, int height)
{
    if (height > 0 && self->defItemHeight != height) {
        RECT rcClient;
        int fontSize = GetWindowFont(self->hwnd)->size;

        self->defItemHeight = height;

        if (self->defItemHeight < fontSize)
            self->defItemHeight = fontSize;

        GetClientRect (self->hwnd, &rcClient);
        self->itemVisibles = self->visHeight/self->defItemHeight;

        //update content height
        _c(self)->setProperty(self,
                NCSP_SWGT_CONTHEIGHT, self->defItemHeight*_c(self)->getItemCount(self));

        _c(self)->setScrollInfo(self, TRUE);
        InvalidateRect (self->hwnd, NULL, TRUE);
    }

    return self->defItemHeight;
}

static int mListBox_setCurSel(mListBox* self, int newHilight)
{
    DWORD dwStyle = GetWindowStyle(self->hwnd);
    HITEM newItem;
    int oldHilight, newTop;

    if (newHilight < 0 || newHilight > _c(self)->getItemCount(self) - 1) {
        if (dwStyle & NCSS_LSTBOX_MULTIPLE)
            return -1;
        else
            return _c(self)->deselectByIdx(self, self->selItem);
    }

    oldHilight = self->itemHilighted;

    if (newHilight >= 0 && newHilight != oldHilight) {
        if (newHilight < self->itemVisibles) {
            newTop = 0;
        }
        else {
            newTop = _c(self)->getItemCount(self) - self->itemVisibles;
            newTop = (newTop > newHilight) ? newHilight : MAX(newTop, 0);
        }

        _change_top_item(self, newTop);
    }

    newItem = _c(self)->getItem (self, newHilight);

    if (newItem) {
        if (!_c(self)->isEnabled(self, newItem)) {
            _c(self)->deselectByIdx(self, newHilight);
            return 0;
        }
        else {
            _c(self)->selectByIdx(self, newHilight);
        }
    }

    if (newHilight >= 0 && newHilight != oldHilight) {
        self->itemHilighted = newHilight;
        self->selItem = self->itemHilighted;
        if (oldHilight >= 0)
            ncsNotifyParent ((mWidget*)self, NCSN_LSTBOX_SELCHANGED);
    }
    return 0;
}

static int mListBox_setSel(mListBox* self, int newSel, int flag)
{
    DWORD dwStyle, flags;
    HITEM newItem;

    if (self == NULL)
        return -1;

    dwStyle = GetWindowStyle(self->hwnd);
    if (!(dwStyle & NCSS_LSTBOX_MULTIPLE))
        return -1;

    newItem = _c(self)->getItem(self, newSel);

    if (newItem) {
        if (!_c(self)->isEnabled(self, newItem))
            return -1;

        flags = _c(self)->getFlags(self, newItem);

        if (flag == -1)
           flags ^= NCSF_ITEM_SELECTED;
        else if (flag == 0)
           flags &= ~NCSF_ITEM_SELECTED;
        else
           flags |= NCSF_ITEM_SELECTED;

        if (flags & NCSF_ITEM_SELECTED)
            _c(self)->select(self, newItem);
        else
            _c(self)->deselect(self, newItem);
        return 0;
    }
    return -1;
}

static DWORD mListBox_setProperty(mListBox* self,
        int id, DWORD value)
{
#ifdef _MGNCS_GUIBUILDER_SUPPORT
    if(id == NCSP_DEFAULT_CONTENT)
    {
        int i;
        char szText[100];

        _c(self)->removeAll(self);
        for(i=0; i<3; i++)
        {
            sprintf(szText,"Item %d", i+1);
            _c(self)->addString(self, szText, 0);
        }
        return 0;
    }
#endif
    if( id >= NCSP_LSTBOX_MAX)
        return -1;

    switch (id)
    {
        case NCSP_LSTBOX_ITEMHEIGHT:
            return _c(self)->setItemHeight(self, 0, value);

        case NCSP_LSTBOX_TOPITEM:
            return mListBox_setTopItem(self, value);

        case NCSP_LSTBOX_HILIGHTEDITEM:
            return _c(self)->setCurSel(self, value);
    }

    return Class(mItemView).setProperty((mItemView*)self, id, value);
}

static DWORD mListBox_getProperty(mListBox* self, int id)
{
    if( id >= NCSP_LSTBOX_MAX)
        return -1;

    switch (id)
    {
        case NCSP_LSTBOX_ITEMHEIGHT:
            return _c(self)->getItemHeight(self, 0);

        case NCSP_LSTBOX_TOPITEM:
            if (_c(self)->getItemCount(self) > 0)
                return TOPITEM;
            return -1;

        case NCSP_LSTBOX_HILIGHTEDITEM:
            return self->itemHilighted;

    }

    return Class(mItemView).getProperty((mItemView*)self, id);
}

static int mListBox_removeItemByIdx(mListBox* self, int index)
{
    HITEM hItem = _c(self)->getItem(self, index);
    return _c(self)->removeItem(self, hItem);
}

static int mListBox_removeItem(mListBox* self, HITEM hItem)
{
    int width, itemWidth = 0;
    int count, top = TOPITEM;

    if (!hItem)
        return -1;

    //get the item width of deleted item.
    if (self->hwnd) {
        itemWidth = getItemWidth(self, 0, (mItem*)hItem, GetWindowStyle(self->hwnd));
    }

    Class(mItemView).removeItem((mItemView*)self, hItem);

    //window has been destroied, return directly.
    if (self->hwnd == HWND_NULL)
        return 0;

    //if deleted item is the item of maximum width, we need recalculate item width.
    if (!_c(self)->isFrozen(self)) {
        width = _c(self)->getProperty(self, NCSP_LSTBOX_ITEMWIDTH);
        if (width <= itemWidth) {
            width = recalcContWidth(self);
            if (width > self->realContWidth)
                _c(self)->setProperty(self, NCSP_LSTBOX_ITEMWIDTH, width);
        }
    }

    //change hilight and select hItem
    count = _c(self)->getItemCount(self);

    if (self->itemHilighted >= count) {
        self->itemHilighted = count -1;
        if (self->itemHilighted < 0)
            self->itemHilighted = 0;
    }

    if (self->itemHilighted < top)
        self->itemHilighted = top;

    if (self->itemHilighted > ITEM_BOTTOM(self))
        self->itemHilighted = ITEM_BOTTOM(self);

    self->selItem = self->itemHilighted;

    //change top hItem
    if (top != 0 && count <= self->itemVisibles) {
        _change_top_item(self, 0);
    }
    else {
        if (_c(self)->indexOf(self, hItem) <= top) {
            top --;
            if (top < 0)
                top = 0;

            _change_top_item(self, top);
        }
    }

    return 0;
}

static BOOL mListBox_delString (mListBox *self, int index)
{
    HITEM item = _c(self)->getItem (self, index);
    return _c(self)->removeItem(self, item);
}

static void mListBox_onPaint (mListBox* self, HDC hdc, const PCLIPRGN pinv_clip)
{
    if (self->font != INV_LOGFONT) {
        SelectFont (hdc, self->font);
    }

    _draw_listbox(self, hdc);
}

static void setFocus (mListBox *self)
{
    if (self->listFlag & NCSF_LSTBOX_FOCUS)
        return;

    self->listFlag |= NCSF_LSTBOX_FOCUS;
}

static void killFocus (mListBox *self)
{
    self->listFlag &= ~NCSF_LSTBOX_FOCUS;
}

static void mListBox_onFontChanged (mListBox* self)
{
    RECT rcClient;
    PLOGFONT font = GetWindowFont (self->hwnd);

    if (self->defItemHeight < font->size)
        self->defItemHeight = font->size;

    if (self->font != INV_LOGFONT) {
        DestroyLogFont(self->font);
        self->font = INV_LOGFONT;
    }

    self->font = CreateLogFontIndirect(font);

    GetClientRect (self->hwnd, &rcClient);
    _c(self)->onSizeChanged(self, &rcClient);

    _c(self)->setScrollInfo(self, TRUE);
    InvalidateRect (self->hwnd, NULL, TRUE);
}

static BOOL mListBox_bold(mListBox* self,
        int index, BOOL bold)
{
    HITEM hItem;
    int count;
    DWORD flags;

    if (self->font == INV_LOGFONT)
        return FALSE;

    count = _c(self)->getItemCount(self);
    if (count<= 0
            || index < 0 || index > count)
        return FALSE;

    hItem = _c(self)->getItem(self, index);

    flags = _c(self)->getFlags(self, hItem);
    if (hItem) {
        if (bold)
            flags |= NCSF_LSTBOX_BOLD;
        else
            flags &= ~NCSF_LSTBOX_BOLD;

        _c(self)->setFlags(self, hItem, flags);
        invalidateUnderItem(self, index, FALSE);
        return TRUE;
    }
    return FALSE;
}

static LRESULT mListBox_wndProc(mListBox* self, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message){
        case MSG_CHAR:
            if (!(_c(self)->onChar (self, wParam, lParam)))
                return 0;

            break;

        case MSG_SETFOCUS:
            setFocus(self);
            break;

        case MSG_KILLFOCUS:
            killFocus(self);
            break;

        case MSG_GETTEXTLENGTH:
        case MSG_GETTEXT:
        case MSG_SETTEXT:
            return -1;

        case MSG_FONTCHANGED:
            mListBox_onFontChanged(self);
            return 0;

        case MSG_LBUTTONDBLCLK:
            ncsNotifyParent((mWidget*)self, NCSN_LSTBOX_DBCLK);
            break;
    }

    return Class(mItemView).wndProc((mItemView*)self,
            message, wParam, lParam);
}

static BOOL mListBox_deselectByIdx (mListBox* self, int index)
{
    HITEM hItem = _c(self)->getItem(self, index);
    return _c(self)->deselect(self, hItem);
}

static void mListBox_deselectAll(mListBox* self)
{
    Class(mItemView).deselectAll((mItemView*)self);
    self->itemHilighted = -1;
}

static int mListBox_select (mListBox* self, HITEM hItem)
{
    DWORD dwStyle;
    int oldSel;

    if (!INSTANCEOF(self, mListBox))
        return -1;

    if (!hItem)
        return -1;

    if (!_c(self)->isEnabled(self, hItem))
        return -1;

    oldSel = _c(self)->getCurSel(self);
    dwStyle = GetWindowStyle (self->hwnd);

    if (dwStyle & NCSS_LSTBOX_MULTIPLE) {
        BOOL isSel = TRUE;
        if (_c(self)->isSelected(self, hItem)) {
            isSel = FALSE;
        }

        if (_c(pItemList)->select(pItemList, hItem, isSel)) {
            _c(self)->refreshItem(self, hItem, NULL);
        }

        return oldSel;
    }

#if 0 /* VW: oldSel is an index */
    if (hItem != _c(self)->indexOf(self, oldSel)) {
#else
    if (hItem != _c(self)->getItem(self, oldSel)) {
#endif
        _c(self)->deselectByIdx(self, oldSel);
        Class(mItemView).select((mItemView*)self, hItem);
    }

    return self->itemHilighted;
}

static int mListBox_selectByIdx (mListBox* self, int index)
{
    HITEM newItem;
    DWORD dwStyle;
    int oldSel;

    if (!INSTANCEOF(self, mListBox))
        return -1;

    newItem = _c(self)->getItem (self, index);
    if (!newItem)
        return -1;

    dwStyle = GetWindowStyle (self->hwnd);

    if (dwStyle & NCSS_LSTBOX_MULTIPLE) {
        return _c(self)->select(self, newItem);
    }

    if (!_c(self)->isEnabled(self, newItem))
        return -1;

    oldSel = _c(self)->getCurSel(self);
    if (index != oldSel) {
        _c(self)->deselectByIdx(self, oldSel);
        _c(self)->select(self, newItem);
    }

    return self->itemHilighted;
}

static int procMouseMsg (mListBox* self, int x, int y, DWORD key_flags, UINT message)
{
    int     oldSel, hit;
    DWORD   dwStyle = GetWindowStyle (self->hwnd), flags;
    BOOL    click_mark = FALSE;
    HITEM   hitItem;
    int     count = _c(self)->getItemCount(self);
    int     mouseX = x, mouseY = y;

    if (count <= 0)
        return 1;

    _c(self)->windowToContent(self, &mouseX, &mouseY);
    hit = mouseY / self->defItemHeight;

    if (hit >= count)
        return 1;

    hitItem = _c(self)->getItem (self, hit);
    if (!hitItem)
        return 1;

    if (!_c(self)->isEnabled(self, hitItem))
        return 1;

    oldSel = _c(self)->selectByIdx(self, hit);

    if (dwStyle & NCSS_LSTBOX_CHECKBOX) {
        //get check mark position
        RECT itemRect, checkBoxRect;
        mListBox_getRectByIdx(self, hit, &itemRect, TRUE);
        getCheckMarkSize(self, getCheckMarkHeight(self), &checkBoxRect);
        OffsetRect(&checkBoxRect, itemRect.left, itemRect.top);

        if (mouseX >= checkBoxRect.left && mouseX <= checkBoxRect.right
               /* && mouseY >= checkBoxRect.top && mouseY <= checkBoxRect.bottom*/) {
            click_mark = TRUE;
            if (message == MSG_LBUTTONUP)
                ncsNotifyParent ((mWidget*)self, NCSN_LSTBOX_CLKCHKMARK);

            if (dwStyle & NCSS_LSTBOX_AUTOCHECK) {
                if (message == MSG_LBUTTONUP) {
                    flags = _c(self)->getFlags(self, hitItem);
                    switch (flags & NCSF_LSTBOX_CHECKMASK) {
                        case NCSF_LSTBOX_CMCHECKED:
                        case NCSF_LSTBOX_CMPARTCHECKED:
                            flags &= ~NCSF_LSTBOX_CHECKMASK;
                            _c(self)->setFlags(self, hitItem, flags);
                            if (message == MSG_LBUTTONUP)
                                ncsNotifyParentEx ((mWidget*)self, NCSN_LSTBOX_CMCANCEL, (DWORD)hit);
                            break;
                        default:
                            flags &= ~NCSF_LSTBOX_CHECKMASK;
                            flags |= NCSF_LSTBOX_CMCHECKED;
                            _c(self)->setFlags(self, hitItem, flags);
                            break;
                    }
                }
            }
        }
    }

    self->itemHilighted = hit;
    if (oldSel >= 0 && oldSel != hit)
        _c(self)->refreshItem(self, _c(self)->getItem(self, oldSel), NULL);

    if (message != MSG_MOUSEMOVE
            || ((dwStyle & NCSS_LSTBOX_MOUSEFOLLOW) && message == MSG_MOUSEMOVE))
        _c(self)->showItemByIdx (self, self->itemHilighted);

    _c(self)->setScrollInfo (self, TRUE);

    if (message == MSG_LBUTTONUP) {
        if (dwStyle & NCSS_LSTBOX_MOUSEFOLLOW) {
            oldSel = self->selItem;
            self->selItem = hit;
        }
        if (oldSel != hit)
            ncsNotifyParent ((mWidget*)self, NCSN_LSTBOX_SELCHANGED);
        if (!click_mark)
            ncsNotifyParent ((mWidget*)self, NCSN_LSTBOX_CLICKED);
    }

    return 0;
}


static int mListBox_onLButtonDown(mListBox *self, int x, int y, DWORD key_flags)
{
    DWORD style = GetWindowStyle(self->hwnd);

    if ((style & NCSS_LSTBOX_MULTIPLE)
            || !(style & NCSS_LSTBOX_MOUSEFOLLOW))
        return 1;

    return procMouseMsg(self, x, y, key_flags, MSG_LBUTTONDOWN);
}

static int mListBox_onLButtonUp(mListBox* self, int x, int y, DWORD key_flags)
{
    return procMouseMsg(self, x, y, key_flags, MSG_LBUTTONUP);
}

static int mListBox_onMouseMove(mListBox* self, int x, int y, DWORD key_flags)
{
    DWORD style = GetWindowStyle(self->hwnd);

    if ((style & NCSS_LSTBOX_MULTIPLE)
            || !(style & NCSS_LSTBOX_MOUSEFOLLOW))
        return 1;

    return procMouseMsg(self, x, y, key_flags, MSG_MOUSEMOVE);
}

static int mListBox_onKeyDown(mListBox* self, int scancode, DWORD key_flags)
{
    int     oldSel, newSel, newTop;
    RECT    rcInv;
    HITEM   newItem;
    DWORD   dwStyle = GetWindowStyle (self->hwnd);
    int     count;

    if (!self)
        return -1;

    count = _c(self)->getItemCount(self);
    if (count == 0)
        return 1;

    newTop = TOPITEM;
    newSel = self->itemHilighted;

    switch (scancode)
    {
        case SCANCODE_HOME:
            newSel = 0;
            newTop = 0;
            break;

        case SCANCODE_END:
            newSel = count - 1;
            if (count > self->itemVisibles)
                newTop = count - self->itemVisibles;
            else
                newTop = 0;
            break;

        case SCANCODE_CURSORBLOCKDOWN:
            newSel ++;
            if (newSel >= count)
                return 0;
            if (newSel > ITEM_BOTTOM (self))
                newTop ++;
            break;

        case SCANCODE_CURSORBLOCKUP:
            newSel --;
            if (newSel < 0)
                return 0;
            if (newSel < TOPITEM)
                newTop --;
            break;

        case SCANCODE_PAGEDOWN:
            newSel += self->itemVisibles;
            if (newSel >= count)
                newSel = count - 1;

            if (count - newSel >= self->itemVisibles)
                newTop = newSel;
            else
                newTop = MAX (count - self->itemVisibles, 0);
            break;

        case SCANCODE_PAGEUP:
            newSel -= self->itemVisibles;
            if (newSel < 0)
                newSel = 0;

            newTop -= self->itemVisibles;
            if (newTop < 0)
                newTop = 0;
            break;

        case SCANCODE_KEYPADENTER:
        case SCANCODE_ENTER:
            ncsNotifyParent ((mWidget*)self, NCSN_LSTBOX_ENTER);
            return 0;

        default:
            return 0;
    }

    GetClientRect (self->hwnd, &rcInv);

    if (self->itemHilighted != newSel) {

        if (TOPITEM != newTop) {

            newItem = _c(self)->getItem(self, newSel);
            if (!newItem) {
                return 1;
            }
            if (!(dwStyle & NCSS_LSTBOX_MULTIPLE)) {
                if(_c(self)->isEnabled(self, newItem)) {
                    oldSel = _c(self)->selectByIdx(self, newSel);

                    self->itemHilighted = newSel;
                    self->selItem = self->itemHilighted;

                    if (oldSel != newSel)
                        ncsNotifyParent ((mWidget*)self, NCSN_LSTBOX_SELCHANGED);
                }
                else
                    _c(self)->deselectByIdx(self, newSel);
            }
            else {
                self->itemHilighted = newSel;
                _c(self)->showItemByIdx(self, self->itemHilighted);
            }
            _change_top_item(self, newTop);
        }
        else {
            if (!(dwStyle & NCSS_LSTBOX_MULTIPLE)) {

                newItem = _c(self)->getItem(self, newSel);
                if (!newItem) {
                    return 1;
                }

                if(!_c(self)->isEnabled(self, newItem)) {
                    _c(self)->deselectByIdx(self, newSel);
                }
                else {
                    oldSel = _c(self)->selectByIdx (self, newSel);

                    if (oldSel != newSel)
                        ncsNotifyParent ((mWidget*)self, NCSN_LSTBOX_SELCHANGED);

                    if (oldSel >= 0) {
                        if (oldSel >= TOPITEM
                                && oldSel <= (ITEM_BOTTOM (self) + 1)) {
                            _c(self)->refreshItem(self,
                                    _c(self)->getItem(self, oldSel), NULL);
                        }
                    }
                }

                if (newSel < newTop) {
                    self->itemHilighted = newSel;
                    self->selItem = self->itemHilighted;
                    return 1;
                }

                self->itemHilighted = newSel;
                self->selItem = self->itemHilighted;
                _c(self)->showItemByIdx(self, newSel);
            }
            else {
                newItem = _c(self)->getItem(self, newSel);
                if (!newItem) {
                    return 1;
                }

                if(!_c(self)->isEnabled(self, newItem))
                    return 1;

                _c(self)->refreshItem(self,
                        _c(self)->getItem(self, self->itemHilighted), NULL);

                self->itemHilighted = newSel;

                _c(self)->showItemByIdx(self, newSel);
            }
        }
    }

    return 1;
}

static int mListBox_findString (mListBox* self,
        int start, char* string, BOOL bExact)
{
    HITEM hItem;
    int stringLen;
    NCS_CB_CMPSTR strCmp;

    if (*string == '\0')
        return -1;

    if (start >= _c(self)->getItemCount(self))
        start = 0;
    else if (start < 0)
        start = 0;

    hItem = _c(self)->getItem (self, start);
    stringLen = strlen (string);
    strCmp = _c(self)->getStrCmpFunc(self);

    while (hItem)
    {
        if (bExact && (stringLen != _c(self)->getTextLen(self, hItem))) {
            hItem = _c(self)->getNext(self, hItem);
            start ++;
            continue;
        }

        if (strCmp(string, _c(self)->getText(self, hItem), stringLen) == 0)
            return start;

        hItem = _c(self)->getNext(self, hItem);
        start ++;
    }

    return -1;
}

static int mListBox_onChar(mListBox* self,
        int asciiCode, DWORD keyFlags)
{
    unsigned char head [2];
    int index, newTop, count;
    HITEM hItem;
    DWORD dwStyle;
    DWORD flags;

    if (HIBYTE (asciiCode))
        return 1;

    head [0] = LOBYTE (asciiCode);
    head [1] = '\0';
    dwStyle = GetWindowStyle(self->hwnd);

    if (head[0] == ' ') {
        hItem = _c(self)->getItem (self, self->itemHilighted);
        if (!hItem) {
            return 1;
        }

        if (!_c(self)->isEnabled(self, hItem))
            return 1;

        flags = _c(self)->getFlags(self, hItem);

        if (dwStyle & NCSS_LSTBOX_MULTIPLE) {
            RECT rcInv;
            GetClientRect (self->hwnd, &rcInv);

            _c(self)->selectByIdx(self, self->itemHilighted);
        }
        else if (dwStyle & NCSS_LSTBOX_CHECKBOX) {
            ncsNotifyParent ((mWidget *)self, NCSN_LSTBOX_CLKCHKMARK);

            if (dwStyle & NCSS_LSTBOX_AUTOCHECK) {

                switch (flags & NCSF_LSTBOX_CHECKMASK) {

                    case NCSF_LSTBOX_CMCHECKED:
                        flags &= ~NCSF_LSTBOX_CHECKMASK;
                        _c(self)->setFlags(self, hItem, flags);
                        ncsNotifyParentEx ((mWidget *)self, NCSN_LSTBOX_CMCANCEL, (DWORD)self->itemHilighted);
                        break;
                    default:
                        flags &= ~NCSF_LSTBOX_CHECKMASK;
                        flags |= NCSF_LSTBOX_CMCHECKED;
                        _c(self)->setFlags(self, hItem, flags);
                        break;
                }

                _c(self)->showItemByIdx (self, self->itemHilighted);
            }
        }
        return 1;
    }

    index = _c(self)->findString (self, self->itemHilighted + 1, (char*)head, FALSE);

    if (index < 0) {
        index = _c(self)->findString (self, 0, (char*)head, FALSE);
    }

    if (index >= 0) {
        count = _c(self)->getItemCount(self);
        if (count - index >= self->itemVisibles)
            newTop = index;
        else
            newTop = MAX (count - self->itemVisibles, 0);

        self->itemHilighted = index;
        self->selItem = self->itemHilighted;

        if (!(dwStyle & NCSS_LSTBOX_MULTIPLE)) {
            hItem = _c(self)->getItem(self, index);
            if (hItem) {
                if(!_c(self)->isEnabled(self, hItem)) {
                    _c(self)->deselectByIdx(self, index);
                }
                else {
                    int oldSel = _c(self)->selectByIdx(self, index);

                    if (oldSel != index)
                        ncsNotifyParent ((mWidget*)self, NCSN_LSTBOX_SELCHANGED);
                }
            }
        }

        _change_top_item(self, newTop);
    }
    return 1;
}

static BOOL mListBox_setCheckMark (mListBox* self,
        int index, DWORD mark)
{
    HITEM hItem;
    DWORD flags;

    if (GetWindowStyle(self->hwnd) & NCSS_LSTBOX_CHECKBOX) {
        BOOL isChecked;
        hItem = _c(self)->getItem (self, index);

        if (hItem) {
            BOOL setChecked = FALSE;
            flags = _c(self)->getFlags (self, hItem);

            switch (mark) {
                case NCSF_LSTBOX_CMCHECKED:
                case NCSF_LSTBOX_CMPARTCHECKED:
                    setChecked = TRUE;
                    break;

                case NCSF_LSTBOX_CMBLANK:
                    break;

                default:
                    return FALSE;
            }

            isChecked = !(flags & NCSF_LSTBOX_CMBLANK);
            flags &= ~NCSF_LSTBOX_CHECKMASK;
            flags |= mark;

            _c(self)->setFlags(self, hItem, flags);
            //cacel checked status
            if (isChecked && !setChecked) {
                ncsNotifyParentEx ((mWidget*)self, NCSN_LSTBOX_CMCANCEL, (DWORD)index);
            }
            _c(self)->refreshItem(self, hItem, NULL);

            return TRUE;
        }
    }

    return FALSE;
}

static DWORD mListBox_getCheckMark (mListBox* self, int index)
{
    HITEM hItem;
    DWORD flags;

    if (GetWindowStyle(self->hwnd) & NCSS_LSTBOX_CHECKBOX) {
        hItem = _c(self)->getItem (self, index);

        if (hItem) {
            flags = _c(self)->getFlags (self, hItem);
            return flags & NCSF_LSTBOX_CHECKMASK;
        }
    }

    return 0;
}

static BOOL mListBox_enableByIdx(mListBox* self, int index, BOOL enable)
{
    HITEM hItem;
    BOOL ret;

    hItem = _c(self)->getItem(self, index);

    if (hItem) {
        ret = Class(mItemView).enable((mItemView*)self, hItem, enable);
        if (ret)
            _c(self)->refreshItem(self, hItem, NULL);

        return TRUE;
    }
    return FALSE;
}

static BOOL mListBox_enable(mListBox* self, HITEM hItem, BOOL enable)
{
    BOOL ret;

    if (hItem) {
        ret = Class(mItemView).enable((mItemView*)self, hItem, enable);
        if (ret) {
            _c(self)->refreshItem(self, hItem, NULL);
        }
        return ret;
    }
    return FALSE;
}

static int mListBox_getCurSel(mListBox* self)
{
    DWORD dwStyle = GetWindowStyle(self->hwnd);

    if (dwStyle & NCSS_LSTBOX_MULTIPLE) {
        return self->itemHilighted;
    }

    return _c(self)->super->getCurSel((mItemView *)self);
}

static int mListBox_hilight(mListBox* self, HITEM hItem)
{
    int index;

    if (!hItem)
        return -1;

    index = _c(self)->indexOf(self, hItem);
    if (GetWindowStyle(self->hwnd) & NCSS_LSTBOX_MULTIPLE) {
        if (index < 0 || index > _c(self)->getItemCount(self) - 1)
            return -1;

        self->itemHilighted = index;
        return index;
    }

    return _c(self)->setCurSel(self, index);
}

static HITEM mListBox_getHilight(mListBox* self)
{
    return _c(self)->getItem(self, self->itemHilighted);
}

static BOOL mListBox_isHilight(mListBox* self, HITEM hItem)
{
    int index = _c(self)->indexOf(self, hItem);

    return _c(self)->isHilightByIdx(self, index);
}

static BOOL mListBox_isHilightByIdx(mListBox* self, int index)
{
    if (index >= 0 && index == self->itemHilighted)
        return TRUE;

    return FALSE;
}

static int mListBox_getSelectionCount(mListBox* self)
{
    int count = 0;
    list_t *me, *first;
    HITEM tmp;

    first = _c(self)->getQueue(self);
    for (me = first->next; me != first; me = me->next) {
        tmp = _c(self)->getListEntry(self, me);
        if (_c(self)->isSelected(self, tmp)) {
            count ++;
        }
    }

    return count;
}

static BOOL mListBox_getSelection(mListBox* self,
        HITEM *pRet, int count)
{
    int i = 0;
    list_t *me, *first;
    HITEM tmp;

    if (!pRet || count <= 0)
        return FALSE;

    first = _c(self)->getQueue(self);
    for (me = first->next; me != first; me = me->next) {
        tmp = _c(self)->getListEntry(self, me);
        if (_c(self)->isSelected(self, tmp)) {
            pRet[i ++] = tmp;
        }
        if (i >= count)
            break;
    }

    return TRUE;
}

#ifdef _MGNCSDB_DATASOURCE
static BOOL mListBox_setSpecificData(mListBox* self, DWORD key, DWORD value, PFreeSpecificData free_special)
{
    if(key == NCSSPEC_OBJ_CONTENT)
    {
        mRecordSet *rs = SAFE_CAST(mRecordSet, value);
        int field_count;
        int field_type;
        if(!rs)
            return FALSE;
        field_count = _c(rs)->getFieldCount(rs);
        if(field_count <= 0)
            return FALSE;

        _c(self)->freeze(self, TRUE);

        //reset content
        _c(self)->resetContent(self);

        field_type = _c(rs)->getFieldType(rs, 1);

        _BEGIN_AUTO_STR
        _c(rs)->seekCursor(rs, NCS_RS_CURSOR_BEGIN, 0);
        while(!_c(rs)->isEnd(rs))
        {
            DWORD dw = _c(rs)->getField(rs, 1);
            if(field_count > 1)
                _c(self)->addString(self, _AUTO_STR(dw, field_type), _c(rs)->getField(rs,2));
            else
                _c(self)->addString(self, _AUTO_STR(dw, field_type), 0);

            _RESET_AUTO_STR
            _c(rs)->seekCursor(rs,NCS_RS_CURSOR_CUR, 1);
        }
        _c(self)->freeze(self, FALSE);

        _END_AUTO_STR
    }

    return Class(mItemView).setSpecificData((mItemView*)self, key, value, free_special);
}
#endif

BEGIN_CMPT_CLASS(mListBox, mItemView)
    CLASS_METHOD_MAP(mListBox, construct);
    CLASS_METHOD_MAP(mListBox, destroy);
    CLASS_METHOD_MAP(mListBox, onPaint);
    CLASS_METHOD_MAP(mListBox, wndProc);
    CLASS_METHOD_MAP(mListBox, onSizeChanged);
    CLASS_METHOD_MAP(mListBox, onKeyDown);
    CLASS_METHOD_MAP(mListBox, onLButtonDown);
    CLASS_METHOD_MAP(mListBox, onLButtonUp);
    CLASS_METHOD_MAP(mListBox, onMouseMove);
    CLASS_METHOD_MAP(mListBox, addString);
    CLASS_METHOD_MAP(mListBox, findString);
    CLASS_METHOD_MAP(mListBox, insertString);
    CLASS_METHOD_MAP(mListBox, delString);
    CLASS_METHOD_MAP(mListBox, addItems);
    CLASS_METHOD_MAP(mListBox, removeItem);
    CLASS_METHOD_MAP(mListBox, removeItemByIdx);
    CLASS_METHOD_MAP(mListBox, getCurSel);
    CLASS_METHOD_MAP(mListBox, setCurSel);
    CLASS_METHOD_MAP(mListBox, setSel);
    CLASS_METHOD_MAP(mListBox, resetContent);
    CLASS_METHOD_MAP(mListBox, select);
    CLASS_METHOD_MAP(mListBox, selectByIdx);
    CLASS_METHOD_MAP(mListBox, deselectAll);
    CLASS_METHOD_MAP(mListBox, deselectByIdx);
    CLASS_METHOD_MAP(mListBox, onChar);
    CLASS_METHOD_MAP(mListBox, setProperty);
    CLASS_METHOD_MAP(mListBox, getProperty);
    CLASS_METHOD_MAP(mListBox, bold);
    CLASS_METHOD_MAP(mListBox, enable);
    CLASS_METHOD_MAP(mListBox, enableByIdx);
    CLASS_METHOD_MAP(mListBox, setCheckMark);
    CLASS_METHOD_MAP(mListBox, getCheckMark);
    CLASS_METHOD_MAP(mListBox, hilight)
    CLASS_METHOD_MAP(mListBox, getHilight)
    CLASS_METHOD_MAP(mListBox, isHilight)
    CLASS_METHOD_MAP(mListBox, isHilightByIdx)
    CLASS_METHOD_MAP(mListBox, setItemHeight)
    CLASS_METHOD_MAP(mListBox, getItemHeight)
    CLASS_METHOD_MAP(mListBox, getSelection)
    CLASS_METHOD_MAP(mListBox, getSelectionCount)
    CLASS_METHOD_MAP(mListBox, freeze)
#ifdef _MGNCSDB_DATASOURCE
    CLASS_METHOD_MAP(mListBox, setSpecificData)
#endif
    CLASS_METHOD_MAP(mListBox, getRect)
    SET_DLGCODE(DLGC_WANTARROWS | DLGC_WANTCHARS | DLGC_WANTENTER);
END_CMPT_CLASS

#endif //_MGNCSCTRL_LISTBOX
