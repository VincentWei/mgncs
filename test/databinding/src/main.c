///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
/*
 *   This file is part of mGNCS, a component for MiniGUI.
 * 
 *   Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */
/**************************************************************
*  This file is generated automatically, don't modify
**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include <mgncs/mgncs.h>

#include "resource.h"
#include "ncs-windows.h"

#ifdef _MGNCSCTRL_COMBOBOX

#define ID_SENDER     100
#define ID_LISTENER 101

static mDataSource*  ctrl_source=NULL;

static void full_info(mCombobox* sender, mListBox* self, const char* type)
{
	mRecordSet *rs;
	char* ctrl_name;
	char szSelect[256];
	int cur;
	if(!ctrl_source)
		return ;

	cur =(int) _c(sender)->getProperty(sender, NCSP_COMB_SELECT);

	if(cur < 0)
		return ;

	ctrl_name = (char*)_c(sender)->getItem(sender, cur);
	if(ctrl_name== NULL)
		return ;

	sprintf(szSelect, "/%s-%s:{$1,$2}", ctrl_name, type);

	rs = _c(ctrl_source)->selectRecordSet(ctrl_source, szSelect, NCS_DS_SELECT_READ);
	if(!rs){
		_c(self)->resetContent(self);
		return ;
	}

	_c(self)->setSpecificData(self, NCSSPEC_OBJ_CONTENT, (DWORD)rs, ncsRefObjFreeSpecificData);
	_c(self)->setCurSel(self, 0);
}

static mWidget* create_widget(mWidget* parent, int cb_id, int ctrl_id, int x, int y)
{
	mCombobox *cb = (mCombobox*)_c(parent)->getChild(parent, cb_id);
	NCS_WND_TEMPLATE wnd_templ;
	int idx;
	const char* strCtrlClass;
	char szClassName[100];
	char szql[100];

	if(!cb)
		return NULL;

	if(!ctrl_source)
		return NULL;

	idx = _c(cb)->getProperty(cb, NCSP_COMB_SELECT);
	if(idx < 0)
		return NULL;

	strCtrlClass = _c(cb)->getItem(cb, idx);
	if(!strCtrlClass)
		return NULL;

	memset(&wnd_templ, 0, sizeof(wnd_templ));

	sprintf(szClassName, "%s" _MGNCS_CLASS_SUFFIX , strCtrlClass);

	wnd_templ.class_name = szClassName;
	wnd_templ.id  = ctrl_id;
	wnd_templ.x = x;
	wnd_templ.y = y;
	sprintf(szql,"/%s/width",strCtrlClass);
	wnd_templ.w = ncsStrToInt(_c(ctrl_source)->getValue(ctrl_source, szql, NULL));
	sprintf(szql,"/%s/height",strCtrlClass);
	wnd_templ.h = ncsStrToInt(_c(ctrl_source)->getValue(ctrl_source, szql, NULL));
	sprintf(szql,"/%s/style",strCtrlClass);
	wnd_templ.style = WS_VISIBLE | ncsStrToInt(_c(ctrl_source)->getValue(ctrl_source, szql, NULL));
	sprintf(szql,"/%s/exstyle",strCtrlClass);
	wnd_templ.ex_style = ncsStrToInt(_c(ctrl_source)->getValue(ctrl_source, szql, NULL));
	wnd_templ.caption = szClassName;

	if(wnd_templ.w <= 0)
		wnd_templ.w = 100;
	if(wnd_templ.h <= 0)
		wnd_templ.h = 30;

	//create
	return ncsCreateWindowIndirect(&wnd_templ, parent->hwnd);
}

static int get_cur_add_data(mListBox* lb, const char** pstr)
{
	int idx;
	if(lb == NULL)
		return 0;

	idx = _c(lb)->getCurSel(lb);
	if(idx < 0)
		return 0;

	if(pstr)
		*pstr = _c(lb)->getText(lb, _c(lb)->getItem(lb,idx));

	return (int)ncsStrToInt(_c(lb)->getAddData(lb, _c(lb)->getItem(lb, idx)));
}

static int get_cur_prop_info(mListBox* lb, int *ptype, const char** pstr)
{
	char* str = NULL;
	int type = NCS_BT_INT;
	int prop = get_cur_add_data(lb, (const char **)&str);
	if(str == NULL){
		if(ptype)
			*ptype = type;
		return 0;
	}

	if(strncmp(str, "str_",4) == 0)
		type = NCS_BT_STR;

	if(ptype)
		*ptype = type;

	if(pstr){
		*pstr = strchr(str,'_');
		if(*pstr)
			*pstr = (*pstr) + 1;
		else
			*pstr = str;
	}

	return prop;
}

//$func #1853825024 NCSN_WIDGET_CLICKED_4141240320_1853825024 -- Need by merge, don't modify
static BOOL main_on_update_clicked (mMainWnd *self, mButton* sender, int id, DWORD param)
{

	//TODO:
	RECT rt = {10, 10, 300, 200};
	mWidget * source;
	mWidget *listener;
	mWidget * groupbox = (mWidget *)_c(self)->getChild(self, ID_GROUPBOX1);
	if(groupbox)
	{
		GetWindowRect(groupbox->hwnd, &rt);
	}

	//get old sender and listener, and destroy them
	source = (mWidget *)_c(self)->getChild(self, ID_SENDER);
	if(source)
		DestroyWindow(source->hwnd);

	listener = (mWidget *)_c(self)->getChild(self, ID_LISTENER);
	if(listener)
		DestroyWindow(listener->hwnd);

	//create new sender
	source = create_widget((mWidget *)self, ID_CB_SOURCE_CTRLS,ID_SENDER,  rt.left + 10, rt.top + 25);
	//if(source)
	//	_c(source)->setProperty(source, NCSP_DEFAULT_CONTENT,0);

	listener = create_widget((mWidget *)self, ID_CB_LISTNER_CTRLS, ID_LISTENER, (rt.left +rt.right)/2+10, rt.top + 25);
//	if(listener)
//		_c(listener)->setProperty(listener, NCSP_DEFAULT_CONTENT, 0);

	if(source && listener)
	{
		char szInfo[512];
		//connet them
		char * str_sender_event=NULL, *str_listener_event=NULL;
		char *str_sender_prop=NULL, *str_listener_prop = NULL;
		int sender_prop_type, listener_prop_type;
		int sender_event =
				get_cur_add_data((mListBox*)_c(self)->getChild(self, ID_LB_SOURCE_EVENTS), (const char **)&str_sender_event);
		int sender_prop =
				get_cur_prop_info((mListBox*)_c(self)->getChild(self, ID_LB_SOURCE_PROPS), &sender_prop_type,(const char **) &str_sender_prop);
		int listener_event =
				get_cur_add_data((mListBox*)_c(self)->getChild(self, ID_LB_LISTENER_EVENTS), (const char **)&str_listener_event);
		int listener_prop =
				get_cur_prop_info((mListBox*)_c(self)->getChild(self, ID_LB_LISTENER_PROPS), &listener_prop_type, (const char **)&str_listener_prop);

		//connect
		ncsConnectBindProps(
				NCS_CMPT_PROP(source,  sender_event, sender_prop, sender_prop_type, NCS_PROP_FLAG_READ|NCS_PROP_FLAG_WRITE),
				NCS_CMPT_PROP(listener, listener_event, listener_prop, listener_prop_type, NCS_PROP_FLAG_READ|NCS_PROP_FLAG_WRITE),
				NCS_BPT_DBL);


		sprintf(szInfo, "From \"%s\" \'s \"%s\"  To \"%s\" \'s \"%s\" At Event \"%s\" And \"%s\"",
					_c(source)->typeName, str_sender_prop,
					_c(listener)->typeName, str_listener_prop,
					str_sender_event, str_listener_event);
		SetWindowText(GetDlgItem(self->hwnd, ID_CUR_BIND_INFO), szInfo);
	}

	return TRUE; /* allowed the event goto next */
}

