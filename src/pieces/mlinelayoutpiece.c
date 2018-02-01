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
#include "mlayoutpiece.h"
#include "mlinelayoutpiece.h"
#include "../mem-slab.h"

#if defined (_MGNCSCTRL_TOOLBAR) || defined (_MGNCSCTRL_IMWORDSEL)

static void mLineLayoutPiece_construct(mLineLayoutPiece* self, DWORD add_data)
{
	Class(mLayoutPiece).construct((mLayoutPiece*)self, add_data);

	self->left = self->top = self->right = self->bottom = 0;
	self->margin = 0;
	self->space = 0;
	self->node = self->tail_node = NULL;
	self->direction = 0;
	self->autoWrap = 0;
	self->line_height = 32;
}

static void mLineLayoutPiece_destroy(mLineLayoutPiece* self)
{
	while(self->node)
	{
		mLineLayoutPieceNode * node = self->node;
		self->node = self->node->next;
		DELPIECE(node->piece);
		_SLAB_FREE(node);
	}
	Class(mLayoutPiece).destroy((mLayoutPiece*)self);
}

static BOOL mLineLayoutPiece_setRenderer(mLineLayoutPiece * self, const char* rdr_name)
{
	mLineLayoutPieceNode * node = self->node;
	if(rdr_name == NULL)
		return FALSE;

	while(node)
	{
		if(node->piece)
			_c(node->piece)->setRenderer(node->piece, rdr_name);
		node = node->next;
	}

	return TRUE;
}

static mHotPiece * mLineLayoutPiece_hitTest(mLineLayoutPiece* self, int x, int y)
{
	mHotPiece *hp;
	if(x > self->left && x < self->right
		&& y > self->top && y < self->bottom)
	{
		mLineLayoutPieceNode * node = self->node;
		while(node)
		{
			if(node->piece && (hp = _c(node->piece)->hitTest(node->piece,x, y)) != NULL)
				return hp;
			node = node->next;
		}
		//return (mHotPiece*)self;
	}
	return NULL;
}

static void mLineLayoutPiece_horzRecalc(mLineLayoutPiece *self)
{
	mLineLayoutPieceNode * minnode=NULL;
	int minnode_count = 0;
	RECT rc_piece;
	mLineLayoutPieceNode * node = self->node;

	rc_piece.left = self->left + self->margin;
	rc_piece.top  = self->top + self->margin;

	if(self->autoWrap)
	{
		rc_piece.bottom = rc_piece.left + self->line_height;
		if(rc_piece.bottom > self->bottom - self->margin)
			rc_piece.bottom = self->bottom - self->margin;
		while(node){
			if(rc_piece.left >= self->right - self->margin){
				rc_piece.top = rc_piece.bottom + self->space;
				if(rc_piece.top >= self->bottom - self->margin)
					break;
				rc_piece.bottom = rc_piece.top + self->line_height;
				if(rc_piece.bottom > self->bottom - self->margin)
					rc_piece.bottom = self->bottom - self->margin;
				rc_piece.left = self->left + self->margin;
			}

			if(node->flags & NCS_LINELAYOUTPIECENODE_FLAG_AUTO)
			{
				int left = self->right - self->margin - rc_piece.left;
				if(node->def_max_size >0 && left > node->def_max_size)
					left = node->def_max_size;
				rc_piece.right = left;
			}
			else
			{
				RECT rc={0,0,16,0};
				_c(node->piece)->getRect(node->piece, &rc);
				rc_piece.right = rc_piece.left + RECTW(rc);
			}
			_c(node->piece)->setRect(node->piece, &rc_piece);
			rc_piece.left = rc_piece.right + self->space;
			node = node->next;
		}
	}
	else
	{
		rc_piece.bottom = self->bottom - self->margin;
		while(node)
		{
			if(rc_piece.left >= self->right - self->margin)
			{
				break;
			}

			if(node->flags & NCS_LINELAYOUTPIECENODE_FLAG_AUTO)
			{
				//get minSize, and set it
				rc_piece.right = rc_piece.left + node->def_min_size;
				if(minnode == NULL)
					minnode = node;
				minnode_count++;
			}
			else
			{
				RECT rc={0,0,16,0};
				_c(node->piece)->getRect(node->piece, &rc);
				rc_piece.right = rc_piece.left + RECTW(rc);
			}
			_c(node->piece)->setRect(node->piece, &rc_piece);
			rc_piece.left = rc_piece.right + self->space;
			node = node->next;
		}

		if(!node && minnode_count > 0)//
		{
			int left = self->right - rc_piece.left + self->space - self->margin;
			int offset = 0;
			if(left > 0){
				int avg = left / minnode_count;
				for(;minnode;minnode=minnode->next)
				{
					RECT rc;
					if(!_c(minnode->piece)->getRect(minnode->piece, &rc))
						continue;
					rc.left += offset;
					rc.right += offset;
					if(minnode->flags & NCS_LINELAYOUTPIECENODE_FLAG_AUTO){
						rc.right += avg;
						offset += avg;
					}
					_c(minnode->piece)->setRect(minnode->piece, &rc);
				}
			}//end left
		}//end count
	}//auto wrap

	while(node){
		RECT rc={0,0,0,0};
		_c(node->piece)->setRect(node->piece, &rc);
		node = node->next;
	}
}

