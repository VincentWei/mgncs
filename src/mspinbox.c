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
#include "mem-slab.h"

#ifdef _MGNCSCTRL_SPINBOX

#define EDITOR_CHILD_ID 100

struct _mSpinBoxItemInfo{
	char * string;
	mSpinBoxItemInfo * next;
};

static BOOL mSpinBox_addItem (mSpinBox *self, char *item)
{
	int count = 0;
	mSpinBoxItemInfo * info = NULL;
    if(item == NULL || self == NULL || !INSTANCEOF(self, mSpinBox)
            || !(GetWindowStyle(self->hwnd) & NCSS_SPNBOX_STRING))
		return FALSE;

	//insert to list
	info =  _SLAB_ALLOC(sizeof(mSpinBoxItemInfo));
	if(!info)
		return FALSE;
	info->string = strdup(item);
	count = 1;

	if(self->info.items == NULL)
		self->info.items = info;
	else if(GetWindowStyle(self->hwnd)&NCSS_SPNBOX_SORT)
	{
		mSpinBoxItemInfo * tn = self->info.items;
		mSpinBoxItemInfo * prev = NULL;
		while(tn)
		{
			int cmp = strcmp(info->string, tn->string);
			if(cmp <= 0)
				break;
			prev = tn;
			tn = tn->next;
		}
		if(prev == NULL) //insert first
		{
			self->info.items = info;
			info->next = tn;
		}
		else
		{
			prev->next = info;
			info->next = tn;
		}
		//get count
		for(tn = self->info.items, count = 0; tn; tn = tn->next,count ++);
	}
	else
	{
		mSpinBoxItemInfo * tn = self->info.items;
		while(tn->next)
		{
			count ++;
			tn = tn->next;
		}
		count ++;
		tn->next = info;
	}

	if(Body){
		SetBodyProp(NCSP_SPNRPIECE_MINPOS, 0);
		SetBodyProp(NCSP_SPNRPIECE_MAXPOS, count-1);
		InvalidateRect(self->hwnd, NULL, TRUE);
	}

    return TRUE;
}

static inline mSpinBoxItemInfo* get_spinner_item(mSpinBox *self, int index)
{
	int i;
	mSpinBoxItemInfo *info;
	info = self->info.items;
	for(i=0; i<index && info; i++, info = info->next);
	return info;
}

static const char *mSpinBox_getItem (mSpinBox *self, int index)
{
	mSpinBoxItemInfo *info;
    if(self == NULL || !INSTANCEOF(self, mSpinBox)
            || !(GetWindowStyle(self->hwnd) & NCSS_SPNBOX_STRING))
		return NULL;

	info = get_spinner_item(self, index);

	return info?info->string:NULL;

}

static BOOL mSpinBox_setItem (mSpinBox *self, int index, char *string)
{
	mSpinBoxItemInfo *info;
    if(string  == NULL || self == NULL || !INSTANCEOF(self, mSpinBox)
            || !(GetWindowStyle(self->hwnd) & NCSS_SPNBOX_STRING))
		return FALSE;

	info = get_spinner_item(self, index);
	if(info)
	{
		if(info->string)
			free(info->string);
		info->string = strdup(string);

		if (index == GetBodyProp(NCSP_SPNRPIECE_CURPOS)){
			SetWindowText(GetDlgItem(self->hwnd, EDITOR_CHILD_ID), info->string);
            InvalidateRect(self->hwnd, NULL, TRUE);
		}

		return TRUE;
	}
	return FALSE;
}

static BOOL mSpinBox_removeItem (mSpinBox *self, int index)
{
	int count;
	mSpinBoxItemInfo *info;
    if(self == NULL || !INSTANCEOF(self, mSpinBox)
            || !(GetWindowStyle(self->hwnd) & NCSS_SPNBOX_STRING) || index < 0)
		return FALSE;

	if(self->info.items == NULL)
		return TRUE;

	if(index == 0)
	{
		info = self->info.items;
		self->info.items = info->next;
	}
	else
	{
		mSpinBoxItemInfo * t = self->info.items;
		int i;
		for(i = 0; i < index -1 && t; i++, t = t->next) ;
		if(!t || !t->next)
			return FALSE;
		info = t->next;
		t->next = info->next;
	}

	if(info->string)
		free(info->string);
	_SLAB_FREE(info);

	//reset the count
	for(info = self->info.items, count=0; info; info = info->next, count++);
    
	if (index >= count){
        SetBodyProp(NCSP_SPNRPIECE_CURPOS, count - 1);
	}
	InvalidateRect(self->hwnd, NULL, TRUE);

	SetBodyProp(NCSP_SPNRPIECE_MAXPOS, count > 0 ? count - 1 : 0);

	return TRUE;
}

