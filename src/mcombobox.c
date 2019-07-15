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

#ifdef _MGNCSDB_DATASOURCE

#include "mdatabinding.h"
#include "mdatasource.h"

#endif

#ifdef _MGNCSCTRL_COMBOBOX

#define  NCS_DEF_LISTHEIGHT     60

#define IDC_CEDIT               1001
#define IDC_CLISTBOX            1002


static BOOL mCombobox_addItem (mCombobox *self, const char *item, DWORD addData)
{
    if(self == NULL || !INSTANCEOF(self, mCombobox))
        return -1;

    _c(self->listbox)->addString (self->listbox, item, addData);

    return TRUE;
}

static const char *mCombobox_getItem (mCombobox *self, int index)
{
    if(self == NULL || !INSTANCEOF(self, mCombobox))
        return NULL;

    return _c(self->listbox)->getTextByIdx (self->listbox, index);
}

static BOOL mCombobox_setItem (mCombobox *self, int index, const char *string)
{
    if(self == NULL || !INSTANCEOF(self, mCombobox))
        return FALSE;

    _c(self->listbox)->setTextByIdx (self->listbox, index, string);
    if(self->edit &&  _c(self->listbox)->getCurSel(self->listbox) == index)
    {
        SetWindowText(self->edit->hwnd, _c(self->listbox)->getTextByIdx(self->listbox, index));
    }

    return TRUE;
}

static BOOL mCombobox_removeItem (mCombobox *self, int index)
{
    int cur_sel;
    if(self == NULL || !INSTANCEOF(self, mCombobox))
        return -1;

    cur_sel = _c(self->listbox)->getCurSel(self->listbox);
    _c(self->listbox)->delString (self->listbox, index);

    if(cur_sel == index)
    {
        int new_sel = _c(self->listbox)->getCurSel(self->listbox);
        if(new_sel == -1)
            SetWindowText(self->edit->hwnd, "");
        else
            SetWindowText(self->edit->hwnd, _c(self->listbox)->getTextByIdx(self->listbox, new_sel));

        ncsNotifyParent((mWidget *)self, NCSN_CMBOX_SELCHANGE);
    }

   return TRUE;
}

static void mCombobox_setAddData(mCombobox *self, int index, DWORD addData)
{
    HITEM item = _c(self->listbox)->getItem(self->listbox, index);
    _c(self->listbox)->setAddData(self->listbox, item, addData);
}

static DWORD mCombobox_getAddData(mCombobox *self, int index)
{
    HITEM item = _c(self->listbox)->getItem(self->listbox, index);
    return _c(self->listbox)->getAddData(self->listbox, item);
}

#define _COMBO_BTN_WIDTH   20
static void inline calcRect (mCombobox* self)
{
    RECT rc;
    int x = 0, edit_h;

    GetClientRect(self->hwnd, &rc);

    self->rcEdit.left   = 0;
    self->rcEdit.top    = 0;

    if(GetWindowStyle(self->hwnd) & NCSS_CMBOX_DROPDOWNLIST){
        self->rcBtn.left   = RECTW(rc) - _COMBO_BTN_WIDTH;
        self->rcBtn.top    = 0;
        self->rcBtn.right  = rc.right;
        self->rcBtn.bottom = rc.bottom;

        edit_h = RECTH(rc);

        self->rcEdit.right  = self->rcBtn.left - 2;
        self->rcEdit.bottom = edit_h;

    } else {
        edit_h = GetSysFontHeight (SYSLOGFONT_CONTROL) + 6;

        self->dropWidth = RECTH(rc) - edit_h - 2;

        self->rcEdit.bottom = edit_h;
        self->rcEdit.right  = RECTW(rc);

        memset(&self->rcBtn, 0, sizeof(RECT));
    }

    ClientToScreen(self->hwnd, &x, &edit_h);

    if (edit_h + self->dropWidth > g_rcScr.bottom)
    {
        self->rcList.left   = 0;
        self->rcList.top    = - 2 - self->dropWidth; //FIXME
        self->rcList.right  = RECTW(rc);
        self->rcList.bottom = -2;
    } else {
        self->rcList.left   = 0;
        self->rcList.top    = self->rcEdit.bottom + 2; //FIXME
        self->rcList.right  = RECTW(rc);
        self->rcList.bottom = self->rcList.top + self->dropWidth;
    }
}

