
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>

#ifdef _MGNCSCTRL_DIALOGBOX

#define IDC_SETTEXT		 100
#define IDC_INSERTTEXT	  110
#define IDC_MLEDIT		  120
#define IDC_FILETEXT		121
#define IDC_LIMITLEN		122

#define IDC_BTN_SETTEXT	 130
#define IDC_BTN_GETTEXT	 140
#define IDC_BTN_READONLY	141
#define IDC_BTN_INSERTTEXT  150
#define IDC_BTN_LOADFILE	151
#define IDC_BTN_SETLIMITLEN 152
#define IDC_BTN_MOVEWIN	 153


#define IDC_PARA_GROUPBOX   199
#define IDC_LABEL_PARAINFO  200
#define IDC_LABEL_PARATXT   201
#define IDC_EDIT_PARAIDX	202
#define IDC_EDIT_PARASTART  203
#define IDC_EDIT_PARACPLEN  204
#define IDC_PARANUM		 205
#define IDC_PARALEN		 206
#define IDC_PARATXT		 207
#define IDC_BTN_GETPARANUM  208
#define IDC_BTN_GETPARALEN  209
#define IDC_BTN_GETPARATXT  210
#define IDC_BTN_SETSEL 211
#define	IDC_EDIT_SETSEL 212

#ifndef TME_GETBKGNDINVALID
#define TME_GETBKGNDINVALID 0xF402
#endif

//static WNDPROC _old_text_proc = NULL;
static PBITMAP pbk_img = NULL;

#define IDC_CARET_GROUPBOX  220
#define IDC_CARET_PARAPOS   221
#define IDC_CARET_CHARPOS   222
#define IDC_LINENUM		 223
#define IDC_BTN_GETLINENUM  224
#define IDC_BTN_SETCARETPOS 225
#define IDC_LABEL_PARAPOS   226
#define IDC_LABEL_CHARPOS   227

#define IDC_OTHER_GROUPBOX  230

#ifdef _MGNCSCTRL_TEXTEDITOR

static int _load_file(HWND hDlg, const char* str_file);

#define DEF_BTN_HANDLER(name) \
	static void name##_onPushed(mWidget* self, int id, int nc, DWORD add_data); \
	static NCS_EVENT_HANDLER name##_handler [] = { \
		NCS_MAP_NOTIFY(NCSN_BUTTON_PUSHED, name##_onPushed), \
		{0, NULL} \
	}; \
	static void name##_onPushed(mWidget* self, int id, int nc, DWORD add_data)


DEF_BTN_HANDLER(movewin)
{
	srand(time(NULL));
	MoveWindow(GetDlgItem(GetParent(self->hwnd), IDC_MLEDIT), 5, 10,
			random()%150 + 50, random()%200 + 100, TRUE);
	SetFocus(GetDlgItem(GetParent(self->hwnd), IDC_MLEDIT));
}

DEF_BTN_HANDLER(setcaretpos)
{
	HWND hDlg = GetParent(self->hwnd);
	mTextEditor* te;
	te = (mTextEditor*)ncsObjFromHandle(GetDlgItem(hDlg, IDC_MLEDIT));
	BOOL bSel = id == IDC_BTN_SETSEL;
	char *linePos = GetDlgItemText2(hDlg, IDC_CARET_PARAPOS, NULL);
	char *charPos = GetDlgItemText2(hDlg, IDC_CARET_CHARPOS, NULL);


	if (linePos && charPos) {
		if ( _c(te)->setCaretPosByPara(te, atoi(linePos), atoi(charPos), bSel)== -1)
			fprintf (stderr,
				"Error => Set caret position(line:%d, char:%d) failure.\n",
				atoi(linePos), atoi(charPos));
		else
			SetFocus(te->hwnd);
	}
	if (linePos)
		free(linePos);
	if (charPos)
		free(charPos);
}

