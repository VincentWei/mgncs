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

static mWidget* get_selected_button(mButtonGroup* self, int *pidx)
{
	mButtonNode* pnode = self->pBtnList;
	int i = 0;

	while(pnode && pnode->pbtn)
	{
		if(_M(pnode->pbtn, getProperty, NCSP_BUTTON_CHECKSTATE) == NCS_BUTTON_CHECKED)
		{
			if(pidx)
				*pidx = i;
			return pnode->pbtn;
		}
		i ++;
		pnode = pnode->next;
	}

	if(pidx)
		*pidx = -1;

	return NULL;
}


BOOL mButtonGroup_removeButton(mButtonGroup *self, mButton* btn)
{
	mButtonNode* pnode, *prev = NULL;
	if(btn == NULL)
		return FALSE;
	
	//is the type?
	pnode = self->pBtnList;
	while(pnode)
	{
		if(pnode->pbtn == (mWidget*)btn) {
			//remove it
			mButtonNode* p = pnode;
			pnode = pnode->next;
			if(prev == NULL)
				self->pBtnList = pnode;
			else
				prev->next = pnode;
			free(p);
			break;
		}
		prev = pnode;
		pnode = pnode->next;
	}
	
	return TRUE;
}

BOOL mButtonGroup_addButton(mButtonGroup *self, mButton *btn)
{
	mButtonNode *temp = self->pBtnList;
	mButtonNode *pre = NULL;

	if(!btn)
		return FALSE;

	while(temp != NULL)
	{
		pre = temp;
		if (temp->pbtn == (mWidget*)btn) {
			return TRUE;
		}
		temp = temp->next;
	}

	if(btn && btn->group)
		mButtonGroup_removeButton((mButtonGroup*)(btn->group), btn);	

	if(_M(btn, getProperty, NCSP_BUTTON_CHECKSTATE) == NCS_BUTTON_CHECKED)
	{
		if(!get_selected_button(self, NULL))
			_M(self, checkBtn, (mWidget*)btn);
		else
			_M(btn, setProperty, NCSP_BUTTON_CHECKSTATE, NCS_BUTTON_UNCHECKED);	
	}


	if (NULL == (temp = malloc(sizeof(mButtonNode)))){
		fprintf(stderr, "[mButtonGroup] addButton Error, malloc NULL.");
		return FALSE;
	}

	temp->pbtn = (mWidget*)btn;
	temp->next = NULL;
	if (pre != NULL) {
		pre->next = temp;
	} else {
		self->pBtnList = temp;
	}
	btn->group = (mWidget*)self;

	return TRUE;
}

static BOOL mButtonGroup_checkBtn(mButtonGroup *self, mButton *button)
{
	BOOL bchanged = FALSE;
	mButtonNode *temp = self->pBtnList;
	if(button)
		_M(button, setProperty, NCSP_BUTTON_CHECKSTATE, NCS_BUTTON_CHECKED);

	while(temp != NULL)
	{
		if(((mButton*)(temp->pbtn)) != button)
		{
			if(_M(temp->pbtn, getProperty, NCSP_BUTTON_CHECKSTATE) == NCS_BUTTON_CHECKED)
			{
				_M(temp->pbtn, setProperty, NCSP_BUTTON_CHECKSTATE, NCS_BUTTON_UNCHECKED);
				bchanged = TRUE;
				break;
			}
		}
		temp = temp->next;
	}

	if(bchanged)
		ncsNotifyParentEx((mWidget*)self, NCSN_BTNGRP_SELCHANGED, (DWORD)(_M(button, getId)));

	return TRUE;
}

static BOOL check_button_by_id(mButtonGroup* self, int id)
{
	mButtonNode* pnode =  self->pBtnList;
	BOOL bSet = FALSE;
	mWidget* pclear = NULL;
	while(pnode)
	{
		mWidget* pbtn = pnode->pbtn;
		if(_M(pbtn, getId) == id)
		{
			bSet = TRUE;
			_M(pbtn, setProperty, NCSP_BUTTON_CHECKSTATE, NCS_BUTTON_CHECKED);
		}
		else if(!pclear && _M(pbtn, getProperty, NCSP_BUTTON_CHECKSTATE) == NCS_BUTTON_CHECKED)
		{
			pclear = pbtn;
		}
		pnode = pnode->next;
	}

	if(!bSet)
		return FALSE;
	
	if(pclear){
		_M(pclear, setProperty, NCSP_BUTTON_CHECKSTATE, NCS_BUTTON_UNCHECKED);
		return TRUE;
	}

	return FALSE;
}