static void editNotifyProc (HWND hCtrl, LINT id, int code, DWORD add_data)
{
    HWND hwnd = GetParent(hCtrl);
    mCombobox *self = (mCombobox*)ncsObjFromHandle(hwnd);

    if (code == NCSN_EDIT_CHANGE)
    {
        ncsNotifyParent((mWidget *)self, NCSN_CMBOX_EDITCHANGE);
    }
    return;
}

static void listboxNotifyProc (HWND hCtrl, LINT id, int code, DWORD add_data)
{
    int sel;
    const char *str;
    HWND hwnd = GetParent(hCtrl);
    mCombobox *self = (mCombobox*)ncsObjFromHandle(hwnd);

    if ((GetWindowStyle(hwnd) & NCSS_CMBOX_DROPDOWNLIST)
            && (code == NCSN_LSTBOX_CLICKED || code == NCSN_LSTBOX_KILLFOCUS)){
        if (IS_SET (self, NCSF_CMBOX_DROPPED)) {
            ShowWindow (self->listbox->hwnd, SW_HIDE);
            CLEAR_STATE (self, NCSF_CMBOX_DROPPED);

            ncsNotifyParent((mWidget *)self, NCSN_CMBOX_SELECTOK);
            ncsNotifyParent((mWidget *)self, NCSN_CMBOX_CLOSEUP);
        }
        return;
    }

    if (code == NCSN_LSTBOX_DBCLK ) {
        ncsNotifyParent((mWidget *)self, NCSN_WIDGET_DBCLICKED);
        return;
    }

    if (code != NCSN_LSTBOX_SELCHANGED)
        return;

    if ((sel = _c(self->listbox)->getCurSel(self->listbox)) >= 0) {
        str = _c(self->listbox)->getTextByIdx(self->listbox, sel);
        SendMessage (self->edit->hwnd, MSG_SETTEXT, 0, (LPARAM)str);
    }

    ncsNotifyParent((mWidget *)self, NCSN_CMBOX_SELCHANGE);

    return;
}

