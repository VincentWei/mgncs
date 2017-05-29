/**************************************************************
*  This file is generated automatically, don't modify
*  Feynman software Technology Co.,Ltd
*
**************************************************************/

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

mDataSource * gDs = NULL;


//$func #3682142208 NCSN_WIDGET_CLICKED_3689816064_3682142208 -- Need by merge, don't modify
static BOOL mainwnd1_on_update_clicked (mMainWnd *self, mButton* sender, int id, DWORD param)
{

	//TODO:

	return TRUE; /* allow the event to go next */
}

//$func #3682142208 NCSN_ITEMV_SELCHANGED_1606344704_2810120192 -- Need by merge, don't modify
static BOOL cn_record_on_lv_record_selchanged (mContainer *self, mListView* sender, int id, DWORD param)
{

	//TODO:
	//get recordset

	return TRUE; /* allow the event to go next */
}

//$func #3682142208 NCSN_WIDGET_CLICKED_3216486400_3682142208 -- Need by merge, don't modify
static BOOL mainwnd1_on_cancel_clicked (mMainWnd *self, mButton* sender, int id, DWORD param)
{

	//TODO:

	return TRUE; /* allow the event to go next */
}

//$func #3682142208 NCSN_WIDGET_CLICKED_3144667136_3682142208 -- Need by merge, don't modify
static BOOL mainwnd1_on_exit_clicked (mMainWnd *self, mButton* sender, int id, DWORD param)
{

	//TODO:
	mRecordSet * rs = (mRecordSet*)GetWindowAdditionalData(self->hwnd);
	_c(rs)->destroy(rs);
	DestroyMainWindow(self->hwnd);
	PostQuitMessage(self->hwnd);
	return TRUE; /* allow the event to go next */
}

//$func #3682142208 NCSN_WIDGET_CLICKED_657229824_3682142208 -- Need by merge, don't modify
static BOOL mainwnd1_on_reload_clicked (mMainWnd *self, mButton* sender, int id, DWORD param)
{

	//TODO:

	return TRUE; /* allow the event to go next */
}

//$func #3682142208 NCSN_WIDGET_CLICKED_3743399936_3682142208
static BOOL mainwnd1_on_opends_clicked (mMainWnd *self, mButton* sender, int id, DWORD param)
{

	//TODO:
	mMainWnd * opends = ntCreateOpends(ghPkg, self->hwnd, 0);
	if(opends){
		_c(opends)->doModal(opends, TRUE);
	}
	return TRUE; /* allow the event to go next */
}

static void remove_container_childrens(mContainer* container)
{
	HWND hPanel = _c(container)->getPanel(container);

	DestroyAllControls(hPanel);

}

static void bind_container_to_record(mContainer* container, mRecordSet *rs)
{
	HWND hPanel = _c(container)->getPanel(container);
	int i;
	int y = 10;
	for(i=0;i<_c(rs)->getFieldCount(rs); i++)
	{
		char szText[100];
		sprintf(szText, "Field %d: ", i+1);
		//create static
		mWidget* w = ncsCreateWindow(NCSCTRL_STATIC,
				szText,
				WS_VISIBLE,
				0,
				0,
				10,  y, 100, 25,
				hPanel,
				NULL, NULL, NULL, 0);

		_c(w)->setProperty(w, NCSP_STATIC_ALIGN, NCS_ALIGN_LEFT);

		//create a sledit
		mSlEdit * edit = (mSlEdit*)ncsCreateWindow(NCSCTRL_SLEDIT,
				"",
				WS_VISIBLE|NCSS_EDIT_LEFT|WS_BORDER,
				0,
				i+1,
				125,y, 250, 25,
				hPanel,
				NULL, NULL, NULL, 0
				);

		//connect event
		ncsConnectBindProps(
				_c(rs)->getFieldBind(rs, i+1,NCS_RSE_CURSOR_UPDATE,NCS_PROP_FLAG_READ|NCS_PROP_FLAG_WRITE),
				NCS_CMPT_PROP(edit, 0, NCSP_WIDGET_TEXT, NCS_BT_STR, NCS_PROP_FLAG_READ|NCS_PROP_FLAG_WRITE),
				NCS_BPT_DBL);

		y += 10;
		y += 25;
	}
	fprintf(stderr, "i is %d\n", i);
	_c(container)->adjustContent(container);

}