//$func #1853825024 NCSN_CMBOX_SELECTOK_4158334976_1815040000
static BOOL lb_source_events_on_cb_source_ctrls_selectok (mListBox *self, mCombobox* sender, int id, DWORD param)
{

	//TODO:
	full_info(sender, self, "events");
	return TRUE; /* allowed the event goto next */
}

//$func #1853825024 NCSN_CMBOX_SELECTOK_4158334976_2050961408
static BOOL lb_source_props_on_cb_source_ctrls_selectok (mListBox *self, mCombobox* sender, int id, DWORD param)
{

	//TODO:
	full_info(sender, self, "props");
	return TRUE; /* allowed the event goto next */
}

//$func #1853825024 NCSN_CMBOX_SELECTOK_4256976896_1304381440
static BOOL lb_listener_events_on_cb_listner_ctrls_selectok (mListBox *self, mCombobox* sender, int id, DWORD param)
{

	//TODO:
	full_info(sender, self, "events");
	return TRUE; /* allowed the event goto next */
}

//$func #1853825024 NCSN_CMBOX_SELECTOK_4256976896_398116864
static BOOL lb_listener_props_on_cb_listner_ctrls_selectok (mListBox *self, mCombobox* sender, int id, DWORD param)
{

	//TODO:
	full_info(sender, self, "props");
	return TRUE; /* allowed the event goto next */
}