DEF_BTN_HANDLER(getlinenum)
{
	HWND hDlg = GetParent(self->hwnd);
	mTextEditor* te = (mTextEditor*)ncsObjFromHandle(GetDlgItem(hDlg, IDC_MLEDIT));

	int linenum = _c(te)->getLineCount(te);
	char text[16];
	sprintf(text,"%i", linenum);
	SetDlgItemText(hDlg, IDC_LABEL_PARATXT, text);
}

static int readOnly = 0;

DEF_BTN_HANDLER(readonly)
{
	HWND hDlg = GetParent(self->hwnd);
	mTextEditor* te = (mTextEditor*)ncsObjFromHandle(GetDlgItem(hDlg, IDC_MLEDIT));

	_c(te)->setProperty(te, NCSP_TE_READONLY, ++readOnly	% 2);
}

DEF_BTN_HANDLER(insert)
{
	HWND hDlg = GetParent(self->hwnd);
	mTextEditor* te = (mTextEditor*)ncsObjFromHandle(GetDlgItem(hDlg, IDC_MLEDIT));

	int len;
	char* text = GetDlgItemText2(hDlg, IDC_INSERTTEXT, &len);
	if (len > 0) {
		_c(te)->insertText(te, text, len);
		free(text);
	}
}

DEF_BTN_HANDLER(settext)
{
	HWND hDlg = GetParent(self->hwnd);
	mTextEditor* te = (mTextEditor*)ncsObjFromHandle(GetDlgItem(hDlg, IDC_MLEDIT));

	int len;
	char *text = GetDlgItemText2(hDlg, IDC_SETTEXT, &len);
	_c(te)->setText(te, text);
	if (text) {
		free(text);
	}
}

DEF_BTN_HANDLER(gettext)
{
	HWND hDlg = GetParent(self->hwnd);
	//mTextEditor* te = (mTextEditor*)ncsObjFromHandle(GetDlgItem(hDlg, IDC_MLEDIT));
	int len;
	char *text = GetDlgItemText2(hDlg, IDC_MLEDIT, &len);
	if (len > 0) {
		fprintf(stderr, "[%s]\n", text);
		free(text);
	}
}

DEF_BTN_HANDLER(loadfile)
{
	HWND hDlg = GetParent(self->hwnd);
	int  len;
	char * file = GetDlgItemText2(hDlg, IDC_FILETEXT, &len);
	_load_file(hDlg, file);
	if (file)
		free(file);
}

DEF_BTN_HANDLER(setlimit)
{
	HWND hDlg = GetParent(self->hwnd);
	//mTextEditor* te = (mTextEditor*)ncsObjFromHandle(GetDlgItem(hDlg, IDC_MLEDIT));

	int len;
	char* text = GetDlgItemText2(hDlg, IDC_LIMITLEN, &len);
	if (len > 0) {
		_c(self)->setProperty(self, NCSP_TE_TEXTLIMIT, atoi(text));
		free(text);
	}

}

DEF_BTN_HANDLER(getparanum)
{
	HWND hDlg = GetParent(self->hwnd);
	mTextEditor* te = (mTextEditor*)ncsObjFromHandle(GetDlgItem(hDlg, IDC_MLEDIT));

	int num = _c(te)->getParaNum(te);
	char text[16];
	sprintf(text, "%i", num);
	SetDlgItemText(hDlg, IDC_LABEL_PARATXT, text);

}

DEF_BTN_HANDLER(getparalen)
{
	HWND hDlg = GetParent(self->hwnd);
	mTextEditor* te = (mTextEditor*)ncsObjFromHandle(GetDlgItem(hDlg, IDC_MLEDIT));

	int len;
	char* index = GetDlgItemText2(hDlg, IDC_EDIT_PARAIDX, &len);
	if (len) {
		char text[16];
		len = _c(te)->getParaLength(te, atoi(index), 0);
		sprintf(text, "%i", len);
		SetDlgItemText(hDlg, IDC_LABEL_PARATXT, text);
		free(index);
	}

}