//$func #3682142208 NCSN_WIDGET_CLICKED_3139178496_1606344704
static BOOL lv_record_on_select_clicked (mListView *self, mButton* sender, int id, DWORD param)
{

	//TODO:
	char szText[512];
	mMainWnd* seldlg ;

	if(gDs == NULL){
		MessageBox(self->hwnd, "The Data Source have not opened!", "error",0);
		return TRUE;
	}

	seldlg = ntCreateMainselect(ghPkg, self->hwnd, (DWORD)szText);
	if(seldlg){
		if(_c(seldlg)->doModal(seldlg, TRUE))
		{
			mRecordSet *rs = _c(gDs)->selectRecordSet(gDs, szText,NCS_DS_SELECT_READ|NCS_DS_SELECT_WRITE);

			if(rs)
			{
				//delete all
				int count ;
				int field_count;

				count = _c(self)->getColumnCount(self);
				field_count = _c(rs)->getFieldCount(rs);

				_c(self)->freeze(self, TRUE);
				_c(self)->removeAll(self);
				count --;
				while(count>=0){
					_c(self)->delColumn(self, 0);
					count --;
				}
				{
					NCS_LISTV_CLMINFO clminfo;
					char szText[100];
					memset(&clminfo, 0, sizeof(clminfo));
					clminfo.width = 300;
					clminfo.text = szText;
					for(count=0; count < field_count; count++){
						clminfo.index = count;
						sprintf(szText, "Field %d", count+1);
						_c(self)->addColumn(self, &clminfo);
					}
				}
				_c(self)->freeze(self, FALSE);

				//set data
				_c(rs)->addRef(rs);
				_c(self)->setSpecificData(self, NCSSPEC_OBJ_CONTENT, (DWORD)rs, ncsRefObjFreeSpecificData);

				//delete containers
				mMainWnd *mw = (mMainWnd*)ncsGetParentObj(self->hwnd);
				mContainer *cn = (mContainer*)_c(mw)->getChild(mw, ID_CN_RECORD);

				remove_container_childrens(cn);
				bind_container_to_record(cn, rs);
				HWND h = GetParent(self->hwnd);
				if (h)
					SetWindowAdditionalData(h, (DWORD)rs);
			}
			//create container info
		}
	}
	return TRUE; /* allow the event to go next */
}

//$func #3682142208 NCSN_WIDGET_CLICKED_3216486400_2810120192
static BOOL cn_record_on_cancel_clicked (mContainer *self, mButton* sender, int id, DWORD param)
{

	//TODO:
	int i=1;
	HWND hPanel = _c(self)->getPanel(self);
	HWND hChild;
	while((hChild = GetDlgItem(hPanel, i)))
	{
		ncsAutoReflectObjectBindProps((mObject*)ncsObjFromHandle(hChild));
		i ++;
	}
	return TRUE; /* allow the event to go next */
}

