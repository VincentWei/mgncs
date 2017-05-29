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
#include "mpopmenumgr.h"
#include "mem-slab.h"

#define SLAB_ALLOC(size)  _SLAB_ALLOC(size)
#define SLAB_FREE(p)      _SLAB_FREE(p)

//serialize save the bmps
#define PMIF_STRING      0x01
#define PMIF_BITMAP      0x02
#define PMIF_STATE       0x04
#define PMIF_CHECKBMP    0x08
#define PMIF_UNCHECKBMP  0x10
#define PMIF_ID          0x20
#define PMIF_SUBMENU     0x40
#define PMIF_USERDATA    0x80

#define PMIT_ITEM  0x0
#define PMIT_CHECK 0x02
#define PMIT_RADIO 0x03
#define PMIT_SEPARATOR 0x04
struct mPopMenuItem{
	unsigned int type:16;
	unsigned int flags:16;
	mPopMenuItem * next;
};

static BOOL popmenuitem_to_menuitem(mPopMenuItem * pmi, MENUITEMINFO *pmii)
{
	UINT *data;
	if(!pmi || !pmii)
		return FALSE;
	memset(pmii, 0, sizeof(MENUITEMINFO));

	pmii->mask = MIIM_TYPE;

	switch(pmi->type){
	case PMIT_RADIO:
		pmii->type = MFT_RADIOCHECK;
	case PMIT_CHECK:
		pmii->type = MFT_MARKCHECK;
		break;
	case PMIT_SEPARATOR:
		pmii->type = MFT_SEPARATOR;
	}

	data = (UINT*)(((char*)pmi)+sizeof(mPopMenuItem));

	if(pmi->flags & PMIF_STRING)
	{
		pmii->typedata = *data;
		data ++;
	}

	if(pmii->type == MFT_STRING && pmi->flags & PMIF_BITMAP)
	{
		if(pmi->flags & PMIF_STRING){
			pmii->type = MFT_BMPSTRING;
			pmii->uncheckedbmp = (PBITMAP)(*data);
		}
		else{
			pmii->type = MFT_BITMAP;
			pmii->typedata = *data;
		}
		data ++;
	}

	if(pmi->flags & PMIF_STATE)
	{
		pmii->mask |= MIIM_STATE;
		pmii->state = *data;
		data ++;
	}

	if(pmi->flags & PMIF_CHECKBMP){
		pmii->mask |= MIIM_CHECKMARKS;
		pmii->checkedbmp = (PBITMAP)*data;
		data ++;
	}

	if((pmii->type == MFT_RADIOCHECK || pmii->type == MFT_MARKCHECK)
			 && pmi->flags & PMIF_UNCHECKBMP)
	{
		pmii->mask |= MIIM_CHECKMARKS;
		pmii->uncheckedbmp = (PBITMAP)*data;
		data ++;
	}

	if(pmi->flags & PMIF_ID)
	{
		pmii->mask |= MIIM_ID;
		pmii->id = *data;
		data ++;
	}
	if(pmi->flags & PMIF_SUBMENU)
	{
		mPopMenuMgr * sub = (mPopMenuMgr*)(*data);
		if(sub){
			pmii->mask |= MIIM_SUBMENU;
			pmii->hsubmenu = _c(sub)->createMenu(sub);
		}
		data ++;
	}

	if(pmi->flags & PMIF_USERDATA)
	{
		pmii->mask |= MIIM_DATA;
		pmii->itemdata = *data;
	}

	return TRUE;
}

static void mPopMenuMgr_construct(mPopMenuMgr *self, DWORD add_data)
{
	Class(mObject).construct((mObject*)self, add_data);

	self->head = NULL;
}

static void mPopMenuMgr_destroy(mPopMenuMgr *self)
{

	while(self->head)
	{
		mPopMenuItem * tmp = self->head;
		self->head = self->head->next;
		SLAB_FREE(tmp);
	}

	Class(mObject).destroy((mObject *)self);
}

