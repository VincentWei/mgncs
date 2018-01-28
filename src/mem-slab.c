
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "mgncs.h"

#include "mem-slab.h"

#define SLAB_ALLOC(size)   malloc(size)
#define SLAB_FREE(p)       free(p)

typedef struct MemSlabFreeCell {
    struct MemSlabFreeCell * next;
} MemSlabFreeCell;

typedef struct MemSlabNode {
    int base_size;
    int total_size;
    int free_count;
    struct MemSlabNode *next;
    struct MemSlabNode *prev;
    MemSlabFreeCell * free_header;
} MemSlabNode;

typedef struct MemSlabHeap {
    int min_size;
    int max_size;
    int node_count;
    MemSlabNode ** nodes;
} MemSlabHeap;

static void * default_heap = NULL;

#define ALIGN_SIZE  4
#define ALIGNN(size,n)     ((size) + ((~((size)&((n)-1))+1)&((n)-1)))
#define SIZENTOIDX(size, n)   ((size)/(n))
#define ALIGN(size)  ALIGNN(size, ALIGN_SIZE)
#define SIZE2IDX(size) SIZENTOIDX(size, ALIGN_SIZE)

#define NODE_MIN_COUNT  (32)
#define NODE_MIN_SIZE   (1024*2)
#define NODE_MAX_USAGE  40

#define DEBUG_VAR(var,format)  printf("%s:%d: " #var "=" format "\n", __FUNCTION__, __LINE__, var)

static MemSlabNode * slab_new_node(int base_size)
{
    int total_size = NODE_MIN_SIZE;
    MemSlabNode * node;
    MemSlabFreeCell * free_cell, * last_cell;
    if(total_size / base_size < NODE_MIN_COUNT)
        total_size = base_size * NODE_MIN_COUNT;
    else
        total_size = ((total_size/base_size)+1)*base_size;

    node = SLAB_ALLOC( total_size + sizeof (MemSlabNode));

    if(node == NULL)
        return NULL;

    node->base_size = base_size;
    node->total_size = total_size;
    node->free_count = total_size / base_size;
    node->next = NULL;
    node->prev = NULL;
    node->free_header = (MemSlabFreeCell*)(((char*)node) + sizeof(MemSlabNode));

    last_cell = (MemSlabFreeCell*)(((char*)(node->free_header)) + total_size - base_size);
    free_cell = node->free_header;
    while(free_cell != last_cell) {
        MemSlabFreeCell * cell = (MemSlabFreeCell*)(((char*)free_cell) + base_size);
        free_cell->next = cell;
        free_cell = cell;
    }

    last_cell->next = NULL;

    return node;
}

static void * slab_get_mem(MemSlabHeap * heap, int size)
{
    int i;
    MemSlabNode * node, *next;
    void *p;

    if(heap == NULL)
        return NULL;

    if(size > heap->max_size)
        return NULL;

    if(size < heap->min_size)
        size = heap->min_size;

    size = ALIGN(size);
    i = SIZE2IDX(size - heap->min_size);

    if(!heap->nodes[i]) {
        //alloc new
        heap->nodes[i] = slab_new_node(size);
        if(!heap->nodes[i])
            return NULL;
    }

    node = heap->nodes[i];

    if(node->free_header == NULL) {
        //alloc new slab
        node = slab_new_node(size);
        if(node == NULL)
            return NULL;

        //insert into the frist
        node->next = heap->nodes[i];
        heap->nodes[i]->prev = node;
        heap->nodes[i] = node;
    }

    p = (void*)node->free_header;
    node->free_count --;
    node->free_header = node->free_header->next;

    next = node->next;
    //sort, find a one which "node" shoud insert after it
    while(next && node->free_count < next->free_count) {
        if(!next->next)
            break;

        next = next->next;
    }

    if(next) {
        if(node->free_count < next->free_count)
            next = next->prev;

        if(next == node)
            return p;

        //remove node from list
        if(!node->prev)
            heap->nodes[i] = node->next;
        else
            node->prev->next = node->next;
        if(node->next)
            node->next->prev = node->prev;

        //insert node after next
        node->prev = next;
        node->next = next->next;
        if(node->next)
            node->next->prev = node;
        next->next = node;
    }

    return p;
}

static int slab_node_usage(MemSlabNode *node)
{
    return (node->free_count * 100) / (node->total_size/node->base_size);
}