static void connect_prop(mSlEdit* self, mListBox* sender, mWidget* target)
{
	//delete the old connects
	ncsRemoveObjectBindProps((mObject*)self);
	SetWindowText(self->hwnd, "");

	if(!target)
		return ;

	//get current prop
	int prop_type;
	int prop = get_cur_prop_info(sender, &prop_type, NULL);
	if(prop <= 0)
		return ;
	ncsConnectBindProps(
					NCS_CMPT_PROP(self, NCSN_SLEDIT_ENTER, NCSP_WIDGET_TEXT, NCS_BT_STR, NCS_PROP_FLAG_READ|NCS_PROP_FLAG_WRITE),
					NCS_CMPT_PROP(target, 0, prop, prop_type,NCS_PROP_FLAG_READ| NCS_PROP_FLAG_WRITE),
					NCS_BPT_DBL);
	ncsAutoReflectObjectBindProps((mObject*)self);
}

//$func #1853825024 NCSN_LSTBOX_SELCHANGED_2050961408_4009177088
static BOOL se_source_prop_on_lb_source_props_selchanged (mSlEdit *self, mListBox* sender, int id, DWORD param)
{

	//TODO:
	connect_prop(self, sender,
			ncsObjFromHandle( GetDlgItem(GetParent(self->hwnd), ID_SENDER) ) );
	return TRUE; /* allowed the event goto next */
}

//$func #1853825024 NCSN_LSTBOX_SELCHANGED_398116864_4293115904
static BOOL se_listener_prop_on_lb_listener_props_selchanged (mSlEdit *self, mListBox* sender, int id, DWORD param)
{

	//TODO:
	connect_prop(self, sender, ncsObjFromHandle(GetDlgItem(GetParent(self->hwnd), ID_LISTENER)));
	return TRUE; /* allowed the event goto next */
}

