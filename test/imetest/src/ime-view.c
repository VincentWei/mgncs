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
 *   This file is part of mGNCS, a component for MiniGUI.
 * 
 *   Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/blog/minigui-licensing-policy/>.
 */
/**************************************************************
*  This file is generated automatically, don't modify it.
**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>

#include "resource.h"
#include "ncs-windows.h"
#include "mobile-ime.h"

#if defined _MGNCSENGINE_DIGITPY && defined _MGNCSCTRL_IMWORDSEL

//START_OF_IMEVIEW_ONUPDATE
static int IdmImeview_onUpdate(mMainWnd* self, int message, WPARAM wParam, LPARAM lParam)
{
	DWORD flags = wParam;
	mIME* ime = mIME_FromView(self->hwnd);

	if(flags & NCS_IME_UPDATE_KEY)
	{
		int idpath[] = {ID_IMKEYS,NCSP_IMWS_ITERATOR,-1};
		_c(self)->setPropertyByPath(self, idpath, (DWORD)ime->key_it);
	}
	_c(ime->key_it)->locate(ime->key_it, 0, SEEK_SET);
	if(flags & NCS_IME_UPDATE_WORDS)
	{
		mIMIterator * sub_it = _c((mDigitIterator*)(ime->key_it))->subIterator((mDigitIterator*)(ime->key_it));
		int idpath[] = {ID_IMWORDS,NCSP_IMWS_ITERATOR,-1};
		if(!sub_it || _c(sub_it)->count(sub_it) <= 0 )
		{
			sub_it = ime->assoc_it;
		}
		_c(self)->setPropertyByPath(self, idpath, (DWORD)sub_it);
	}
	return 0;
}
//END_OF_IMEVIEW_ONUPDATE

//START_OF_IMEVIEW_ONGETSELWORD
static int IdmImeview_onGetSelWord(mMainWnd* self, int message, WPARAM wParam, LPARAM lParam)
{
	int idpath[] ={ID_IMWORDS,NCSP_IMWS_CURWORD,-1};
	if(wParam) //is raw word
		idpath[1] = NCSP_IMWS_CURRAWWORD;

	return (int)_c(self)->getPropertyByPath(self, idpath);
}
//END_OF_IMEVIEW_ONGETSELWORD

//$func @4114546688 onCreate -- Need by merge, don't modify
static BOOL IdmImeview_onCreate (mWidget* self, DWORD dwAddData)
{

	//TODO:
	/*mIME *ime = mIME_FromView(self->hwnd);
	int idpath[] = {ID_IMKEYS,  NCSP_IMWS_CHARSETCONVERTER, -1};
	_c(self)->setPropertyByPath(self, idpath, (DWORD)&(ime->charset_conv));
	idpath[0] = ID_IMWORDS;
	_c(self)->setPropertyByPath(self, idpath, (DWORD)&(ime->charset_conv));*/
	return TRUE;
}


//START_OF_IMEVIEW_ONKEYDOWN
//$func @4114546688 onKeyDown
static BOOL IdmImeview_onKeyDown (mWidget* self, int message, int scancode, DWORD key_status)
{

	//TODO:
	if(scancode == SCANCODE_CURSORBLOCKLEFT
					|| scancode == SCANCODE_CURSORBLOCKRIGHT
					|| scancode == SCANCODE_CURSORBLOCKUP
					|| scancode == SCANCODE_CURSORBLOCKDOWN)
	{
		mIMWordSel * imw = (mIMWordSel*) _c(self)->getChild(self, ID_IMWORDS);
		if(!mIMWordSel_isEmpty(imw))
		{
			SendMessage(imw->hwnd, MSG_KEYDOWN, scancode, key_status);
			return TRUE;
		}
		mIMView_passMessage(self->hwnd, MSG_KEYDOWN, scancode, key_status);
	}
	return FALSE; /*continue Default message process*/
}
//END_OF_IMEVIEW_ONKEYDOWN

