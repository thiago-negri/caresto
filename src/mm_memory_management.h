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
#define mm_arena_zero ((mm_arena){ .offset = 0, .size = 0, .buffer = NULL })
mm_arena mm_arena_create(size_t size);
void *mm_arena_alloc(mm_arena *a, size_t size);
void mm_arena_reset(mm_arena *a);
void mm_arena_destroy(mm_arena *a);
size_t mm_arena_save_offset(mm_arena *a);
void mm_arena_restore_offset(mm_arena *a, size_t offset);

#endif

