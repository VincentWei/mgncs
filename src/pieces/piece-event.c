///////////////////////////////////////////////////////////////////////////////
//
//                          IMPORTANT NOTICE
//
// The following open source license statement does not apply to any
// entity in the Exception List published by FMSoft.
//
// For more information, please visit:
//
// https://www.fmsoft.cn/exception-list
//
//////////////////////////////////////////////////////////////////////////////
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

#include "mem-slab.h"
#include "hashtable.h"

#if 1
#define CALLOC(size, count)    slab_new_mem(NULL, (size)*(count), 0)
#define FREE(p)                slab_free_mem(NULL, p)
#else
#define CALLOC(size, count)    calloc(size, count)
#define FREE(p)                free(p)
#endif

#define MAX_EVENT_SOURCE  23

#ifdef _MGRM_THREADS
static pthread_mutex_t _event_locker;
#define EVENT_LOCK()  (pthread_mutex_lock(&_event_locker))
#define EVENT_UNLOCK() (pthread_mutex_unlock(&_event_locker))
#define RET_UNLOCK(ret) do{EVENT_UNLOCK(); return (ret); } while(0)
#define RETVOID_UNLOCK() do{EVENT_UNLOCK(); return ; } while(0)
#else
#define EVENT_LOCK()
#define EVENT_UNLOCK()
#define RET_UNLOCK(ret)  return (ret)
#define RETVOID_UNLOCK()  return
#endif

static int _event_raising = 0;
#define EVENT_DEL_FLAGS  0x1
typedef struct EventHandlerEntry {
	mObject *listener;
	unsigned int      flags;
	NCS_CB_ONPIECEEVENT event_handler;
	struct EventHandlerEntry *next;
}EventHandlerEntry;

typedef struct EventHandlerList {
	int event_id;
	EventHandlerEntry * header;
	struct EventHandlerList * next;
}EventHandlerList;

typedef struct EventNode {
	mObject *source;
	EventHandlerList * lists;
	int handle_to_del; //used when to delete node
}EventNode;

/////////////////////////
static HashTable * ht_event_source = NULL;

void init_piece_event_info(void)
{
#ifdef _MGRM_THREADS
    pthread_mutex_init(&_event_locker, NULL);
#endif
    ht_event_source = NULL;
    _event_raising = 0;
}

void deinit_piece_event_info(void)
{
#ifdef _MGRM_THREADS
    pthread_mutex_destroy(&_event_locker);
#endif
    hash_free(ht_event_source);
    ht_event_source = NULL;
}

static void free_event_handler_list(EventHandlerList *list)
{
	EventHandlerEntry *entry;
	if(!list)
		return;

	entry = list->header;
	while(entry)
	{
		EventHandlerEntry * p = entry;
		entry = entry->next;
		FREE(p);
	}
	FREE(list);
}

static void free_event_node(EventNode* node)
{
	EventHandlerList *list;
	if(!node)
		return;

	list = node->lists;
	while(list)
	{
		EventHandlerList * tl = list;
		list = list->next;
		free_event_handler_list(tl);
	}
    FREE(node);
}

static EventNode * get_event_node(mObject *source, BOOL bnew)
{
	EventNode* node;
	ht_key_t key;
	if(!source)
		return NULL;

	if(ht_event_source == NULL)
	{
		ht_event_source = hash_new(MAX_EVENT_SOURCE, (freeObj)free_event_node);
	}

	if(!ht_event_source)
		return NULL;

	key = (ht_key_t)/*_c(source)->hash(source)*/source;
	

	node = (EventNode*)hash_get(ht_event_source, key);

	if(!node && bnew)
	{
		node = (EventNode*)CALLOC(sizeof(EventNode),1);
		if(!node)
			return NULL;

		//printf("node(%p)={%p,%p}\n",node,node->source, node->lists);
		node->source = source;
		//insert into hash table
		hash_insert(ht_event_source, key, (void*)node);
	}

	return node;
}

static void mark_node_delete(EventNode* node)
{
	EventHandlerList* list;
	EventHandlerEntry* entry;
	if(!node)
		return;

	list = node->lists;
	while(list)
	{
		entry = list->header;
		while(entry)
		{
			entry->flags |= EVENT_DEL_FLAGS;
			node->handle_to_del ++;
			entry = entry->next;
		}
		list = list->next;
	}
}

