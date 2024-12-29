#include <stdlib.h>
#include <mm_memory_management.h>

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

mm_arena mm_arena_create(size_t size) {
    unsigned char *buffer = (unsigned char *) mm_alloc(size);
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
    if (a->buffer != NULL) {
        mm_free(a->buffer);
    }
    a->offset = 0;
    a->size = 0;
    a->buffer = NULL;
}
