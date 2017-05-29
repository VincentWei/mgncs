/*
 ============================================================================
 Name        : databinding.c
 Author      : dongjunjie
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

int MiniGUIMain(int argc, const char* argv[])
{
#ifdef ntStartWindowEx
	MSG Msg;
	char f_package[MAX_PATH];
	mMainWnd *mWin;
	HPACKAGE hPkg;

	ncsInitialize();
	sprintf(f_package, "%s", "res/databinding.res");
	SetResPath("./");

	hPkg = ncsLoadResPackage (f_package);
	if (hPkg == HPACKAGE_NULL)
		return 1;

	SetDefaultWindowElementRenderer(ncsGetString(hPkg, NCSRM_SYSSTR_DEFRDR));

	mWin = ntStartWindowEx(hPkg, HWND_DESKTOP, (HICON)0, (HMENU)0, (DWORD)0);

	while(GetMessage(&Msg, mWin->hwnd))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	MainWindowThreadCleanup(mWin->hwnd);
	ncsUnloadResPackage(hPkg);
#endif

	return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif
