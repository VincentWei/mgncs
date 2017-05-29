#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncsconfig.h"
#include "mcommon.h"
#include "mobject.h"
#include "mcomponent.h"
#include "mwidget.h"
#include "mstatic.h"
#include "mbutton.h"
#include "mgroupbox.h"
#include "mbuttongroup.h"
#include "mrdr.h"
#include "piece.h"

#define DEBUG() fprintf(stderr, "line: %d, func: %s\n", __LINE__, __func__)

static mWidget* get_selected_button(mButtonGroup* self, int *pidx)
{
	mButtonNode* pnode = self->pBtnList;
	int i = 0;

	while(pnode && pnode->pbtn)
	{
		if(_c(pnode->pbtn)->getProperty(pnode->pbtn, NCSP_BUTTON_CHECKSTATE) == NCS_BUTTON_CHECKED)
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
		if(pnode->pbtn == (mWidget*)btn)
		{
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
		if (temp->pbtn == (mWidget*)btn)
		{
			return TRUE;
		}
		temp = temp->next;
	}

	if(btn && btn->group)
		mButtonGroup_removeButton((mButtonGroup*)(btn->group), btn);	


	if(_c(btn)->getProperty(btn, NCSP_BUTTON_CHECKSTATE) == NCS_BUTTON_CHECKED)
	{
		if(!get_selected_button(self, NULL))
			_c(self)->checkBtn(self, (mWidget*)btn);
		else
			_c(btn)->setProperty(btn, NCSP_BUTTON_CHECKSTATE, NCS_BUTTON_UNCHECKED);	
	}


	temp = malloc(sizeof(mButtonNode));
	temp->pbtn = (mWidget*)btn;
	temp->next = NULL;
	if (pre != NULL)
	{
		pre->next = temp;
	}
	else
	{
		self->pBtnList = temp;
	}
	btn->group = (mWidget*)self;

	return TRUE;
}

BOOL mButtonGroup_checkBtn(mButtonGroup *self, mButton *button)
{
	BOOL bchanged = FALSE;
	mButtonNode *temp = self->pBtnList;
	if(button)
		_c(button)->setProperty(button, NCSP_BUTTON_CHECKSTATE, NCS_BUTTON_CHECKED);

	while(temp != NULL)
	{
		if(((mButton*)(temp->pbtn)) != button)
		{
			if(_c(temp->pbtn)->getProperty(temp->pbtn, NCSP_BUTTON_CHECKSTATE) == NCS_BUTTON_CHECKED)
			{
				_c(temp->pbtn)->setProperty(temp->pbtn, NCSP_BUTTON_CHECKSTATE, NCS_BUTTON_UNCHECKED);
				bchanged = TRUE;
				break;
			}
		}
		temp = temp->next;
	}

	if(bchanged)
		ncsNotifyParentEx((mWidget*)self, NCSN_BTNGRP_SELCHANGED, (DWORD)(_c(button)->getId(button)));

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
		if(_c(pbtn)->getId(pbtn) == id)
		{
			bSet = TRUE;
			_c(pbtn)->setProperty(pbtn, NCSP_BUTTON_CHECKSTATE, NCS_BUTTON_CHECKED);
		}
		else if(!pclear && _c(pbtn)->getProperty(pbtn, NCSP_BUTTON_CHECKSTATE) == NCS_BUTTON_CHECKED)
		{
			pclear = pbtn;
		}
		pnode = pnode->next;
	}

	if(!bSet)
		return FALSE;
	
	if(pclear){
		_c(pclear)->setProperty(pclear, NCSP_BUTTON_CHECKSTATE, NCS_BUTTON_UNCHECKED);
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
		if(!pclear && _c(pnode->pbtn)->getProperty(pnode->pbtn, NCSP_BUTTON_CHECKSTATE) == NCS_BUTTON_CHECKED)
			pclear = pnode->pbtn;
	}

	if(!pnode)
		return FALSE;
	
	if(pclear == pnode->pbtn)
		return TRUE;

	if(pclear)
		_c(pclear)->setProperty(pclear, NCSP_BUTTON_CHECKSTATE, NCS_BUTTON_UNCHECKED);
	
	if(pnode->pbtn)
		_c(pnode->pbtn)->setProperty(pnode->pbtn, NCSP_BUTTON_CHECKSTATE, NCS_BUTTON_CHECKED);

	return TRUE;
}

static DWORD mButtonGroup_getProperty(mButtonGroup* self, int id)
{
	mWidget* psel;
	if(id >= NCSP_BTNGRP_MAX)
		return (DWORD)-1;

	switch(id)
	{
	case NCSP_BTNGRP_SELID:
		psel = get_selected_button(self,NULL);
		if(psel)
			return _c(psel)->getId(psel);
		return FALSE;
	case NCSP_BTNGRP_SELIDX:
		{
			int idx;
			psel = get_selected_button(self,&idx);
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
		return _c(self)->checkBtn(self,(mWidget*)value);
	}

	return Class(mGroupBox).setProperty((mGroupBox*)self, id, value);
}

BEGIN_CMPT_CLASS(mButtonGroup, mGroupBox)
	CLASS_METHOD_MAP(mButtonGroup, addButton)
	CLASS_METHOD_MAP(mButtonGroup, checkBtn)
	CLASS_METHOD_MAP(mButtonGroup, setProperty)
	CLASS_METHOD_MAP(mButtonGroup, getProperty)
END_CMPT_CLASS
