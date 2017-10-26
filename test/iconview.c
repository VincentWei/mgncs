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

#if defined _MGNCSCTRL_ICONVIEW && defined _MGNCSCTRL_DIALOGBOX

#define	IDC_ICONVIEW	100
#define	IDC_ADD			600
#define	IDC_DELETE		601

static HWND hIconView;

static BITMAP myicons[12];

static const char* iconfiles[12] = {
	"./res/acroread.png",
	"./res/icons.png",
	"./res/looknfeel.png",
	"./res/package_games.png",
	"./res/tux.png",
	"./res/xemacs.png",
	"./res/gimp.png",
	"./res/kpilot.png",
	"./res/multimedia.png",
	"./res/realplayer.png",
	"./res/usb.png",
	"./res/xmms.png"
};

static const char *iconlabels[12] = {
	"acroread",
	"icons",
	"looknfeel",
	"games",
	"tux",
	"xemacs",
	"gimp",
	"kpilot",
	"multimedia",
	"realplayer",
	"usb",
	"xmms"
};


static void mymain_onClose(mWidget* self, int message)
{
	_c((mDialogBox*)self)->endDialog((mDialogBox*)self, IDCANCEL);
}

static BOOL mymain_onKeyDown(mWidget* self, int message, int code,
		DWORD key_status)
{
	if (message == MSG_KEYDOWN) {
		if (code == SCANCODE_REMOVE) {
			mIconView *iconView;
			int curSel, count;
			HITEM delItem;

			iconView = (mIconView*)ncsObjFromHandle(
					GetDlgItem (self->hwnd, IDC_ICONVIEW));
			count = _c(iconView)->getItemCount(iconView);

			if (iconView) {
				curSel = _c(iconView)->getCurSel(iconView);

				if (curSel >= 0) {
					delItem = _c(iconView)->getItem(iconView, curSel);
					_c(iconView)->removeItem(iconView, delItem);
					if (curSel == count -1)
						curSel--;
					_c(iconView)->setCurSel(iconView, curSel);
				}
			}
		}
	}

	return FALSE;
}

static BOOL initIconView(mDialogBox* self)
{
	NCS_ICONV_ITEMINFO info;
	static int i = 0, j = 0, pos = 0;
	mIconView *iconView;

	for (i = 0; i < TABLESIZE(myicons); i++) {
		LoadBitmap (HDC_SCREEN, &myicons[i], iconfiles[i]);
	}

	hIconView = GetDlgItem (self->hwnd, IDC_ICONVIEW);

	iconView = (mIconView*)ncsObjFromHandle(hIconView);
	_c(iconView)->setIconSize(iconView, 55, 65);

	for (j = 0; j < 3; j ++) {
		for (i = 0; i < TABLESIZE(myicons); i++) {
			pos = 0;
			memset (&info, 0, sizeof(NCS_ICONV_ITEMINFO));
			info.bmp = &myicons[i];
			info.index = TABLESIZE(myicons) * j + i;
			info.label = iconlabels[i];
			info.addData = (DWORD)iconlabels[i];
			_c(iconView)->addItem(iconView, &info, &pos);

		}
	}

	_c(iconView)->setCurSel(iconView, 0);

	return TRUE;
}

static void iv_notify (mWidget *self, int id, int nc, DWORD add_data)
{
	if (nc == NCSN_ICONV_CLICKED) {
		if (self) {
			int idx;
			const char* text;
			mIconView *cls = (mIconView*)self;

			idx = _c(cls)->indexOf(cls, (HITEM)add_data);
			text = _c(cls)->getText(cls, (HITEM)add_data);

			fprintf (stderr, "click icon[%d], text is %s \n", idx, text);
		}
	}
}

static NCS_EVENT_HANDLER iv_handlers[] = {
	NCS_MAP_NOTIFY(NCSN_ICONV_CLICKED, iv_notify),
	NCS_MAP_NOTIFY(NCSN_ICONV_SELCHANGED, iv_notify),
	{ 0, NULL }
};