static void mCombobox_construct (mCombobox *self, DWORD addData)
{
    NCS_RDR_INFO edit_rdr_info, list_rdr_info;
    DWORD list_style, edit_style, ex_style, combo_style;

    g_stmPanelCls.construct((mPanel*)self, addData);

    combo_style = GetWindowStyle(self->hwnd);

    list_style = WS_VSCROLL | NCSS_NOTIFY | WS_THINFRAME;

    if (combo_style & NCSS_CMBOX_DROPDOWNLIST){
        list_style |= NCSS_LSTBOX_MOUSEFOLLOW;
    } else {
        list_style |= WS_VISIBLE | WS_BORDER;
    }

    self->dropWidth = NCS_DEF_LISTHEIGHT;

    if (combo_style & NCSS_CMBOX_SORT)
        list_style |= NCSS_LSTBOX_SORT;

    edit_style = WS_CHILD | NCSS_NOTIFY;
    if (!(combo_style & NCSS_CMBOX_EDITNOBORDER)) edit_style |= WS_BORDER;
    if (combo_style & NCSS_CMBOX_EDITBASELINE)    edit_style |= NCSS_EDIT_BASELINE;
    if (combo_style & NCSS_CMBOX_UPPERCASE)       edit_style |= NCSS_EDIT_UPPERCASE;
    if (combo_style & NCSS_CMBOX_LOWERCASE)       edit_style |= NCSS_EDIT_LOWERCASE;
    if (combo_style & NCSS_CMBOX_READONLY)        edit_style |= NCSS_EDIT_READONLY;

    if (GetWindowExStyle (self->hwnd) & WS_EX_TRANSPARENT)
        ex_style = WS_EX_TRANSPARENT;
    else
        ex_style = WS_EX_NONE;

    edit_rdr_info.glb_rdr = self->renderer->rdr_name;
    edit_rdr_info.ctl_rdr = self->renderer->rdr_name;
    edit_rdr_info.elements = NULL;

    self->edit= (mSlEdit *)ncsCreateWindow (NCSCTRL_SLEDIT,
            GetWindowCaption(self->hwnd), edit_style, ex_style,
            IDC_CEDIT, 0, 0, 10, 10, self->hwnd,
            NULL, &edit_rdr_info, NULL, 0);

    if (GetWindowStyle(self->hwnd) & NCSS_CMBOX_DROPDOWNLIST) {
        ex_style |= 0x20000000L; // (WS_EX_CTRLASMAINWIN)
    }
    SetWindowFont(self->edit->hwnd, GetWindowFont(self->hwnd));

    list_rdr_info.glb_rdr = self->renderer->rdr_name;
    list_rdr_info.ctl_rdr = self->renderer->rdr_name;
    list_rdr_info.elements = NULL;

    self->listbox = (mListBox *)ncsCreateWindow (NCSCTRL_LISTBOX,
            "", list_style, ex_style ,
            IDC_CLISTBOX, 0, 0, 10, 10, self->hwnd,
            NULL, &list_rdr_info, NULL, 0);

    SetWindowFont(self->listbox->hwnd, GetWindowFont(self->hwnd));

    SetNotificationCallback(self->edit->hwnd, editNotifyProc);
    SetNotificationCallback(self->listbox->hwnd, listboxNotifyProc);

}

static LRESULT mCombobox_wndProc (mCombobox* self, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case MSG_SETFOCUS:
        {
            if ((GetWindowStyle(self->hwnd) & NCSS_CMBOX_AUTOFOCUS)
                    || (GetWindowStyle(self->hwnd) & WS_TABSTOP))
                SetFocusChild (self->edit->hwnd);

            ncsNotifyParent ((mWidget *)self, NCSN_CMBOX_SETFOCUS);
            break;
        }
        case MSG_KILLFOCUS:
        {
            if ((GetWindowStyle(self->hwnd) & NCSS_CMBOX_DROPDOWNLIST)
                    && IS_SET (self, NCSF_CMBOX_DROPPED)) {
                ShowWindow (self->listbox->hwnd, SW_HIDE);
                CLEAR_STATE (self, NCSF_CMBOX_DROPPED);

                ncsNotifyParent ((mWidget *)self, NCSN_CMBOX_SELECTCANCEL);
                ncsNotifyParent ((mWidget *)self, NCSN_CMBOX_CLOSEUP);
            }

            SendMessage (self->edit->hwnd, MSG_KILLFOCUS, 0, 0);

            ncsNotifyParent ((mWidget *)self, NCSN_CMBOX_KILLFOCUS);
            break;
        }
        case MSG_MOUSEMOVEIN:
        {
            BOOL in_out = (BOOL)wParam;

            if((GetWindowStyle(self->hwnd) & NCSS_CMBOX_DROPDOWNLIST)
                    &&!in_out && IS_SET (self, NCSF_CMBOX_HILIGHT)) {
                CLEAR_STATE (self, NCSF_CMBOX_HILIGHT);
                InvalidateRect (self->hwnd, &self->rcBtn, TRUE);
            }
            break;
        }
        case MSG_SIZECHANGING:
        {
            const RECT* rcExpect = (const RECT*)wParam;
            RECT* rcResult = (RECT*)lParam;

            if (!(GetWindowStyle(self->hwnd) & NCSS_CMBOX_DROPDOWNLIST)) {
                int height = GetSysFontHeight (SYSLOGFONT_CONTROL) + 6;

                rcResult->left = rcExpect->left;
                rcResult->top = rcExpect->top;
                rcResult->right = rcExpect->right;
                rcResult->bottom = rcExpect->top + height;
                rcResult->bottom += self->dropWidth + 2;
                return 0;
            }
            break;
        }
        case MSG_FONTCHANGED:
        {
            if(self->edit)
                SetWindowFont(self->edit->hwnd, GetWindowFont(self->hwnd));
            if(self->listbox)
                SetWindowFont(self->listbox->hwnd, GetWindowFont(self->hwnd));
            return 0;
        }
    }

    return Class(mPanel).wndProc((mPanel*)self, message, wParam, lParam);
}