static void mLineLayoutPiece_vertRecalc(mLineLayoutPiece *self)
{
	mLineLayoutPieceNode * minnode=NULL;
	int minnode_count = 0;
	RECT rc_piece;
	mLineLayoutPieceNode * node = self->node;

	rc_piece.left = self->left + self->margin;
	rc_piece.top  = self->top + self->margin;

	if(self->autoWrap)
	{
		rc_piece.right = rc_piece.top + self->line_height;
		if(rc_piece.right > self->right - self->margin)
			rc_piece.right = self->right - self->margin;
		while(node){
			if(rc_piece.top >= self->bottom - self->margin){
				rc_piece.left = rc_piece.right + self->space;
				if(rc_piece.left >= self->right - self->margin)
					break;
				rc_piece.right = rc_piece.left + self->line_height;
				if(rc_piece.right > self->right - self->margin)
					rc_piece.right = self->right - self->margin;
				rc_piece.top = self->top + self->margin;
			}

			if(node->flags & NCS_LINELAYOUTPIECENODE_FLAG_AUTO)
			{
				int left = self->bottom - self->margin - rc_piece.top;
				if(node->def_max_size >0 && left > node->def_max_size)
					left = node->def_max_size;
				rc_piece.bottom = left;
			}
			else
			{
				RECT rc={0,0,0,16};
				_c(node->piece)->getRect(node->piece, &rc);
				rc_piece.bottom = rc_piece.top + RECTH(rc);
			}
			_c(node->piece)->setRect(node->piece, &rc_piece);
			rc_piece.top = rc_piece.bottom + self->space;
			node = node->next;
		}
	}
	else
	{
		rc_piece.right = self->right - self->margin;
		while(node)
		{
			if(rc_piece.top >= self->bottom - self->margin)
			{
				break;
			}

			if(node->flags & NCS_LINELAYOUTPIECENODE_FLAG_AUTO)
			{
				//get minSize, and set it
				rc_piece.bottom = rc_piece.top + node->def_min_size;
				if(minnode == NULL)
					minnode = node;
				minnode_count++;
			}
			else
			{
				RECT rc={0,0,16,0};
				_c(node->piece)->getRect(node->piece, &rc);
				rc_piece.bottom = rc_piece.top + RECTH(rc);
			}
			_c(node->piece)->setRect(node->piece, &rc_piece);
			rc_piece.top = rc_piece.bottom + self->space;
			node = node->next;
		}

		if(!node && minnode_count > 0)//
		{
			int left = self->bottom - rc_piece.top + self->space - self->margin;
			int offset = 0;
			if(left > 0){
				int avg = left / minnode_count;
				for(;minnode;minnode=minnode->next)
				{
					RECT rc;
					if(!_c(minnode->piece)->getRect(minnode->piece, &rc))
						continue;
					rc.top += offset;
					rc.bottom += offset;
					if(minnode->flags & NCS_LINELAYOUTPIECENODE_FLAG_AUTO){
						rc.bottom += avg;
						offset += avg;
					}
					_c(minnode->piece)->setRect(minnode->piece, &rc);
				}
			}//end left
		}//end count
	}//auto wrap

	//clear the disable pieces
	while(node){
		RECT rc={0,0,0,0};
		_c(node->piece)->setRect(node->piece, &rc);
		node = node->next;
	}
}