//$func #3682142208 NCSN_WIDGET_CLICKED_3689816064_2810120192
static BOOL cn_record_on_update_clicked (mContainer *self, mButton* sender, int id, DWORD param)
{

	//TODO:

	int i=1;
	HWND hPanel = _c(self)->getPanel(self);
	HWND hChild;
	while((hChild = GetDlgItem(hPanel, i)))
	{
		ncsRaiseComponentBindProps((mComponent*)ncsObjFromHandle(hChild), 0);
		i ++;
	}
	//update current listview
	if(i > 1)
	{
		mMainWnd * mw = (mMainWnd*)ncsGetParentObj(self->hwnd);
		mListView * lv = (mListView *)_c(mw)->getChild(mw, ID_LV_RECORD);
		//refresh current
		mRecordSet* rs = (mRecordSet*)_c(lv)->getSpecificData(lv, NCSSPEC_OBJ_CONTENT, NULL);
		if(rs && !_c(rs)->isEnd(rs))
		{
			int field_count = _c(rs)->getFieldCount(rs);
			_BEGIN_AUTO_STR
			_c(lv)->freeze(lv, TRUE);
			for(i=1; i<= field_count; i++)
			{
				_c(lv)->setItemText(lv, _c(lv)->getCurSel(lv), i-1,_AUTO_STR(_c(rs)->getField(rs, i), _c(rs)->getFieldType(rs, i)) );

				fprintf(stderr, "test %d\n", i);
			}
			_END_AUTO_STR
		}
		_c(rs)->update(rs);
		_c(lv)->freeze(lv, FALSE);
	}

	return TRUE; /* allow the event to go next */
}
#if 0
//$func #3682142208 NCSN_WIDGET_CLICKED_4252262400_1606344704
static BOOL lv_record_on_append_clicked (mListView *self, mButton* sender, int id, DWORD param)
{

	//TODO:

	return TRUE; /* allow the event to go next */
}
#endif
//$func #3682142208 NCSN_WIDGET_CLICKED_2760863744_1606344704
static BOOL lv_record_on_delrd_clicked (mListView *self, mButton* sender, int id, DWORD param)
{

	//TODO:
	mRecordSet *rs = (mRecordSet*)_c(self)->getSpecificData(self, NCSSPEC_OBJ_CONTENT, NULL);
	if(rs)
	{
		if(!_c(rs)->deleteRecord(rs))
		{
			MessageBox(self->hwnd, "Cannot Delete Record!", "Error", MB_OK);
			return TRUE;
		}
		//update the listview
		_c(self)->removeItem(self,
				_c(self)->getItem(self, _c(self)->getCurSel(self)));

		NCS_LISTV_FINDINFO lvfind;
		memset(&lvfind, 0, sizeof(lvfind));
		lvfind.data.addData = _c(rs)->getCursor(rs);
		lvfind.type = NCSID_LISTV_FT_ADDDATA;
		HITEM hItem = _c(self)->findItem(self, &lvfind);
		_c(self)->setCurSel(self, _c(self)->indexOf(self, hItem));

	}
	return TRUE; /* allow the event to go next */
}

//$func #3682142208 NCSN_WIDGET_CLICKED_4290740224_1606344704
static BOOL lv_record_on_appendrd_clicked (mListView *self, mButton* sender, int id, DWORD param)
{

	//TODO:
	mRecordSet *rs = (mRecordSet*)_c(self)->getSpecificData(self, NCSSPEC_OBJ_CONTENT, NULL);
	if(rs)
	{
		_c(rs)->newRecord(rs, NCS_RS_INSERT_APPEND);
		_c(rs)->addRef(rs);
		_c(self)->setSpecificData(self, NCSSPEC_OBJ_CONTENT, (DWORD)rs, NULL);
		_c(rs)->seekCursor(rs, NCS_RS_CURSOR_END,0);
		_c(self)->setCurSel(self, _c(self)->getItemCount(self)-1);
	}
	return TRUE; /* allow the event to go next */
}