//START_OF_IMEVIEW_ONCHAR
//$func @4114546688 onChar
static BOOL IdmImeview_onChar (mWidget* self, int message, int scancode, DWORD key_status)
{

	//TODO:
	char ch = scancode;
	if(ch == '*' || ch == '0')
	{
		mIMWordSel * imw = (mIMWordSel*) _c(self)->getChild(self, ID_IMKEYS);
		if(!mIMWordSel_isEmpty(imw))
		{
			SendMessage(imw->hwnd, MSG_KEYDOWN, ch=='*'?SCANCODE_CURSORBLOCKRIGHT:SCANCODE_CURSORBLOCKLEFT, key_status);
			return TRUE;
		}
		mIMView_passMessage(self->hwnd, MSG_KEYDOWN, scancode, key_status);
	}
	return FALSE; /*continue default message process*/
}
//END_OF_IMEVIEW_ONCHAR

//$func @4114546688 onSetFocus
static void IdmImeview_onSetFocus (mWidget* self, int message)
{

	//TODO:
	printf(" ++++++ ime set focus\n");
}

//$handle @4114546688 -- Need by merge, don't modify
static NCS_EVENT_HANDLER IdmImeview_handlers [] = {
	{MSG_CREATE,IdmImeview_onCreate},
	{MSG_CHAR,IdmImeview_onChar},
	{MSG_SETFOCUS,IdmImeview_onSetFocus},
	{MSG_KEYDOWN,IdmImeview_onKeyDown},
//$user -- TODO add your handlers hear
	{NCS_IMM_UPDATE, IdmImeview_onUpdate},
	{NCS_IMM_GETSELWORD, IdmImeview_onGetSelWord},
	{-1,NULL}
};

//START_OF_IMEVIEW_IMWORDS_ON_IMEKEYS_SELCHANGED
//$func #4114546688 NCSN_IMWS_SELCHANGED_1505738752_2102675456 -- Need by merge, don't modify
static BOOL imwords_on_imkeys_selchanged (mIMWordSel *self, mIMWordSel* sender, int id, DWORD param)
{

	//TODO:
	mIME* ime = mIME_FromView(GetParent(self->hwnd));
	mIMIterator * sub_it;
	sub_it = _c((mDigitIterator*)(ime->key_it))->subIterator((mDigitIterator*)(ime->key_it));
	_c(self)->setProperty(self, NCSP_IMWS_ITERATOR,(DWORD)sub_it);
	return TRUE; /* allow the event to go next */
}
//END_OF_IMEVIEW_IMWORDS_ON_IMEKEYS_SELCHANGED

//$connect #4114546688 -- Need by merge, don't modify
static NCS_EVENT_CONNECT_INFO IdmImeview_connects [] = {
	{1505738752, 2102675456, NCSN_IMWS_SELCHANGED, (NCS_CB_ONOBJEVENT)imwords_on_imkeys_selchanged},/* ID_IMKEYS - ID_IMWORDS */
//$user -- TODO add your handlers hear
	{-1, -1, 0, NULL}
};

//$mainhandle -- Need by merge, don't modify
NCS_EVENT_HANDLER_INFO mainwnd_IdmImeview_handlers [] = {
	{-180420608, IdmImeview_handlers}, /*IDM_IMEVIEW*/
//$user --TODO: Add your handlers here
	{-1, NULL}
};

//$mainwnd_entry -- don't modify
NCS_WND_EXPORT mMainWnd* ntCreateIdmImeviewEx(HPACKAGE package, HWND hParent, HICON h_icon, HMENU h_menu, DWORD user_data)
{
	// please don't change me
	return ncsCreateMainWindowIndirectFromID(package,
		IDM_IMEVIEW, /* window id */
		hParent,
		h_icon,
		h_menu,
		mainwnd_IdmImeview_handlers,
		IdmImeview_connects,
		user_data);
}
#endif // _MGNCSENGINE_IME
