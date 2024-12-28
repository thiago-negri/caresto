#include <stdlib.h>
#include <mm_memory_management.h>

//
// ARENA
//

mm_arena mm_arena_create(size_t size) {
    unsigned char *buffer = (unsigned char *) malloc(size);
    if (buffer == NULL) {
        size = 0;
    }
    return (mm_arena){
        .offset = 0,
        .size = size,
        .buffer = buffer,
    };
}

void *mm_arena_alloc(mm_arena *a, size_t size) {
    size_t free_space = a->size - a->offset;
    if (size > free_space) {
        return NULL;
    }
    void *ptr = a->buffer + a->offset;
    a->offset += size;
    return ptr;
}

void mm_arena_reset(mm_arena *a) {
    a->offset = 0;
}

void mm_arena_destroy(mm_arena *a) {
    free(a->buffer);
}
