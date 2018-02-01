/* 
    This file is part of mGNCS, a component for MiniGUI.

    Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#ifdef _MGNCSCTRL_TOOLBAR

static mObject* mToolBar_createBody(mToolBar *self)
{
	//create a mlinelayoutpiece
	mLineLayoutPiece *linelayout = NEWPIECE(mLineLayoutPiece);
	linelayout->space = 2;
	return (mObject*)linelayout;
}

static void mToolBar_destroy(mToolBar *self)
{
	mLineLayoutPiece * lp = SAFE_CAST(mLineLayoutPiece, self->body);

	if(lp)
	{
		mLineLayoutPieceNode* pnode = lp->node;
		while(pnode)
		{
			FREETOOLITEM(pnode->piece);
			pnode->piece = NULL;
			pnode = pnode->next;
		}
	}

	return Class(mWidget).destroy((mWidget*)self);
}

static BOOL mToolBar_setProperty(mToolBar *self, int id, DWORD value)
{
	if(id >= NCSP_TLBAR_MAX)
		return FALSE;

	switch(id)
    {
    }

	return Class(mWidget).setProperty((mWidget*)self, id, value);
}

static DWORD mToolBar_getProperty(mToolBar *self, int id)
{
	if(id >= NCSP_TLBAR_MAX)
		return 0;


	return Class(mWidget).getProperty((mWidget*)self, id);
}

static void update_other_radioitem(mToolBar *self, mLineLayoutPieceNode * node, int state)
{
	mLineLayoutPieceNode *prev = node->prev;
	mLineLayoutPieceNode *next = node->next;
	while(prev && ncsIsPushToolItem(prev->piece))
	{
		if(_c(prev->piece)->setProperty(prev->piece, NCSP_ABP_CHECKSTATE, state))
			mHotPiece_update(prev->piece, (mObject*)self, TRUE);
		prev = prev->prev;
	}
	while(next && ncsIsPushToolItem(next->piece))
	{
		if(_c(next->piece)->setProperty(next->piece, NCSP_ABP_CHECKSTATE, state))
			mHotPiece_update(next->piece, (mObject*)self, TRUE);
		next = next->next;
	}
}

static BOOL mToolBar_onPiece(mToolBar *self, mHotPiece *sender, int event_id, DWORD param)
{
	switch(event_id)
	{
	case NCSN_TOOLITEM_CLICKED:
		SendMessage(self->hwnd, MSG_COMMAND, ncsToolItem_getId(sender), 0);
		break;
	case NCSN_TOOLITEM_PUSHED:
		break;
	case NCSN_TOOLITEM_SHOWMENU:
		ncsToolItem_showMenu(sender, (mObject*)self);
		break;
	case NCSN_TOOLITEM_STATE_CHANGED:
		//TODO:
		ncsNotifyParentEx((mWidget*)self,NCSN_TLBAR_ITEMCHAGED, ncsToolItem_getId(sender));
		if(!(_c(sender)->getProperty(sender, NCSP_ABP_AUTOCHECK))
			&& _c(sender)->getProperty(sender,NCSP_ABP_CHECKSTATE)==NCS_ABP_CHECKED)
		{
			mLineLayoutPiece *linelayout = (mLineLayoutPiece*)(self->body);
			mLineLayoutPieceNode *node;
			mHotPiece_update(sender, (mObject*)self, TRUE);
			if(linelayout && (node=_c(linelayout)->find(linelayout,sender))!=NULL){
				update_other_radioitem(self,node,NCS_ABP_UNCHECKED);
			}
		}
		break;
	}
	return FALSE;
}

static BOOL mToolBar_addItem(mToolBar *self, void *item,int type, int min_size, int max_size)
{
	mLineLayoutPiece * linelayout;
	mHotPiece *piece;
	SIZE minsize;
	if(!(piece=SAFE_CAST(mHotPiece, item)))
		return FALSE;

	if(!self->body || !(linelayout=SAFE_CAST(mLineLayoutPiece, self->body)))
		return FALSE;

	if(_c(linelayout)->addPiece(linelayout, piece, type, min_size, max_size))
	{
		int event_ids[]={
				NCSN_TOOLITEM_CLICKED,
				NCSN_TOOLITEM_PUSHED,
				NCSN_TOOLITEM_SHOWMENU,
				NCSN_TOOLITEM_STATE_CHANGED,
				0
		};
		RECT rc;
		GetClientRect(self->hwnd, &rc);
		minsize.cx = 12;
		minsize.cy = RECTH(rc);
		_c(piece)->autoSize(piece, (mObject*)self, &minsize, NULL);
		if(self->renderer)
			_c(piece)->setRenderer(piece, self->renderer->rdr_name);
		else
			_c(piece)->setRenderer(piece, "classic");
		//resize
		_c(linelayout)->recalc(linelayout);
		InvalidateRect(self->hwnd, NULL, TRUE);

		if(ncsIsPushToolItem(item))
		{
			ncsAddEventListeners((mObject*)piece, (mObject*)self,(NCS_CB_ONPIECEEVENT)mToolBar_onPiece, event_ids);
		}

		return TRUE;
	}
	return FALSE;
}

static LRESULT mToolBar_wndProc(mToolBar *self, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == MSG_COMMAND)
	{
		return SendMessage(GetParent(self->hwnd), MSG_COMMAND, wParam, lParam);
	}
	return Class(mWidget).wndProc((mWidget*)self, message, wParam, lParam);
}

static mLineLayoutPieceNode* get_node_by_id(mToolBar *self, int id)
{
	mLineLayoutPiece *linelayout = (mLineLayoutPiece*)(self->body);
	mLineLayoutPieceNode * node = linelayout->node;
	if(id <= 0)
		return NULL;
	while(node)
	{
		if(ncsToolItem_getId(node->piece) == id)
			return node;
	}
	return NULL;
}

static int mToolBar_isItemChecked(mToolBar* self, int id)
{
	mLineLayoutPieceNode * node = get_node_by_id(self, id);
	if(node)
		return _c(node->piece)->getProperty(node->piece,NCSP_ABP_CHECKSTATE)==NCS_ABP_CHECKED?
			NCS_TOOLITEM_CHECKED:NCS_TOOLITEM_UNCHECKED;

	return 0;
}

static int mToolBar_checkItem(mToolBar* self, int id, int state)
{
	mLineLayoutPieceNode * node = get_node_by_id(self, id);

	if(!node)
		return 0;
	if(ncsIsPushToolItem(node->piece) && _c(node->piece)->getProperty(node->piece, NCSP_ABP_CHECKABLE))
	{
		if(_c(node->piece)->getProperty(node->piece, NCSP_ABP_AUTOCHECK))
		{
			if(_c(node->piece)->setProperty(node->piece, NCSP_ABP_CHECKSTATE,
				state == NCS_TOOLITEM_CHECKED? NCS_ABP_CHECKED:NCS_ABP_UNCHECKED))
				mHotPiece_update(node->piece, (mObject*)self, TRUE);
		}
		else
		{
			if(NCS_TOOLITEM_CHECKED != state)
				return 0;

			if(_c(node->piece)->setProperty(node->piece, NCSP_ABP_CHECKSTATE,
				state == NCS_TOOLITEM_CHECKED? NCS_ABP_CHECKED:NCS_ABP_UNCHECKED))
			{
				mHotPiece_update(node->piece, (mObject*)self, TRUE);
				update_other_radioitem(self, node, NCS_ABP_UNCHECKED);
			}
			return 1;
		}
	}

	return 0;
}

static void mToolBar_onPaint(mToolBar *self, HDC hdc, const CLIPRGN * pinv)
{
	if(Body)
		_c(Body)->paint(Body, hdc, (mObject*)self, NCS_PIECE_PAINT_VERT);
}

BEGIN_CMPT_CLASS(mToolBar, mWidget)
	SET_DLGCODE(DLGC_WANTALLKEYS         )
	CLASS_METHOD_MAP(mToolBar, createBody)
	CLASS_METHOD_MAP(mToolBar, destroy)
	CLASS_METHOD_MAP(mToolBar, setProperty)
	CLASS_METHOD_MAP(mToolBar, getProperty)
	CLASS_METHOD_MAP(mToolBar, addItem    )
	CLASS_METHOD_MAP(mToolBar, wndProc    )
	CLASS_METHOD_MAP(mToolBar, onPaint    )
	CLASS_METHOD_MAP(mToolBar, isItemChecked)
	CLASS_METHOD_MAP(mToolBar, checkItem)
END_CMPT_CLASS

#endif //_MGNCSCTRL_TOOLBAR
