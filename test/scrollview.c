/*
** $Id: scrollview.c 1682 2017-10-26 06:47:52Z weiym $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mgncs/mgncs.h>

#if defined _MGNCSCTRL_DIALOGBOX && defined _MGNCSCTRL_ICONVIEW


#define IDC_SCROLLVIEW	100
#define CAPTION			"My Friends"

const char *people[] = {
	"cao cao",
	"sun quan",
	"liu bei",
	"zhu ge liang",
	"guan yu",
	"pang tong",
	"si ma yu",
};

NCS_RDR_INFO rdr_info = {
	"classic","classic",NULL
	//"fashion","fashion",NULL
	///"skin","skin",NULL
	//"flat","flat",NULL
};


static void sv_notify (mWidget *self, int id, int nc, DWORD add_data)
{
	if (nc == NCSN_SCRLV_CLICKED) {
		if (self) {
			const char* info;
			mIconView *cls = (mIconView*)self;

			info = (const char*)_c(cls)->getAddData(cls, (HITEM)add_data);

			fprintf (stderr, "current item's addition data %s \n", info);
		}
	}
}

static NCS_EVENT_HANDLER sv_handlers[] = {
	NCS_MAP_NOTIFY(NCSN_SCRLV_CLICKED, sv_notify),
	{0, NULL }
};

static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_SCROLLVIEW,
		IDC_SCROLLVIEW,
		10, 10, 320, 150,
		WS_BORDER | WS_VISIBLE | NCSS_NOTIFY
			| NCSS_SCRLV_SORT,
		WS_EX_NONE,
		"",
		NULL, //props,
		&rdr_info,
		sv_handlers, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
};

static BOOL mymain_onKeyDown(mWidget* self,
		int message, int code, DWORD key_status)
{
	if (message == MSG_KEYDOWN) {
		if (code == SCANCODE_REMOVE) {
			mScrollView *scrollView;
			int curSel, count;
			HITEM delItem;

			scrollView = (mScrollView*)ncsObjFromHandle(
					GetDlgItem (self->hwnd, IDC_SCROLLVIEW));
			count = _c(scrollView)->getItemCount(scrollView);

			if (scrollView) {
				curSel = _c(scrollView)->getCurSel(scrollView);

				if (curSel >= 0) {
					delItem = _c(scrollView)->getItem(scrollView, curSel);
					_c(scrollView)->removeItem(scrollView, delItem);
					if (curSel == count -1)
						curSel--;
					_c(scrollView)->setCurSel(scrollView, curSel);
				}
			}
		}
	}

	return FALSE;
}

static NCS_EVENT_HANDLER mymain_handlers[] = {
	{MSG_KEYDOWN, mymain_onKeyDown},
	{0, NULL }
};

static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_DIALOGBOX,
	7,
	0, 0, 350, 200,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
	"My Friends",
	NULL,
	&rdr_info,
	mymain_handlers,
	_ctrl_templ,
	sizeof(_ctrl_templ)/sizeof(NCS_WND_TEMPLATE),
	0,
	0, 0,
};

int myCmpItem (mItemManager *manager, HITEM hItem1, HITEM hItem2)
{
	mScrollView *scrollView = (mScrollView*)manager->obj;
	const char *name1;
	const char *name2;

	if (scrollView) {
		name1 = (const char*)_c(scrollView)->getAddData(scrollView, hItem1);
		name2 = (const char*)_c(scrollView)->getAddData(scrollView, hItem2);
		return strcmp (name1, name2);
	}

	return 0;
}

void myDrawItem (mItemView *self, HITEM hItem, HDC hdc, RECT *rcDraw)
{
	const char *name = (const char*)_c(self)->getAddData(self, hItem);
	gal_pixel oldBrushClr, oldTextClr;
	BOOL isHilite = FALSE;

	SetBkMode (hdc, BM_TRANSPARENT);

	if (_c(self)->isHilight(self, hItem)) {
		isHilite = TRUE;
		oldBrushClr = SetBrushColor (hdc, PIXEL_blue);
		FillBox (hdc, rcDraw->left+1,
				rcDraw->top+1, RECTWP(rcDraw)-2, RECTHP(rcDraw)-1);
		oldTextClr = SetTextColor (hdc, PIXEL_lightwhite);
	}

	Rectangle (hdc, rcDraw->left, rcDraw->top, rcDraw->right - 1, rcDraw->bottom);
	TextOut (hdc, rcDraw->left + 3, rcDraw->top + 2, name);

	if (isHilite) {
		SetBrushColor (hdc, oldBrushClr);
		SetTextColor (hdc, oldTextClr);
	}
}

static BOOL initScrollView(mDialogBox* self)
{
	int i;
	HITEM hItem;
	HWND hScrollView = GetDlgItem (self->hwnd, IDC_SCROLLVIEW);
	mScrollView *scrollView = (mScrollView*)ncsObjFromHandle(hScrollView);
	NCS_SCRLV_ITEMINFO info;

	if (!scrollView)
		return FALSE;
	_c(scrollView)->freeze(scrollView, TRUE);
	//set itemCmp, set itemDraw
	_c(scrollView)->setItemCmpFunc(scrollView, myCmpItem);
	_c(scrollView)->setItemDraw(scrollView, myDrawItem);

	for (i = 0; i < TABLESIZE(people); i++) {
		info.height = 32;
		info.addData = (DWORD)people[i];
		info.index = i;
		hItem = _c(scrollView)->addItem(scrollView, &info, NULL);
	}

	_c(scrollView)->freeze(scrollView, FALSE);

	return TRUE;
}

int MiniGUIMain(int argc, const char* argv[])
{
	if (argc > 1) {
		rdr_info.glb_rdr = argv[1];
		rdr_info.ctl_rdr = argv[1];
	}

	ncsInitialize();

	mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect(
			&mymain_templ, HWND_DESKTOP);

	initScrollView(mydlg);
	_c(mydlg)->doModal(mydlg, TRUE);

	ncsUninitialize();

	return 0;
}

#else //_MGNCSCTRL_DIALOGBOX _MGNCSCTRL_ICONVIEW

int main (void)
{
	printf("\n==========================================================\n");
	printf("======== You haven't enable the dialogbox, iconview contorl =====\n");
	printf("==========================================================\n");
	printf("============== ./configure --enable-dialogbox --enable-iconview ==========\n");
	printf("==========================================================\n\n");
	return 0;
}
#endif	//_MGNCSCTRL_DIALOGBOX _MGNCSCTRL_ICONVIEW
