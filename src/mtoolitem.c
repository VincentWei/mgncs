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
    <http://www.minigui.com/blog/minigui-licensing-policy/>.
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
#include "mem-slab.h"

#define SLAB_ALLOC(size)  _SLAB_ALLOC(size)
#define SLAB_FREE(p)      _SLAB_FREE(p)


/////////////////////////////////////////////////////

mObject* ncsNewToolItem(int id, mObjectClass* _class, DWORD param)
{
	int *pid;
	mObject *obj;
	if(_class == NULL)
		return NULL;

	pid = (int*)SLAB_ALLOC(sizeof(int)+_class->objSize);

	obj = (mObject*)(pid + 1);

	*pid = id;

	obj->_class = _class;
	_class->construct(obj, param);
	return obj;
}

void ncsFreeToolItem(mObject* obj)
{
	if(!obj)
		return ;

	_c(obj)->destroy(obj);

	SLAB_FREE(((int*)obj) - 1);
}


////////////////////////////////////////////////////
static void mPushToolItem_raiseEvent(mPushToolItem *self, int event_id, DWORD param)
{
	int id;
	switch(event_id)
	{
	case NCSN_ABP_CLICKED:
		if(INSTANCEOF(self, mMenuToolItem))
			id = NCSN_TOOLITEM_SHOWMENU;
		else
			id = NCSN_TOOLITEM_CLICKED;
		break;
	case NCSN_ABP_PUSHED:
		id = NCSN_TOOLITEM_PUSHED;
		break;
	case NCSN_ABP_STATE_CHANGED:
		id = NCSN_TOOLITEM_STATE_CHANGED;
		break;
	default:
		return ;
	}
	ncsRaiseEvent((mObject*)self, id, 0);
}

static void mPushToolItem_construct(mPushToolItem *self, DWORD add_data){
	mPushToolItemCreateInfo * create_info = (mPushToolItemCreateInfo*)add_data;

	if(!create_info)
		return;

	Class(mAbstractButtonPiece).construct((mAbstractButtonPiece*)self, 0);

	self->body = _c(self)->createContent(self, create_info);
	self->flags = 0;
	self->flags |= NCSS_ABP_FLAT;
}

static BOOL mPushToolItem_setCheck(mPushToolItem *self, int state)
{
	mHotPiece *body = (mHotPiece*)(self->body);
	if(body){
		int check_state = NCS_ABP_UNCHECKED;
		if(state == NCS_TOOLITEM_CHECKED)
			check_state = NCS_ABP_CHECKED;
		return _c(body)->setProperty(body, NCSP_ABP_CHECKSTATE, check_state);
	}
	return FALSE;
}

static int mPushToolItem_getCheck(mPushToolItem *self)
{
	mHotPiece *body = (mHotPiece*)(self->body);
	if(body){
		return _c(body)->getProperty(body,NCSP_ABP_CHECKSTATE) == NCS_ABP_CHECKED?
					NCS_TOOLITEM_CHECKED:NCS_TOOLITEM_UNCHECKED;
	}
	return NCS_TOOLITEM_UNCHECKED;
}

static mHotPiece * mPushToolItem_createContent(mPushToolItem* self, mPushToolItemCreateInfo* create_info)
{
	mHotPiece *content = NULL;
	if(create_info->str && create_info->toolImg){
		//create a image label piece
		mHotPiece *pieces[2];
		pieces[0] = (mHotPiece*)NEWPIECEEX(mToolImageItemPiece, create_info->toolImg);
		pieces[1] = (mHotPiece*)NEWPIECEEX(mLabelPiece, create_info->str);
		content = (mHotPiece*)NEWPIECEEX(mPairPiece, pieces);
		_c(content)->setProperty(content,NCSP_PAIRPIECE_FIRST_SIZE_TYPE,NCS_PAIRPIECE_ST_AUTO);
	}
	else if(create_info->str)
	{
		//create a label only
		content = (mHotPiece*)NEWPIECEEX(mLabelPiece, create_info->str);
		if(create_info->flags & NCS_TOOLITEM_FLAG_TEXT_LEFT)
			_c(content)->setProperty(content, NCSP_PAIRPIECE_SECOND_AS_FIRST, 1);
		if(create_info->flags & NCS_TOOLITEM_LABEL_TEXT_LEFT)
			_c(content)->setProperty(content, NCSP_LABELPIECE_ALIGN, 0);
		else if(create_info->flags & NCS_TOOLITEM_LABEL_TEXT_RIGHT)
			_c(content)->setProperty(content, NCSP_LABELPIECE_ALIGN, 1);
		else if(create_info->flags & NCS_TOOLITEM_LABEL_TEXT_CENTER)
			_c(content)->setProperty(content, NCSP_LABELPIECE_ALIGN, 2);
		else if(create_info->flags & NCS_TOOLITEM_LABEL_TEXT_TOP)
			_c(content)->setProperty(content, NCSP_LABELPIECE_VALIGN, 0);
		else if(create_info->flags & NCS_TOOLITEM_LABEL_TEXT_BOTTOM)
			_c(content)->setProperty(content, NCSP_LABELPIECE_VALIGN, 1);
		else if(create_info->flags & NCS_TOOLITEM_LABEL_TEXT_VCENTER)
			_c(content)->setProperty(content, NCSP_LABELPIECE_VALIGN, 2);
		if(create_info->flags & NCS_TOOLITEM_FLAG_VERT)
			_c(content)->setProperty(content, NCSP_PAIRPIECE_DIRECTION, 1);//vert
	}
	else if(create_info->toolImg)
	{
		//crete a tool image piece only
		content = (mHotPiece*)NEWPIECEEX(mToolImageItemPiece, create_info->toolImg);
	}
	return content;
}