static mObject* create_pieces(mSpinBox *self, mWidget * editor, DWORD dwStyle)
{
    mObject *body;
	mHotPiece *wrapper = (mHotPiece*)NEWPIECEEX(mWidgetWrapperPiece, editor);

	mHotPiece *piece;
	if(dwStyle & NCSS_SPNR_HORIZONTAL)
		piece = (mHotPiece*)NEWPIECEEX(mHSpinBoxPiece, wrapper);
	else
		piece = (mHotPiece*)NEWPIECEEX(mVSpinBoxPiece, wrapper);

	// ready to call super.createBody
	self->body = (mObject*)piece;

    body =  Class(mSpinner).createBody((mSpinner*)self);

    if(dwStyle & NCSS_SPNBOX_STRING)
	{
        self->info.items = NULL;
		_c(HP(body))->setProperty(HP(body), NCSP_SPNRPIECE_MAXPOS,   100);
		_c(HP(body))->setProperty(HP(body), NCSP_SPNRPIECE_MINPOS,   0);
		_c(HP(body))->setProperty(HP(body), NCSP_SPNRPIECE_LINESTEP, 1);
	}
	else
	{
		static char * num_format = "%d";
		self->info.num_format = num_format;
	}

    return body;
}

static  BOOL editor_onChar(mEdit * edit, UINT message, int ch, DWORD key_flags)
{
	mSpinBox * self = (mSpinBox*)(GetWindowAdditionalData(edit->hwnd));

	if(GetWindowStyle(self->hwnd)&NCSS_SPNBOX_STRING)
		return TRUE;
	else //number
	{
		if(!(ch >= '0' && ch <= '9')
			&& !(ch!='.' || ch!=',' || ch!=127)
			&& !(ch!='x' || ch!='X'))
			return TRUE;
	}
	return FALSE;
}

static int editor_onSetPieceProperty(mEdit *edit, UINT message, WPARAM wParam, LPARAM lParam)
{
	mSpinBox * self = (mSpinBox*)(GetWindowAdditionalData(edit->hwnd));

	if(wParam != PIECECOMM_PROP_POS)
		return FALSE;

	if(GetWindowStyle(self->hwnd)&NCSS_SPNBOX_STRING)
	{
		const char* str = _c(self)->getItem(self, (int)lParam);
		if(str == NULL)
			str = "";
		SetWindowText(edit->hwnd, str);
	}
	else //number
	{
		char szText[32];
		sprintf(szText, self->info.num_format, (int)lParam);
		SetWindowText(edit->hwnd, szText);
	}

	return TRUE;
}

static void editor_onChanged(mEdit *edit, LINT id, int nc, DWORD param)
{
	mSpinBox * self = (mSpinBox*)(GetWindowAdditionalData(edit->hwnd));
	char szText[100];

	if(!(GetWindowStyle(self->hwnd)&NCSS_SPNBOX_STRING))//number
	{
		int pos;
		GetWindowText(edit->hwnd, szText, sizeof(szText)-1);
		pos = strtol(szText, NULL, 0);
		_c(self)->setProperty(self, NCSP_SPNR_CURPOS, pos);
	}
}

static inline DWORD spinbox_style_to_editor_style(DWORD dwStyle) {
	return	WS_VISIBLE
			| (dwStyle&NCSS_SPNBOX_EDITBASELINE?NCSS_EDIT_BASELINE:0)
			| (dwStyle&NCSS_SPNBOX_EDITNOBORDER ? 0 : WS_BORDER)
			| (dwStyle&NCSS_SPNBOX_READONLY?NCSS_EDIT_READONLY:0)
			| (dwStyle&NCSS_SPNBOX_UPPERCASE?NCSS_EDIT_UPPERCASE:0)
			| (dwStyle&NCSS_SPNBOX_LOWERCASE?NCSS_EDIT_LOWERCASE:0);
}

static mObject* mSpinBox_createBody(mSpinBox *self)
{
	mObject *body;
	DWORD dwStyle = GetWindowStyle(self->hwnd);
	NCS_EVENT_HANDLER editor_handlers[] = {
		{MSG_CHAR, editor_onChar},
		{MSG_SETPIECE_PROPERTY, editor_onSetPieceProperty},
		{NCS_NOTIFY_CODE(NCSN_EDIT_CHANGE), editor_onChanged},
		{0, NULL}
	};
	//create a editor
	mWidget * editor = ncsCreateWindow(NCSCTRL_SLEDIT, "",
		spinbox_style_to_editor_style(dwStyle),
		WS_EX_NONE,
		EDITOR_CHILD_ID,
		0, 0, 0, 0,
		self->hwnd,
		NULL,
		NULL, //rdr_info
		editor_handlers, //handlers,
		(DWORD)self);

	body = create_pieces(self, editor, dwStyle);

	return body;
}