static int each_marked_node(void* user, EventNode* node)
{
	EventHandlerList * list , *prev_list;
	if(!node || node->handle_to_del <= 0)
		return 0;

	prev_list = NULL;
	list = node->lists;
	while(list){
		EventHandlerEntry * entry = list->header;
		EventHandlerEntry * entry_prev = NULL;
		while(entry)
		{
			if(entry->flags & EVENT_DEL_FLAGS)
			{
				//delete it
				if(entry_prev)
					entry_prev->next = entry->next;
				else
					list->header = entry->next;

				FREE(entry);
				node->handle_to_del --;
				if(!entry_prev)
					entry = list->header;
				else
					entry = entry_prev->next;
				continue;
			}
			entry_prev = entry;
			entry = entry->next;
		}

		if(list->header == NULL) //delete the list
		{
			if(!prev_list)
				node->lists = list->next;
			else
				prev_list->next = list->next;

			FREE(list);
			if(!prev_list)
				list = node->lists;
			else
				list = prev_list->next;
			continue;
		}
		prev_list = list;
		list = list->next;
	}

	return node->lists?0:HT_FOR_EACH_DELETE;
}

static void process_all_mark_del_nodes(void)
{
	hash_for_each(ht_event_source, (eachObj)each_marked_node, NULL);
}

BOOL ncsAddEventListener(mObject *event_source,
	mObject* listener,
	NCS_CB_ONPIECEEVENT event,
	int event_id)
{
	int events[2] = {event_id, 0};
	return ncsAddEventListeners(event_source, listener, event, events);
}

static BOOL insert_on_entry(EventNode* node, int event_id, mObject *listener, NCS_CB_ONPIECEEVENT event)
{
	EventHandlerList * list;
	EventHandlerEntry * entry;

	//find list
	list = node->lists;
	while(list && list->event_id != event_id)
	{
		list = list->next;
	}

	if(list == NULL)
	{
		list = (EventHandlerList*)CALLOC(sizeof(EventHandlerList),1);
		if(!list)
			return FALSE;

		list->event_id = event_id;
		//insert into the node
		list->next = node->lists;
		node->lists = list;
	}

	//find the entry
	entry = list->header;
	while(entry)
	{
		if(entry->listener == listener && entry->event_handler == event)
		{
			entry->flags &= ~EVENT_DEL_FLAGS;
			return TRUE;
		}
		entry = entry->next;
	}

	//new
	entry = (EventHandlerEntry*)CALLOC(sizeof(EventHandlerEntry),1);
	if(!entry)
		return FALSE;
	entry->listener = listener;
	entry->event_handler = event;
	entry->next = list->header;
	list->header = entry;
	return TRUE;
}

BOOL ncsAddEventListeners(mObject *event_source,
	mObject* listener,
	NCS_CB_ONPIECEEVENT event,
	int* event_ids)
{
	EventNode * node;
	if(!event_source || !listener || !event || !event_ids)
		return FALSE;

	EVENT_LOCK();

	node = get_event_node((mObject*)event_source, TRUE);

	if(!node)
		RET_UNLOCK(FALSE);

	while(event_ids[0])
	{
		insert_on_entry(node, event_ids[0], listener, event);
		event_ids ++;
	}
	RET_UNLOCK(TRUE);
}

static inline EventHandlerList* get_event_handler_list(mObject *sender, int event_id)
{
	EventHandlerList * list;
	EventNode *node = get_event_node(sender, FALSE);
	if(!node)
		return NULL;

	for(list = node->lists;
		list && list->event_id != event_id;
		list = list->next);

	return list;

}

void ncsRaiseEvent(mObject *sender, int event_id, DWORD param)
{
	EventHandlerList * list;
	EventHandlerEntry * entry;
	if(!sender)
		return ;

	EVENT_LOCK();
    _M(sender, addRef);
	list = get_event_handler_list(sender, event_id);

	if(!list)
	{
        _M(sender, release);
		EVENT_UNLOCK();
		return;
	}

	entry = list->header;

	_event_raising ++;

	while(entry)
	{
		NCS_CB_ONPIECEEVENT event_handler = 
			entry->flags & EVENT_DEL_FLAGS ? NULL : entry->event_handler;

		if(event_handler)
		{
			mObject *listener = entry->listener;
			_M(listener, addRef);

			EVENT_UNLOCK();
			if(!(event_handler(listener, sender, event_id, param))) 
			{
                _M(listener, release);
				EVENT_LOCK();
				break;
			}

            _M(listener, release);
			EVENT_LOCK();
		}
		entry = entry->next;
	}
    _M(sender, release);

	_event_raising --;

	if(_event_raising <= 0)
	{
		_event_raising = 0;
		process_all_mark_del_nodes();
	}
	EVENT_UNLOCK();
}

static int each_event_node(void *user, EventNode * node)
{
	EventHandlerList * list , *prev_list;
	if(!node)
		return 0;

	prev_list = NULL;
	list = node->lists;
	if(_event_raising <= 0)
	{
		while(list){
			EventHandlerEntry * entry = list->header;
			EventHandlerEntry * entry_prev = NULL;
			while(entry)
			{
				if(entry->listener == (mObject*)user)
				{
					//delete it
					if(entry_prev)
						entry_prev->next = entry->next;
					else
						list->header = entry->next;

					FREE(entry);
					if(!entry_prev)
						entry = list->header;
					else
						entry = entry_prev->next;
					continue;
				}
				entry_prev = entry;
				entry = entry->next;
			}

			if(list->header == NULL) //delete the list
			{
				if(!prev_list)
					node->lists = list->next;
				else
					prev_list->next = list->next;

				FREE(list);
				if(!prev_list)
					list = node->lists;
				else
					list = prev_list->next;
				continue;
			}
			prev_list = list;
			list = list->next;
		}

		return node->lists?0:HT_FOR_EACH_DELETE;
	}
	else
	{
		//just mark
		while(list){
			EventHandlerEntry * entry = list->header;
			while(entry)
			{
				if(entry->listener == (mObject*)user)
				{
					entry->flags |= EVENT_DEL_FLAGS;
					node->handle_to_del ++;
				}
				entry = entry->next;
			}
			list = list->next;
		}
		return 0;
	}
}

BOOL ncsRemoveEventListener(mObject * listener)
{
	if(!listener || ht_event_source == NULL)
		return FALSE;
	
	EVENT_LOCK();

	hash_for_each(ht_event_source, (eachObj)each_event_node, (void*)listener);
	RET_UNLOCK(TRUE);
}

BOOL ncsRemoveEventSource(mObject *source)
{
	if(!source)
	{
		return FALSE;
	}

	EVENT_LOCK();

	if(_event_raising > 0)
	{
		//do mark
		EventNode * node = get_event_node((mObject*)source, FALSE);
		if(node)
			mark_node_delete(node);
	}
	else
	{
		hash_delete(ht_event_source, /*_c(source)->hash(source)*/(ht_key_t)source);
	}
	
	RET_UNLOCK(TRUE);
}

BOOL ncsRemoveEventConnect(mObject *listener, mObject* source, int event)
{
	EventHandlerList * list, *prevl = NULL;
	EventHandlerEntry * entry, *preve = NULL;
	EventNode* node;

	if(!listener || !source)
		return FALSE;

	EVENT_LOCK();

	node = get_event_node(source, FALSE);
	if(!node)
		RET_UNLOCK(FALSE);

	list = node->lists;

	while(list)
	{
		if(event <= 0 || list->event_id == event)
		{
			entry = list->header; 
			if(_event_raising <= 0)
			{
				preve = NULL;
				while(entry)
				{
					if(entry->listener == listener)
					{
						//delete
						EventHandlerEntry* p = entry;
						entry = entry->next;

						if(list->header == p)
							list->header = entry;
						else
							preve->next = entry;

						FREE(p);
						continue;
					}

					preve = entry;
					entry = entry->next;
				}

				if(list->header == NULL) //delete it
				{
					EventHandlerList* p = list;
					list = list->next;
					if( p == node->lists)
						node->lists = list;
					else
						prevl->next = list;
					FREE(p);
					continue;
				}

				prevl = list;
			}
			else
			{
				while(entry)
				{
					if(entry->listener == listener)
					{
						entry->flags |= EVENT_DEL_FLAGS;
						node->handle_to_del ++;
					}
					entry = entry->next;
				}
			}

		}
		list = list->next;
	}

	if(_event_raising <= 0 && node->lists == NULL) //delete all
	{
		hash_delete(ht_event_source, /*_c(source)->hash(source)*/(ht_key_t)source);
	}

	RET_UNLOCK(TRUE);
}