static BOOL mPushToolItem_setProperty(mPushToolItem *self, int id, DWORD value)
{
	if(Class(mAbstractButtonPiece).setProperty((mAbstractButtonPiece*)self, id, value))
		return TRUE;

	return self->body ? _c(self->body)->setProperty(self->body, id, value) : FALSE;
}

static DWORD mPushToolItem_getProperty(mPushToolItem *self, int id)
{
	if(id >= ABP_PROP_BEGIN && id <= ABP_PROP_END)
		return Class(mAbstractButtonPiece).getProperty((mAbstractButtonPiece*)self, id);
	if(self->body)
	{
		return _c(self->body)->getProperty(self->body, id);
	}
    return 0;
}

static void mPushToolItem_paint(mPushToolItem *self, HDC hdc, mWidget* owner, DWORD add_data)
{
	if(self->body)
	{
		add_data = (add_data & (~NCS_PIECE_PAINT_CHECK_STATE_MASK) ) | NCS_PIECE_PAINT_MAKE_CHECK(self->check_state);
		if(self->state == NCS_ABP_PUSHED)
		{
			mHotPiece* body = self->body;
			RECT rc;
			_c(body)->getRect(body, &rc);
			OffsetRect(&rc, 1,1);
			_c(body)->setRect(body, &rc);
			_c(body)->paint(body, hdc, (mObject*)owner, add_data);
			OffsetRect(&rc, -1,-1);
			_c(body)->setRect(body, &rc);
		}
		else
			_c(self->body)->paint(self->body, hdc, (mObject*)owner, add_data);
	}
}

BEGIN_MINI_CLASS(mPushToolItem, mAbstractButtonPiece)
	CLASS_METHOD_MAP(mPushToolItem, construct)
	CLASS_METHOD_MAP(mPushToolItem, setCheck )
	CLASS_METHOD_MAP(mPushToolItem, getCheck )
	CLASS_METHOD_MAP(mPushToolItem, createContent)
	CLASS_METHOD_MAP(mPushToolItem, raiseEvent)
	CLASS_METHOD_MAP(mPushToolItem, paint)
    CLASS_METHOD_MAP(mPushToolItem, getProperty)
    CLASS_METHOD_MAP(mPushToolItem, setProperty)
END_MINI_CLASS


////////////////////////////////////////
//menuitem

static void mMenuToolItem_destroy(mMenuToolItem *self)
{
	if(self->popmenu)
		_c(self->popmenu)->release(self->popmenu);
	Class(mPushToolItem).destroy((mPushToolItem*)self);
}

static mHotPiece * mMenuToolItem_createContent(mMenuToolItem * self, mPushToolItemCreateInfo* create_info)
{
	mHotPiece * content = Class(mPushToolItem).createContent((mPushToolItem*)self,create_info);
	mDownArrowPiece * downArrow = NEWPIECE(mDownArrowPiece);
	mHotPiece *pieces[2]={(mHotPiece*)downArrow, content};
	content = (mHotPiece*)NEWPIECEEX(mPairPiece, pieces);

	_c(content)->setProperty(content,NCSP_PAIRPIECE_FIRST_SIZE_TYPE,NCS_PAIRPIECE_ST_FIXED);
	_c(content)->setProperty(content,NCSP_PAIRPIECE_FIRST_SIZE,8);

	if(!(create_info->flags & NCS_TOOLITEM_FLAG_TEXT_LEFT))
		_c(content)->setProperty(content, NCSP_PAIRPIECE_SECOND_AS_FIRST, 1);

	return content;
}

