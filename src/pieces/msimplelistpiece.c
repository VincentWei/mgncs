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
#include <assert.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "mhotpiece.h"
#include "mcontainerpiece.h"
#include "mlayoutpiece.h"
#include "mlinelayoutpiece.h"
#include "msimplelistpiece.h"

#ifdef _MGNCSCTRL_IMWORDSEL

#define LAYOUT_PIECE(self)   ((mLineLayoutPiece*)(self->body))

static void mSimpleListPiece_construct(mSimpleListPiece* self, DWORD param)
{
	_SUPER(mContainerPiece, self, construct, param);

	self->body = (mHotPiece*)NEWPIECE(mLineLayoutPiece);
}

static mLineLayoutPieceNode*  find_last_node(mLineLayoutPiece *l)
{
	mLineLayoutPieceNode* node = l->node;
	if(!node)
		return NULL;

	while(node->next)
		node = node->next;
	return node;
}

static int get_sel_index(mSimpleListPiece* self)
{
	int i = 0;
	mLineLayoutPiece* l = LAYOUT_PIECE(self);
	mLineLayoutPieceNode* node = l->node;
	if(!self->cur_node || !self->cur_node->piece)
		return -1;

	while(node != self->cur_node)
	{
		node = node->next;
		i ++;
	}

	if(node == NULL)
		return -1;
	return i;
}

static void set_cur_node(mSimpleListPiece* self, mLineLayoutPieceNode* new_cur_node, mWidget* owner)
{
	if(self->cur_node == new_cur_node)
		return ;

	if(self->cur_node && self->cur_node->piece)
	{
		_c(self->cur_node->piece)->setProperty(self->cur_node->piece, PIECECOMM_PROP_SELECT, FALSE);
		mHotPiece_update(self->cur_node->piece, (mObject*)owner, TRUE);
	}

	if(new_cur_node->piece)
	{
		_c(new_cur_node->piece)->setProperty(new_cur_node->piece, PIECECOMM_PROP_SELECT, TRUE);
		mHotPiece_update(new_cur_node->piece, (mObject*)owner, TRUE);
	}
	self->cur_node = new_cur_node;
	RAISE_EVENT(self, NCSN_SIMPLELISTPIECE_CHANGED, (DWORD)get_sel_index(self));
}

static int mSimpleListPiece_processMessage(mSimpleListPiece* self, int message, WPARAM wParam, LPARAM lParam, mWidget* owner)
{
	mLineLayoutPiece* l = LAYOUT_PIECE(self);
	mLineLayoutPieceNode* node = NULL;
	switch(message)
	{
	case MSG_KEYDOWN:
		switch(wParam)
		{
		case SCANCODE_CURSORBLOCKLEFT:
			if(self->cur_node == NULL)
				node = l->node;
			else
				node = self->cur_node->next;

			set_cur_node(self, node, owner);
			if(node == NULL)
				RAISE_EVENT(self, NCSN_SIMPLELISTPIECE_REACHHEAD, 0);
			break;
		case SCANCODE_CURSORBLOCKRIGHT:
			if(self->cur_node == NULL)
				node = find_last_node(l);
			else
				node = self->cur_node->prev;
			if(node == NULL)
				RAISE_EVENT(self, NCSN_SIMPLELISTPIECE_REACHTAIL, 0);
			set_cur_node(self, node, owner);
			break;
		}
		break;
	case MSG_LBUTTONDOWN:
		{
			//hit test the current node
			mLineLayoutPieceNode* node = l->node;
			int x = LOSWORD(lParam);
			int y = HISWORD(lParam);
			while(node)
			{
				if(node->piece)
				{
					RECT rc;
					if(_c(node->piece)->getRect(node->piece,&rc) && PtInRect(&rc, x, y))
						break;
				}
				node = node->next;
			}

			if(node)
			{
				set_cur_node(self, node, owner);
			}
		}
		break;
	}
	return 0;
}

static void mSimpleListPiece_paint(mSimpleListPiece* self, HDC hdc, mWidget* owner, DWORD add_data)
{
	if(self->cur_node && self->cur_node->piece) //draw selection area
	{
		RECT rc;
		mWidgetRenderer* rdr = owner->renderer;
		if(_c(self->cur_node->piece)->getRect(self->cur_node->piece, &rc))
		{
			rdr->drawItem(owner, hdc, &rc, NCSRS_SELECTED);
		}
	}

	_c(self->body)->paint(self->body, hdc, (mObject*)owner, add_data);
}

static BOOL mSimpleListPiece_setProperty(mSimpleListPiece* self, int id, DWORD value)
{
	if(id == NCSP_SIMPLELISTPIECE_CURINDEX)
	{
		int i;
		mLineLayoutPiece* l = LAYOUT_PIECE(self);
		mLineLayoutPieceNode* node = l->node;
		for(i = 0; i < (int)value && node; i++)
		{
			node = node->next;
		}

		if(node == NULL)
			return FALSE;
		if(self->cur_node && self->cur_node->piece)
			_c(self->cur_node->piece)->setProperty(self->cur_node->piece, PIECECOMM_PROP_SELECT, FALSE);
		self->cur_node = node;
		if(node->piece)
			_c(node->piece)->setProperty(node->piece, PIECECOMM_PROP_SELECT, TRUE);
		return TRUE;
	}

	return _SUPER(mContainerPiece, self, setProperty, id, value);
}

static DWORD mSimpleListPiece_getProperty(mSimpleListPiece* self, int id)
{
	if(id == NCSP_SIMPLELISTPIECE_CURINDEX)
	{
		return (DWORD)get_sel_index(self);
	}
	return _SUPER(mSimpleListPiece, self, getProperty, id);
}

static void mSimpleListPiece_reset(mSimpleListPiece* self, void(*free_piece)(mHotPiece*))
{
	mLineLayoutPiece* l = LAYOUT_PIECE(self);
	if(l)
	{
		_c(l)->reset(l, free_piece);
		self->cur_node = NULL;
	}
}

BEGIN_MINI_CLASS(mSimpleListPiece, mContainerPiece)
	CLASS_METHOD_MAP(mSimpleListPiece, paint)
	CLASS_METHOD_MAP(mSimpleListPiece, construct)
	CLASS_METHOD_MAP(mSimpleListPiece, processMessage)
	CLASS_METHOD_MAP(mSimpleListPiece, getProperty)
	CLASS_METHOD_MAP(mSimpleListPiece, setProperty)
	CLASS_METHOD_MAP(mSimpleListPiece, reset)
END_MINI_CLASS

#endif		//_MGNCSCTRL_IMWORDSEL