static void btn_notify(mWidget* self, int id, int nc, DWORD add_data)
{
	mIconView *iconView =
		(mIconView*)ncsGetChildObj(GetParent(self->hwnd), IDC_ICONVIEW);

	if (!iconView)
		return;

	switch (id) {
		case IDC_ADD: {
			char buff[12];
			int count, pos = 0;
			NCS_ICONV_ITEMINFO info;

			count = _c(iconView)->getItemCount(iconView);
			sprintf (buff, "NewIcon%i", count);

			memset (&info, 0, sizeof(NCS_ICONV_ITEMINFO));
			info.bmp = &myicons[0];
			info.index = count;
			info.label = buff;
			info.addData = (DWORD)"NewIcon";
			if (_c(iconView)->addItem(iconView, &info, &pos))
				_c(iconView)->setCurSel(iconView, pos);
			break;
		}

		case IDC_DELETE: {
			int count, sel;
			char *label = NULL;
			HITEM hItem;

			sel = _c(iconView)->getCurSel(iconView);
			count = _c(iconView)->getItemCount(iconView);
			hItem = _c(iconView)->getItem(iconView, sel);

			if (sel >= 0) {
				label = (char*)_c(iconView)->getAddData(iconView, hItem);
				_c(iconView)->removeItem(iconView, hItem);

				if (sel == count -1)
					sel --;

				_c(iconView)->setCurSel(iconView, sel);
			}
			break;
		}
	}
}

static NCS_EVENT_HANDLER btn_handlers[] = {
	NCS_MAP_NOTIFY(NCSN_BUTTON_PUSHED, btn_notify),
	{ 0, NULL }
};

static NCS_RDR_INFO iv_rdr_info = {
	"flat", "flat", NULL
};

//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_ICONVIEW,
		IDC_ICONVIEW,
		10, 10, 290, 300,
		WS_BORDER | WS_CHILD | WS_VISIBLE | NCSS_NOTIFY | NCSS_ICONV_LOOP,
		WS_EX_NONE,
		"",
		NULL, //props,
		&iv_rdr_info, //rdr_info
		iv_handlers, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_BUTTON,
		IDC_ADD,
		60, 330, 50, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
		"add",
		NULL, //props,
		NULL, //rdr_info
		btn_handlers, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_BUTTON,
		IDC_DELETE,
		170, 330, 50, 30,
		WS_VISIBLE | NCSS_NOTIFY,
		WS_EX_NONE,
		"delete",
		NULL, //props,
		NULL, //rdr_info
		btn_handlers, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
};

static NCS_EVENT_HANDLER mymain_handlers[] = {
	{ MSG_CLOSE, mymain_onClose },
	{ MSG_KEYDOWN, mymain_onKeyDown },
	{ 0, NULL }
};

//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_DIALOGBOX,
	7,
	0, 0, 310, 400,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
	"my friends",
	NULL,
	NULL,
	mymain_handlers,
	_ctrl_templ,
	sizeof(_ctrl_templ) / sizeof(NCS_WND_TEMPLATE),
	0,
	0,
	0,
};

int MiniGUIMain(int argc, const char* argv[])
{
	if (argc > 1) {
		iv_rdr_info.glb_rdr = argv[1];
		iv_rdr_info.ctl_rdr = argv[1];
	}

	ncsInitialize();

	mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect(
			&mymain_templ, HWND_DESKTOP);

	initIconView(mydlg);
	_c(mydlg)->doModal(mydlg, TRUE);

	ncsUninitialize();

	return 0;
}
#else //_MGNCSCTRL_ICONVIEW _MGNCSCTRL_DIALOGBOX

int main (void)
{
	printf("\n==========================================================\n");
	printf("======== You haven't enable the iconview, dialogbox contorl =====\n");
	printf("==========================================================\n");
	printf("============== ./configure --enable-iconview --enable-dialogbox ==========\n");
	printf("==========================================================\n\n");
	return 0;
}
#endif	//_MGNCSCTRL_ICONVIEW _MGNCSCTRL_DIALOGBOX

