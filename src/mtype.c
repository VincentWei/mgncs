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

static void int_to_str(int src, NCS_SET_VALUE set_value, void* self){
	char buff[32];
	sprintf(buff,"%d",src);
	set_value(self, (DWORD)buff);
}

static void int_to_char(int src, NCS_SET_VALUE set_value, void* self){
	set_value(self, (DWORD)((char)src));
}

static void str_to_int(char* src,NCS_SET_VALUE set_value, void *self){
	set_value(self,(DWORD)(strtol(src, NULL, 0)));
}

static void str_to_char(char* src, NCS_SET_VALUE set_value, void* self){
	set_value(self,src?*src:0);
}

static void char_to_int(char ch, NCS_SET_VALUE set_value, void* self){
	set_value(self, (DWORD)ch);
}

static void char_to_str(char ch, NCS_SET_VALUE set_value, void* self){
	char sz[2] = {ch, 0};
	set_value(self,(DWORD)sz);
}

static void * type_trans_table[][NCS_BT_MAX] = {
	{ NULL, int_to_str, int_to_char},
	{ str_to_int, NULL, str_to_char},
	{ char_to_int, char_to_str, NULL}
};

typedef void (*type_trans_t)(DWORD v, NCS_SET_VALUE, void*);

int ncsTransType(DWORD value, int src_type, int dest_type, NCS_SET_VALUE set_value, void *self)
{
	type_trans_t type_trans;
	if(src_type == dest_type){
		set_value(self, value);
		return SBPE_OK;
	}

	if(src_type < 0 || src_type >= NCS_BT_MAX
		|| dest_type < 0 || dest_type >= NCS_BT_MAX)
		return SBPE_MISSMATCH;

	type_trans = (type_trans_t)(type_trans_table[src_type-1][dest_type-1]);

	if(!type_trans)
		return SBPE_MISSMATCH;

	(*type_trans)(value, set_value, self);

	return SBPE_OK;
}

////////////////////////////////////////////
//

static void str_set_value(const char** dest, const char* str)
{
	*dest = str?strdup(str):NULL;
}

char* ncsTypeToString(DWORD value, int src_type)
{
	char* str;
	if(SBPE_OK == ncsTransType(value, src_type, 
                NCS_BT_STR, (NCS_SET_VALUE)str_set_value, &str))
		return str;
	return NULL;
}

/////////////////////////////////////////
//

#ifndef _MGNCS_AUTOSTR_POOL_SIZE
#define AUTO_STR_POOL_SIZE  1024*4 //on page
#else
#define AUTO_STR_POOL_SIZE _MGNCS_AUTOSTR_POOL_SIZE
#endif


#define MIN_AUTO_STR_POOL_SIZE  256

typedef struct {
	unsigned int size;
	unsigned int free_at;
	char * head;
}AUTOSTRPOOL;

HAUTOPOOL ncsCreateAutoPool(int size)
{
	AUTOSTRPOOL *pool;
	if(size == -1)
		size = AUTO_STR_POOL_SIZE;
	else if(size < MIN_AUTO_STR_POOL_SIZE)
		size = MIN_AUTO_STR_POOL_SIZE;

	pool = (AUTOSTRPOOL*)malloc(sizeof(AUTOSTRPOOL));
	pool->size = size;
	pool->head = malloc(size);
	pool->free_at = 0;

	return (HAUTOPOOL)pool;
}

void ncsResetAutoPool(HAUTOPOOL hPool)
{
	AUTOSTRPOOL* pool = (AUTOSTRPOOL*)hPool;
	if(!pool)
		return ;

	pool->free_at = 0;
}

void ncsFreeAutoPool(HAUTOPOOL hPool)
{
	if(hPool){
		AUTOSTRPOOL * pool = (AUTOSTRPOOL*)hPool;
		if(pool->head)
			free(pool->head);
		free(pool);
	}
}

static void * alloc_from_pool(HAUTOPOOL hPool, int size)
{
	AUTOSTRPOOL* pool = (AUTOSTRPOOL*)hPool;
	//char *str_end;
	void *p;
	if(!pool)
		return NULL;

	if(size < 0)
		size = 0;

	if((pool->free_at + size) > pool->size)
	{
		//alloc
		int allocsize = (size/MIN_AUTO_STR_POOL_SIZE+1)*MIN_AUTO_STR_POOL_SIZE;
		allocsize += pool->size;
		pool->head =  (char*)realloc(pool->head, allocsize);
		if(!pool->head)
			return NULL;

		pool->size = allocsize;
	}

	p = (void*)(pool->head + pool->free_at);
	pool->free_at += size;

	return p;
}

struct AUTOSTRINFO{
	HAUTOPOOL hPool;
	char * str;
};

static void set_auto_str(struct AUTOSTRINFO *ainfo,const char* str)
{
	int len ;
	if(!str)
		return;
	len = strlen(str);
	ainfo->str = alloc_from_pool(ainfo->hPool, len+1);
	if(!ainfo->str)
		return;
	strcpy(ainfo->str, str);
}

char* ncsTypeToAutoString(DWORD value, int src_type, HAUTOPOOL hAutoPool)
{
	struct AUTOSTRINFO auto_str_info={
		hAutoPool,
		NULL
	};

	if(src_type == NCS_BT_STR)
		return (char*)value;

	if(SBPE_OK == ncsTransType(value, src_type, 
                NCS_BT_STR, (NCS_SET_VALUE)set_auto_str, &auto_str_info))
		return auto_str_info.str;

	return NULL;
}

/////////////////////////////////////////
//
void _common_set_value(DWORD* self, DWORD dst_v)
{
	*self = dst_v;
}

int ncsTypeToInt(DWORD value, int src_type)
{
	int v = 0;
	if(SBPE_OK == ncsTransType(value, src_type, 
                NCS_BT_INT, (NCS_SET_VALUE)_common_set_value, (void*)&v))
		return v;
	return 0;
}

int ncsTypeToChar(DWORD value, int src_type)
{
	char v = 0;
	if(SBPE_OK == ncsTransType(value, src_type, 
                NCS_BT_INT, (NCS_SET_VALUE)_common_set_value, (void*)&v))
		return v;
	return 0;
}

