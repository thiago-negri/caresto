#include <stdlib.h>

#include <engine/em_memory.h>
#include <engine/et_test.h>

//
// ALLOC
//
void *em_alloc(size_t size) {
    void *ptr = malloc(size);
    return ptr;
}

void em_free(void *ptr) { free(ptr); }

//
// ARENA
//
struct em_arena em_arena_create(size_t size, unsigned char *buffer) {
    return (struct em_arena){
        .offset = 0,
        .size = size,
        .buffer = buffer,
    };
}

void *em_arena_alloc(struct em_arena *a, size_t size) {
    size_t free_space = a->size - a->offset;
    if (size > free_space) {
        return NULL;
    }
    void *ptr = a->buffer + a->offset;
    a->offset += size;
    return ptr;
}

void em_arena_reset(struct em_arena *a) { a->offset = 0; }

void em_arena_destroy(struct em_arena *a) {
    a->offset = 0;
    a->size = 0;
    a->buffer = NULL;
}

size_t em_arena_save_offset(struct em_arena *a) { return a->offset; }

/// FIXME(tnegri): arena restore
/// This is a bit dangerous when the arena is passed around.  Probably not worth
/// it.  Remove?
void em_arena_restore_offset(struct em_arena *a, size_t offset) {
    a->offset = offset;
}

ET_TEST(arena) {
    unsigned char buffer[10];

    struct em_arena arena = em_arena_create(10, buffer);
    ET_ASSERT(arena.offset == 0);
    ET_ASSERT(arena.size == 10);
    ET_ASSERT(arena.buffer == buffer);

    int *first_int = (int *)em_arena_alloc(&arena, 4);
    ET_ASSERT(first_int != NULL);
    ET_ASSERT(arena.offset == 4);

    size_t offset = em_arena_save_offset(&arena);
    ET_ASSERT(offset == 4);

    int *second_int = (int *)em_arena_alloc(&arena, 4);
    ET_ASSERT(second_int != NULL);
    ET_ASSERT(arena.offset == 8);

    // OOM
    int *third_int = (int *)em_arena_alloc(&arena, 4);
    ET_ASSERT(third_int == NULL);
    ET_ASSERT(arena.offset == 8);

    em_arena_restore_offset(&arena, offset);
    ET_ASSERT(arena.offset == 4);

    int *fourth_int = (int *)em_arena_alloc(&arena, 4);
    ET_ASSERT(second_int != NULL);
    ET_ASSERT(arena.offset == 8);

    // 2nd and 4th share the same memory
    *second_int = 2;
    ET_ASSERT(*fourth_int == 2);

    em_arena_destroy(&arena);
    ET_ASSERT(arena.offset == 0);
    ET_ASSERT(arena.size == 0);
    ET_ASSERT(arena.buffer == NULL);

    ET_DONE;
}