static void slab_release_mem(MemSlabHeap * heap, void *p)
{
    unsigned char *pmem = (unsigned char*)p;
    MemSlabNode * node;
    MemSlabFreeCell *cell, *tempcell;
    MemSlabNode * prev, *next;
    int i;

    if(heap == NULL)
        return ;

    for(i = 0; i<heap->node_count; i++) {
        node = heap->nodes[i];
        while(node) {
            unsigned char* pstart = ((unsigned char*)node) + sizeof(MemSlabNode);
            unsigned char *pend = pstart + node->total_size;

            if(pmem >= pstart && pmem <pend)
                goto FOUND;

            node = node->next;
        }
    }

    //not found
    return ;

FOUND:
	//check double free
	tempcell = node->free_header;
	while(tempcell) {
		if((void*)tempcell == (void*)pmem) {
			fprintf(stderr, "the %p is double freed\n", pmem);
			abort();
			return;
		}
		tempcell = tempcell->next;
	}

    cell = (MemSlabFreeCell*)pmem;

    cell->next = node->free_header;
    node->free_header = cell;
    node->free_count ++;

    //we need free it ?
    if((node->prev != NULL || node->next != NULL) &&  node->free_count == node->total_size/node->base_size) {
        int usage = 0;
        prev = node->prev;
        next = node->next;
        if(prev)
            usage = slab_node_usage(prev);
        else
            usage = slab_node_usage(next);

        if(usage <= NODE_MAX_USAGE) { //we can delete this NULL node
            if(prev)
                prev->next = next;
            if(next)
                next->prev = prev;

            if(heap->nodes[i] == node) {
                heap->nodes[i] = next;
            }

            //delete node
            SLAB_FREE(node);
            return ;
        }
    }

    if(node == heap->nodes[i])
        return ;

    //sort, move up
    prev = node->prev;
    while(prev && prev->free_count < node->free_count) {
        prev = prev->prev;
    }

    if(prev && prev->free_count >= node->free_count)
        prev = prev->next;

    if(prev && prev == node)
        return ;

    //remove node from list
    if(node->prev)
        node->prev->next = node->next;
    if(node->next)
        node->next->prev = node->prev;

    //insert node before  prev
    if(!prev) {
        node->prev = NULL;
        node->next = heap->nodes[i];
        heap->nodes[i]->prev = node;
        heap->nodes[i] = node;
    } else {
        node->prev = prev->prev;
        node->next = prev;
        prev->prev = node;
        if(node->prev)
            node->prev->next = node;
    }

}

void *slab_new_heap(int min_size, int max_size)
{
    MemSlabHeap * heap;
    int node_count;

    if(min_size < 0 || max_size < min_size || (max_size - min_size) < ALIGN_SIZE)
        return NULL;

    min_size = ALIGN(min_size);
    max_size = ALIGN(max_size);
    node_count = SIZE2IDX(max_size-min_size) + 1;
    if(node_count <= 0)
        return NULL;

    heap = (MemSlabHeap*)SLAB_ALLOC(sizeof(MemSlabHeap) + node_count * sizeof(void*));
    if(!heap)
        return NULL;

    heap->min_size = min_size;
    heap->max_size = max_size;
    heap->node_count = node_count;
    heap->nodes = (MemSlabNode**)(((char*)heap)+sizeof(MemSlabHeap));

    memset(heap->nodes, 0, sizeof(MemSlabNode*)*node_count);

    return (void*)heap;
}

void slab_free_heap(void *hp)
{
    MemSlabNode * node;
    MemSlabHeap * heap = (MemSlabHeap*)hp;
    int i;
    if(heap == NULL)
        return;

    for(i = 0; i<heap->node_count; i++) {
        node = heap->nodes[i];
        while(node) {
            MemSlabNode * tnode = node;
            node = node->next;
            SLAB_FREE(tnode);
        }
    }

    SLAB_FREE(heap);
}

void * slab_new_mem(void *heap, int size, int padding)
{
    void *p ;

    if(heap == NULL)
        heap = default_heap;

    p = slab_get_mem((MemSlabHeap*)heap, size);
    if(!p)
        return NULL;

    memset(p, padding, size);

    return p;
}

void slab_free_mem(void *heap, void *p)
{
    if(p == NULL)
        return ;

    if(heap == NULL)
        heap = default_heap;

    slab_release_mem((MemSlabHeap*)heap, p);
}

/////////////////////////////////

int slab_init_default_heap()
{
    if(!default_heap)
        default_heap = slab_new_heap(8, 256);

    return default_heap != NULL;
}

void slab_unit_default_heap()
{
    slab_free_heap(default_heap);
    default_heap = NULL;
}