//$connect #1853825024 -- Need by merge, don't modify
static NCS_EVENT_CONNECT_INFO Main_connects [] = {
	{ID_CB_SOURCE_CTRLS, ID_LB_SOURCE_EVENTS, NCSN_CMBOX_SELECTOK, (NCS_CB_ONOBJEVENT)lb_source_events_on_cb_source_ctrls_selectok},
	{ID_UPDATE, ID_MAIN, NCSN_WIDGET_CLICKED, (NCS_CB_ONOBJEVENT)main_on_update_clicked},
	{ID_CB_LISTNER_CTRLS, ID_LB_LISTENER_EVENTS, NCSN_CMBOX_SELECTOK, (NCS_CB_ONOBJEVENT)lb_listener_events_on_cb_listner_ctrls_selectok},
	{ID_LB_SOURCE_PROPS, ID_SE_SOURCE_PROP, NCSN_LSTBOX_SELCHANGED, (NCS_CB_ONOBJEVENT)se_source_prop_on_lb_source_props_selchanged},
	{ID_LB_LISTENER_PROPS, ID_SE_LISTENER_PROP, NCSN_LSTBOX_SELCHANGED, (NCS_CB_ONOBJEVENT)se_listener_prop_on_lb_listener_props_selchanged},
	{ID_CB_SOURCE_CTRLS, ID_LB_SOURCE_PROPS, NCSN_CMBOX_SELECTOK, (NCS_CB_ONOBJEVENT)lb_source_props_on_cb_source_ctrls_selectok},
	{ID_CB_LISTNER_CTRLS, ID_LB_LISTENER_PROPS, NCSN_CMBOX_SELECTOK, (NCS_CB_ONOBJEVENT)lb_listener_props_on_cb_listner_ctrls_selectok},
//$user -- TODO add your handlers hear
	{-1, -1, 0, NULL}
};

//$func @1853825024 onClose -- Need by merge, don't modify
static BOOL Main_onClose (mWidget* self, int message)
{

	//TODO:
	PostQuitMessage(self->hwnd);
	DestroyMainWindow(self->hwnd);
	if(ctrl_source)
		_c(ctrl_source)->release(ctrl_source);
	return FALSE; /*continue default message process*/
}

//$func @1853825024 onCreate
static BOOL Main_onCreate (mWidget* self, DWORD dwAddData)
{

	//TODO:
	//create data source
	ctrl_source = ncsCreateEtcDataSource("./controls.ini", NCS_DS_SELECT_READ);
	if(ctrl_source)
	{
		mRecordSet* rs = _c(ctrl_source)->selectRecordSet(ctrl_source, "/controls@count:{$control}", NCS_DS_SELECT_READ);
		if(rs)
		{
			mCombobox* sender = (mCombobox*)_c(self)->getChild(self, ID_CB_SOURCE_CTRLS);
			if(sender){
				_c(sender)->setSpecificData(sender, NCSSPEC_OBJ_CONTENT,(DWORD)rs, ncsRefObjFreeSpecificData);
				_c(sender)->setProperty(sender, NCSP_COMB_SELECT, 0);
			}
			mCombobox* listener = (mCombobox*)_c(self)->getChild(self, ID_CB_LISTNER_CTRLS);
			if(listener){
				_c(listener)->setSpecificData(listener, NCSSPEC_OBJ_CONTENT,(DWORD)rs, ncsRefObjFreeSpecificData);
				_c(listener)->setProperty(listener, NCSP_COMB_SELECT, 0);
			}
		}
	}
	return _c(self)->onCreate(self, dwAddData);
}

//$handle @1853825024 -- Need by merge, don't modify
static NCS_EVENT_HANDLER Main_handlers [] = {
	{MSG_CLOSE, Main_onClose},
	{MSG_CREATE, Main_onCreate},
//$user -- TODO add your handlers hear
	{-1,NULL}
};

//$mainhandle -- Need by merge, don't modify
NCS_EVENT_HANDLER_INFO mainwnd_Main_handlers [] = {
	{ID_MAIN, Main_handlers},
//$user --TODO Add your handlers hear
	{-1, NULL}
};

//$mainwnd_entry -- don't modify
NCS_WND_EXPORT mMainWnd* ntCreateMainEx(HPACKAGE package, HWND hParent, HICON h_icon, HMENU h_menu, DWORD user_data)
{
	// pelease don't change me
	return ncsCreateMainWindowIndirectFromID(package,
		ID_MAIN, /* window id */
		hParent,
		h_icon,
		h_menu,
		mainwnd_Main_handlers,
		Main_connects,
		user_data);
}

#endif	//_MGNCSCTRL_COMBOBOX
