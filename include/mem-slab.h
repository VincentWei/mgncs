
#ifndef MEM_SLAB_H
#define MEM_SLAB_H

#ifdef __cplusplus
extern "C" {
#endif

MGNCS_EXPORT void* slab_new_heap(int min_size, int max_size);

MGNCS_EXPORT void slab_free_heap(void *heap);

MGNCS_EXPORT void* slab_new_mem(void *heap, int size, int padding);

MGNCS_EXPORT void slab_free_mem(void *heap, void *p);

MGNCS_EXPORT int slab_init_default_heap(void);

MGNCS_EXPORT void slab_unit_default_heap(void);

#define _SLAB_ALLOC(size)  slab_new_mem(NULL, (size), 0)
#define _SLAB_FREE(p)   slab_free_mem(NULL, p)

#ifdef __cplusplus
}
#endif

#endif