static int mCombobox_onMouseMove(mCombobox *self, int x, int y, DWORD key_flags)
{
    if (GetWindowStyle(self->hwnd) & NCSS_CMBOX_DROPDOWNLIST)
    {
        if(PtInRect(&self->rcBtn, x, y))
        {
            if(!IS_SET (self, NCSF_CMBOX_HILIGHT)) {
                    SET_STATE (self, NCSF_CMBOX_HILIGHT);
                InvalidateRect (self->hwnd, &self->rcBtn, TRUE);
                return 0;
            }
        }
        else
        {
            if(IS_SET (self, NCSF_CMBOX_HILIGHT)) {
                CLEAR_STATE (self, NCSF_CMBOX_HILIGHT);
                InvalidateRect (self->hwnd, &self->rcBtn, TRUE);
                return 0;
            }
        }
    }
    return 1;
}

static int mCombobox_onLButtonDown(mCombobox *self, int x, int y, DWORD key_flags)
{
    if(PtInRect(&self->rcBtn, x, y)) {
        if (GetWindowStyle(self->hwnd) & NCSS_CMBOX_DROPDOWNLIST)
        {
            if (IS_SET (self, NCSF_CMBOX_DROPPED)) {
                CLEAR_STATE (self, NCSF_CMBOX_DROPPED);
            } else {
                ShowWindow (self->listbox->hwnd, SW_SHOW);
                SetFocusChild (self->listbox->hwnd);
                SET_STATE (self, NCSF_CMBOX_DROPPED);
                ncsNotifyParent((mWidget *)self, NCSN_CMBOX_DROPDOWN);
            }

        } else {
            SetFocusChild (self->listbox->hwnd);
        }
        return 0;
    }

    return 1; //call default value
}

static int mCombobox_onKeyDown (mCombobox *self, int scancode, DWORD key_flags)
{
    HWND hCurFocus;
    int dlgCode = 0;

    hCurFocus = GetFocusChild (self->hwnd);

    if (hCurFocus) {
        dlgCode = ncsGetDlgCode(hCurFocus);
        if (dlgCode & DLGC_WANTALLKEYS)
            /* continue*/
            return 1;
    }

    if (!(GetWindowStyle (self->hwnd) & NCSS_CMBOX_DROPDOWNLIST)){
        _c(self->listbox)->onKeyDown(self->listbox, scancode, key_flags);
        return 0;
    }

    switch (scancode)
    {
        case SCANCODE_ESCAPE:
            if (IS_SET (self, NCSF_CMBOX_DROPPED)) {
                ShowWindow (self->listbox->hwnd, SW_HIDE);
                CLEAR_STATE (self, NCSF_CMBOX_DROPPED);

                ncsNotifyParent((mWidget *)self, NCSN_CMBOX_SELECTCANCEL);
                ncsNotifyParent((mWidget *)self, NCSN_CMBOX_CLOSEUP);

                SetFocusChild (self->edit->hwnd);
            }
            return 0;

        case SCANCODE_KEYPADENTER:
        case SCANCODE_ENTER:
            if (IS_SET (self, NCSF_CMBOX_DROPPED)) {
                ShowWindow (self->listbox->hwnd, SW_HIDE);
                CLEAR_STATE (self, NCSF_CMBOX_DROPPED);

                ncsNotifyParent((mWidget *)self, NCSN_CMBOX_SELECTOK);
                ncsNotifyParent((mWidget *)self, NCSN_CMBOX_CLOSEUP);

                SetFocusChild (self->edit->hwnd);
            }
            return 0;

        case SCANCODE_CURSORBLOCKUP:
        case SCANCODE_PAGEUP:
        case SCANCODE_CURSORBLOCKDOWN:
        case SCANCODE_PAGEDOWN:
            if (!IS_SET (self, NCSF_CMBOX_DROPPED)) {
                ShowWindow (self->listbox->hwnd, SW_SHOW);
                SetFocusChild (self->listbox->hwnd);
                SET_STATE (self, NCSF_CMBOX_DROPPED);

                ncsNotifyParent((mWidget *)self, NCSN_CMBOX_DROPDOWN);
            } else {
                _c(self->listbox)->onKeyDown(self->listbox, scancode, key_flags);
            }
            return 0;
    }
    return 0;
}

