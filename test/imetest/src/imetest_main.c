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

