#pragma once

//
// ARENA
//
struct ea_arena {
    size_t offset;
    size_t size;
    unsigned char *buffer;
};
void ea_arena_init(struct ea_arena *a, size_t size, unsigned char *buffer);
void *ea_arena_alloc(struct ea_arena *a, size_t size);
void ea_arena_reset(struct ea_arena *a);
size_t ea_arena_save_offset(struct ea_arena *a);
void ea_arena_restore_offset(struct ea_arena *a, size_t offset);

//
// ALLOC
//
void *ea_alloc(size_t size);
void ea_free(void *ptr);