BOOL mPopMenuMgr_addItem(mPopMenuMgr *self,
	int type, const char* str, PBITMAP bmp,
	int id, int state, mPopMenuMgr *subMenu,
	DWORD add_data)
{
	mPopMenuItem item, *pitem, *ptemp;
	UINT datas[8];
	int count;

	memset(&item, 0, sizeof(item));

	switch(type){
	case MFT_STRING:
	case MFT_BITMAP:
	case MFT_BMPSTRING:
		item.type = PMIT_ITEM;
		break;
	case MFT_RADIOCHECK:
		item.type = PMIT_RADIO;
		break;
	case MFT_MARKCHECK:
		item.type = PMIT_CHECK;
		break;
	case MFT_SEPARATOR:
		item.type = PMIT_SEPARATOR;
		break;
	}

	count = 0;
	item.flags = 0;

	if(str){
		item.flags|=PMIF_STRING;
		datas[count++] = (UINT)strdup(str);
	}

	if(bmp){
		item.flags |= PMIF_BITMAP;
		datas[count++] = (UINT)bmp;
	}

	if(state != 0){
		item.flags |= PMIF_STATE;
		datas[count++] = (UINT)state;
	}

	if(id>=0){
		item.flags |= PMIF_ID;
		datas[count++] = (UINT)id;
	}

	if(subMenu){
		item.flags |= PMIF_SUBMENU;
		datas[count++] = (UINT)subMenu;
	}

	if(add_data != 0){
		item.flags |= PMIF_USERDATA;
		datas[count++] = (UINT)add_data;
	}

	//alloc mem
	pitem = (mPopMenuItem*)SLAB_ALLOC(sizeof(mPopMenuItem)+count*sizeof(UINT));
	if(!pitem)
		return FALSE;

	//memcpy(pitem, &item, sizeof(mPopMenuItem));
	pitem->type = item.type;
	pitem->flags = item.flags;
	pitem->next = NULL;
	if(count > 0)
		memcpy(((char*)pitem)+sizeof(mPopMenuItem), datas, sizeof(UINT)*count);

	ptemp = self->head;
	if(ptemp == NULL){
		self->head = pitem;
	}
	else
	{
		while(ptemp->next)
			ptemp = ptemp->next;
		ptemp->next = pitem;
	}

	return TRUE;
}

static void mPopMenuMgr_addSeparator(mPopMenuMgr *self)
{
	_c(self)->addItem(self, MFT_SEPARATOR, NULL, NULL, 0, 0 , NULL, 0);
}

static HMENU mPopMenuMgr_createMenu(mPopMenuMgr * self)
{
	mPopMenuItem * mitem = self->head;
	HMENU hMenu;
	int i=0;
	MENUITEMINFO mii;
	if(!self->head)
		return (HMENU)0;

	memset(&mii, 0, sizeof(mii));
	mii.mask = MIIM_STRING;
	mii.typedata = (DWORD)"pop";
	hMenu = CreatePopupMenu(&mii);
	while(mitem)
	{
		popmenuitem_to_menuitem(mitem, &mii);
		InsertMenuItem(hMenu,i, MF_BYPOSITION, &mii);
		mitem = mitem->next;
		i ++;
	}

	return StripPopupHead(hMenu);
}

static void mPopMenuMgr_popMenu(mPopMenuMgr *self, mObject *owner)
{
	HMENU hMenu = _c(self)->createMenu(self);
	if(hMenu)
	{
		POINT pt;
		GetCursorPos(&pt);
		TrackPopupMenu(hMenu,0,pt.x, pt.y, ((mWidget*)owner)->hwnd);
	}
}

static int get_popmenuitem_id(mPopMenuItem *item)
{
	UINT* data = (UINT*)(((char*)item)+sizeof(mPopMenuItem));
	if(item->flags & PMIF_STRING)
		data ++;
	if(item->flags & PMIF_BITMAP)
		data ++;
	if(item->flags & PMIF_STATE)
		data ++;
	if(item->flags & PMIF_CHECKBMP)
		data ++;
	if(item->flags & PMIF_UNCHECKBMP)
		data ++;
	return (int)data[0];
}

static BOOL mPopMenuMgr_getMenuItem(mPopMenuMgr* self, int idx, MENUITEMINFO *pmii, BOOL byCommand)
{
	mPopMenuItem * item;
    if (!pmii)
        return FALSE;

	item = self->head;
	if(byCommand)
		for(; item && get_popmenuitem_id(item)!=idx; item = item->next);
	else{
		int i;
		for(i=0; item && i!= idx; item = item->next, i++);
	}

	if(!item)
		return FALSE;
	
    return popmenuitem_to_menuitem(item, pmii);
}

BEGIN_MINI_CLASS(mPopMenuMgr, mObject)
	CLASS_METHOD_MAP(mPopMenuMgr, construct)
	CLASS_METHOD_MAP(mPopMenuMgr, destroy  )
	CLASS_METHOD_MAP(mPopMenuMgr, addItem  )
	CLASS_METHOD_MAP(mPopMenuMgr, addSeparator)
	CLASS_METHOD_MAP(mPopMenuMgr, createMenu)
	CLASS_METHOD_MAP(mPopMenuMgr, popMenu)
	CLASS_METHOD_MAP(mPopMenuMgr, getMenuItem)
END_MINI_CLASS
