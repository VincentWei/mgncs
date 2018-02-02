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
/*
 ============================================================================
 Name        : imetest.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Main Window in MiniGUI
 ============================================================================
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

#include "mobile-ime.h"

#if defined _MGNCSENGINE_DIGITPY && defined _MGNCSCTRL_IMWORDSEL

HPACKAGE hPackage = HPACKAGE_NULL;

mMobileIMManager * immanger = NULL;

int MiniGUIMain(int argc, const char* argv[])
{
#ifdef ntStartWindowEx
	MSG Msg;
	char f_package[MAX_PATH];
	mMainWnd *mWin;

	ncsInitialize();
	MGNCS_INIT_CLASS(mMobileIMManager);
	sprintf(f_package, "%s", "res/imetest.res");
	SetResPath("./");

	hPackage = ncsLoadResPackage (f_package);
	if (hPackage == HPACKAGE_NULL) {
		printf ("load resource package:%s failure.\n", f_package);
		return 1;
	}

	SetDefaultWindowElementRenderer(ncsGetString(hPackage, NCSRM_SYSSTR_DEFRDR));

	immanger = (mMobileIMManager *)NEW(mMobileIMManager);

	mWin = ntStartWindowEx(hPackage, HWND_DESKTOP, (HICON)0, (HMENU)0, (DWORD)0);


	while(GetMessage(&Msg, mWin->hwnd))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	DELETE(immanger);

	ncsUnloadResPackage(hPackage);
	ncsUninitialize();
#endif

	return 0;
}
#else //_MGNCSENGINE_DIGITPY

int main (void)
{
	printf("\n==========================================================\n");
	printf("======== You haven't enable the digitpyengine  =====\n");
	printf("==========================================================\n");
	printf("============== ./configure --enable-digitpyengine ==========\n");
	printf("==========================================================\n\n");
	return 0;
}

#endif // _MGNCSENGINE_DIGITPY

