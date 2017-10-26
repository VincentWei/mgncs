#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include <mgncs/mgncs.h>

#ifdef _MGNCSCTRL_DIALOGBOX


#define	IDC_IMAGE1		100
#define	IDC_IMAGE2		101
#define	IDC_IMAGE3		102
#define	IDC_IMAGE4		103
#define	IDC_IMAGE5		104
#define	IDC_IMAGE6		105
#define	IDC_IMAGE7		106
#define	IDC_IMAGE8		107

static BITMAP icon;
static BITMAP bitmap;


static void set_icon_info(mWidget* self, int id, PBITMAP pbmp, int align_id, int align)
{
	mImage *img;
	img = (mImage*)ncsGetChildObj(self->hwnd, id);

	if (img) {
		_c(img)->setProperty(img, NCSP_IMAGE_IMAGE, (DWORD)pbmp);
		_c(img)->setProperty(img, align_id, align);
	}
}


static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
	mImage *img = NULL;

	if (0 == LoadBitmap(HDC_SCREEN, &icon, "./res/icons.png")) {
		set_icon_info(self, IDC_IMAGE1, &icon, NCSP_STATIC_ALIGN, NCS_ALIGN_LEFT);
		set_icon_info(self, IDC_IMAGE2, &icon, NCSP_STATIC_ALIGN, NCS_ALIGN_CENTER);
		set_icon_info(self, IDC_IMAGE3, &icon, NCSP_STATIC_ALIGN, NCS_ALIGN_RIGHT);
		set_icon_info(self, IDC_IMAGE4, &icon, NCSP_STATIC_VALIGN, NCS_VALIGN_TOP);
		set_icon_info(self, IDC_IMAGE5, &icon, NCSP_STATIC_VALIGN, NCS_VALIGN_CENTER);
		set_icon_info(self, IDC_IMAGE6, &icon, NCSP_STATIC_VALIGN, NCS_VALIGN_BOTTOM);
	}

	if (0 == LoadBitmap(HDC_SCREEN, &bitmap, "./res/image_test.jpg")) {
		img = (mImage*)ncsGetChildObj(self->hwnd, IDC_IMAGE7);
		if (img) {
			_c(img)->setProperty(img, NCSP_IMAGE_IMAGE, (DWORD)&bitmap);
			_c(img)->setProperty(img, NCSP_IMAGE_DRAWMODE, NCS_DM_SCALED);
		}

		img = (mImage*)ncsGetChildObj(self->hwnd, IDC_IMAGE8);
		if (img) {
			_c(img)->setProperty(img, NCSP_IMAGE_IMAGE, (DWORD)&bitmap);
			_c(img)->setProperty(img, NCSP_IMAGE_DRAWMODE, NCS_DM_TILED);
		}
	}
	return TRUE;
}

static void mymain_onClose(mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
	UnloadBitmap(&bitmap);
}


//Propties for

//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = {

	{
		NCSCTRL_STATIC,
		0,
		10, 10, 100, 20,
		WS_VISIBLE,
		WS_EX_NONE,
		"image - left",
		NULL,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_IMAGE,
		IDC_IMAGE1,
		10, 30, 100, 100,
		WS_BORDER | WS_VISIBLE,
		WS_EX_NONE,
		"",
		NULL,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_STATIC,
		0,
		130, 10, 100, 20,
		WS_VISIBLE,
		WS_EX_NONE,
		"image - center",
		NULL,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_IMAGE,
		IDC_IMAGE2,
		130, 30, 100, 100,
		WS_BORDER | WS_VISIBLE,
		WS_EX_NONE,
		"",
		NULL,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_STATIC,
		0,
		250, 10, 100, 20,
		WS_VISIBLE,
		WS_EX_NONE,
		"image - right",
		NULL,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_IMAGE,
		IDC_IMAGE3,
		250, 30, 100, 100,
		WS_BORDER | WS_VISIBLE,
		WS_EX_NONE,
		"",
		NULL,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_STATIC,
		0,
		10, 140, 100, 20,
		WS_VISIBLE,
		WS_EX_NONE,
		"image - top",
		NULL,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_IMAGE,
		IDC_IMAGE4,
		10, 160, 100, 100,
		WS_BORDER | WS_VISIBLE,
		WS_EX_NONE,
		"",
		NULL,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_STATIC,
		0,
		130, 140, 100, 20,
		WS_VISIBLE,
		WS_EX_NONE,
		"image - middle",
		NULL,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_IMAGE,
		IDC_IMAGE5,
		130, 160, 100, 100,
		WS_BORDER | WS_VISIBLE,
		WS_EX_NONE,
		"",
		NULL,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_STATIC,
		0,
		250, 140, 100, 20,
		WS_VISIBLE,
		WS_EX_NONE,
		"image - middle",
		NULL,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_IMAGE,
		IDC_IMAGE6,
		250, 160, 100, 100,
		WS_BORDER | WS_VISIBLE,
		WS_EX_NONE,
		"",
		NULL,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_STATIC,
		0,
		10, 270, 100, 20,
		WS_VISIBLE,
		WS_EX_NONE,
		"bitmap - scaled",
		NULL,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_IMAGE,
		IDC_IMAGE7,
		10, 290, 280, 150,
		WS_BORDER | WS_VISIBLE,
		WS_EX_NONE,
		"",
		NULL,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_STATIC,
		0,
		300, 270, 100, 20,
		WS_VISIBLE,
		WS_EX_NONE,
		"bitmap-tiled",
		NULL,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
	{
		NCSCTRL_IMAGE,
		IDC_IMAGE8,
		300, 290, 280, 150,
		WS_BORDER | WS_VISIBLE,
		WS_EX_NONE,
		"",
		NULL,
		NULL, //rdr_info
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	},
};


static NCS_EVENT_HANDLER mymain_handlers[] = {
	{ MSG_CREATE, mymain_onCreate },
	{ MSG_CLOSE, mymain_onClose },
	{ 0, NULL }
};

//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_DIALOGBOX,
	1,
	0, 0, 600, 480,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
	"image Test ....",
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
	ncsInitialize();

	mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect(
			&mymain_templ, HWND_DESKTOP);

	_c(mydlg)->doModal(mydlg, TRUE);

	ncsUninitialize();

	return 0;
}

#else //_MGNCSCTRL_DIALOGBOX

int main (void)
{
	printf("\n==========================================================\n");
	printf("======== You haven't enable the dialogbox contorl =====\n");
	printf("==========================================================\n");
	printf("============== ./configure --enable-dialogbox ==========\n");
	printf("==========================================================\n\n");
	return 0;
}
#endif	//_MGNCSCTRL_DIALOGBOX
