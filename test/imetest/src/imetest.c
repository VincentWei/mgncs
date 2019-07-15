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
 *   <http://www.minigui.com/en/about/licensing-policy/>.
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

#if defined _MGNCSENGINE_DIGITPY && defined _MGNCSCTRL_IMWORDSEL

//$func @1442562048 onCreate -- Need by merge, don't modify
static BOOL Mainwnd1_onCreate (mWidget* self, DWORD dwAddData)
{

	//TODO:
	int idpath[] = {ID_IMEGROUP, NCSP_BTNGRP_SELIDX, -1};
	int id = _c(self)->getPropertyByPath(self, idpath);
	if(id == ID_RBPTI)
		_c(immanger)->setIME(immanger,"abc*");
	else
		_c(immanger)->setIME(immanger, "pinyin");
	return TRUE;
}

//$handle @1442562048 -- Need by merge, don't modify
static NCS_EVENT_HANDLER Mainwnd1_handlers [] = {
	{MSG_CREATE,Mainwnd1_onCreate},
//$user -- TODO add your handlers hear
	{-1,NULL}
};

//$func #1442562048 NCSN_BTNGRP_SELCHANGED_1230929920_1442562048 -- Need by merge, don't modify
static BOOL mainwnd1_on_imegroup_radiobuttonchanged (mMainWnd *self, mButtonGroup* sender, int id, DWORD param)
{

	//TODO:
	if(param == ID_RBPTI)
		_c(immanger)->setIME(immanger,"abc*");
	else
		_c(immanger)->setIME(immanger, "pinyin");
	return TRUE; /* allow the event to go next */
}

//$connect #1442562048 -- Need by merge, don't modify
static NCS_EVENT_CONNECT_INFO Mainwnd1_connects [] = {
	{1230929920, 1442562048, NCSN_BTNGRP_SELCHANGED, (NCS_CB_ONOBJEVENT)mainwnd1_on_imegroup_radiobuttonchanged},/* ID_IMEGROUP - ID_MAINWND1 */
//$user -- TODO add your handlers hear
	{-1, -1, 0, NULL}
};

//$mainhandle -- Need by merge, don't modify
NCS_EVENT_HANDLER_INFO mainwnd_Mainwnd1_handlers [] = {
	{1442562048, Mainwnd1_handlers}, /*ID_MAINWND1*/
//$user --TODO: Add your handlers here
	{-1, NULL}
};

//$mainwnd_entry -- don't modify
NCS_WND_EXPORT mMainWnd* ntCreateMainwnd1Ex(HPACKAGE package, HWND hParent, HICON h_icon, HMENU h_menu, DWORD user_data)
{
	// please don't change me
	return ncsCreateMainWindowIndirectFromID(package,
		ID_MAINWND1, /* window id */
		hParent,
		h_icon,
		h_menu,
		mainwnd_Mainwnd1_handlers,
		Mainwnd1_connects,
		user_data);
}
#endif //#ifdef _MGNCSENGINE_IME