static int mCombobox_onSizeChanged(mCombobox *self, RECT* rtClient)
{
    calcRect(self);

    if (self->edit) {
        MoveWindow (self->edit->hwnd,
                self->rcEdit.left, self->rcEdit.top,
                RECTW(self->rcEdit), RECTH(self->rcEdit), TRUE);
        ShowWindow(self->edit->hwnd, SW_SHOW);
    }

    if (self->listbox->hwnd) {
        MoveWindow (self->listbox->hwnd,
                self->rcList.left, self->rcList.top,
                RECTW(self->rcList), RECTH(self->rcList), TRUE);
    }
    return 0;
}

static void mCombobox_onPaint (mCombobox *self, HDC hdc, const PCLIPRGN pinv_clip)
{
    if (!(GetWindowStyle(self->hwnd) & NCSS_CMBOX_DROPDOWNLIST)
            || IsRectEmpty (&self->rcBtn))
        return;

    self->renderer->drawDropDownBtn(self, hdc, &self->rcBtn);

    return;
}

static BOOL mCombobox_setProperty (mCombobox* self, int id, DWORD value)
{
#ifdef _MGNCS_GUIBUILDER_SUPPORT
    if(id == NCSP_DEFAULT_CONTENT)
    {
        if(self->listbox)
            _c(self->listbox)->setProperty(self->listbox, id, value);
        _c(self)->setProperty(self, NCSP_COMB_SELECT, 0);
        return TRUE;
    }
#endif
    if (id == NCSP_COMB_ITEMHEIGHT || id == NCSP_COMB_ITEMCOUNT)
        return _c(self->listbox)->setProperty(self->listbox, id, value);

    if (id >= NCSP_COMB_MAX)
        return FALSE;

    switch (id)
    {
        case NCSP_COMB_DROPDOWNHEIGHT:
        {
            RECT rc;
            if ((int)value < 0)
                return FALSE;

            self->dropWidth = (int)value;

            if(GetWindowStyle(self->hwnd) & NCSS_CMBOX_DROPDOWNLIST){
                GetWindowRect(self->listbox->hwnd, &rc);
                MoveWindow(self->listbox->hwnd, rc.left, rc.top,
                        RECTW(rc), self->dropWidth, TRUE);
            } else { //NCSS_CMBOX_SIMPLE
                int edit_h = GetSysFontHeight (SYSLOGFONT_CONTROL) + 6;
                GetWindowRect(self->hwnd, &rc);
                MoveWindow(self->hwnd, rc.left, rc.top, RECTW(rc),
                        edit_h + self->dropWidth + 2, TRUE);
            }

            return TRUE;
        }
        case NCSP_COMB_ITEMCOUNT:
            return FALSE;

        case NCSP_WIDGET_TEXT:
        {
            SendMessage (self->edit->hwnd, MSG_SETTEXT, 0, (LPARAM)value);
            return TRUE;
        }
        case NCSP_COMB_TEXTLIMIT:
            return _c(self->edit)->setProperty(self->edit, NCSP_EDIT_LIMITTEXT, value);

        case NCSP_COMB_SELECT:
        {
            int index;
            const char *selection;

            if ((int)value >= _c(self->listbox)->getProperty(self->listbox, NCSP_COMB_ITEMCOUNT)
                    || (int)value < 0
                    || -1 == _c(self->listbox)->setCurSel(self->listbox, value))
            {
                SetWindowText (self->edit->hwnd, "");
                return FALSE;
            }

            index = _c(self->listbox)->getCurSel(self->listbox);
            selection =  _c(self->listbox)->getTextByIdx(self->listbox, index);
            SetWindowText (self->edit->hwnd, selection);

            return TRUE;
        }
        case NCSP_WIDGET_RDR:
        {
            if(Class(mPanel).setProperty((mPanel*)self, id, value))
            {
                if(self->edit)
                    _c(self->edit)->setProperty(self->edit, id, value);
                if(self->listbox)
                    _c(self->listbox)->setProperty(self->listbox, id, value);
                return TRUE;
            }
            return FALSE;
        }
    }

    return Class(mPanel).setProperty ((mPanel *)self, id, value);
}

