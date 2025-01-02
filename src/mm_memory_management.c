#include <stdlib.h>

#include <mm_memory_management.h>
#include <t_test.h>

//
// ALLOC
//
void *mm_alloc(size_t size) {
    void *ptr = malloc(size);
    return ptr;
}

void mm_free(void *ptr) {
    free(ptr);
}


//
// ARENA
//
struct mm_arena mm_arena_create(size_t size, unsigned char *buffer) {
    return (struct mm_arena){
        .offset = 0,
        .size = size,
        .buffer = buffer,
    };
}

void *mm_arena_alloc(struct mm_arena *a, size_t size) {
    size_t free_space = a->size - a->offset;
    if (size > free_space) {
        return NULL;
    }
    void *ptr = a->buffer + a->offset;
    a->offset += size;
    return ptr;
}

void mm_arena_reset(struct mm_arena *a) {
    a->offset = 0;
}

void mm_arena_destroy(struct mm_arena *a) {
    a->offset = 0;
    a->size = 0;
    a->buffer = NULL;
}

size_t mm_arena_save_offset(struct mm_arena *a) {
    return a->offset;
}

void mm_arena_restore_offset(struct mm_arena *a, size_t offset) {
    a->offset = offset;
}

T_TEST(arena) {
    unsigned char buffer[10];

    struct mm_arena arena = mm_arena_create(10, buffer);
    T_ASSERT(arena.offset == 0);
    T_ASSERT(arena.size == 10);
    T_ASSERT(arena.buffer == buffer);

    int *first_int = (int *) mm_arena_alloc(&arena, 4);
    T_ASSERT(first_int != NULL);
    T_ASSERT(arena.offset == 4);

    size_t offset = mm_arena_save_offset(&arena);
    T_ASSERT(offset == 4);

    int *second_int = (int *) mm_arena_alloc(&arena, 4);
    T_ASSERT(second_int != NULL);
    T_ASSERT(arena.offset == 8);

    // OOM
    int *third_int = (int *) mm_arena_alloc(&arena, 4);
    T_ASSERT(third_int == NULL);
    T_ASSERT(arena.offset == 8);

    mm_arena_restore_offset(&arena, offset);
    T_ASSERT(arena.offset == 4);

    int *fourth_int = (int *) mm_arena_alloc(&arena, 4);
    T_ASSERT(second_int != NULL);
    T_ASSERT(arena.offset == 8);

    // 2nd and 4th share the same memory
    *second_int = 2;
    T_ASSERT(*fourth_int == 2);

    mm_arena_destroy(&arena);
    T_ASSERT(arena.offset == 0);
    T_ASSERT(arena.size == 0);
    T_ASSERT(arena.buffer == NULL);

    T_DONE;
}

