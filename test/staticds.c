
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "../include/mgncs.h"

#define IDC_LISTVIEW 100

static const NCS_LISTV_CLMRD _header[] = {
	{"Name", "", 100, NCSF_LSTCLM_LEFTALIGN},
	{"Sex", "", 80, NCSF_LSTCLM_LEFTALIGN},
	{"Age", "", 80, NCSF_LSTCLM_LEFTALIGN}
};

static const char* _content[][3] = {
	{"Jime", "Male","15"},
	{"Lily", "Female","12"},
	{"Tom", "Male","11"}
};

#if 0
static int getline(char* buff){
	int i;
	for(i=0; (buff[i]=getchar())!='\n'; i++);
	buff[i] = 0;
	return i;
}
#endif

static const char author[] = "dongjunjie";
static const char version[] = "1.0";
static const char date[] = "2009/08/10";

#if 0
static void show_recordset(mRecordSet *rs)
{
	if(rs == NULL)
		return ;

	int field_count = _c(rs)->getFieldCount(rs);

	printf("--------------------------------------------------------------------------------------\n");
	while(!_c(rs)->isEnd(rs))
	{
		int i;
		for(i=1; i<=field_count ; i++)
		{
			DWORD dw = _c(rs)->getField(rs,i);
			switch(_c(rs)->getFieldType(rs,i))
			{
			case NCS_BT_INT:
				printf("%ld\t", dw);
				break;
			case NCS_BT_STR:
				printf("%s\t", dw);
				break;
			default:
				printf("%x\t",dw);
			}
		}
		printf("\n");
		_c(rs)->seekCursor(rs, NCS_RS_CURSOR_CUR, 1);
	}
}
#endif

static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
	//TODO : initialize
	mListView *lv = (mListView *)_c(self)->getChild(self, IDC_LISTVIEW);

	if(lv)
	{
		mRecordSet *rs;
		rs = _c(g_pStaticDS)->selectRecordSet(g_pStaticDS, "/listview/header", NCS_DS_SELECT_READ);
		_c(lv)->setSpecificData(lv, NCSSPEC_LISTV_HDR, (DWORD)rs, NULL);
		rs = _c(g_pStaticDS)->selectRecordSet(g_pStaticDS, "/listview/content", NCS_DS_SELECT_READ);
		_c(lv)->setSpecificData(lv, NCSSPEC_OBJ_CONTENT, (DWORD)rs, NULL);
	}
	
	return TRUE;
}

static void mymain_onClose(mWidget* self, int message)
{
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(0);
}



static NCS_EVENT_HANDLER mymain_handlers[] = {
	{MSG_CREATE, mymain_onCreate},
	{MSG_CLOSE, mymain_onClose},
	{0, NULL}
};

static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_LISTVIEW, 
		IDC_LISTVIEW,
        10, 10, 320, 220,
        WS_BORDER | WS_VISIBLE | NCSS_LISTV_SORT 
            | NCSS_LISTV_LOOP,
		WS_EX_NONE,
		"",
		NULL, //props,
		NULL,
		NULL, //handlers,
		NULL, //controls
		0,
		0 //add data
	}
};



//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_MAINWND, 
	1,
	0, 0, 800, 600,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
    "Data Source ....",
	NULL,
	NULL,
	mymain_handlers,
	_ctrl_templ,
	sizeof(_ctrl_templ)/sizeof(NCS_WND_TEMPLATE),
	0,
	0, 0,
};



int MiniGUIMain(int argc,const  char* argv[])
{
	ncsInitialize();
	
	ncsRegisterStaticData("/listview/header", (void *)_header, 3,  sizeof(NCS_LISTV_CLMRD)/sizeof(DWORD), sizeof(DWORD),gListVColumnRecordTypes);
	ncsRegisterStaticData("/listview/content", (void *)_content, 3,  3, sizeof(char*),NULL);
	
	ncsRegisterStaticValue("/version/author", (DWORD)author, NCS_BT_STR);
	ncsRegisterStaticValue("/version/ver-info", (DWORD)version, NCS_BT_STR);
	ncsRegisterStaticValue("/version/date", (DWORD)date, NCS_BT_STR);

	mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect 
                                (&mymain_templ, HWND_DESKTOP);

	_c(mydlg)->doModal(mydlg, TRUE);

	MainWindowThreadCleanup(mydlg->hwnd);

	return 0;
}


