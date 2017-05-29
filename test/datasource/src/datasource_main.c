/*
 ============================================================================
 Name        : datasource.c
 Author      : dongjunjie
 Version     :
 Copyright   : feynman
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

	MainWindowThreadCleanup(mWin->hwnd);
	ncsUnloadResPackage(hPkg);
#endif

	return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif
