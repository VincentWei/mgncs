#ifndef HASHTABLE_H
#define HASHTABLE_H

//typedef unsigned int ht_key_t;

#define INVALID_KEY 0u

#define HTE_OK 0
#define HTE_EXIST 1
#define HTE_NOTEXIST 2
#define HTE_INVALID_KEY 3

typedef unsigned long ht_key_t;

typedef struct _HashEntry{
	ht_key_t key;
	void *data;
	struct _HashEntry *next;
}HashEntry;

typedef void (*freeObj)(void* obj);
typedef struct _HashTable{
	int count;
	HashEntry ** entries;
    freeObj      free_obj;
}HashTable;

MGNCS_EXPORT int hash_insert(HashTable* ht,ht_key_t key, void *data);

MGNCS_EXPORT int hash_delete(HashTable* ht,ht_key_t key);

MGNCS_EXPORT void * hash_get(HashTable* ht,ht_key_t key);

MGNCS_EXPORT HashTable * hash_new(int count, freeObj func);

MGNCS_EXPORT void hash_free(HashTable *ht);

#define HT_FOR_EACH_CONTINUE 0 
#define HT_FOR_EACH_DELETE   0x1
#define HT_FOR_EACH_BREAK    0x02
typedef int(*eachObj)(void*, void*);
MGNCS_EXPORT void hash_for_each(HashTable *ht, eachObj each, void* user);

#endif