static BOOL mLineLayoutPiece_setRect(mLineLayoutPiece *self, const RECT *prc)
{
	if(prc == NULL)
		return FALSE;

	self->left = (short)prc->left;
	self->top = (short)prc->top;
	self->right = (short)prc->right;
	self->bottom = (short)prc->bottom;
	//recalc the size

	if(self->direction)
		mLineLayoutPiece_vertRecalc(self);
	else
		mLineLayoutPiece_horzRecalc(self);

	return TRUE;
}

static BOOL mLineLayoutPiece_getRect(mLineLayoutPiece* self, PRECT prc)
{
	if(!prc )
		return FALSE;

	prc->left = self->left;
	prc->top  = self->top;
	prc->right = self->right;
	prc->bottom = self->bottom;

	return TRUE;
}

static BOOL mLineLayoutPiece_setProperty(mLineLayoutPiece *self, int id, DWORD value)
{
	mLineLayoutPieceNode * node = self->node;
	switch(id)
	{
	case NCSP_LINELAYOUTPIECE_LINE_HEIGHT:
		if((int)value <= 0)
			return FALSE;
		self->line_height = (int)value;
		return TRUE;
	case NCSP_LINELAYOUTPiECE_AUTOWRAP:
		self->autoWrap = value?1:0;
		return TRUE;
	case NCSP_LINELAYOUTPIECE_DIRECTION:
		self->direction = value?1:0;
		return TRUE;
	case NCSP_LINELAYOUTPIECE_SPACE:
		if(self->space == (unsigned char)value)
			return FALSE;
		self->space = (unsigned char)value;
		_c(self)->recalc(self);
		return TRUE;
	case NCSP_LINELAYOUTPIECE_MARGIN:
		if(self->margin == (unsigned char)value)
			return FALSE;
		self->margin = (unsigned char)value;
		_c(self)->recalc(self);
		return TRUE;
	}

	for(;node;node=node->next)
	{
		if(node->piece)
			_c(node->piece)->setProperty(node->piece, id, value);
	}

	return Class(mLayoutPiece).setProperty((mLayoutPiece*)self, id, value);

}

static int get_node_count(mLineLayoutPiece* self)
{
	int i;
	mLineLayoutPieceNode * node = self->node;
	for(i=0;node;node=node->next, i++);
	return i;
}

static DWORD mLineLayoutPiece_getProperty(mLineLayoutPiece *self, int id)
{
	switch(id)
	{
	case NCSP_LINELAYOUTPIECE_SPACE:
		return self->space;

	case NCSP_LINELAYOUTPIECE_MARGIN:
		return self->margin;

	case NCSP_LINELAYOUTPIECE_COUNT:
		return get_node_count(self);
	case NCSP_LINELAYOUTPIECE_LINE_HEIGHT:
		return self->line_height;

	case NCSP_LINELAYOUTPiECE_AUTOWRAP:
		return self->autoWrap;
	case NCSP_LINELAYOUTPIECE_DIRECTION:
		return self->direction;
	}

	return Class(mLayoutPiece).getProperty((mLayoutPiece*)self, id);
}

static void mLineLayoutPiece_paint(mLineLayoutPiece *self, HDC hdc, mWidget* owner, DWORD add_data)
{
	mLineLayoutPieceNode * node = self->node;
	for(;node;node=node->next)
	{
		_c(node->piece)->paint(node->piece, hdc, (mObject*)owner,add_data);
	}

}


static BOOL mLineLayoutPiece_autoSize(mLineLayoutPiece *self, mWidget *owner, const SIZE *pszMin, const SIZE *pszMax)
{
	return FALSE;
}

