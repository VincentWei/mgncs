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
#include "hashtable.h"

#define MIN_COUNT  5

#define Key2Idx(ht, key)  (key)%((ht)->count)

#if 1
#define CALLOC(size, count)    slab_new_mem(NULL, (size)*(count), 0)
#define FREE(p)                slab_free_mem(NULL, p)
#else
#define CALLOC(size, count)    calloc(size, count)
#define FREE(p)                free(p)
#endif

HashTable * hash_new(int count, freeObj func)
{
	HashTable * ht;

	if(count < MIN_COUNT)
		count = MIN_COUNT;
	
	ht = (HashTable*)calloc(sizeof(HashTable) + sizeof(HashEntry*)*count, 1);

	assert(ht);

	ht->entries = (HashEntry**)((unsigned char*)(ht) + sizeof(HashTable));

	ht->count = count;

	ht->free_obj = func;

	//printf("---ht-new=%p\n", ht);
	return ht;
}

void hash_free(HashTable * ht)
{
	int i;
	if(ht == NULL)
		return ;

	for(i=0; i<ht->count; i++)
	{
		HashEntry * entry = ht->entries[i];
		while(entry)
		{
			HashEntry * tmp = entry;
			if(ht->free_obj)
				ht->free_obj(entry->data);
			entry = entry->next;
			FREE(tmp);
		}
	}

	free(ht);
}

int hash_insert(HashTable* ht, ht_key_t key, void *data)
{
	HashEntry *entry = NULL;

	int idx;

	if(ht == NULL)
		return -1;

	if(key == INVALID_KEY)
		return  HTE_INVALID_KEY;
	
	idx = Key2Idx(ht, key);
	
	if(idx <0 || idx>=ht->count)
		return HTE_INVALID_KEY;
	
	//printf("---ht=%p, count=%d,entries+%d=%p\n", ht, ht->count ,idx, ht->entries+idx);
	//printf("--entries[%d]=%p\n", idx, ht->entries[idx]);
	entry = ht->entries[idx];


	while(entry && entry->key != key)
		entry = entry->next;

	if(entry) //find, replace
	{
		if(ht->free_obj)
			ht->free_obj(entry->data);
		entry->data = data;
		return HTE_OK;
	}


	entry = (HashEntry*)CALLOC(1, sizeof(HashEntry));
	//printf("insert: idx=%d,key=%d, entry=%p, data=%p\n", idx,key, entry,data);
	entry->data = data;
	entry->key = key;
	entry->next = ht->entries[idx];
	ht->entries[idx] = entry;

	//printf("insert: enries[%d]=%p\n",idx, ht->entries[idx]);

	return HTE_OK;
}

int hash_delete(HashTable *ht, ht_key_t key)
{
	HashEntry * entry, *prev;
	int idx;

	if(ht == NULL)
		return -1;

	if(key == INVALID_KEY)
		return HTE_INVALID_KEY;

	idx = Key2Idx(ht, key);
	if(idx < 0 || idx >= ht->count)
		return HTE_INVALID_KEY;

	entry = prev = ht->entries[idx];

	while(entry && entry->key != key){
		prev = entry;
		entry = entry->next;
	}

	if(entry) //find, delete it
	{
		if(prev == entry){
			ht->entries[idx] = entry->next;
		}
		else{
			prev->next = entry->next;
		}

		if(ht->free_obj)
			ht->free_obj(entry->data);
		FREE(entry);
	}

	return HTE_OK;
}

void * hash_get(HashTable *ht, ht_key_t key)
{
	HashEntry *entry = NULL;

	int idx;

	if(ht == NULL)
		return NULL;

	if(key == INVALID_KEY)
		return  NULL;
	
	idx = Key2Idx(ht, key);
	
	//printf("get: idx=%d, key=%d\n", idx, key);
	if(idx <0 || idx>=ht->count)
		return NULL;
	
	entry = ht->entries[idx];
	
	//printf("get: entries[%d]=%p\n", idx,ht->entries[idx]);

	while(entry && entry->key != key) {
		//printf("find: key:%d = entry->key:%d\n", key, entry->key);
		entry = entry->next;
	}
	
	//printf("get entry=%p, data=%p\n", entry, entry?entry->data:NULL);

	return entry?entry->data:NULL;
}


void hash_for_each(HashTable *ht, eachObj each, void* user)
{
	HashEntry * entry, * prev;
	int i;

	if(ht == NULL || each == NULL)
		return;

	for(i=0; i< ht->count; i++)
	{
		entry = ht->entries[i];
		prev = NULL;
		while(entry)
		{
			int r = (*each)(user, entry->data);
			if(r & HT_FOR_EACH_DELETE)
			{
				if(prev == NULL) //entry is head
					ht->entries[i] = entry->next;
				else
					prev->next = entry->next;
				if(ht->free_obj)
					ht->free_obj(entry->data);
				FREE(entry);

				if(r & HT_FOR_EACH_BREAK)
					return;

				if(!prev)
					entry = ht->entries[i];
				else
					entry = prev->next;
				continue;
			}

			if(r & HT_FOR_EACH_BREAK)
				return;

			prev = entry;
			entry = entry->next;
		}
	}
}
