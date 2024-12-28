#ifndef _MM_MEMORY_MANAGEMENT_H
#define _MM_MEMORY_MANAGEMENT_H

//
// ARENA
//
typedef struct {
    size_t offset;
    size_t size;
    unsigned char *buffer;
} mm_arena;
mm_arena mm_arena_create(size_t size);
void *mm_arena_alloc(mm_arena *a, size_t size);
void mm_arena_reset(mm_arena *a);
void mm_arena_destroy(mm_arena *a);

#endif

