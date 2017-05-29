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

#include "mgncs.h"

#include "resource.h"
#include "ncs-windows.h"

#include "mobile-ime.h"

HPACKAGE hPackage = HPACKAGE_NULL;

mMobileIMManager * immanger = NULL;

int MiniGUIMain(int argc, const char* argv[])
{
#ifdef ntStartWindowEx
	MSG Msg;
	char f_package[MAX_PATH];
	mMainWnd *mWin;

	ncsInitialize();
	sprintf(f_package, "%s", "res/imetest.res");
	SetResPath("./");

	hPackage = ncsLoadResPackage (f_package);
	if (hPackage == HPACKAGE_NULL) {
		printf ("load resource package:%s failure.\n", f_package);
		return 1;
	}

	SetDefaultWindowElementRenderer(ncsGetString(hPackage, NCSRM_SYSSTR_DEFRDR));

	MGNCS_INIT_CLASS(mMobileIMManager);
	immanger = NEW(mMobileIMManager);

	mWin = ntStartWindowEx(hPackage, HWND_DESKTOP, (HICON)0, (HMENU)0, (DWORD)0);


	while(GetMessage(&Msg, mWin->hwnd))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	DELETE(immanger);

	MainWindowThreadCleanup(mWin->hwnd);
	ncsUnloadResPackage(hPackage);
	ncsUninitialize();
#endif

	return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif
