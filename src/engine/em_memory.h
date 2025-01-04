#ifndef EM_MEMORY_H
#define EM_MEMORY_H

//
// ARENA
//
struct em_arena {
    size_t offset;
    size_t size;
    unsigned char *buffer;
};
struct em_arena em_arena_create(size_t size, unsigned char *buffer);
void *em_arena_alloc(struct em_arena *a, size_t size);
void em_arena_reset(struct em_arena *a);
void em_arena_destroy(struct em_arena *a);
size_t em_arena_save_offset(struct em_arena *a);
void em_arena_restore_offset(struct em_arena *a, size_t offset);

//
// ALLOC
//
void *em_alloc(size_t size);
void em_free(void *ptr);

#endif // EM_MEMORY_H