static BOOL mMenuToolItem_showMenu(mMenuToolItem *self, mWidget * owner)
{
	//HMENU hMenu;
	if(!owner || !self->popmenu)
		return FALSE;

	_c(self->popmenu)->popMenu(self->popmenu, (mObject*)owner);
	return TRUE;
}

BEGIN_MINI_CLASS(mMenuToolItem, mPushToolItem)
	CLASS_METHOD_MAP(mMenuToolItem, createContent)
	CLASS_METHOD_MAP(mMenuToolItem, showMenu)
	CLASS_METHOD_MAP(mMenuToolItem, destroy)
END_MINI_CLASS

/**
 *  mCheckToolItem
 *
 */

/**
 *  mRadioToolItem
 *
 */

/**
 *  mWidgetToolItem
 */

void* ncsCreatePushToolItem(int id, mToolImage *img, const char* str, UINT flags)
{ 
	mPushToolItemCreateInfo createInfo;
	//mPushToolItem * toolitem;
	createInfo.str = str;
	createInfo.toolImg = img;
	createInfo.flags = flags;

	return NEWTOOLITEM(id, mPushToolItem, (DWORD)&createInfo);
}

void* ncsCreateMenuToolItem(int id, mToolImage *img, const char* str, UINT flags, mPopMenuMgr *menu)
{
	mPushToolItemCreateInfo createInfo;
	mMenuToolItem * toolitem;
	createInfo.str = str;
	createInfo.toolImg = img;
	createInfo.flags = flags;

	toolitem = NEWTOOLITEM(id, mMenuToolItem, &createInfo);
	toolitem->popmenu = menu;
	if(menu)
		_c(menu)->addRef(menu);
	return toolitem;
}

void* ncsCreateCheckToolItem(int id, mToolImage *img, const char* str, UINT flags, int state)
{
	mPushToolItem * toolitem = (mPushToolItem*)ncsCreatePushToolItem(id, img, str, flags);
	if(toolitem)
	{
		_c(toolitem)->setProperty(toolitem, NCSP_ABP_CHECKABLE,1);
		_c(toolitem)->setProperty(toolitem, NCSP_ABP_AUTOCHECK,1);
		_c(toolitem)->setProperty(toolitem, NCSP_ABP_CHECKSTATE, state == NCS_TOOLITEM_CHECKED ? NCS_ABP_CHECKED : 0);
	}
	return toolitem;
}

void* ncsCreateRadioToolItem(int id, mToolImage *img, const char* str, UINT flags)
{
	mPushToolItem * toolitem = (mPushToolItem*)ncsCreatePushToolItem(id, img, str, flags);
	if(toolitem)
	{
		_c(toolitem)->setProperty(toolitem, NCSP_ABP_CHECKABLE,1);
	}
	return toolitem;
}

void* ncsCreateWidgetToolItem(mWidget* widget)
{
	if(!widget)
		return NULL;

	return NEWTOOLITEM(_c(widget)->getId(widget),mWidgetWrapperPiece, widget);
}

void* ncsCreateSeparatorItem()
{
	return NEWTOOLITEM(-1,mSeparatorPiece,0);
}

int ncsToolItem_getId(void *self)
{
	if(self == NULL)
		return -1;
	
	return ((int*)self)[-1];
}

int ncsToolItem_setId(void *self, int id)
{
	int oldid = -1;
	if(self == NULL)
		return -1;
	
	oldid = ((int*)self)[-1];

	((int*)self)[-1] = id;

	return oldid;
}

BOOL ncsToolItem_setCheck(void *self, int check_state)
{
	if(INSTANCEOF(self, mPushToolItem)){
		mPushToolItem *p = (mPushToolItem*)self;
		return _c(p)->setCheck(p, check_state);
	}
	return FALSE;
}

int ncsToolItem_getCheck(void *self)
{
	if(INSTANCEOF(self, mPushToolItem)){
		mPushToolItem *p = (mPushToolItem*)self;
		return _c(p)->getCheck(p);
	}
	return NCS_TOOLITEM_UNCHECKED;
}
BOOL ncsToolItem_showMenu(void*self, mObject *owner)
{
	if(INSTANCEOF(self, mMenuToolItem))
		return _c((mMenuToolItem*)self)->showMenu((mMenuToolItem*)self, (mWidget*)owner);
	return FALSE;
}