static mLineLayoutPieceNode * test_and_new_lineLayoutPiece(mLineLayoutPiece* self, mHotPiece* piece, int type, int min_size, int max_size)
{
	mLineLayoutPieceNode * node = self->node;
	mLineLayoutPieceNode * new_node;

	if(!piece)
		return NULL;

	if(node){
		while(node && node->piece != piece)
			node = node->next;

		if(node && node->piece == piece)
			return NULL;
	}

	new_node  = (mLineLayoutPieceNode*)_SLAB_ALLOC(sizeof(mLineLayoutPieceNode));
	new_node->piece = piece;
	new_node->def_min_size = min_size;
	new_node->def_max_size = max_size;
	new_node->flags = type&0xFF;
	return new_node;
}

static BOOL mLineLayoutPiece_addPiece(mLineLayoutPiece *self, mHotPiece *piece, int type, int min_size, int max_size)
{
	mLineLayoutPieceNode* node;
	node = test_and_new_lineLayoutPiece(self, piece, type, min_size, max_size);
	if(!node)
		return FALSE;

	node->next = NULL;
	node->prev = self->tail_node;
	if(self->tail_node)
		self->tail_node->next = node;
	self->tail_node = node;
	if(self->node == NULL)
		self->node = node;
	return TRUE;
}

static BOOL mLineLayoutPiece_appendPiece(mLineLayoutPiece *self, mHotPiece *piece, int type, int min_size, int max_size)
{
	mLineLayoutPieceNode* node;
	node = test_and_new_lineLayoutPiece(self, piece, type, min_size, max_size);
	if(!node)
		return FALSE;
	node->next = NULL;
	node->prev = self->tail_node;
	if(self->tail_node)
		self->tail_node->next = node;
	self->tail_node = node;
	if(self->node == NULL)
		self->node = node;
	return TRUE;
}

static void mLineLayoutPiece_recalc(mLineLayoutPiece *self)
{
	if(self->direction)
		mLineLayoutPiece_vertRecalc(self);
	else
		mLineLayoutPiece_horzRecalc(self);
}

static mLineLayoutPieceNode * mLineLayoutPiece_find(mLineLayoutPiece *self, mHotPiece *piece)
{
	mLineLayoutPieceNode * node;
	if(!piece)
		return NULL;

	for(node = self->node; node; node = node->next)
		if(node->piece == piece)
			return node;

	return NULL;
}

static void mLineLayoutPiece_reset(mLineLayoutPiece* self, void(*free_piece)(mHotPiece*))
{
	mLineLayoutPieceNode* node = self->node;
	while(node)
	{
		mLineLayoutPieceNode* t = node;
		node = node->next;
		if(free_piece)
			free_piece(t->piece);
		//free node
		_SLAB_FREE(t);
	}
	self->node = NULL;
	self->tail_node = NULL;
}

BEGIN_MINI_CLASS(mLineLayoutPiece, mLayoutPiece)
	CLASS_METHOD_MAP(mLineLayoutPiece, construct  )
	CLASS_METHOD_MAP(mLineLayoutPiece, destroy    )
	CLASS_METHOD_MAP(mLineLayoutPiece, setRenderer)
	CLASS_METHOD_MAP(mLineLayoutPiece, hitTest    )
	CLASS_METHOD_MAP(mLineLayoutPiece, setRect    )
	CLASS_METHOD_MAP(mLineLayoutPiece, getRect    )
	CLASS_METHOD_MAP(mLineLayoutPiece, setProperty)
	CLASS_METHOD_MAP(mLineLayoutPiece, getProperty)
	CLASS_METHOD_MAP(mLineLayoutPiece, paint      )
	CLASS_METHOD_MAP(mLineLayoutPiece, autoSize   )
	CLASS_METHOD_MAP(mLineLayoutPiece, addPiece   )
	CLASS_METHOD_MAP(mLineLayoutPiece, appendPiece)
	CLASS_METHOD_MAP(mLineLayoutPiece, recalc     )
	CLASS_METHOD_MAP(mLineLayoutPiece, find       )
	CLASS_METHOD_MAP(mLineLayoutPiece, reset      )
END_MINI_CLASS

#endif //_MGNCSCTRL_TOOLBAR