//$connect #3682142208 -- Need by merge, don't modify
static NCS_EVENT_CONNECT_INFO Mainwnd1_connects [] = {
	{ID_CANCEL, ID_MAINWND1, NCSN_WIDGET_CLICKED, (NCS_CB_ONOBJEVENT)mainwnd1_on_cancel_clicked},
	{ID_LV_RECORD, ID_CN_RECORD, NCSN_ITEMV_SELCHANGED, (NCS_CB_ONOBJEVENT)cn_record_on_lv_record_selchanged},
	{ID_UPDATE, ID_CN_RECORD, NCSN_WIDGET_CLICKED, (NCS_CB_ONOBJEVENT)cn_record_on_update_clicked},
	{ID_RELOAD, ID_MAINWND1, NCSN_WIDGET_CLICKED, (NCS_CB_ONOBJEVENT)mainwnd1_on_reload_clicked},
	{ID_UPDATE, ID_MAINWND1, NCSN_WIDGET_CLICKED, (NCS_CB_ONOBJEVENT)mainwnd1_on_update_clicked},
	{IDB_OPENDS, ID_MAINWND1, NCSN_WIDGET_CLICKED, (NCS_CB_ONOBJEVENT)mainwnd1_on_opends_clicked},
	{ID_SELECT, ID_LV_RECORD, NCSN_WIDGET_CLICKED, (NCS_CB_ONOBJEVENT)lv_record_on_select_clicked},
	{IDB_DELRD, ID_LV_RECORD, NCSN_WIDGET_CLICKED, (NCS_CB_ONOBJEVENT)lv_record_on_delrd_clicked},
	{IDB_APPENDRD, ID_LV_RECORD, NCSN_WIDGET_CLICKED, (NCS_CB_ONOBJEVENT)lv_record_on_appendrd_clicked},
	{ID_EXIT, ID_MAINWND1, NCSN_WIDGET_CLICKED, (NCS_CB_ONOBJEVENT)mainwnd1_on_exit_clicked},
	{ID_CANCEL, ID_CN_RECORD, NCSN_WIDGET_CLICKED, (NCS_CB_ONOBJEVENT)cn_record_on_cancel_clicked},
//$user -- TODO add your handlers hear
	{-1, -1, 0, NULL}
};

//$func @1606344704 onSelChanged -- Need by merge, don't modify
static void LvRecord_onSelChanged (mItemView* self, int id, int nc, DWORD add_data)
{

	//TODO:
	mRecordSet *rs = (mRecordSet*)_c(self)->getSpecificData(self, NCSSPEC_OBJ_CONTENT,NULL);
	if(rs)
	{
		int cursor = _c(self)->getAddData(self, _c(self)->getItem(self,_c(self)->getCurSel(self)));
		_c(rs)->setCursor(rs, cursor);
	}
}

//$handle @1606344704 -- Need by merge, don't modify
static NCS_EVENT_HANDLER LvRecord_handlers [] = {
	{NCS_NOTIFY_CODE(NCSN_ITEMV_SELCHANGED), LvRecord_onSelChanged},
//$user -- TODO add your handlers hear
	{-1,NULL}
};

//$func @2810120192 onCreate -- Need by merge, don't modify
static BOOL CnRecord_onCreate (mWidget* self, DWORD dwAddData)
{

	//TODO:
	mContainer * cn = (mContainer*)self;
	cn->leftMargin = 5;
	cn->topMargin  = 5;
	cn->rightMargin = 5;
	cn->bottomMargin = 5;
	return _c(self)->onCreate(self, dwAddData);
}

//$handle @2810120192 -- Need by merge, don't modify
static NCS_EVENT_HANDLER CnRecord_handlers [] = {
	{MSG_CREATE, CnRecord_onCreate},
//$user -- TODO add your handlers hear
	{-1,NULL}
};

//$mainhandle -- Need by merge, don't modify
NCS_EVENT_HANDLER_INFO mainwnd_Mainwnd1_handlers [] = {
	{ID_LV_RECORD, LvRecord_handlers},
	{ID_CN_RECORD, CnRecord_handlers},
//$user --TODO: Add your handlers here
	{-1, NULL}
};

//$mainwnd_entry -- don't modify
NCS_WND_EXPORT mMainWnd* ntCreateMainwnd1Ex(HPACKAGE package, HWND hParent, HICON h_icon, HMENU h_menu, DWORD user_data)
{
	// please don't change me
	return ncsCreateMainWindowIndirectFromID(package,
		ID_MAINWND1, /* window id */
		hParent,
		h_icon,
		h_menu,
		mainwnd_Mainwnd1_handlers,
		Mainwnd1_connects,
		user_data);
}

