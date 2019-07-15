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

#if defined _MGNCSCTRL_LISTVIEW && defined _MGNCSCTRL_CONTAINER && defined _MGNCSCTRL_COMBOBOX

HPACKAGE ghPkg = HPACKAGE_NULL;

void init_datasource(void)
{
	static  const char* ds_types[][2] = {
			{"Text Data Source", (const char*)DSTYPE_TEXT},
			{"Etc Data Source", (const char*)DSTYPE_ETC},
			{"SQL Data Source", (const char*)DSTYPE_SQLITE},
			{"XML Data Source", (const char*)DSTYPE_XML},
	};
	static unsigned char field_types[] = {NCS_BT_STR, NCS_BT_INT, NCS_BT_STR};
	ncsRegisterStaticData("/open-ds/dstype", (void *)ds_types, 4, 2, sizeof(char*), field_types);
}

int MiniGUIMain(int argc, const char* argv[])
{
#ifdef ntStartWindowEx
	MSG Msg;
	char f_package[MAX_PATH];
	mMainWnd *mWin;
	HPACKAGE hPkg;

	ncsInitialize();
	sprintf(f_package, "%s", "res/datasource.res");
	SetResPath("./");

	init_datasource();

	hPkg = ncsLoadResPackage (f_package);
	if (hPkg == HPACKAGE_NULL)
		return 1;

	ghPkg = hPkg;

	SetDefaultWindowElementRenderer(ncsGetString(hPkg, NCSRM_SYSSTR_DEFRDR));

	mWin = ntStartWindowEx(hPkg, HWND_DESKTOP, (HICON)0, (HMENU)0, (DWORD)0);

	while(GetMessage(&Msg, mWin->hwnd))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	ncsUnloadResPackage(hPkg);
	ncsUninitialize();
#endif

	return 0;
}


#else //_MGNCSCTRL_LISTVIEW && _MGNCSCTRL_CONTAINER && _MGNCSCTRL_COMBOBOX

int main (void)
{
	printf("\n==========================================================\n");
	printf("======== You haven't enable the combobox, listview, container contorl =====\n");
	printf("==========================================================\n");
	printf("============== ./configure --enable-combobox --enable-listview --enable-container ==========\n");
	printf("==========================================================\n\n");
	return 0;
}
#endif	//_MGNCSCTRL_COMBOBOX _MGNCSCTRL_CONTAINER
