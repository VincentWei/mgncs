/**************************************************************
*  This file is generated automatically, don't modify
*  Feynman software Technology Co.,Ltd
*
**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>
#include <mgutils/mgutils.h>

#include "resource.h"
#include "ncs-windows.h"

#if defined _MGNCSCTRL_LISTVIEW && defined _MGNCSCTRL_CONTAINER && defined _MGNCSCTRL_COMBOBOX

//$func #3614941184 NCSN_WIDGET_CLICKED_3155369984_3614941184
static BOOL opends_on_ok_clicked (mMainWnd *self, mButton* sender, int id, DWORD param)
{

	//TODO:
	//get type of
	int type;
	char szSource[256];
	mDataSource *ds = NULL;

	mCombobox * comb = (mCombobox*)_c(self)->getChild(self, ID_DSTYPE);
	mSlEdit  * edit = (mSlEdit*)_c(self)->getChild(self, ID_DSFILE);

	type = _c(comb)->getProperty(comb, NCSP_COMB_SELECT) + 1;
	GetWindowText(edit->hwnd, szSource, sizeof(szSource));

	if(type == DSTYPE_TEXT){
		ds = ncsCreateTextDataSource(szSource, 0, NCS_DS_SELECT_READ|NCS_DS_SELECT_WRITE);
	}
	else if(type == DSTYPE_ETC){
		ds = ncsCreateEtcDataSource(szSource, NCS_DS_SELECT_READ|NCS_DS_SELECT_WRITE);
	}
	else if (type == DSTYPE_SQLITE){
#ifdef _MGNCSDB_SQLITE
		ds = ncsCreateSQLiteDataSource(szSource, 0);
#endif
	}
	else if (type == DSTYPE_XML){
#ifdef _MGNCSDB_XML
		ds = ncsCreateXMLDataSource(szSource, 0);
#endif
	}
	if(ds)
	{
		if(gDs)
			_c(gDs)->release(gDs);
		gDs = ds;
	}

	_c(self)->endDialog(self, 1);

	return TRUE; /* allow the event to go next */
}

//$func #3614941184 NCSN_WIDGET_CLICKED_1072560128_3614941184
static BOOL opends_on_cancel_clicked (mMainWnd *self, mButton* sender, int id, DWORD param)
{

	//TODO:
	_c(self)->endDialog(self, 0);
	return TRUE; /* allow the event to go next */
}

//$func #3614941184 NCSN_WIDGET_CLICKED_3644037120_1752162304
static BOOL dsfile_on_openfile_clicked (mSlEdit *self, mButton* sender, int id, DWORD param)
{

	//TODO:
	FILEDLGDATA fdd;
	memset(&fdd, 0, sizeof(fdd));

	strcpy(fdd.filter, "Text File(*.txt)|Etc File(*.ini)|DataBase File(*.db)|XML File(*.xml)");

	if (getcwd(fdd.filepath, sizeof(fdd.filepath))
            && FileOpenSaveDialog(NULL, self->hwnd, NULL, &fdd))
	{
		SetWindowText(self->hwnd, fdd.filefullname);
	}

	return TRUE; /* allow the event to go next */
}

//$connect #3614941184 -- Need by merge, don't modify
static NCS_EVENT_CONNECT_INFO Opends_connects [] = {
	{IDOK, ID_OPENDS, NCSN_WIDGET_CLICKED, (NCS_CB_ONOBJEVENT)opends_on_ok_clicked},
	{ID_OPENFILE, ID_DSFILE, NCSN_WIDGET_CLICKED, (NCS_CB_ONOBJEVENT)dsfile_on_openfile_clicked},
	{IDCANCEL, ID_OPENDS, NCSN_WIDGET_CLICKED, (NCS_CB_ONOBJEVENT)opends_on_cancel_clicked},
//$user -- TODO add your handlers hear
	{-1, -1, 0, NULL}
};

//$func @3614941184 onCreate -- Need by merge, don't modify
static BOOL Opends_onCreate (mWidget* self, DWORD dwAddData)
{

	//TODO:
	mCombobox * comb = (mCombobox *)_c(self)->getChild(self, ID_DSTYPE);
	if(comb)
	{
		mRecordSet* rs = _c(g_pStaticDS)->selectRecordSet(g_pStaticDS, "/open-ds/dstype", NCS_DS_SELECT_READ);
		_c(comb)->setSpecificData(comb, NCSSPEC_OBJ_CONTENT, (DWORD)rs, ncsRefObjFreeSpecificData);
		_c(rs)->release(rs);
	}
	return _c(self)->onCreate(self, dwAddData);
}

//$handle @3614941184 -- Need by merge, don't modify
static NCS_EVENT_HANDLER Opends_handlers [] = {
	{MSG_CREATE, Opends_onCreate},
//$user -- TODO add your handlers hear
	{-1,NULL}
};

//$mainhandle -- Need by merge, don't modify
NCS_EVENT_HANDLER_INFO mainwnd_Opends_handlers [] = {
	{ID_OPENDS, Opends_handlers},
//$user --TODO: Add your handlers here
	{-1, NULL}
};

//$mainwnd_entry -- don't modify
NCS_WND_EXPORT mMainWnd* ntCreateOpendsEx(HPACKAGE package, HWND hParent, HICON h_icon, HMENU h_menu, DWORD user_data)
{
	// please don't change me
	return ncsCreateMainWindowIndirectFromID(package,
		ID_OPENDS, /* window id */
		hParent,
		h_icon,
		h_menu,
		mainwnd_Opends_handlers,
		Opends_connects,
		user_data);
}

#endif //_MGNCSCTRL_COMBOBOX