static DWORD mCombobox_getProperty (mCombobox* self, int id)
{
    if (id == NCSP_COMB_ITEMHEIGHT || id == NCSP_COMB_ITEMCOUNT)
        return _c(self->listbox)->getProperty(self->listbox, id);

    if (id >= NCSP_COMB_MAX)
        return 0;

    switch (id)
    {
        case NCSP_COMB_DROPDOWNHEIGHT :
        {
            return (DWORD)self->dropWidth;
        }
        case NCSP_WIDGET_TEXT:
        {
            int len;
            char *string;

            len = _c(self->edit)->getTextLength(self->edit);
            if (len <= 0)
                return 0;

            string = malloc(len + 1);
            if (!string)
                return 0;

            if (0 > SendMessage (self->edit->hwnd, MSG_GETTEXT,
                        (WPARAM)len, (LPARAM)string))
                return 0;

            return (DWORD)string;
        }
        case NCSP_COMB_TEXTLIMIT:
            return (DWORD)_c(self->edit)->getProperty(self->edit, NCSP_EDIT_LIMITTEXT);

        case NCSP_COMB_SELECT:
            return (DWORD)_c(self->listbox)->getCurSel(self->listbox);
    }

    return Class(mPanel).getProperty ((mPanel *)self, id);
}

static BOOL mCombobox_setSpecificData(mCombobox* self, DWORD key, DWORD value, PFreeSpecificData free_special)
{
    if(key == NCSSPEC_OBJ_CONTENT)
    {
        if(!self->listbox)
            return FALSE;
        return _c(self->listbox)->setSpecificData(self->listbox, key, value, free_special);
    }

    return Class(mPanel).setSpecificData((mPanel*)self, key, value, free_special);
}

static DWORD mCombobox_getSpecificData(mCombobox* self, DWORD key, BOOL *pok)
{
    if(key == NCSSPEC_OBJ_CONTENT)
    {
        if(!self->listbox)
        {
            if(*pok)
                *pok = FALSE;
            return 0L;
        }
        return _c(self->listbox)->getSpecificData(self->listbox, key, pok);
    }

    return Class(mPanel).getSpecificData((mPanel*)self, key, pok);
}

