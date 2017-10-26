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

#include <mgncs/mgncs.h>

#include "resource.h"
#include "ncs-windows.h"

#define MY_TEXT  100


//$func #2028552192 NCSN_WIDGET_CLICKED_1072560128_2028552192 -- Need by merge, don't modify
static BOOL mainselect_on_cancel_clicked (mMainWnd *self, mButton* sender, int id, DWORD param)
{

	//TODO:

	_c(self)->endDialog(self, 0);

	return TRUE; /* allow the event to go next */
}

//$func #2028552192 NCSN_WIDGET_CLICKED_3155369984_2028552192
static BOOL mainselect_on_ok_clicked (mMainWnd *self, mButton* sender, int id, DWORD param)
{

	//TODO:
	char *strText = (char*)_c(self)->getSpecificData(self, MY_TEXT, NULL);
	if(strText)
	{
		GetDlgItemText(self->hwnd, ID_MLSELECT, strText, 512);
		_c(self)->endDialog(self, 1);
	}
	else
		_c(self)->endDialog(self, 0);

	return TRUE; /* allow the event to go next */
}

//$connect #2028552192 -- Need by merge, don't modify
static NCS_EVENT_CONNECT_INFO Mainselect_connects [] = {
	{IDCANCEL, ID_MAINSELECT, NCSN_WIDGET_CLICKED, (NCS_CB_ONOBJEVENT)mainselect_on_cancel_clicked},
	{IDOK, ID_MAINSELECT, NCSN_WIDGET_CLICKED, (NCS_CB_ONOBJEVENT)mainselect_on_ok_clicked},
//$user -- TODO add your handlers hear
	{-1, -1, 0, NULL}
};

//$func @2028552192 onCreate -- Need by merge, don't modify
static BOOL Mainselect_onCreate (mWidget* self, DWORD dwAddData)
{

	//TODO:

	_c(self)->setSpecificData(self, MY_TEXT, dwAddData, NULL);
	SetFocus(GetDlgItem(self->hwnd, ID_MLSELECT));


	return _c(self)->onCreate(self, dwAddData);
}

//$handle @2028552192 -- Need by merge, don't modify
static NCS_EVENT_HANDLER Mainselect_handlers [] = {
	{MSG_CREATE, Mainselect_onCreate},
//$user -- TODO add your handlers hear
	{-1,NULL}
};

//$mainhandle -- Need by merge, don't modify
NCS_EVENT_HANDLER_INFO mainwnd_Mainselect_handlers [] = {
	{ID_MAINSELECT, Mainselect_handlers},
//$user --TODO: Add your handlers here
	{-1, NULL}
};

//$mainwnd_entry -- don't modify
NCS_WND_EXPORT mMainWnd* ntCreateMainselectEx(HPACKAGE package, HWND hParent, HICON h_icon, HMENU h_menu, DWORD user_data)
{
	// please don't change me
	return ncsCreateMainWindowIndirectFromID(package,
		ID_MAINSELECT, /* window id */
		hParent,
		h_icon,
		h_menu,
		mainwnd_Mainselect_handlers,
		Mainselect_connects,
		user_data);
}

