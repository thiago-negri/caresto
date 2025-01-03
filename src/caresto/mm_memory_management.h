#ifndef MM_MEMORY_MANAGEMENT_H
#define MM_MEMORY_MANAGEMENT_H

//
// ARENA
//
struct mm_arena {
    size_t offset;
    size_t size;
    unsigned char *buffer;
};
struct mm_arena mm_arena_create(size_t size, unsigned char *buffer);
void *mm_arena_alloc(struct mm_arena *a, size_t size);
void mm_arena_reset(struct mm_arena *a);
void mm_arena_destroy(struct mm_arena *a);
size_t mm_arena_save_offset(struct mm_arena *a);
void mm_arena_restore_offset(struct mm_arena *a, size_t offset);

//
// ALLOC
//
void *mm_alloc(size_t size);
void mm_free(void *ptr);

#endif // MM_MEMORY_MANAGEMENT_H