/////////////////////////////////////////////////////////
mToolImage * ncsNewToolImage(PBITMAP pbmp, int cell_count, BOOL autoUnload, BOOL bVert)
{
	mToolImage * toolImg;
	if(pbmp == NULL || cell_count <= 0)
		return NULL;

	toolImg = (mToolImage*)SLAB_ALLOC(sizeof(mToolImage));
	toolImg->pbmp = pbmp;
	toolImg->cell_count = cell_count;
	toolImg->flags = 0;
	if(autoUnload)
		toolImg->flags |= TOOLIMGF_UNLOAD;
	if(bVert)
		toolImg->flags |= TOOLIMGF_VERTCELL;

	return toolImg;
}
void ncsFreeToolImage(mToolImage *mti)
{
	if(!mti)
		return;

	if(mti->flags & TOOLIMGF_UNLOAD)
	{
		UnloadBitmap(mti->pbmp);
		free(mti->pbmp);
	}
	SLAB_FREE(mti);
}

mToolImage * ncsNewToolImageFromFile(const char *fileName, int cell_count, BOOL autoUnload, BOOL bVert)
{
	PBITMAP pbmp = (PBITMAP)malloc(sizeof(BITMAP));
	if(LoadBitmapFromFile(HDC_SCREEN, pbmp, fileName)!=0){
		free(pbmp);
		return NULL;
	}
	return ncsNewToolImage(pbmp, cell_count, autoUnload, bVert);
}

int ncsGetToolItemType(void *toolitem)
{
	if(toolitem == NULL)
		return NCS_UNKNOWNTOOLITEM;

	if(INSTANCEOF(toolitem, mSeparatorPiece))
		return NCS_SEPARATORTOOLITEM;
	if(INSTANCEOF(toolitem, mMenuToolItem))
		return NCS_MENUTOOLITEM;
	if(INSTANCEOF(toolitem, mPushToolItem))
		return NCS_PUSHTOOLITEM;
	if(INSTANCEOF(toolitem, mWidgetWrapperPiece))
		return NCS_WIDGETTOOLITEM;

	return NCS_UNKNOWNTOOLITEM;
}

BOOL ncsIsPushToolItem(void *toolitem)
{
	return INSTANCEOF(toolitem, mPushToolItem);
}

BOOL ncsIsMenuToolItem(void *toolitem)
{
	return INSTANCEOF(toolitem, mMenuToolItem);
}
BOOL ncsIsSeparatorToolItem(void *toolitem)
{
	return INSTANCEOF(toolitem, mSeparatorPiece);
}

BOOL ncsIsWidgetToolItem(void *toolitem)
{
	return INSTANCEOF(toolitem, mWidgetWrapperPiece);
}

//BOOL ncsGetToolImageCell(mToolImage *mti, int idx, RECT *prc);
BOOL ncsDrawToolImageCell(mToolImage *mti, HDC hdc, int idx, const RECT *prc)
{
	int x=0, y=0;
	int xpos, ypos;
	int w, h;
	int cell_width, cell_height;
	if(!mti || !prc)
		return FALSE;

	if(idx < 0 )
		idx = 0;
	if(idx >= mti->cell_count)
		idx = mti->cell_count - 1;

	if(mti->flags&TOOLIMGF_VERTCELL)
	{
		y = mti->pbmp->bmHeight * idx / mti->cell_count;
		cell_width = mti->pbmp->bmWidth;
		cell_height = mti->pbmp->bmHeight / mti->cell_count;
	}
	else
	{
		x = mti->pbmp->bmWidth * idx / mti->cell_count;
		cell_width = mti->pbmp->bmWidth / mti->cell_count;
		cell_height = mti->pbmp->bmHeight;
	}

	xpos = prc->left;
	ypos = prc->top;
	w = RECTWP(prc);
	h = RECTHP(prc);

	if(cell_width < w)
	{
		w = cell_width;
		xpos = (prc->left + prc->right - cell_width)/2;
	}
	if(cell_height < h)
	{
		h = cell_height;
		ypos = (prc->top + prc->bottom - cell_height)/2;
	}

	//printf("xpos=%d, ypos=%d, w=%d, h=%d, x=%d, y=%d, hdc=%p, bmp=%p\n",
	//		xpos, ypos, w, h,x,y, hdc, mti->pbmp			);
	//printf("toolbitmap bits=%p, pbmp=%p\n", mti->pbmp->bmBits, mti->pbmp);

	FillBoxWithBitmapPart(hdc, xpos, ypos, w, h,
			0, 0,
			mti->pbmp, x, y);

	return TRUE;
}




///////////////////////////////
BOOL ncsInitToolItems()
{
	Class(mPushToolItem).classConstructor((mObjectClass*)(void *)&Class(mPushToolItem));
	Class(mMenuToolItem).classConstructor((mObjectClass*)(void *)&Class(mMenuToolItem));
	return TRUE;
}