DEF_BTN_HANDLER(getparatext)
{
	HWND hDlg = GetParent(self->hwnd);
	mTextEditor* te = (mTextEditor*)ncsObjFromHandle(GetDlgItem(hDlg, IDC_MLEDIT));
	int copy_len = (int)GetDlgItemInt(hDlg, IDC_EDIT_PARACPLEN, NULL, TRUE);

	char buff[256];

	if (_c(te)->getParaText(te, (int)GetDlgItemInt(hDlg, IDC_EDIT_PARAIDX, NULL, TRUE),
				(int)GetDlgItemInt(hDlg, IDC_EDIT_PARASTART, NULL, TRUE),
				copy_len < sizeof(buff) ? copy_len : sizeof(buff)-1, buff) > 0) {
		SetDlgItemText(hDlg, IDC_LABEL_PARATXT, buff);
	}


}


static NCS_WND_TEMPLATE TestCtrlData [] = {
	{
		NCSCTRL_TEXTEDITOR,
		IDC_MLEDIT,
		5, 10, 200, 300,
		WS_VISIBLE | WS_BORDER | WS_VSCROLL | /*NCSS_TE_AUTOWRAP*/ NCSS_TE_BASELINE,
		0,
		NULL,
		NULL,//properties
		NULL,//rdr_info,
		NULL,//handlers,
		NULL,//controls
		0
	},
	{
		NCSCTRL_SLEDIT,
		IDC_SETTEXT,
		340, 10, 250, 30,
		WS_VISIBLE | WS_BORDER,
		0,
		"Input text to \nmledit.\ntest ok?",
	},
	{
		NCSCTRL_SLEDIT,
		IDC_INSERTTEXT,
		340, 50, 250, 30,
		WS_VISIBLE | WS_BORDER,
		0,
		"Insert text.\nfor test \ncarriage return.",
		0
	},
	{
		NCSCTRL_SLEDIT,
		IDC_FILETEXT,
		340, 130, 250, 30,
		WS_VISIBLE | WS_BORDER,
		0,
		"textedit.c",
		0
	},
	{
		NCSCTRL_GROUPBOX,
		IDC_PARA_GROUPBOX,
		210, 170, 380, 170,
		WS_VISIBLE,
		0,
		"ParagraphOPS",
		0
	},
	{
		NCSCTRL_STATIC,
		IDC_LABEL_PARAINFO,
		220, 200, 350, 30,
		WS_VISIBLE,
		0,
		"para info (index, start, copy_len < 256):",
		0
	},
	{
		NCSCTRL_SLEDIT,
		IDC_EDIT_PARAIDX,
		260, 220, 50, 30,
		WS_VISIBLE | WS_BORDER,
		0,
		"0",
		0
	},
	{
		NCSCTRL_SLEDIT,
		IDC_EDIT_PARASTART,
		340, 220, 50, 30,
		WS_VISIBLE | WS_BORDER,
		0,
		"0",
		0
	},
	{
		NCSCTRL_SLEDIT,
		IDC_EDIT_PARACPLEN,
		420, 220, 50, 30,
		WS_VISIBLE | WS_BORDER,
		0,
		"1",
		0
	},
	{
		NCSCTRL_STATIC,
		IDC_LABEL_PARATXT,
		220, 300, 350, 30,
		WS_VISIBLE | WS_BORDER,
		0,
		"",
		0
	},
	{
		NCSCTRL_BUTTON,
		IDC_BTN_SETTEXT,
		220, 10, 100, 30,
		WS_VISIBLE |  WS_TABSTOP,
		0,
		"SetText",
		NULL,//properties
		NULL,//rdr_info,
		settext_handler,//handlers,
		NULL,//controls
		0
	},
	{
		NCSCTRL_BUTTON,
		IDC_BTN_INSERTTEXT,
		220, 50, 100, 30,
		WS_VISIBLE |  WS_TABSTOP,
		0,
		"InsertText",
		NULL,//properties
		NULL,//rdr_info,
		insert_handler,//handlers,
		NULL,//controls
		0
	},
	{
		NCSCTRL_BUTTON,
		IDC_BTN_GETTEXT,
		220, 90, 100, 30,
		WS_VISIBLE |  WS_TABSTOP,
		0,
		"GetText",
		NULL,//properties
		NULL,//rdr_info,
		gettext_handler,//handlers,
		NULL,//controls
		0
	},
	{
		NCSCTRL_BUTTON,
		IDC_BTN_READONLY,
		380, 90, 150, 30,
		WS_VISIBLE |  WS_TABSTOP,
		0,
		"Switch ReadOnly",
		NULL,//properties
		NULL,//rdr_info,
		readonly_handler,//handlers,
		NULL,//controls
		0
	},
	{
		NCSCTRL_BUTTON,
		IDC_BTN_LOADFILE,
		220, 130, 100, 30,
		WS_VISIBLE |  WS_TABSTOP,
		0,
		"LoadFile",
		NULL,//properties
		NULL,//rdr_info,
		loadfile_handler,//handlers,
		NULL,//controls
		0
	},
	{
		NCSCTRL_BUTTON,
		IDC_BTN_GETLINENUM,
		480, 220, 90, 30,
		WS_VISIBLE |  WS_TABSTOP | WS_GROUP,
		0,
		"GetLineNum",
		NULL,//properties
		NULL,//rdr_info,
		getlinenum_handler,//handlers,
		NULL,//controls
		0
	},
	{
		NCSCTRL_BUTTON,
		IDC_BTN_GETPARALEN,
		220, 260, 100, 30,
		WS_VISIBLE |  WS_TABSTOP | WS_GROUP,
		0,
		"GetParaLen",
		NULL,//properties
		NULL,//rdr_info,
		getparalen_handler,//handlers,
		NULL,//controls
		0
	},
	{
		NCSCTRL_BUTTON,
		IDC_BTN_GETPARATXT,
		350, 260, 100, 30,
		WS_VISIBLE |  WS_TABSTOP,
		0,
		"GetParaText",
		NULL,//properties
		NULL,//rdr_info,
		getparatext_handler,//handlers,
		NULL,//controls
		0
	},
	{
		NCSCTRL_BUTTON,
		IDC_BTN_GETPARANUM,
		470, 260, 100, 30,
		WS_VISIBLE |  WS_TABSTOP | WS_GROUP,
		0,
		"GetParaNum",
		NULL,//properties
		NULL,//rdr_info,
		getparanum_handler,//handlers,
		NULL,//controls
		0
	},
	{
		NCSCTRL_GROUPBOX,
		IDC_CARET_GROUPBOX,
		5, 310, 200, 140,
		WS_VISIBLE ,
		0,
		"Others",
		0
	},
	{
		NCSCTRL_SLEDIT,
		IDC_LIMITLEN,
		15, 330, 70, 30,
		WS_VISIBLE | WS_BORDER,
		0,
		"10",
		0
	},
	{
		NCSCTRL_BUTTON,
		IDC_BTN_SETLIMITLEN,
		100, 330, 100, 30,
		WS_VISIBLE |  WS_TABSTOP,
		0,
		"SetLimitLen",
		NULL,//properties
		NULL,//rdr_info,
		setlimit_handler,//handlers,
		NULL,//controls
		0
	},
	{
		NCSCTRL_BUTTON,
		IDC_BTN_MOVEWIN,
		20, 390, 160, 30,
		WS_VISIBLE |  WS_TABSTOP,
		0,
		"Random MoveEditWin",
		NULL,//properties
		NULL,//rdr_info,
		movewin_handler,//handlers,
		NULL,//controls
		0
	},
	//caret ops
	{
		NCSCTRL_GROUPBOX,
		IDC_CARET_GROUPBOX,
		210, 340, 380, 110,
		WS_VISIBLE ,
		0,
		"CaretOPS",
		0
	},
	{
		NCSCTRL_STATIC,
		IDC_LABEL_PARAPOS,
		220, 365, 80, 30,
		WS_VISIBLE,
		0,
		"ParaIdx:",
		0
	},
	{
		NCSCTRL_SLEDIT,
		IDC_CARET_PARAPOS,
		310, 360, 80, 30,
		WS_VISIBLE | WS_BORDER,
		0,
		"0",
		0
	},
	{
		NCSCTRL_STATIC,
		IDC_LABEL_CHARPOS,
		410, 365, 80, 30,
		WS_VISIBLE,
		0,
		"CharPos:",
		0
	},
	{
		NCSCTRL_SLEDIT,
		IDC_CARET_CHARPOS,
		490, 360, 80, 30,
		WS_VISIBLE | WS_BORDER,
		0,
		"0",
		0
	},
	{
		NCSCTRL_BUTTON,
		IDC_BTN_SETCARETPOS,
		250, 410, 100, 30,
		WS_VISIBLE |  WS_TABSTOP | WS_GROUP,
		0,
		"SetCaretPos",
		NULL,//properties
		NULL,//rdr_info,
		setcaretpos_handler,//handlers,
		NULL,//controls
		0
	},
	{
		NCSCTRL_BUTTON,
		IDC_BTN_SETSEL,
		410, 410, 100, 30,
		WS_VISIBLE |  WS_TABSTOP | WS_GROUP,
		0,
		"SetSel",
		NULL,//properties
		NULL,//rdr_info,
		setcaretpos_handler,//handlers,
		NULL,//controls
		0
	},
};

