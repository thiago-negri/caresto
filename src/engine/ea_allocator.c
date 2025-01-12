#include <engine/ea_allocator.h>
#include <engine/et_test.h>
#include <stdlib.h>

//
// ALLOC
//
void *ea_alloc(size_t size) {
    void *ptr = malloc(size);
    return ptr;
}

void ea_free(void *ptr) { free(ptr); }

//
// ARENA
//
void ea_arena_init(struct ea_arena *a, size_t size, unsigned char *buffer) {
    a->offset = 0;
    a->size = size;
    a->buffer = buffer;
}

void *ea_arena_alloc(struct ea_arena *a, size_t size) {
    size_t free_space = a->size - a->offset;
    if (size > free_space) {
        return NULL;
    }
    void *ptr = a->buffer + a->offset;
    a->offset += size;
    return ptr;
}

void ea_arena_reset(struct ea_arena *a) { a->offset = 0; }

size_t ea_arena_save_offset(struct ea_arena *a) { return a->offset; }

void ea_arena_restore_offset(struct ea_arena *a, size_t offset) {
    a->offset = offset;
}

ET_TEST(ea_arena) {
    unsigned char buffer[10];

    struct ea_arena arena = {};
    ea_arena_init(&arena, 10, buffer);
    ET_ASSERT(arena.offset == 0);
    ET_ASSERT(arena.size == 10);
    ET_ASSERT(arena.buffer == buffer);

    int *first_int = (int *)ea_arena_alloc(&arena, 4);
    ET_ASSERT(first_int != NULL);
    ET_ASSERT(arena.offset == 4);

    size_t offset = ea_arena_save_offset(&arena);
    ET_ASSERT(offset == 4);

    int *second_int = (int *)ea_arena_alloc(&arena, 4);
    ET_ASSERT(second_int != NULL);
    ET_ASSERT(arena.offset == 8);

    // OOM
    int *third_int = (int *)ea_arena_alloc(&arena, 4);
    ET_ASSERT(third_int == NULL);
    ET_ASSERT(arena.offset == 8);

    ea_arena_restore_offset(&arena, offset);
    ET_ASSERT(arena.offset == 4);

    int *fourth_int = (int *)ea_arena_alloc(&arena, 4);
    ET_ASSERT(second_int != NULL);
    ET_ASSERT(arena.offset == 8);

    // 2nd and 4th share the same memory
    *second_int = 2;
    ET_ASSERT(*fourth_int == 2);

    ET_DONE;
}