static BOOL check_button_by_idx(mButtonGroup* self, int idx)
{
	mButtonNode* pnode =  self->pBtnList;
	mWidget* pclear = NULL;
	int i;
	for(i = 0; i < idx && pnode;i++, pnode = pnode->next){
		if(!pclear && _M(pnode->pbtn, getProperty, NCSP_BUTTON_CHECKSTATE) == NCS_BUTTON_CHECKED)
			pclear = pnode->pbtn;
	}

	if(!pnode)
		return FALSE;
	
	if(pclear == pnode->pbtn)
		return TRUE;

	if(pclear)
		_M(pclear, setProperty, NCSP_BUTTON_CHECKSTATE, NCS_BUTTON_UNCHECKED);
	
	if(pnode->pbtn)
		_M(pnode->pbtn, setProperty, NCSP_BUTTON_CHECKSTATE, NCS_BUTTON_CHECKED);

	return TRUE;
}

/* VW: not used.
static BOOL check_button(mButtonGroup* self, mWidget* btn)
{
	mWidget* pclear = NULL;
	mButtonNode* pnode =  self->pBtnList;

	if(!btn)
		return FALSE;

	while(pnode)
	{
		mWidget* pbtn = pnode->pbtn;
		if(pbtn == btn) {
			if(pclear)
				break;
		} else if(!pclear && _M(pbtn, getProperty, NCSP_BUTTON_CHECKSTATE) == NCS_BUTTON_CHECKED) {
			pclear = pbtn;
		}
		pnode = pnode->next;
	}

	if(!pnode)
		return FALSE;

	_M(btn, setProperty, NCSP_BUTTON_CHECKSTATE, NCS_BUTTON_CHECKED);
	
	if(pclear){
		_M(pclear, setProperty, NCSP_BUTTON_CHECKSTATE, NCS_BUTTON_UNCHECKED);
	}
	
	return TRUE;
}
*/

static DWORD mButtonGroup_getProperty(mButtonGroup* self, int id)
{
	mWidget* psel;
	if(id >= NCSP_BTNGRP_MAX)
		return (DWORD)-1;

	switch(id)
	{
		case NCSP_BTNGRP_SELID:
			psel = get_selected_button(self, NULL);
			if(psel)
				return _M(psel, getId);
			return FALSE;
		case NCSP_BTNGRP_SELIDX:
			{
				int idx;
				psel = get_selected_button(self, &idx);
				return (DWORD)idx;
			}
		case NCSP_BTNGRP_SELOBJ:
			return (DWORD)get_selected_button(self, NULL);
	}

	return Class(mGroupBox).getProperty((mGroupBox*)self, id);
}

static BOOL mButtonGroup_setProperty(mButtonGroup* self, int id, DWORD value)
{
	if(id >= NCSP_BTNGRP_MAX)
		return FALSE;

	switch(id)
	{
		case NCSP_BTNGRP_SELID:
			return check_button_by_id(self, (int)value);
		case NCSP_BTNGRP_SELIDX:
			return check_button_by_idx(self, (int)value);
		case NCSP_BTNGRP_SELOBJ:
			return _M(self, checkBtn, (mWidget*)value);
	}

	return Class(mGroupBox).setProperty((mGroupBox*)self, id, value);
}

static void mButtonGroup_destroy(mButtonGroup *self)
{	
	mButtonNode* pnode = NULL;
	
	while (NULL != (pnode = self->pBtnList))
	{
		self->pBtnList = pnode->next;
		free(pnode);
	}
}

BEGIN_CMPT_CLASS(mButtonGroup, mGroupBox)
	CLASS_METHOD_MAP(mButtonGroup, addButton)
	CLASS_METHOD_MAP(mButtonGroup, checkBtn)
	CLASS_METHOD_MAP(mButtonGroup, setProperty)
	CLASS_METHOD_MAP(mButtonGroup, getProperty)
	CLASS_METHOD_MAP(mButtonGroup, destroy)
END_CMPT_CLASS
