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

#include "../include/mgncs.h"

static BOOL mymain_onCreate(mWidget* self, DWORD add_data)
{
	//TODO : initialize
	return TRUE;
}

static void mymain_onClose(mWidget* self, int message)
{
    _c((mDialogBox*)self)->endDialog((mDialogBox*)self, IDCANCEL);
}

#define IDL_DIR    100
#define IDL_FILE   110
#define IDC_PATH   120

static void fill_boxes (mDialogBox *dialog, const char* path)
{
    mListBox        *listDir, *listFile;
    NCS_LSTBOX_ITEMINFO   lbii;

    listFile = (mListBox *)ncsGetChildObj(dialog->hwnd, IDL_FILE);

#ifdef _NOUNIX_ 
    _c(listFile)->addString (listFile, "file.1", 0);
    _c(listFile)->addString (listFile, "file.2", 0);
    _c(listFile)->addString (listFile, "file.3", 0);
#else
    struct dirent* dir_ent;
    DIR*   dir;
    struct stat ftype;
    char   fullpath [PATH_MAX + 1];

    listDir = (mListBox *)ncsGetChildObj(dialog->hwnd, IDL_DIR);

    _c(listFile)->resetContent (listFile);
    _c(listDir)->resetContent (listDir);

    SetWindowText (GetDlgItem (dialog->hwnd, IDC_PATH), path);
    lbii.flag = 0;
    lbii.image = 0;
    
    if ((dir = opendir (path)) == NULL)
         return;

    while ( (dir_ent = readdir ( dir )) != NULL ) {

        /* Assemble full path name. */
        strncpy (fullpath, path, PATH_MAX);
        strcat (fullpath, "/");
        strcat (fullpath, dir_ent->d_name);
        
        if (stat (fullpath, &ftype) < 0 ) {
           continue;
        }

        if (S_ISDIR (ftype.st_mode)) {
            _c(listDir)->addString(listDir, dir_ent->d_name, 0);
        }
        else if (S_ISREG (ftype.st_mode)) {
            _c(listFile)->addString(listFile, dir_ent->d_name, 0);
        }
    }
    _c(listFile)->setCurSel(listFile, 10);

    closedir (dir);
#endif
}

static NCS_RDR_INFO lb_rdr_info[] =
{
    {"flat", "flat", NULL},
    //{"skin", "skin", NULL},
};

static NCS_PROP_ENTRY static_props [] = {
	{NCSP_STATIC_ALIGN, NCS_ALIGN_LEFT},
	{0, 0}
};
//Controls
static NCS_WND_TEMPLATE _ctrl_templ[] = {
	{
		NCSCTRL_STATIC, 
		IDC_STATIC,
		10, 10, 130, 15,
		WS_VISIBLE,
		WS_EX_NONE,
		"directories",
		static_props, 
		lb_rdr_info,
		NULL,
		NULL,
		0,
		0
	},
	{
		NCSCTRL_LISTBOX, 
		IDL_DIR,
		10, 30, 130, 100,
		WS_BORDER | WS_VISIBLE | NCSS_NOTIFY, //sort
		WS_EX_NONE,
		"",
		NULL, 
		lb_rdr_info, 
		NULL,
		NULL, 
		0,
		0 
	},
	{
		NCSCTRL_STATIC, 
		IDC_STATIC,
		150, 10, 130, 15,
		WS_VISIBLE,
		WS_EX_NONE,
		"files",
		static_props, 
		lb_rdr_info, 
		NULL,
		NULL,
		0,
		0 
	},
	{
		NCSCTRL_LISTBOX, 
		IDL_FILE,
		150, 30, 130, 100,
		WS_BORDER | WS_VISIBLE | NCSS_LSTBOX_AUTOCHECKBOX | NCSS_LSTBOX_SORT, //sort
		WS_EX_NONE,
		"",
		NULL,
		lb_rdr_info, 
		NULL,
		NULL,
		0,
		0 
	},
	{
		CTRL_STATIC, 
		IDC_PATH,
		10, 150, 290, 15,
		SS_SIMPLE | WS_VISIBLE,
		WS_EX_NONE,
		"path",
		NULL,
		NULL,
		NULL,
		NULL,
		0,
		0
	},
	{
		CTRL_BUTTON, 
		IDOK,
        10, 170, 130, 25,
        WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,
		WS_EX_NONE,
		"delete",
		NULL, 
		NULL, 
		NULL,
		NULL,
		0,
		0 
	},
	{
		CTRL_BUTTON, 
		IDCANCEL,
        150, 170, 130, 25,
        WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
		WS_EX_NONE,
		"cancel",
		NULL, 
		NULL, 
		NULL,
		NULL,
		0,
		0
	},
};


static NCS_EVENT_HANDLER mymain_handlers[] = {
	{MSG_CREATE, mymain_onCreate },
	{MSG_CLOSE, mymain_onClose },
	{0, NULL }
};

//define the main window template
static NCS_MNWND_TEMPLATE mymain_templ = {
	NCSCTRL_DIALOGBOX, 
	7,
	100, 100, 304, 225,
	WS_CAPTION | WS_BORDER | WS_VISIBLE,
	WS_EX_NONE,
    "deleting_the_files",
	NULL,
	NULL,
	mymain_handlers,
	_ctrl_templ,
	sizeof(_ctrl_templ)/sizeof(NCS_WND_TEMPLATE),
	0,
	0, 0,
};

int MiniGUIMain(int argc, const char* argv[])
{
    char cwd [MAX_PATH + 1];

	if(argc > 1)
	{
		lb_rdr_info[0].glb_rdr = argv[1];
		lb_rdr_info[0].ctl_rdr = argv[1];
	}


	ncsInitialize();
	mDialogBox* mydlg = (mDialogBox *)ncsCreateMainWindowIndirect 
                                (&mymain_templ, HWND_DESKTOP);

    fill_boxes (mydlg, getcwd (cwd, MAX_PATH));
	_c(mydlg)->doModal(mydlg, TRUE);


	MainWindowThreadCleanup(mydlg->hwnd);
	return 0;
}