static int mSpinBox_onKeyDown(mSpinBox *self, int scancode, DWORD key_flags)
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

    switch (scancode)
    {
        case SCANCODE_HOME:
			if(Body)
			{
				int min = GetBodyProp(NCSP_SPNRPIECE_MINPOS);
				if(SetBodyProp(NCSP_SPNRPIECE_CURPOS, min))
					InvalidateRect(self->hwnd, NULL, TRUE);
			}
            break;
        case SCANCODE_END:
			if(Body)
			{
				int max = GetBodyProp(NCSP_SPNRPIECE_MAXPOS);
				if(SetBodyProp(NCSP_SPNRPIECE_CURPOS, max))
					InvalidateRect(self->hwnd, NULL, TRUE);
			}
            break;
        case SCANCODE_CURSORBLOCKDOWN:
        case SCANCODE_CURSORBLOCKUP:
        case SCANCODE_PAGEDOWN:
        case SCANCODE_PAGEUP:
			if(Body)
			{
				mSpinnerPiece * spinner = (mSpinnerPiece*)Body;
				_c(spinner)->lineStep(spinner, SCANCODE_CURSORBLOCKDOWN == scancode || SCANCODE_PAGEDOWN == scancode);
			}
			break;
    }
    return 0;
}

static void mSpinBox_destroy(mSpinBox *self)
{
	if(GetWindowStyle(self->hwnd)&NCSS_SPNBOX_STRING)
	{
		mSpinBoxItemInfo *item = self->info.items;
		while(item)
		{
			mSpinBoxItemInfo* t = item;
			item = item->next;
			if(t->string)
				free(t->string);
			_SLAB_FREE(t);
		}
	}

	Class(mSpinner).destroy((mSpinner*)self);
}

#ifdef _MGNCS_GUIBUILDER_SUPPORT
static BOOL mSpinBox_refresh(mSpinBox *self)
{
    DWORD dwStyle = GetWindowStyle(self->hwnd);
	mWidget * editor = ncsGetChildObj(self->hwnd, EDITOR_CHILD_ID);
	if(editor == NULL)
	{
		return Class(mWidget).refresh((mWidget*)self);
	}

	ExcludeWindowStyle(editor->hwnd, 0xFFFFFFFF);
	IncludeWindowStyle(editor->hwnd, spinbox_style_to_editor_style(GetWindowStyle(self->hwnd)));
	UpdateWindow(editor->hwnd, TRUE);

    DELPIECE(self->body);
	self->body = create_pieces(self, editor, dwStyle);

	if(self->renderer)
		_c(Body)->setRenderer(Body, self->renderer->rdr_name);
	else
		_c(Body)->setRenderer(Body, "classic");
	return TRUE;

}
#endif

static BOOL mSpinBox_onCreate(mSpinBox *self, LPARAM lParam)
{
	if(Class(mSpinner).onCreate((mSpinner*)self, lParam) && Body)
	{
		int cur = GetBodyProp(NCSP_SPNRPIECE_CURPOS);
		if(GetWindowStyle(self->hwnd) & NCSS_SPNBOX_STRING)
		{
			const char* str = _c(self)->getItem(self, cur);
			if(str)
				SetWindowText(GetDlgItem(self->hwnd, EDITOR_CHILD_ID), str);
		}
		else
		{
			char szNum[32];
			sprintf(szNum, "%d", cur);
			SetWindowText(GetDlgItem(self->hwnd, EDITOR_CHILD_ID), szNum);
		}
		return TRUE;
	}
	return FALSE;
}

static LRESULT mSpinBox_wndProc (mSpinBox* self, UINT message, WPARAM wParam, LPARAM lParam)
{
    PLOGFONT self_font;
    switch(message) {
        case MSG_SETFOCUS:
        {
            if ((GetWindowStyle(self->hwnd) & NCSS_SPNBOX_AUTOFOCUS))
                SetFocusChild (GetDlgItem(self->hwnd, EDITOR_CHILD_ID));
            break;
        }
        case MSG_KILLFOCUS:
        {
            SendDlgItemMessage (self->hwnd, EDITOR_CHILD_ID, MSG_KILLFOCUS, 0, 0);
            break;
        }
        case MSG_FONTCHANGED:
        {
            /* set editor's font with mSpinBox object's font */
            self_font = GetWindowFont(self->hwnd);
            SetWindowFont (GetDlgItem(self->hwnd, EDITOR_CHILD_ID), self_font);
            break;
        }
		default:
		break;
	}
	return Class(mSpinner).wndProc((mSpinner*)self, message, wParam, lParam);
}

BEGIN_CMPT_CLASS (mSpinBox, mSpinner)
    CLASS_METHOD_MAP (mSpinBox, createBody)
    CLASS_METHOD_MAP (mSpinBox, onCreate)
    CLASS_METHOD_MAP (mSpinBox, destroy)
    CLASS_METHOD_MAP (mSpinBox, onKeyDown)
	CLASS_METHOD_MAP (mSpinBox, addItem)
	CLASS_METHOD_MAP (mSpinBox, removeItem)
	CLASS_METHOD_MAP (mSpinBox, setItem)
	CLASS_METHOD_MAP (mSpinBox, getItem)
	CLASS_METHOD_MAP (mSpinBox, wndProc)
#ifdef _MGNCS_GUIBUILDER_SUPPORT
	CLASS_METHOD_MAP (mSpinBox, refresh)
#endif
	SET_DLGCODE(DLGC_WANTALLKEYS)
END_CMPT_CLASS

#endif //_MGNCSCTRL_SPINBOX