static BOOL testdlg_onCreate(mWidget* self, DWORD add_data)
{
	mTextEditor* te = (mTextEditor*)ncsGetChildObj(self->hwnd, IDC_MLEDIT);
	printf("-- pbk_img=%p\n", pbk_img);
	_c(te)->setProperty(te, NCSP_WIDGET_BKIMAGE, (DWORD)pbk_img);

	return TRUE;
}

static void testdlg_onClose(mMainWnd* self, int message)
{
	_c(self)->endDialog(self, 0);
}


static NCS_EVENT_HANDLER testdlg_handlers[] = {
	{MSG_CREATE, testdlg_onCreate},
	{MSG_CLOSE, testdlg_onClose},
	{0, NULL}
};

static NCS_MNWND_TEMPLATE TestDlgTmpl = {
	NCSCTRL_MAINWND,
	1,
	0, 0, 640, 480,
	WS_BORDER | WS_CAPTION,
	WS_EX_NONE, //WS_EX_AUTOSECONDARYDC,
	"Test textedit control...",
	NULL,
	NULL,
	testdlg_handlers,
	TestCtrlData,
	sizeof(TestCtrlData)/sizeof(NCS_WND_TEMPLATE),
	0,
	0,0
};

static int _load_file(HWND hDlg, const char* str_file)
{
	char szFile[512];
	char* buffer;
	int   len;
	FILE* fp;

	if (str_file == NULL) {
		GetDlgItemText(hDlg, IDC_FILETEXT, szFile, sizeof(szFile));
		str_file = szFile;
	}

	fp = fopen(str_file, "rt");
	if (fp == NULL)
		return 0;

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (len > 0) {
		int readLength = 0;
		buffer = (char*)malloc(len+1);
		readLength = fread(buffer, 1, len, fp);
		buffer[readLength] = 0;
		SetDlgItemText(hDlg, IDC_MLEDIT, buffer);
		free(buffer);
	}

	fclose(fp);
	SetFocus(GetDlgItem(hDlg, IDC_MLEDIT));

	return 0;
}

int MiniGUIMain (int argc, const char* argv[])
{
	fprintf(stderr, "usage %s [bkimage-file]\n", argv[0]);

	if (argc > 1) {
		static BITMAP bmp;
		if (LoadBitmapFromFile(HDC_SCREEN, &bmp, argv[1]) == 0)
			pbk_img = &bmp;
	}

	ncsInitialize();

	mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect(
			&TestDlgTmpl, HWND_DESKTOP);

	_c(mydlg)->doModal(mydlg, TRUE);

	ncsUninitialize();

	return 0;
}

#endif

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

