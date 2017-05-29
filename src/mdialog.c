
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncsconfig.h"
#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#include "mwidget.h"
#include "mpanel.h"
#include "mmainwnd.h"
#include "mdialog.h"

static int mDialogBox_wndProc(mDialogBox* self, int message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case MSG_COMMAND:
		if(GetWindowStyle(self->hwnd) & NCSS_MNWND_MODE)
		{
			switch(LOWORD(wParam))
			{
			case IDCANCEL:
				_c(self)->endDialog(self, 0);
				return 0;
			case IDOK:
				_c(self)->endDialog(self, 1);
				return 0;
			}
		}
		break;
	}
	return Class(mMainWnd).wndProc((mMainWnd*)self, message, wParam, lParam);
}



BEGIN_CMPT_CLASS(mDialogBox, mMainWnd)
	CLASS_METHOD_MAP(mDialogBox, wndProc)
END_CMPT_CLASS