#ifdef _MGNCS_GUIBUILDER_SUPPORT
static BOOL mCombobox_refresh(mCombobox *self)
{
    DWORD list_style, edit_style, combo_style, ex_style;

    combo_style = GetWindowStyle(self->hwnd);

    edit_style = WS_CHILD | NCSS_NOTIFY;
    if (!(combo_style & NCSS_CMBOX_EDITNOBORDER))
        edit_style |= WS_BORDER;
    if (combo_style & NCSS_CMBOX_EDITBASELINE)
        edit_style |= NCSS_EDIT_BASELINE;
    if (combo_style & NCSS_CMBOX_UPPERCASE)
        edit_style |= NCSS_EDIT_UPPERCASE;
    if (combo_style & NCSS_CMBOX_LOWERCASE)
        edit_style |= NCSS_EDIT_LOWERCASE;
    if (combo_style & NCSS_CMBOX_READONLY)
        edit_style |= NCSS_EDIT_READONLY;

    ExcludeWindowStyle(self->edit->hwnd, 0xFFFFFFFF);
    IncludeWindowStyle(self->edit->hwnd, edit_style);

    if (GetWindowExStyle (self->hwnd) & WS_EX_TRANSPARENT)
        ex_style = WS_EX_TRANSPARENT;
    else
        ex_style = WS_EX_NONE;

    ExcludeWindowExStyle(self->edit->hwnd, 0xFFFFFFFF);
    IncludeWindowExStyle(self->edit->hwnd, ex_style);

    list_style = WS_VSCROLL | NCSS_NOTIFY | WS_THINFRAME;
    if (combo_style & NCSS_CMBOX_DROPDOWNLIST){
        list_style |= NCSS_LSTBOX_MOUSEFOLLOW;
    } else {
        list_style |= WS_VISIBLE | WS_BORDER;
    }

    ExcludeWindowStyle(self->listbox->hwnd, 0xFFFFFFFF);
    IncludeWindowStyle(self->listbox->hwnd, list_style);

    if (GetWindowStyle(self->hwnd) & NCSS_CMBOX_DROPDOWNLIST) {
        ex_style |= 0x20000000L; // (WS_EX_CTRLASMAINWIN)
    }

    ExcludeWindowExStyle(self->listbox->hwnd, 0xFFFFFFFF);
    IncludeWindowExStyle(self->listbox->hwnd, ex_style);

    if(self->renderer){
        SetWindowElementRenderer(self->edit->hwnd, self->renderer->rdr_name, NULL);
        SetWindowElementRenderer(self->listbox->hwnd, self->renderer->rdr_name, NULL);
    } else {
        SetWindowElementRenderer(self->edit->hwnd, "", NULL);
        SetWindowElementRenderer(self->listbox->hwnd, "", NULL);
    }
    UpdateWindow(self->edit->hwnd, TRUE);
    UpdateWindow(self->listbox->hwnd, TRUE);
    return TRUE;
}
#endif

BEGIN_CMPT_CLASS (mCombobox, mPanel)
    CLASS_METHOD_MAP (mCombobox, construct)
    CLASS_METHOD_MAP (mCombobox, wndProc)
    CLASS_METHOD_MAP (mCombobox, onPaint)
    CLASS_METHOD_MAP (mCombobox, onMouseMove)
    CLASS_METHOD_MAP (mCombobox, onLButtonDown)
    CLASS_METHOD_MAP (mCombobox, onKeyDown)
    CLASS_METHOD_MAP (mCombobox, onSizeChanged)
    CLASS_METHOD_MAP (mCombobox, setProperty)
    CLASS_METHOD_MAP (mCombobox, getProperty)
    CLASS_METHOD_MAP (mCombobox, addItem)
    CLASS_METHOD_MAP (mCombobox, removeItem)
    CLASS_METHOD_MAP (mCombobox, setItem)
    CLASS_METHOD_MAP (mCombobox, getItem)
    CLASS_METHOD_MAP (mCombobox, setAddData)
    CLASS_METHOD_MAP (mCombobox, getAddData)
    CLASS_METHOD_MAP (mCombobox, setSpecificData)
    CLASS_METHOD_MAP (mCombobox, getSpecificData)
#ifdef _MGNCS_GUIBUILDER_SUPPORT
    CLASS_METHOD_MAP (mCombobox, refresh)
#endif
    SET_DLGCODE(DLGC_WANTALLKEYS)
END_CMPT_CLASS

#endif        //_MGNCSCTRL_COMBOBOX
